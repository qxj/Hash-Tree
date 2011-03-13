/* @(#)FileHashTree.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-03-11 17:50:16>
 * Copyright 2011 Julian Qian
 * Version: $Id: FileHashTree.h,v 0.0 2011/03/11 04:57:55 jqian Exp $
 */

#ifndef _FILEHASHTREE_H
#define _FILEHASHTREE_H 1

#include "FileHashNode.h"

class FileHashTree {
public:
    enum {
        TREE_NODES_COUNT = 31,
        TREE_LEAVES_PTR = 15,
        FILE_TRUNK_COUNT = 256,
        MAX_FILE_SIZE = 2147483648U // 2GB
    };
    FileHashTree();
    ~FileHashTree();

    void build(const char* file);
    DigestType rootDigest();

private:
    HashNode* tree_[TREE_NODES_COUNT];         // 4-level perfect binary tree
};

#endif /* _FILEHASHTREE_H */
