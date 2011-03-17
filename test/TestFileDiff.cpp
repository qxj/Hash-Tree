// @(#)TestFileDiff.cpp
// Time-stamp: <Julian Qian 2011-03-17 16:21:48>
// Copyright 2011 Julian Qian
// Version: $Id: TestFileDiff.cpp,v 0.0 2011/03/16 08:11:15 jqian Exp $

#include <stdio.h>
#include "FileHashTree.h"

using namespace std;

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("usage:\n\ttestdf <file1> <file2>\n");
        return -1;
    }

    const char* file1 = argv[1];
    if(access(file1, R_OK)){
        printf("file %s existed?\n", file1);
        return -1;
    }
    const char* file2 = argv[2];
    if(access(file2, R_OK)){
        printf("file %s existed?\n", file2);
        return -1;
    }
    FileHashTree ht1;
    FileHashTree ht2;

    ht1.build(file1);
    ht2.build(file2);

    FileHashTreeManager htm;
    htm.diff(&ht1, &ht2);

    FileHashTreeManager::OffsetList& ol = htm.offsets();

    printf("%d diff blocks. offset: \n", ol.size());
    for (FileHashTreeManager::OffsetList::iterator itr = ol.begin();
         itr != ol.end(); ++itr) {
        printf("%d ", *itr * DTBlock::DATA_BLOCK_SIZE);
    }
    printf("\n");

    return 0;
}

