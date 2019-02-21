/* Comp 40 Homework 4 - Arith
bitpack.c
Minh-Duc Nguyen
Trung Truong
October 21, 2018
*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "except.h"
#include <bitpack.h>
#include "assert.h"

extern Except_T Bitpack_Overflow;

uint64_t shift_uint64_left(uint64_t n, unsigned distance)
{
        assert (distance < sizeof(int64_t) * 8);
        return n << distance;
}
int64_t shift_int64_left(int64_t n, unsigned distance)
{
        assert (distance < sizeof(int64_t) * 8);
        return n << distance;
}

int64_t shift_int64_right(int64_t n, unsigned distance)
{
        assert (distance < sizeof(int64_t) * 8);
        return n >> distance;
}

uint64_t shift_uint64_right(uint64_t n, unsigned distance)
{       
        assert (distance < sizeof(uint64_t) * 8);
        return n >> distance;           
}

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        if (width == 0) {
                if (n != 0)
                        return false;
                else
                        return true;
        }
        assert(width > 0);
        return (n < (uint64_t)1 << width);  
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
        // assert(width > 0);
        if (width == 0) {
                if (n != 0)
                        return false;
                else
                        return true;
        }
        int64_t temp = ((int64_t)1 << (width - 1)) - 1;
        return (n <= temp && n >= ~temp);
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert (width <= 64);
        assert (width + lsb <= 64);
        uint64_t mask = ~0;

        if (width == 0)
                return 0;

        mask = shift_uint64_right(mask, sizeof(uint64_t) * 8 - width);
        mask = shift_uint64_left(mask, lsb);
        return (shift_uint64_right((word & mask), lsb));
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert (width <= 64);
        assert (width + lsb <= 64);
        uint64_t mask = ~0;

        if (width == 0)
                return 0;

        mask = shift_uint64_right(mask, sizeof(uint64_t) * 8 - width);
        mask = shift_uint64_left(mask, lsb);

        int64_t output = shift_int64_right((word & mask), lsb);
        output = shift_int64_left(output, sizeof(uint64_t) * 8 - width);
        output = shift_int64_right(output, sizeof(uint64_t) * 8 - width);

        return output;
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                                          uint64_t value)
{
        assert (width <= 64 && width > 0);
        assert (width + lsb <= 64);

        Except_T Bitpack_Overflow = { "Overflow packing bits" };
        
        if (!Bitpack_fitsu(value, width))
                RAISE(Bitpack_Overflow);

        uint64_t mask_temp = ~0;

        mask_temp = shift_uint64_right(mask_temp, 
                                       sizeof(uint64_t) * 8 - width);
        mask_temp = shift_uint64_left(mask_temp, lsb);

        uint64_t mask_word = ~mask_temp;

        word &= mask_word;
        value = shift_uint64_left(value, lsb);

        return word | value;
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                                          int64_t value)
{
        assert (width <= 64 && width > 0);
        assert (width + lsb <= 64);

        Except_T Bitpack_Overflow = { "Overflow packing bits" };
        if (!Bitpack_fitss(value, width))
                RAISE(Bitpack_Overflow);

        uint64_t mask_temp = ~0;

        mask_temp = shift_uint64_right(mask_temp, 
                                       sizeof(uint64_t) * 8 - width);

        uint64_t mask_val = mask_temp;
        mask_temp = shift_uint64_left(mask_temp, lsb);

        uint64_t mask_word = ~mask_temp;

        word &= mask_word;
        value &= mask_val;

        value = shift_uint64_left(value, lsb);

        return word | value;
}