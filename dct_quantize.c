/* Comp 40 Homework 4 - Arith
dct_quantize.h
Minh-Duc Nguyen
Trung Truong
October 21, 2018
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assert.h"
#include <math.h>
#include "bitpack.h"
#include "arith40.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "dct_quantize.h"
#include "component_vid.h"

const int BLKSIZE = 2;

const int A_WIDTH = 6;
const int B_WIDTH = 6;
const int C_WIDTH = 6;
const int D_WIDTH = 6;
const int I_PB_WIDTH = 4;
const int I_PR_WIDTH = 4;

const int A_LSB = 26;
const int B_LSB = 20;
const int C_LSB = 14;
const int D_LSB = 8;
const int I_PB_LSB = 4;
const int I_PR_LSB = 0;

/***********************************************
*       Compress40 functions                 *
************************************************/
uint64_t word_pack(unsigned a, signed b, signed c, signed d, 
                        unsigned index_pb, unsigned index_pr)
{
        uint64_t word = 0;
        word = Bitpack_newu(word, A_WIDTH, A_LSB, a);
        word = Bitpack_news(word, B_WIDTH, B_LSB, b);
        word = Bitpack_news(word, C_WIDTH, C_LSB, c);
        word = Bitpack_news(word, D_WIDTH, D_LSB, d);
        word = Bitpack_newu(word, I_PB_WIDTH, I_PB_LSB, index_pb);
        word = Bitpack_newu(word, I_PR_WIDTH, I_PR_LSB, index_pr);

        return word;
}

unsigned average_pb(float pb_1, float pb_2, float pb_3, float pb_4)
{
        float average = (pb_1 + pb_2 + pb_3 + pb_4) / 4;
        return Arith40_index_of_chroma(average);
}

unsigned average_pr(float pr_1, float pr_2, float pr_3, float pr_4)
{
        float average = (pr_1 + pr_2 + pr_3 + pr_4) / 4;
        return Arith40_index_of_chroma(average);
}

unsigned calculate_a(float y1, float y2, float y3, float y4)
{
        float a = (y1 + y2 + y3 + y4)/4.0;
        return round(a * 63.0);
}

signed calculate_b(float y1, float y2, float y3, float y4)
{
        float b = (y4 + y3 - y2 - y1)/4.0;

        if (b < -0.3)
                return -31;
        else if (b > 0.3)
                return 31;
        return b * 31 / 0.3;
}

signed calculate_c(float y1, float y2, float y3, float y4)
{
        float c = (y4 - y3 + y2 - y1)/4.0;

        if (c < -0.3)
                return -31;
        else if (c > 0.3)
                return 31;
        return c * 31 / 0.3;
}

signed calculate_d(float y1, float y2, float y3, float y4)
{
        float d = (y4 - y3 - y2 + y1)/4.0;
        
        if (d < -0.3)
                return -31;
        else if (d > 0.3)
                return 31;
        return d * 31 / 0.3;
}

Bit_word_matrix compress(Pnm_compressed img)
{
        Bit_word_matrix matrix = malloc(sizeof(struct Bit_word_matrix));
        A2Methods_T matrix_methods = uarray2_methods_plain;

        matrix->methods = matrix_methods;
        matrix->width = img->width / 2;
        matrix->height = img->height / 2;

        int word_size = 32;
        matrix->word_array = matrix_methods->new(matrix->width, matrix->height,
                word_size);
        assert(matrix->word_array);

        img->methods->map_default(img->pixels, apply_compress, matrix);

        return matrix;
}

void apply_compress(int i, int j, A2Methods_UArray2 pixels,
                        void *elem, void* cl)
{
        A2Methods_T methods = uarray2_methods_blocked;
        Bit_word_matrix matrix = cl;
        (void) elem;
        if (i % BLKSIZE != 0 || j % BLKSIZE != 0)
                return;

        A2Methods_Object *pixel_1, *pixel_2, *pixel_3, *pixel_4;

        pixel_1 = methods->at(pixels, i, j);
        pixel_2 = methods->at(pixels, i+1, j);
        pixel_3 = methods->at(pixels, i, j+1);
        pixel_4 = methods->at(pixels, i+1, j+1);

        unsigned index_pb = average_pb(((Pnm_ypbpr)pixel_1)->pb, 
                        ((Pnm_ypbpr)pixel_2)->pb, ((Pnm_ypbpr)pixel_3)->pb,
                        ((Pnm_ypbpr)pixel_4)->pb);

        unsigned index_pr = average_pr(((Pnm_ypbpr)pixel_1)->pr, 
                        ((Pnm_ypbpr)pixel_2)->pr, ((Pnm_ypbpr)pixel_3)->pr,
                        ((Pnm_ypbpr)pixel_4)->pr);
        
        unsigned a = calculate_a(((Pnm_ypbpr)pixel_1)->y, 
                        ((Pnm_ypbpr)pixel_2)->y, ((Pnm_ypbpr)pixel_3)->y,
                        ((Pnm_ypbpr)pixel_4)->y);
        signed b = calculate_b(((Pnm_ypbpr)pixel_1)->y, 
                        ((Pnm_ypbpr)pixel_2)->y, ((Pnm_ypbpr)pixel_3)->y,
                        ((Pnm_ypbpr)pixel_4)->y);
        signed c = calculate_c(((Pnm_ypbpr)pixel_1)->y, 
                        ((Pnm_ypbpr)pixel_2)->y, ((Pnm_ypbpr)pixel_3)->y,
                        ((Pnm_ypbpr)pixel_4)->y);
        signed d = calculate_d(((Pnm_ypbpr)pixel_1)->y, 
                        ((Pnm_ypbpr)pixel_2)->y, ((Pnm_ypbpr)pixel_3)->y,
                        ((Pnm_ypbpr)pixel_4)->y);

        uint64_t word = word_pack(a, b, c, d, index_pb, index_pr);

        A2Methods_Object *curr_pixel;
        curr_pixel = matrix->methods->at(matrix->word_array, i/2, j/2);
        *(uint64_t*)curr_pixel = word;
}

/***********************************************
*       Decompress40 functions                 *
************************************************/

Pnm_compressed unpack(Bit_word_matrix matrix)
{
        Pnm_compressed ypbpr = malloc(sizeof(struct Pnm_compressed));
        A2Methods_T methods = uarray2_methods_blocked;
        ypbpr->methods = methods;
        ypbpr->width = matrix->width * 2;
        ypbpr->height = matrix->height * 2;

        ypbpr->pixels = methods->new_with_blocksize(ypbpr->width, 
                ypbpr->height, sizeof(struct Pnm_ypbpr), BLKSIZE);

        assert(ypbpr->pixels);
        matrix->methods->map_row_major(matrix->word_array,
                apply_unpack, ypbpr);

        return ypbpr;
}

void apply_unpack(int i, int j, A2Methods_UArray2 word_array,
                        void *elem, void* cl)
{       
        Pnm_compressed ypbpr = cl;

        (void) word_array;
        uint64_t word = *(uint64_t*) elem;

        float pb = chroma_pb(word);
        float pr = chroma_pr(word);
        float a = calculate_local_a(word);
        float b = calculate_local_b(word);
        float c = calculate_local_c(word);
        float d = calculate_local_d(word);

        float y1 = calculate_y1(a, b, c, d);
        float y2 = calculate_y2(a, b, c, d);
        float y3 = calculate_y3(a, b, c, d);
        float y4 = calculate_y4(a, b, c, d);

        A2Methods_Object *pixel_1, *pixel_2, *pixel_3, *pixel_4;
        pixel_1 = ypbpr->methods->at(ypbpr->pixels, i * 2, j * 2);
        pixel_2 = ypbpr->methods->at(ypbpr->pixels, i * 2 + 1, j * 2);
        pixel_3 = ypbpr->methods->at(ypbpr->pixels, i * 2, j * 2 + 1);
        pixel_4 = ypbpr->methods->at(ypbpr->pixels, i * 2 + 1, j * 2 + 1);

        ((Pnm_ypbpr)pixel_1)->y = y1;
        ((Pnm_ypbpr)pixel_2)->y = y2;
        ((Pnm_ypbpr)pixel_3)->y = y3;
        ((Pnm_ypbpr)pixel_4)->y = y4;

        ((Pnm_ypbpr)pixel_1)->pb = pb;
        ((Pnm_ypbpr)pixel_1)->pr = pr;
        ((Pnm_ypbpr)pixel_2)->pb = pb;
        ((Pnm_ypbpr)pixel_2)->pr = pr;
        ((Pnm_ypbpr)pixel_3)->pb = pb;
        ((Pnm_ypbpr)pixel_3)->pr = pr;
        ((Pnm_ypbpr)pixel_4)->pb = pb;
        ((Pnm_ypbpr)pixel_4)->pr = pr;
}

void convert_to_original(Pnm_compressed orig, Pnm_ppm decompressed)
{
        decompressed->width = orig->width;
        decompressed->height = orig->height;
        decompressed->denominator = 255;
        decompressed->methods = uarray2_methods_plain;

        unsigned w = decompressed->width;
        unsigned h = decompressed->height;

        decompressed->pixels = decompressed->methods->new(w, h, 
                                sizeof(struct Pnm_rgb));

        orig->methods->map_default(orig->pixels, 
                convert_ypbpr_to_rgb, &decompressed);
}


float chroma_pb(uint64_t word)
{
        unsigned index_pb = Bitpack_getu(word, I_PB_WIDTH, I_PB_LSB);
        return Arith40_chroma_of_index(index_pb);
}

float chroma_pr(uint64_t word)
{
        unsigned index_pr = Bitpack_getu(word, I_PR_WIDTH, I_PR_LSB);
        return Arith40_chroma_of_index(index_pr);
}

float calculate_local_a(uint64_t word)
{
        unsigned local_a = Bitpack_getu(word, A_WIDTH, A_LSB);
        return local_a / 63.0;
}

float calculate_local_b(uint64_t word)
{
        signed local_b = Bitpack_gets(word, B_WIDTH, B_LSB);
        return local_b / (31 / 0.3);
}

float calculate_local_c(uint64_t word)
{
        signed local_c = Bitpack_gets(word, C_WIDTH, C_LSB);
        return local_c / (31 / 0.3);
}

float calculate_local_d(uint64_t word)
{
        signed local_d = Bitpack_gets(word, D_WIDTH, D_LSB);
        return local_d / (31 / 0.3);
}

float calculate_y1(float a, float b, float c, float d)
{
        return (a - b - c + d);
}

float calculate_y2(float a, float b, float c, float d)
{
        return (a - b + c - d);
}

float calculate_y3(float a, float b, float c, float d)
{
        return (a + b - c - d);
}

float calculate_y4(float a, float b, float c, float d)
{
        return (a + b + c + d);
}