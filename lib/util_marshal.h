/* @(#)util_marshal.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-03-17 13:49:40>
 * Copyright 2011 Julian Qian
 * Version: $Id: util_marshal.h,v 0.0 2011/03/17 05:49:38 jqian Exp $
 */

#ifndef _UTIL_MARSHAL_H
#define _UTIL_MARSHAL_H 1

// marshal & unmarshal -- begin
char* push_string(char* buf, const char* data, const int len);
char* pop_string(const char* buf, char* data, const int len);
char* push_data(char* buf, const char* data, const int len);
char* pop_data(const char* buf, char* data, const int len);
char* push_char(char* buf, const char& c);
char* pop_char(const char* buf, char& c);
char* push_uchar(char* buf, const unsigned char& c);
char* pop_uchar(const char* buf, unsigned char& c);
char* push_uint32(char* buf, const unsigned int& u);
char* pop_uint32(const char* buf, unsigned int& u);

#endif /* _UTIL_MARSHAL_H */
