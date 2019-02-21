/* Comp 40 Homework 4 - Arith
component_vid.h
Minh-Duc Nguyen
Trung Truong
October 21, 2018
*/
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "dct_quantize.h"
#include "struct.h"

#ifndef _COMPONENTVID_H
#define _COMPONENTVID_H

void trim_and_convert_original(Pnm_ppm *, Pnm_compressed *);
void convert_rgb_to_ypbpr(int, int, A2Methods_UArray2, void *, void *);
Pnm_ypbpr rgb_to_ypbpr_formulae(Pnm_rgb, unsigned);
void convert_ypbpr_to_rgb(int, int, A2Methods_UArray2, void*, void*);
Pnm_rgb ypbpr_to_rgb_formulae(Pnm_ypbpr);

float check_bound(float f, float low, float high);

#endif