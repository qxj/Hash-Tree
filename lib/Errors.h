/* @(#)Errors.h -*- mode: c++ -*-
 * Time-stamp: <Julian Qian 2011-03-17 14:18:53>
 * Copyright 2011 Julian Qian
 * Version: $Id: Errors.h,v 0.0 2011/03/11 05:32:04 jqian Exp $
 */

#ifndef _ERRORS_H
#define _ERRORS_H 1

#include <errno.h>
#include <string.h>
#include "Logging.h"

#define EC_OK      0
#define EC_ESYS    100           // sys error
#define EC_EMATCH  101           // size mismatch

#define EC_NO_RETURN(FMT, ...)   LERROR(FMT, ##__VA_ARGS__); return;
#define EC_RETURN(VAL, FMT, ...) LERROR(FMT, ##__VA_ARGS__); return VAL;
#define EC_SYS_RETURN(FMT, ...)  LERROR(FMT, ##__VA_ARGS__); return EC_ESYS;

#endif /* _ERRORS_H */
