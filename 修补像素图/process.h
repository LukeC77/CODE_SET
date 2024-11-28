/**************************************************************
 *
 *                     process.h
 *
 *     Assignment:      filesofpix
 *     Authors:         Yifan Cao
 *                      Xiaoyan Xie
 *     Date:            9/15/2024
 *
 *     Summary:
 *     This is the interface for implementation file process.c .
 *
 **************************************************************/

#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>
#include <stdio.h>
#include "readaline.h"
#include "table.h"
#include "except.h"
#include "seq.h"
#include <stdbool.h>

void data_processing(FILE* fp, char* file_name);
void vfree(const void *key, void **value, void *cl, int num);
unsigned extract(char* line, int** digi, char** non_digi, unsigned size);
void output(Seq_T SQ, char* file_name);
void route_to_table(Table_T table, 
                    int* digi, 
                    char* non_digi, 
                    unsigned non_digi_len,
                    char** target);

#endif