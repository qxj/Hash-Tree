/* @(#)FileHashTree.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-04-20 17:31:55>
 * Copyright 2011 Julian Qian
 * Version: $Id: FileHashTree.h,v 0.0 2011/03/11 04:57:55 jqian Exp $
 */

#ifndef _FILEHASHTREE_H
#define _FILEHASHTREE_H 1

#include "FileHashNode.h"

class FileHashTree {
public:
    enum {
        KILO_SIZE = 1024
    };

    friend class FileHashTreeManager;

    FileHashTree(unsigned blockKiloSize = 512,
                 unsigned fileKiloSize = 2097152,
                 unsigned trunksCount = 256);
    ~FileHashTree();

    // generate hash tree
    void build(const char* filename);
    // unserilize hash tree
    char* unserilize(char* p);
    // serilize hash tree
    char* serilize(char* p);
    // fetch entitle file hash tree length
    static unsigned getLength(char* p);
    //
    DigestType rootDigest();
    // approximately size
    unsigned maxSerilizeSize() const {
        return nodesCount_ * sizeof(DTNode) + (nodesCount_/2) * trunksCount_ * sizeof(DTBlock);
    }

private:
    HashNode** tree_;         // ptr to a perfect binary tree

    unsigned blockSize_;
    unsigned trunksCount_;
    unsigned nodesCount_;
};

class FileHashTreeManager {
public:
    FileHashTreeManager()
        : lastblk_(), offsets_() {}

    int diff(FileHashTree* dst, FileHashTree* src);

    typedef std::vector<size_t> OffsetList;
    OffsetList& offsets(){ return offsets_; }
    DTLeaf::BlockInfo& lastblk() { return lastblk_; }
private:
    void compareTrees_(HashNode* dst[], HashNode* src[], int ptr,
                       unsigned nodes, unsigned trunks, unsigned blksize);

    DTLeaf::BlockInfo lastblk_; // if lastblk_.ptr != 0, reach source end.
    OffsetList offsets_;
};

#endif /* _FILEHASHTREE_H */
