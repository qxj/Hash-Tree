/* @(#)FileHashNode.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-04-20 17:36:31>
 * Copyright 2011 Julian Qian
 * Version: $Id: FileHashNode.h,v 0.0 2011/03/11 05:22:15 jqian Exp $
 */

#ifndef _FILEHASHNODE_H
#define _FILEHASHNODE_H 1

#include <unistd.h>
#include <vector>

#include "HashNode.h"

class DTBlock: public HashNode {
public:
    // enum { DATA_BLOCK_SIZE = 524288 }; // 512KB
    DTBlock()
        : HashNode(BLOCK) {}
};

class DTLeaf: public HashNode {
public:
    struct BlockInfo {
        BlockInfo()
            : ptr(0), length(0) {}
        unsigned ptr;
        unsigned length;
    };

    explicit DTLeaf(const unsigned blockSize)
        : HashNode(LEAF),
          blocks_(),
          lastblk_(),
          blockSize_(blockSize){}

    void init(const char* filename,
              off_t offset,
              unsigned length);

    typedef std::vector<DTBlock> BlockList;
    BlockList& blocks(){ return blocks_; }
    BlockInfo& lastblk() { return lastblk_; }

    // override
    char* unserilize(char* p);
    char* serilize(char* p);
private:
    BlockList blocks_;
    BlockInfo lastblk_;
    unsigned blockSize_;
};

class DTNode: public HashNode {
public:
    DTNode()
        : HashNode(NODE) {}
};

#endif /* _FILEHASHNODE_H */
