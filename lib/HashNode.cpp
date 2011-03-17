// @(#)HashNode.cpp
// Time-stamp: <Julian Qian 2011-03-17 16:27:39>
// Copyright 2011 Julian Qian
// Version: $Id: HashNode.cpp,v 0.0 2011/03/11 07:22:05 jqian Exp $

#include <boost/uuid/sha1.hpp>
#include <sstream>

#include "util_marshal.h"
#include "HashNode.h"

using namespace boost::uuids::detail;
using std::string;


const std::string
DigestType::toString() const{
    std::ostringstream oss;
    for (int i = 0; i < N; ++i) {
        oss << std::hex << digest_[i];
    }
    return oss.str();
}

char*
DigestType::unserilize(char* p){
    for (int i = 0; i < N; ++i) {
        p = pop_uint32(p, digest_[i]);
    }
    return p;
}

char*
DigestType::serilize(char* p){
    for (int i = 0; i < N; ++i) {
        p = push_uint32(p, digest_[i]);
    }
    return p;
}

int
DigestType::process(const char* buf, const size_t len){
    if(buf){
        sha1 sha1calc;
        sha1calc.process_bytes(buf, len);
        sha1calc.get_digest(digest_);
        return 0;
    }
    return -1;
}


int
HashNode::digestChildren(const HashNode* l, const HashNode* r){
    string dstr;
    if(l){
        dstr += (l->digest_).toString();
    }
    if(r){
        dstr += (r->digest_).toString();;
    }
    if(! dstr.empty()){
        this->digest_.process(dstr.c_str(), dstr.length());
        return 0;
    }
    return -1;
}

// marshal & unmarshal
char*
HashNode::unserilize(char* p){
    p = pop_char(p, type_);
    p = digest_.unserilize(p);
    return p;
}

char*
HashNode::serilize(char* p){
    p = push_char(p, type_);
    p = digest_.serilize(p);
    return p;
}
