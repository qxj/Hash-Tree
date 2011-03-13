/* @(#)Logging.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-03-11 13:31:47>
 * Copyright 2011 Julian Qian
 * Version: $Id: Logging.h,v 0.0 2011/03/11 05:29:11 jqian Exp $
 */

#ifndef _LOGGING_H
#define _LOGGING_H 1

#include <stdio.h>

// TODO: implement a logger

#define LDEBUG(FMT, ...) fprintf(stderr, FMT, ##__VA_ARGS__);
#define LINFO(FMT, ...) fprintf(stderr, FMT, ##__VA_ARGS__);
#define LERROR(FMT, ...) fprintf(stderr, FMT, ##__VA_ARGS__);
#define LFATAL(FMT, ...) fprintf(stderr, FMT, ##__VA_ARGS__);

#endif /* _LOGGING_H */
