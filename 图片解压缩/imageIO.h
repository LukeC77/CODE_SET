/*
 *     imageIO.h
 *     Yifan Cao(ycao12), Wesly Zhou (wzhou05)
 *     10/16/2024
 *     CS40 HW4 arith
 * 
 *     Purpose: Tnterface for imageIO. Contains all of the functions related
 *              to reading in or writing out an image from a file. The 
 *              functions for PPM (uncompressed) images and binary 
 *              (compressed) images are kept together because they operate 
 *              in the same problem domain. 
 */

#ifndef IMAGEIO_H
#define IMAGEIO_H

#include "a2methods.h"
#include "a2blocked.h"
#include "pnm.h"
#include "seq.h"

typedef A2Methods_UArray2 A2;

Pnm_ppm readPPM(FILE *input);
void writePPM(Pnm_ppm pixmap);
void writeCompressed(A2 uarray2b, Seq_T seq);
struct Pnm_ppm readCompressed(FILE *input, Seq_T seq);


#endif 