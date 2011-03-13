/* @(#)HashNode.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-03-11 16:30:10>
 * Copyright 2011 Julian Qian
 * Version: $Id: HashNode.h,v 0.0 2011/03/11 05:05:28 jqian Exp $
 */

#ifndef _HASHNODE_H
#define _HASHNODE_H 1

#include <string>

class DigestType {              // defined for boost::uuid::sha1
public:
    enum { N = 5 };

    typedef unsigned int digest_type[N]; // from sha1.hpp

    DigestType(){
        for (int i = 0; i < N; ++i) {
            digest_[i] = 0;
        }
    }
    DigestType(const DigestType& dt){
        if(this != &dt){
            for (int i = 0; i < N; ++i) {
                digest_[i] = dt.digest_[i];
            }
        }
    }
    explicit DigestType(digest_type& dt){
        for (int i = 0; i < N; ++i) {
            digest_[i] = dt[i];
        }
    }
    DigestType& operator=(const DigestType& dt){
        if(this != &dt){
            for (int i = 0; i < N; ++i) {
                digest_[i] = dt.digest_[i];
            }
        }
        return *this;
    }
    DigestType& operator=(digest_type& dt){
        for (int i = 0; i < N; ++i) {
            digest_[i] = dt[i];
        }
    }
    bool operator==(const DigestType& dt){
        if(this != &dt){
            for (int i = 0; i < N; ++i) {
                if(digest_[i] != dt.digest_[i]){
                    return false;
                }
            }
        }
        return true;
    }
    bool operator==(digest_type& dt){
        for (int i = 0; i < N; ++i) {
            if(digest_[i] != dt[i]){
                return false;
            }
        }
        return true;
    }

    digest_type& digest() { return digest_; }

    int process(const char* buf, const size_t len);

    const std::string toString() const;
private:
    digest_type digest_;
};

class HashNode {
public:
    enum { NODE, LEAF, BLOCK };
    explicit HashNode(int type)
        : digest_(), type_(type) {}
    virtual ~HashNode() {}
    bool isLeaf(){ return type_ == LEAF; }
    DigestType digest(){ return digest_; }
    void digest(DigestType& dt){ digest_ = dt; }
    int digestChildren(const HashNode* l, const HashNode* r);
protected:
    DigestType digest_;
    int type_;
};

#endif /* _HASHNODE_H */
