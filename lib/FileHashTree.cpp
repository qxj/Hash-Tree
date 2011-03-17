// @(#)FileHashTree.cpp
// Time-stamp: <Julian Qian 2011-03-17 17:33:24>
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

FileHashTree::FileHashTree(){
    for (int i = 0; i < TREE_NODES_COUNT; ++i) {
        tree_[i] = NULL;
    }
}

FileHashTree::~FileHashTree(){
    for (int i = 0; i < TREE_NODES_COUNT; ++i) {
        if(tree_[i]) delete tree_[i];
    }
}

static void build_binary_tree(HashNode* root[], int ptr){
    if(ptr >= FileHashTree::TREE_LEAVES_PTR){
        return;
    }
    int lptr = 2 * ptr + 1;
    int rptr = 2 * ptr + 2;
    if(root[lptr] == NULL){ // left
        build_binary_tree(root, lptr);
    }
    if(root[rptr] == NULL){ // right
        build_binary_tree(root, rptr);
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
    assert(statb.st_size < MAX_FILE_SIZE);

    uint32_t filesize = statb.st_size;
    int trunksize = FILE_TRUNK_COUNT * DTBlock::DATA_BLOCK_SIZE;
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
        DTLeaf* leaf = new DTLeaf;
        tree_[TREE_LEAVES_PTR + i] = leaf;

        grp.add_thread(new boost::thread(boost::bind(&DTLeaf::init, leaf, file, offset, length)));
    }
    grp.join_all();

    // build tree
    build_binary_tree(tree_, 0);
}

char*
FileHashTree::unserilize(char* p){
    unsigned int len;
    p = pop_uint32(p, len);
    for (int i = 0; i < TREE_NODES_COUNT; ++i) {
        if(i < TREE_LEAVES_PTR){ // nodes
            DTNode node;
            p = node.unserilize(p);
            if(node.empty()){
                tree_[i] = NULL;
            }else{
                tree_[i] = new DTNode; // alloc
                *(tree_[i]) = node;
            }
        }else{                   // leaves
            DTLeaf leaf;
            p = leaf.unserilize(p);
            if(leaf.empty()){
                tree_[i] = NULL;
            }else{
                tree_[i] = new DTLeaf;
                *(tree_[i]) = leaf;
            }
        }
    }
    return p;
}

char*
FileHashTree::serilize(char* p){
    assert(tree_[0]);
    char* pt = p;
    unsigned int len;
    p = push_uint32(p, len);    // place holder
    for (int i = 0; i < TREE_NODES_COUNT; ++i) {
        if(i < TREE_LEAVES_PTR){ // nodes
            if(tree_[i] == NULL){
                DTNode node;
                p = node.serilize(p);
            }else{
                DTNode* node = dynamic_cast<DTNode*>(tree_[i]);
                p = node->serilize(p);
            }
        }else{                   // leaves
            if(tree_[i] == NULL){
                DTLeaf leaf;
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
FileHashTreeManager::compareTrees_(HashNode* dst[], HashNode* src[], int ptr){
    if(ptr >= FileHashTree::TREE_LEAVES_PTR){
        // compare leaves
        DTLeaf* dstLeaf = dynamic_cast<DTLeaf*>(dst[ptr]);
        DTLeaf* srcLeaf = dynamic_cast<DTLeaf*>(src[ptr]);

        int offset = (ptr - FileHashTree::TREE_LEAVES_PTR) *
            (FileHashTree::FILE_TRUNK_COUNT);

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
                            offsets_.push_back(offset * DTBlock::DATA_BLOCK_SIZE);
                        }
                    }
                }else{
                    return;
                }
            }
            // append
            for (; srcItr != srcBlocks.end(); ++srcItr) {
                offsets_.push_back(offset * DTBlock::DATA_BLOCK_SIZE);
                ++ offset;
            }
        }
    }else if(dst[ptr] || dst[ptr]){
        if(dst[ptr] && dst[ptr] && dst[ptr]->digest() == src[ptr]->digest()){
            return;
        }
        compareTrees_(dst, src, 2*ptr+1);
        compareTrees_(dst, src, 2*ptr+2);
    }
}

int
FileHashTreeManager::diff(FileHashTree* dst, FileHashTree* src){
    assert( src && dst);
    compareTrees_(dst->tree_, src->tree_, 0);
}
