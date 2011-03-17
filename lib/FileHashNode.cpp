// @(#)FileHashNode.cpp
// Time-stamp: <Julian Qian 2011-03-17 16:20:05>
// Copyright 2011 Julian Qian
// Version: $Id: FileHashNode.cpp,v 0.0 2011/03/11 05:22:50 jqian Exp $

#include <stdlib.h>
#include "Errors.h"
#include "util_marshal.h"
#include "FileHashNode.h"

using std::string;

void
DTLeaf::init(const char* filename,
             const uint32_t offset,
             const uint32_t length){
    // read file
    FILE* fd = fopen(filename, "r");
    if(fseek(fd, offset, SEEK_SET)){
        EC_NO_RETURN("failed to seek offset, %s.",
                     strerror(errno));
    }
    int count = length / DTBlock::DATA_BLOCK_SIZE;
    char* buffer = (char*)malloc(DTBlock::DATA_BLOCK_SIZE);
    for (int i = 0; i <= count; ++i) {
        size_t rlen = fread(buffer, 1, DTBlock::DATA_BLOCK_SIZE, fd);
        if(rlen < DTBlock::DATA_BLOCK_SIZE){
            if(feof(fd)){       // end of file

            }else if(ferror(fd)){
                EC_NO_RETURN("read data failed, %s.",
                             strerror(errno));
            }else{

            }
        }
        DigestType dt;
        dt.process(buffer, rlen);

        // add data block item
        DTBlock block;
        block.digest(dt);
        blocks_.push_back(block);
    }
    fclose(fd);

    // calculate
    BlockList::iterator itr;
    string sumstr;
    for (itr = blocks_.begin(); itr != blocks_.end(); ++ itr) {
        // TODO: how to calc children more reasonably
        sumstr += itr->digest().toString();
    }
    digest_.process(sumstr.c_str(), sumstr.length());
}

char*
DTLeaf::unserilize(char* p){
    p = HashNode::unserilize(p);
    unsigned int len;
    p = pop_uint32(p, len);
    blocks_.reserve(len);
    for (int i = 0; i < len; ++i) {
        DTBlock block;
        p = block.unserilize(p);
        blocks_.push_back(block);
    }
    return p;
}

char*
DTLeaf::serilize(char* p){
    p = HashNode::serilize(p);
    unsigned int len = blocks_.size();
    p = push_uint32(p, len);
    for (BlockList::iterator itr = blocks_.begin();
         itr != blocks_.end(); ++ itr) {
        p = itr->serilize(p);
    }
    return p;
}
