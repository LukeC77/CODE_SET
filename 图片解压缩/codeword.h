/*
 *     codeword.h
 *     Yifan Cao(ycao12), Wesly Zhou (wzhou05)
 *     10/16/2024
 *     CS40 HW4 arith
 * 
 *     Purpose: Interface for codeword. Contains two functions used to pack 
 *              code words and unpack code words.
 */

#ifndef CODEWORD_H
#define CODEWORD_H

#include "bitpack.h"
#include "seq.h"
#include "component.h"
#include "codeinfo.h"
#include "mem.h"
#include <stdlib.h>

void packWords(Seq_T code_info, Seq_T code_words);
void unpackWords(Seq_T code_words, Seq_T code_info);

#endif