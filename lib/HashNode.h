/* @(#)HashNode.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-04-25 17:55:08>
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
    DigestType& operator=(const digest_type& dt){
        for (int i = 0; i < N; ++i) {
            digest_[i] = dt[i];
        }
    }
    bool operator==(const DigestType& dt) const {
        if(this != &dt){
            for (int i = 0; i < N; ++i) {
                if(digest_[i] != dt.digest_[i]){
                    return false;
                }
            }
        }
        return true;
    }
    bool operator==(const digest_type& dt) const {
        for (int i = 0; i < N; ++i) {
            if(digest_[i] != dt[i]){
                return false;
            }
        }
        return true;
    }
    bool operator!=(const DigestType& dt) const {
        if(this != &dt){
            for (int i = 0; i < N; ++i) {
                if(digest_[i] != dt.digest_[i]){
                    return true;
                }
            }
        }
        return false;
    }
    operator const digest_type& () const {
        return digest_;
    }

    int process(const char* buf, const size_t len);

    const std::string toString() const;

    // serilize & unserilize
    char* unserilize(char* str);
    char* serilize(char* str);

    bool empty(){
        for (int i = 0; i < N; ++i) {
            if(digest_[i] != 0) {
                return false;
            }
        }
        return true;
    }
private:
    digest_type digest_;
};

class HashNode {
public:
    explicit HashNode()
        : digest_() {}
    virtual ~HashNode() {}
    const DigestType digest() const { return digest_; }
    void digest(DigestType& dt){ digest_ = dt; }
    int digestChildren(const HashNode* l, const HashNode* r);

    // serilize & unserilize
    char* serilize(char* str);
    char* unserilize(char* str);

    bool empty(){ return digest_.empty(); }
protected:
    DigestType digest_;
};

#endif /* _HASHNODE_H */
