/* Comp 40 Homework 4 - Arith
compress40.c
Minh-Duc Nguyen
Trung Truong
October 21, 2018
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assert.h"
#include <math.h>
#include "compress40.h"
#include "bitpack.h"
#include "arith40.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "component_vid.h"
#include "dct_quantize.h"


/* Functions Prototypes */
extern void compress40(FILE *);
extern void decompress40(FILE *);

/* Compress40 Prototypes */
void print_compressed_image(Bit_word_matrix, int, int);
void apply_print(int, int, A2Methods_UArray2, void *, void *);

/* Decompress40 Prototypes */
Bit_word_matrix read_header(FILE*);
void print_decompressed_image(Pnm_ppm);


/***********************************************
*       Compress40 functions                   *
************************************************/
extern void compress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_blocked; 
        assert(methods);

        Pnm_ppm image = Pnm_ppmread(input, methods);

        int w = image->width;
        int h = image->height;
        Pnm_compressed new_img = malloc(sizeof(struct Pnm_compressed));
        assert(new_img);

        trim_and_convert_original(&image, &new_img);

        Bit_word_matrix word_matrix = compress(new_img);

        print_compressed_image(word_matrix, w, h);

        word_matrix->methods->free(&(word_matrix->word_array));
        free(word_matrix);

        new_img->methods->free(&(new_img->pixels));
        free(new_img);
}

void print_compressed_image(Bit_word_matrix m, int w, int h)
{
        printf("COMP40 Compressed image format 2\n%u %u", w, h);
        printf("\n");
        m->methods->map_row_major(m->word_array, apply_print, m);
}

void apply_print(int i, int j, A2Methods_UArray2 word_array,
                        void *elem, void* cl)
{
        (void) cl;
        (void) i;
        (void) j;
        (void) word_array;

        uint64_t current_word = *((uint64_t*)elem);

        putchar(Bitpack_getu(current_word, 8, 24));
        putchar(Bitpack_getu(current_word, 8, 16));
        putchar(Bitpack_getu(current_word, 8, 8));
        putchar(Bitpack_getu(current_word, 8, 0));
}

/***********************************************
*       Decompress40 functions                 *
************************************************/
extern void decompress40(FILE *input)
{
        Pnm_ppm decompressed_image = malloc(sizeof(struct Pnm_ppm));
        
        Bit_word_matrix word_matrix = read_header(input);
        Pnm_compressed ypbpr = unpack(word_matrix);
        convert_to_original(ypbpr, decompressed_image);

        print_decompressed_image(decompressed_image);

        word_matrix->methods->free(&(word_matrix->word_array));
        free(word_matrix);
        ypbpr->methods->free(&(ypbpr->pixels));
        free(ypbpr);
}

Bit_word_matrix read_header(FILE* input)
{
        unsigned width, height;
        int read = fscanf(input, 
                "COMP40 Compressed image format 2\n%u %u", &width, &height);

        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        A2Methods_T methods = uarray2_methods_plain; 

        Bit_word_matrix word_matrix = malloc(sizeof(struct Bit_word_matrix));
        assert(word_matrix);
        
        if (width % 2 == 1) 
                width = (width - 1) / 2;
        else 
                width = width / 2;

        if (height % 2 == 1) 
                height = (height - 1) / 2;
        else 
                height = height / 2;

        word_matrix->width = width;
        word_matrix->height = height;
        word_matrix->methods = methods;  

        int word_size = 32;
        A2Methods_UArray2 array = methods->new(width, height, word_size);
        assert(array);      
        word_matrix->word_array = array; 

        for (unsigned j = 0; j < height; j++) {
                for (unsigned i = 0; i < width; i++) {
                        A2Methods_Object *current;
                        current = methods->at(array, i, j);
                        uint64_t word = 0;

                        for (int order = 3; order >= 0; order--) {
                                uint64_t byte = fgetc(input);
                                assert(!feof(input));
                                word = Bitpack_newu(word, 8, order * 8,
                                        (uint64_t)byte);
                        }

                        *(uint64_t*)current = word;  

                }
        }
        return word_matrix;
}

void print_decompressed_image(Pnm_ppm decompressed)
{
        Pnm_ppmwrite(stdout, decompressed);
        Pnm_ppmfree(&decompressed);
}
