// @(#)TestSerilize.cpp
// Time-stamp: <Julian Qian 2011-03-17 17:25:58>
// Copyright 2011 Julian Qian
// Version: $Id: TestSerilize.cpp,v 0.0 2011/03/17 07:39:57 jqian Exp $

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileHashTree.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("usage:\n\ttestsl <file name>\n");
        return -1;
    }
    char* buf = (char*)malloc(FileHashTree::MAX_SERILIZE_SIZE);

    const char* file = argv[1];
    if(access(file, R_OK)){
        printf("file %s existed?\n", file);
        return -1;
    }

    FileHashTree ht;
    ht.build(file);
    char* p = ht.serilize(buf);
    unsigned int len = p - buf;
    char file1[50];
    snprintf(file1, 50, "%s.fht", file);

    // write ht
    FILE* fp = fopen(file1, "w");
    if(len != fwrite(buf, 1, len, fp)){
        printf("failed to serilize fht.\n");
    }else{
        printf("serilize %d bytes to %s.\n", len, file1);
    }
    fclose(fp);

#if 0
    // read ht
    fp = fopen(file1, "r");
    fread(buf, 4, 1, fp);
    len = FileHashTree::getLength(buf);
    fread(buf + 4, 1, len - 4, fp);
    FileHashTree ht1;
    p = ht1.unserilize(buf);
    printf("read len: %d, parse len: %d\n", len, p - buf);
    fclose(fp);

    // diff
    FileHashTreeManager htm;
    htm.diff(&ht1, &ht);

    FileHashTreeManager::OffsetList& ol = htm.offsets();

    printf("%d diff blocks. offset: \n", ol.size());
    for (FileHashTreeManager::OffsetList::iterator itr = ol.begin();
         itr != ol.end(); ++itr) {
        printf("%d ", *itr * DTBlock::DATA_BLOCK_SIZE);
    }
    printf("\n");
#endif
    //
    free(buf);
    return 0;
}


