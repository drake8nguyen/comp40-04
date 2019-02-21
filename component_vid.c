/* Comp 40 Homework 4 - Arith
component_vid.c
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
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "component_vid.h"

const float Y_THRESHOLD_LOW = 0;
const float PB_THRESHOLD_LOW = -0.5;
const float PR_THRESHOLD_LOW = -0.5;

const float Y_THRESHOLD_HIGH = 1;
const float PB_THRESHOLD_HIGH = 0.5;
const float PR_THRESHOLD_HIGH = 0.5;

const float RGB_THRESHOLD_LOW = 0;
const float RGB_THRESHOLD_HIGH = 1;

const unsigned DENOMINATOR = 255;

/***********************************************
*       Compress40 functions                   *
************************************************/
void trim_and_convert_original(Pnm_ppm *orig, Pnm_compressed *trim)
{
        Pnm_ppm orig_img = *orig;
        Pnm_compressed trim_img = *trim;

        trim_img->methods = orig_img->methods;
        trim_img->denominator = orig_img->denominator;
        
        if (orig_img->width % 2 == 1) 
                trim_img->width = orig_img->width - 1;
        else 
                trim_img->width = orig_img->width;
        if (orig_img->height % 2 == 1)
                trim_img->height = orig_img->height - 1;
        else
                trim_img->height = orig_img->height;

        unsigned w = trim_img->width;
        unsigned h = trim_img->height;

        trim_img->pixels = trim_img->methods->new_with_blocksize(w, h,
                sizeof(struct Pnm_ypbpr), 2);
        trim_img->methods->map_default(orig_img->pixels,
                convert_rgb_to_ypbpr, trim);

        Pnm_ppmfree(orig);
}

void convert_rgb_to_ypbpr(int i, int j, A2Methods_UArray2 orig_pixels,
                        void *elem, void* cl)
{
        (void) orig_pixels;
        Pnm_compressed *trim_img = cl;
        unsigned width = (*trim_img)->width;
        unsigned height = (*trim_img)->height;

        unsigned denominator = (*trim_img)->denominator;

        if (i >= (int)width || j >= (int)height)
                return;

        A2Methods_Object *curr_pixel;
        curr_pixel = (*trim_img)->methods->at((*trim_img)->pixels, i, j);
        
        Pnm_ypbpr convert = rgb_to_ypbpr_formulae((Pnm_rgb)elem, denominator);

        *((Pnm_ypbpr) curr_pixel) = *convert;
        free(convert);
}

Pnm_ypbpr rgb_to_ypbpr_formulae(Pnm_rgb rgb, unsigned denominator) 
{
        Pnm_ypbpr ypbpr = malloc(sizeof(struct Pnm_ypbpr));
        assert (ypbpr);
        float r = rgb->red / (float) denominator;
        float g = rgb->green / (float) denominator;
        float b = rgb->blue / (float) denominator;

        ypbpr->y = 0.299 * r + 0.587 * g + 0.114 * b;
        ypbpr->pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        ypbpr->pr = 0.5 * r - 0.418688 * g - 0.081312 * b;

        
        ypbpr->y = check_bound(ypbpr->y, Y_THRESHOLD_LOW, Y_THRESHOLD_HIGH);
        ypbpr->pb = check_bound(ypbpr->pb, PB_THRESHOLD_LOW, PB_THRESHOLD_HIGH);
        ypbpr->pr = check_bound(ypbpr->pr, PR_THRESHOLD_LOW, PR_THRESHOLD_HIGH);
        return ypbpr;
}

/***********************************************
*       Decompress40 functions                 *
************************************************/
void convert_ypbpr_to_rgb(int i, int j, A2Methods_UArray2 orig_pixels,
                        void *elem, void* cl)
{
        (void) orig_pixels;
        Pnm_ppm *rgb = cl;
        A2Methods_Object *curr_pixel;
        curr_pixel = (*rgb)->methods->at((*rgb)->pixels, i, j);

        Pnm_rgb convert = ypbpr_to_rgb_formulae((Pnm_ypbpr)elem);   

        *((Pnm_rgb) curr_pixel) = *convert;
        free(convert);
}

Pnm_rgb ypbpr_to_rgb_formulae(Pnm_ypbpr ypbpr) 
{
        Pnm_rgb rgb = malloc(sizeof(struct Pnm_rgb));
        assert (rgb);
        
        float y = ypbpr->y;
        float pb = ypbpr->pb;
        float pr = ypbpr->pr;

        float r = 1.0 * y + 0.0 * pb + 1.402 * pr;
        float g = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
        float b = 1.0 * y + 1.772 * pb + 0.0 * pr;

        r = check_bound(r, RGB_THRESHOLD_LOW, RGB_THRESHOLD_HIGH);
        g = check_bound(g, RGB_THRESHOLD_LOW, RGB_THRESHOLD_HIGH);
        b = check_bound(b, RGB_THRESHOLD_LOW, RGB_THRESHOLD_HIGH); 

        rgb->red = round(r * DENOMINATOR);
        rgb->green = round(g * DENOMINATOR);
        rgb->blue = round(b * DENOMINATOR);

        return rgb;
}

float check_bound(float f, float low, float high)
{
        if (f < low)
                return low;
        else if (f > high)
                return high;
        return f;
}