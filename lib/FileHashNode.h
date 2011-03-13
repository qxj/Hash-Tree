/* @(#)FileHashNode.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-03-11 16:27:17>
 * Copyright 2011 Julian Qian
 * Version: $Id: FileHashNode.h,v 0.0 2011/03/11 05:22:15 jqian Exp $
 */

#ifndef _FILEHASHNODE_H
#define _FILEHASHNODE_H 1

#include <stdint.h>
#include <vector>

#include "HashNode.h"

class DTBlock: public HashNode {
public:
    enum { DATA_BLOCK_SIZE = 524288 }; // 512KB
    DTBlock()
        : HashNode(BLOCK) {}
};

class DTLeaf: public HashNode {
public:
    DTLeaf()
        : HashNode(LEAF) {}

    void init(const char* filename,
              const uint32_t offset,
              const uint32_t length);

private:
    typedef std::vector<DTBlock> BlockList;
    BlockList blocks_;
};

class DTNode: public HashNode {
public:
    DTNode()
        : HashNode(NODE) {}
};

#endif /* _FILEHASHNODE_H */
