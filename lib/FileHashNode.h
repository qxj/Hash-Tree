/* @(#)FileHashNode.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-04-25 17:50:26>
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
    DTBlock()
        : HashNode() {}
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
        : HashNode(),
          blocks_(),
          lastblk_(),
          blockSize_(blockSize){}

    void init(const char* filename,
              off_t offset,
              unsigned length);

    typedef std::vector<DTBlock> BlockList;
    const BlockList& blocks() const { return blocks_; }
    const BlockInfo& lastblk() const { return lastblk_; }

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
        : HashNode() {}
};

#endif /* _FILEHASHNODE_H */
