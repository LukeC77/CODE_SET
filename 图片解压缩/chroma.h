/*
 *     chroma.h
 * 
 *     Purpose: Interface for chroma. Contains two functions that allow for
 *              conversion between chroma values as floats and scaled integers.
 *              The average of the chroma values for a given block or code word
 *              maintained.
 */

#ifndef CHROMA_H
#define CHROMA_H

#include "seq.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "component.h"
#include "codeinfo.h"
#include "mem.h"
#include "arith40.h"
#include <stdio.h>


#define A2 A2Methods_UArray2

void encodeChroma(A2 arrayCV, Seq_T seq);
void decodeChroma(A2 arrayCV, Seq_T seq);

#undef A2
#endif