// @(#)TestSync.cpp
// Time-stamp: <Julian Qian 2011-04-19 11:56:25>
// Copyright 2011 Julian Qian
// Version: $Id: TestSync.cpp,v 0.0 2011/03/17 09:17:45 jqian Exp $

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "FileHashTree.h"

int read_fht(const char* f, char* buf){
    unsigned int len;
    FILE* fp = fopen(f, "r");
    if(4 != fread(buf, 1, 4, fp)){
        fprintf(stderr, "failed to read length of %s.\n", f);
        goto FAILED;
    }
    len = FileHashTree::getLength(buf);
    if((len -4) != fread(buf + 4, 1, len - 4, fp)){
        fprintf(stderr, "failed to read remaing of %s.\n", f);
        goto FAILED;
    }
    fclose(fp);
    return len;
FAILED:
    fclose(fp);
    return -1;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("usage:\n\ttestsync <source file> <target file>\n");
        return -1;
    }
    const char* file1 = argv[1];
    const char* file2 = argv[2];
    char fht1[50];
    snprintf(fht1, 50, "%s.fht", file1);
    if(access(fht1, R_OK)){
        printf("file %s existed?\n", fht1);
        return -1;
    }
    char fht2[50];
    snprintf(fht2, 50, "%s.fht", file2);
    if(access(fht2, R_OK)){
        printf("file %s existed?\n", fht2);
        return -1;
    }
    //
    // read fht
    FileHashTree ht1;
    FileHashTree ht2;

    char* buf = (char*)malloc(ht1.maxSerilizeSize());

    if(read_fht(fht1, buf) > 0){
        ht1.unserilize(buf);
    }
    if(read_fht(fht2, buf) > 0){
        ht2.unserilize(buf);
    }
    free(buf);

    FileHashTreeManager htm;
    htm.diff(&ht2, &ht1);
    FileHashTreeManager::OffsetList& offs = htm.offsets();
    DTLeaf::BlockInfo& lastblk = htm.lastblk();
    FileHashTreeManager::OffsetList::iterator itr;
    int len;
    int f1 = open(file1, O_RDONLY);
    int f2 = open(file2, O_WRONLY);
    int blksize = 512*1024;
    buf = (char*)malloc(blksize);
    for (itr = offs.begin(); itr != offs.end(); ++ itr) {
        if(*itr == lastblk.ptr){ // read end
            lseek(f1, lastblk.ptr, SEEK_SET);
            len = read(f1, buf, lastblk.length);
            lseek(f2, lastblk.ptr, SEEK_SET);
            write(f2, buf, lastblk.length);
            ftruncate(f2, lastblk.ptr + lastblk.length);
        }else{
            lseek(f1, *itr, SEEK_SET);
            len = read(f1, buf, blksize);
            lseek(f2, *itr, SEEK_SET);
            write(f2, buf, blksize);
        }
    }
    free(buf);
    close(f1);
    close(f2);
    printf("sync ok!\n");
    return 0;
}


