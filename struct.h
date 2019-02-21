/* Comp 40 Homework 4 - Arith
struct.h
Minh-Duc Nguyen
Trung Truong
October 21, 2018
*/
#ifndef _STRUCTH_
#define _STRUCTH_

typedef struct Pnm_ypbpr 
{
        float y;
        float pb;
        float pr;
} *Pnm_ypbpr;

typedef struct Pnm_compressed 
{
        unsigned width, height, denominator;
        A2Methods_UArray2 pixels;
        const struct A2Methods_T *methods;
} *Pnm_compressed;

typedef struct Bit_word_matrix
{
        unsigned width, height;
        A2Methods_UArray2 word_array;
        const struct A2Methods_T *methods;
} *Bit_word_matrix;

#endif