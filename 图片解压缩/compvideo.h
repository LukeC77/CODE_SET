/*
 *     compvideo.h
 *     Yifan Cao(ycao12), Wesly Zhou (wzhou05)
 *     10/16/2024
 *     CS40 HW4 arith
 * 
 *     Purpose: Interface for compvideo. Contains two functions that allow for
 *              transformataion between RGB color space and component video
 *              color space.
 */

#ifndef COMPVIDEO_H
#define COMPVIDEO_H

#include "a2methods.h"
#include "uarray.h"
#include "pnm.h"
#include "a2blocked.h"
#include "assert.h"
#include "component.h"

#define A2 A2Methods_UArray2

A2 rgbToCV(Pnm_ppm pixmap);
void cvToRGB(A2 arrayCV, Pnm_ppm pixmap);

#undef A2
#endif 