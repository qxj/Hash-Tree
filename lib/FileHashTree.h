/* @(#)FileHashTree.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-03-17 18:17:36>
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
        MAX_SERILIZE_SIZE = 90000,
        MAX_FILE_SIZE = 2147483648U // 2GB
    };

    friend class FileHashTreeManager;

    FileHashTree();
    ~FileHashTree();

    // generate hash tree
    void build(const char* filename);
    // unserilize hash tree
    char* unserilize(char* p);
    // serilize hash tree
    char* serilize(char* p);
    static unsigned int getLength(char* p);
    //
    DigestType rootDigest();

private:
    HashNode* tree_[TREE_NODES_COUNT];         // 4-level perfect binary tree
};

class FileHashTreeManager {
public:
    FileHashTreeManager()
        : lastblk_(), offsets_() {}

    int diff(FileHashTree* dst, FileHashTree* src);

    typedef std::vector<unsigned int> OffsetList;
    OffsetList& offsets(){ return offsets_; }
    DTLeaf::BlockInfo& lastblk() { return lastblk_; }
private:
    void compareTrees_(HashNode* dst[], HashNode* src[], int ptr);

    DTLeaf::BlockInfo lastblk_; // if lastblk_.ptr != 0, reach source end.
    OffsetList offsets_;
};

#endif /* _FILEHASHTREE_H */
