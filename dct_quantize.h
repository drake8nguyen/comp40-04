/* Comp 40 Homework 4 - Arith
dct_quantize.h
Minh-Duc Nguyen
Trung Truong
October 21, 2018
*/
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "struct.h"

#ifndef _DCTQUANTIZE_H
#define _DCTQUANTIZE_H

/*Compression function prototypes*/
uint64_t word_pack(unsigned, signed, signed, signed, unsigned, unsigned);
unsigned average_pb(float, float, float, float);
unsigned average_pr(float, float, float, float);
unsigned calculate_a(float, float, float, float);
signed calculate_b(float, float, float, float);
signed calculate_c(float, float, float, float);
signed calculate_d(float, float, float, float);
void apply_compress(int, int, A2Methods_UArray2, void *, void *);
Bit_word_matrix compress(Pnm_compressed);

/*Decompression function prototypes*/
Pnm_compressed unpack(Bit_word_matrix);
void apply_unpack(int, int, A2Methods_UArray2, void *, void*);
void convert_to_original (Pnm_compressed, Pnm_ppm);
float chroma_pb(uint64_t);
float chroma_pr(uint64_t);
float calculate_local_a(uint64_t);
float calculate_local_b(uint64_t);
float calculate_local_c(uint64_t);
float calculate_local_d(uint64_t);
float calculate_y1(float, float, float, float);
float calculate_y2(float, float, float, float);
float calculate_y3(float, float, float, float);
float calculate_y4(float, float, float, float);

#endif