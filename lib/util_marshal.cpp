// @(#)util_marshal.cpp
// Time-stamp: <Julian Qian 2011-03-17 13:52:52>
// Copyright 2011 Julian Qian
// Version: $Id: util_marshal.cpp,v 0.0 2011/03/17 05:49:46 jqian Exp $

#include <arpa/inet.h>
#include <string.h>

#include "util_marshal.h"

// marshal & unmarshal
char* push_string(char* buf, const char* data, const int datalen) {
    ::strncpy(buf, data, datalen);
    return (buf + datalen);
}
char* push_data(char* buf, const char* data, const int datalen) {
    ::memcpy(buf, data, datalen);
    return (buf + datalen);
}
char* push_char(char* buf, const char& s){
    *((char*)buf) = s;
    return (buf + sizeof(char));
}
char* push_uchar(char* buf, const unsigned char& s){
    *((unsigned char*)buf) = s;
    return (buf + sizeof(unsigned char));
}
char* push_uint32(char* buf, const unsigned int& s){
    unsigned int t = htonl(s);
    ::memcpy(buf, &t, sizeof(unsigned int)); // avoid align
    return (buf + sizeof(unsigned int));
}
char* pop_string(const char* buf, char* data, const int len){
    ::strncpy(data, buf, len);
    // data[len-1] = '\0';
    return const_cast<char*>(buf+len);
}

char* pop_data(const char* buf, char* data, const int len){
    ::memcpy(data, buf, len);
    return const_cast<char*>(buf+len);
}
char* pop_char(const char* buf, char& v){
    v = *(const_cast<char*>(buf));
    return const_cast<char*>(buf + sizeof(char));
}
char* pop_uchar(const char* buf, unsigned char& v){
    v = (unsigned char) (*(const_cast<char*>(buf)));
    return const_cast<char*>(buf + sizeof(unsigned char));
}
char* pop_uint32(const char* buf, unsigned int &v){
    unsigned int t;
    ::memcpy(&t, buf, sizeof(unsigned int));
    v = ntohl(t);
    return const_cast<char*>(buf + sizeof(unsigned int));
}
