// @(#)FileHashTree.cpp
// Time-stamp: <Julian Qian 2011-04-19 10:40:56>
// Copyright 2011 Julian Qian
// Version: $Id: FileHashTree.cpp,v 0.0 2011/03/11 06:31:20 jqian Exp $

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include <boost/thread/thread.hpp>

#include "Errors.h"
#include "util_marshal.h"
#include "FileHashTree.h"

unsigned pow2_eq(unsigned n){
    return (n & (n-1)) == 0;
}

unsigned pow2_min(unsigned x){
    int i = 0;
    x = x - 1;
    while ( x ) {
        x = x >> 1;
        i++;
    }
    return 1 << i;
}

unsigned pow2(unsigned n){
    if (pow2_eq(n))
        return n;
    return pow2_min(n);
}

FileHashTree::FileHashTree(unsigned blockKiloSize,
                           unsigned fileKiloSize,
                           unsigned trunksCount)
    : blockSize_(blockKiloSize * KILO_SIZE),
      trunksCount_(trunksCount),
      tree_(NULL){
    unsigned cnt = fileKiloSize / (blockKiloSize * trunksCount);
    nodesCount_ = pow2(cnt) * 2 -1; // nodes count = height^2 -1

    tree_ = (HashNode**)malloc(sizeof(HashNode*) * nodesCount_);
    for (int i = 0; i < nodesCount_; ++i) {
        tree_[i] = NULL;
    }
}

FileHashTree::~FileHashTree(){
    for (int i = 0; i < nodesCount_; ++i) {
        if(tree_[i]) delete tree_[i];
    }
    if(tree_){
        free(tree_);
        tree_ = NULL;
    }
}

static void build_binary_tree(HashNode* root[], unsigned cnt, int ptr){
    if(ptr >= cnt/2){           // leaf nodes' position starts from
                                // the index of nodesCount_/2
        return;
    }
    int lptr = 2 * ptr + 1;
    int rptr = 2 * ptr + 2;
    if(root[lptr] == NULL){ // left
        build_binary_tree(root, cnt, lptr);
    }
    if(root[rptr] == NULL){ // right
        build_binary_tree(root, cnt, rptr);
    }
    if(root[lptr] || root[rptr]){
        root[ptr] = new DTNode;
        root[ptr]->digestChildren(root[lptr], root[rptr]);
    }
}

void
FileHashTree::build(const char* file){
    struct stat statb;
    if(stat(file, &statb)){
        EC_NO_RETURN("failed to fetch file stat, %s.\n", strerror(errno));
    }
    // assert(statb.st_size < MAX_FILE_SIZE);

    unsigned filesize = statb.st_size;
    int trunksize = trunksCount_ * blockSize_;
    int count = filesize / trunksize;

    // calc leaf digest
    boost::thread_group grp;
    for (int i = 0; i <= count; ++i) {
        int offset = i * trunksize;
        int length = trunksize;
        if(i == count){         // final trunk
            length = filesize - i * trunksize;
            if(length == 0){
                break;
            }
        }
        DTLeaf* leaf = new DTLeaf(blockSize_);
        tree_[(nodesCount_/2) + i] = leaf;

        grp.add_thread(new boost::thread(boost::bind(&DTLeaf::init, leaf, file, offset, length)));
    }
    grp.join_all();

    // build tree
    build_binary_tree(tree_, nodesCount_, 0);
}

char*
FileHashTree::unserilize(char* p){
    unsigned int len;
    p = pop_uint32(p, len);
    p = pop_uint32(p, nodesCount_);
    p = pop_uint32(p, blockSize_);
    for (int i = 0; i < nodesCount_; ++i) {
        if(i < nodesCount_/2){ // nodes
            DTNode node;
            p = node.unserilize(p);
            if(node.empty()){
                tree_[i] = NULL;
            }else{
                tree_[i] = new DTNode; // alloc
                *(tree_[i]) = node;
            }
        }else{                   // leaves
            DTLeaf leaf(blockSize_);
            p = leaf.unserilize(p);
            if(leaf.empty()){
                tree_[i] = NULL;
            }else{
                tree_[i] = new DTLeaf(blockSize_);
                DTLeaf* lptr = dynamic_cast<DTLeaf*>(tree_[i]);
                *lptr = leaf;
            }
        }
    }
    return p;
}

char*
FileHashTree::serilize(char* p){
    assert(tree_[0]);
    char* pt = p;
    unsigned len;
    p = push_uint32(p, len);    // place holder
    p = push_uint32(p, nodesCount_);
    p = push_uint32(p, blockSize_);
    for (int i = 0; i < nodesCount_; ++i) {
        if(i < nodesCount_/2){ // nodes
            if(tree_[i] == NULL){
                DTNode node;
                p = node.serilize(p);
            }else{
                DTNode* node = dynamic_cast<DTNode*>(tree_[i]);
                p = node->serilize(p);
            }
        }else{                   // leaves
            if(tree_[i] == NULL){
                DTLeaf leaf(blockSize_);
                p = leaf.serilize(p);
            }else{
                DTLeaf* node = dynamic_cast<DTLeaf*>(tree_[i]);
                p = node->serilize(p);
            }
        }
    }
    push_uint32(pt, p - pt);    // record serilized total length
    return p;
}

unsigned int
FileHashTree::getLength(char* p){
    unsigned int len;
    pop_uint32(p, len);
    return len;
}

DigestType
FileHashTree::rootDigest(){
    assert(tree_[0]);
    return tree_[0]->digest();
}

// FileHashTreeManager
//
void
FileHashTreeManager::compareTrees_(HashNode* dst[], HashNode* src[], int ptr,
                                   unsigned nodes, unsigned trunks, unsigned blksize){
    if(ptr >= nodes/2){
        // compare leaves
        DTLeaf* dstLeaf = dynamic_cast<DTLeaf*>(dst[ptr]);
        DTLeaf* srcLeaf = dynamic_cast<DTLeaf*>(src[ptr]);

        unsigned int baseoff = (ptr - (nodes/2)) * trunks;
        unsigned int offset = baseoff;

        DTLeaf::BlockInfo& srclb = srcLeaf->lastblk();

        if(srcLeaf != NULL){
            DTLeaf::BlockList& srcBlocks = srcLeaf->blocks();
            DTLeaf::BlockList::iterator srcItr = srcBlocks.begin();

            if(dstLeaf != NULL){
                if(dstLeaf->digest() != srcLeaf->digest()){
                    DTLeaf::BlockList::iterator dstItr;
                    DTLeaf::BlockList& dstBlocks = dstLeaf->blocks();
                    for (dstItr = dstBlocks.begin() ;
                         dstItr != dstBlocks.end() && srcItr != srcBlocks.end();
                         ++dstItr, ++srcItr, ++offset) {
                        if(dstItr->digest() != srcItr->digest()){
                            offsets_.push_back(offset * blksize);
                        }
                    }
                }else{
                    return;
                }
            }
            // append
            for (; srcItr != srcBlocks.end(); ++srcItr) {
                offsets_.push_back(offset * blksize);
                ++ offset;
            }
            // read src end
            if(offset >= srclb.ptr + baseoff){
                lastblk_.ptr = (srclb.ptr + baseoff) * blksize;
                lastblk_.length = srclb.length;
            }
        }
    }else if(dst[ptr] || dst[ptr]){
        if(dst[ptr] && dst[ptr] && dst[ptr]->digest() == src[ptr]->digest()){
            return;
        }
        compareTrees_(dst, src, 2*ptr+1, nodes, trunks, blksize);
        compareTrees_(dst, src, 2*ptr+2, nodes, trunks, blksize);
    }
}

int
FileHashTreeManager::diff(FileHashTree* dst, FileHashTree* src){
    assert( src && dst);
    if(dst->nodesCount_ != src->nodesCount_ ||
       dst->trunksCount_ != src->trunksCount_ ||
       dst->blockSize_ != src->blockSize_){
        EC_RETURN(EC_ETYPE, "different hash tree!");
    }
    compareTrees_(dst->tree_, src->tree_, 0,
                  dst->nodesCount_, dst->trunksCount_, dst->blockSize_);
    return 0;
}
