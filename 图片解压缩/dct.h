/*
 *     dct.h
 *     Yifan Cao(ycao12), Wesly Zhou (wzhou05)
 *     10/16/2024
 *     CS40 HW4 arith
 * 
 *     Purpose: Interface for dct. Contains two functions to transform between
 *              pixel space and DCT space.
 */

#ifndef DCT_H
#define DCT_H

#include "seq.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "component.h"
#include "codeinfo.h"
#include <stdlib.h>
#include <math.h>

#define A2 A2Methods_UArray2

void psToDCT(A2 arrayCV, Seq_T seq);
void dctToPS(A2 arrayCV, Seq_T seq);

#undef A2
#endif