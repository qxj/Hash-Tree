// @(#)FileHashTree.cpp
// Time-stamp: <Julian Qian 2011-03-11 17:49:12>
// Copyright 2011 Julian Qian
// Version: $Id: FileHashTree.cpp,v 0.0 2011/03/11 06:31:20 jqian Exp $

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include <boost/thread/thread.hpp>

#include "Errors.h"
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

DigestType
FileHashTree::rootDigest(){
    assert(tree_[0]);
    return tree_[0]->digest();
}
