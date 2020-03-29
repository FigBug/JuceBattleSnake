#pragma once

#include <JuceHeader.h>

#include "MainComponent.h"

static unsigned short const stackblur_mul[255] =
{
    512,512,456,512,328,456,335,512,405,328,271,456,388,335,292,512,
    454,405,364,328,298,271,496,456,420,388,360,335,312,292,273,512,
    482,454,428,405,383,364,345,328,312,298,284,271,259,496,475,456,
    437,420,404,388,374,360,347,335,323,312,302,292,282,273,265,512,
    497,482,468,454,441,428,417,405,394,383,373,364,354,345,337,328,
    320,312,305,298,291,284,278,271,265,259,507,496,485,475,465,456,
    446,437,428,420,412,404,396,388,381,374,367,360,354,347,341,335,
    329,323,318,312,307,302,297,292,287,282,278,273,269,265,261,512,
    505,497,489,482,475,468,461,454,447,441,435,428,422,417,411,405,
    399,394,389,383,378,373,368,364,359,354,350,345,341,337,332,328,
    324,320,316,312,309,305,301,298,294,291,287,284,281,278,274,271,
    268,265,262,259,257,507,501,496,491,485,480,475,470,465,460,456,
    451,446,442,437,433,428,424,420,416,412,408,404,400,396,392,388,
    385,381,377,374,370,367,363,360,357,354,350,347,344,341,338,335,
    332,329,326,323,320,318,315,312,310,307,304,302,299,297,294,292,
    289,287,285,282,280,278,275,273,271,269,267,265,263,261,259
};

static unsigned char const stackblur_shr[255] =
{
    9, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
    17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
};

static void applyStackBlurBW (Image& img, unsigned int radius)
{
    const unsigned int w = (unsigned int)img.getWidth();
    const unsigned int h = (unsigned int)img.getHeight();

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    radius = jlimit (2u, 254u, radius);

    unsigned char    stack[(254 * 2 + 1) * 1];

    unsigned int x, y, xp, yp, i, sp, stack_start;

    unsigned char* stack_ptr = nullptr;
    unsigned char* src_ptr = nullptr;
    unsigned char* dst_ptr = nullptr;

    unsigned long sum, sum_in, sum_out;

    unsigned int wm = w - 1;
    unsigned int hm = h - 1;
    unsigned int w1 = (unsigned int) data.lineStride;
    unsigned int div = (unsigned int) (radius * 2) + 1;
    unsigned int mul_sum = stackblur_mul[radius];
    unsigned char shr_sum = stackblur_shr[radius];

    for (y = 0; y < h; ++y)
    {
        sum = sum_in = sum_out = 0;

        src_ptr = data.getLinePointer (int (y));

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[i];
            stack_ptr[0] = src_ptr[0];
            sum += src_ptr[0] * (i + 1);
            sum_out += src_ptr[0];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= wm)
                src_ptr += 1;

            stack_ptr = &stack[1 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            sum += src_ptr[0] * (radius + 1 - i);
            sum_in += src_ptr[0];
        }

        sp = radius;
        xp = radius;
        if (xp > wm)
            xp = wm;

        src_ptr = data.getLinePointer (int (y)) + (unsigned int)data.pixelStride * xp;
        dst_ptr = data.getLinePointer (int (y));

        for (x = 0; x < w; ++x)
        {
            dst_ptr[0] = (unsigned char)((sum * mul_sum) >> shr_sum);
            dst_ptr += 1;

            sum -= sum_out;

            stack_start = sp + div - radius;

            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[1 * stack_start];

            sum_out -= stack_ptr[0];

            if (xp < wm)
            {
                src_ptr += 1;
                ++xp;
            }

            stack_ptr[0] = src_ptr[0];

            sum_in += src_ptr[0];
            sum    += sum_in;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp*1];

            sum_out += stack_ptr[0];
            sum_in  -= stack_ptr[0];
        }
    }

    for (x = 0; x < w; ++x)
    {
        sum = sum_in = sum_out = 0;

        src_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[i * 1];
            stack_ptr[0] = src_ptr[0];
            sum           += src_ptr[0] * (i + 1);
            sum_out       += src_ptr[0];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= hm)
                src_ptr += w1;

            stack_ptr = &stack[1 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            sum += src_ptr[0] * (radius + 1 - i);
            sum_in += src_ptr[0];
        }

        sp = radius;
        yp = radius;
        if (yp > hm)
            yp = hm;

        src_ptr = data.getLinePointer (int (yp)) + (unsigned int)data.pixelStride * x;
        dst_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (y = 0; y < h; ++y)
        {
            dst_ptr[0] = (unsigned char)((sum * mul_sum) >> shr_sum);
            dst_ptr += w1;

            sum -= sum_out;

            stack_start = sp + div - radius;
            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[1 * stack_start];

            sum_out -= stack_ptr[0];

            if (yp < hm)
            {
                src_ptr += w1;
                ++yp;
            }

            stack_ptr[0] = src_ptr[0];

            sum_in += src_ptr[0];
            sum    += sum_in;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp * 1];

            sum_out += stack_ptr[0];
            sum_in  -= stack_ptr[0];
        }
    }
}

static void applyStackBlurRGB (Image& img, unsigned int radius)
{
    const unsigned int w = (unsigned int)img.getWidth();
    const unsigned int h = (unsigned int)img.getHeight();

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    radius = jlimit (2u, 254u, radius);

    unsigned char    stack[(254 * 2 + 1) * 3];

    unsigned int x, y, xp, yp, i, sp, stack_start;

    unsigned char* stack_ptr = nullptr;
    unsigned char* src_ptr = nullptr;
    unsigned char* dst_ptr = nullptr;

    unsigned long sum_r, sum_g, sum_b, sum_in_r, sum_in_g, sum_in_b,
                  sum_out_r, sum_out_g, sum_out_b;

    unsigned int wm = w - 1;
    unsigned int hm = h - 1;
    unsigned int w3 = (unsigned int) data.lineStride;
    unsigned int div = (unsigned int)(radius * 2) + 1;
    unsigned int mul_sum = stackblur_mul[radius];
    unsigned char shr_sum = stackblur_shr[radius];

    for (y = 0; y < h; ++y)
    {
        sum_r = sum_g = sum_b =
        sum_in_r = sum_in_g = sum_in_b =
        sum_out_r = sum_out_g = sum_out_b = 0;

        src_ptr = data.getLinePointer (int (y));

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[3 * i];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            sum_r += src_ptr[0] * (i + 1);
            sum_g += src_ptr[1] * (i + 1);
            sum_b += src_ptr[2] * (i + 1);
            sum_out_r += src_ptr[0];
            sum_out_g += src_ptr[1];
            sum_out_b += src_ptr[2];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= wm)
                src_ptr += 3;

            stack_ptr = &stack[3 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            sum_r += src_ptr[0] * (radius + 1 - i);
            sum_g += src_ptr[1] * (radius + 1 - i);
            sum_b += src_ptr[2] * (radius + 1 - i);
            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
        }

        sp = radius;
        xp = radius;
        if (xp > wm)
            xp = wm;

        src_ptr = data.getLinePointer (int (y)) + (unsigned int)data.pixelStride * xp;
        dst_ptr = data.getLinePointer (int (y));

        for (x = 0; x < w; ++x)
        {
            dst_ptr[0] = (unsigned char)((sum_r * mul_sum) >> shr_sum);
            dst_ptr[1] = (unsigned char)((sum_g * mul_sum) >> shr_sum);
            dst_ptr[2] = (unsigned char)((sum_b * mul_sum) >> shr_sum);
            dst_ptr += 3;

            sum_r -= sum_out_r;
            sum_g -= sum_out_g;
            sum_b -= sum_out_b;

            stack_start = sp + div - radius;

            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[3 * stack_start];

            sum_out_r -= stack_ptr[0];
            sum_out_g -= stack_ptr[1];
            sum_out_b -= stack_ptr[2];

            if (xp < wm)
            {
                src_ptr += 3;
                ++xp;
            }

            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];

            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_r    += sum_in_r;
            sum_g    += sum_in_g;
            sum_b    += sum_in_b;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp*3];

            sum_out_r += stack_ptr[0];
            sum_out_g += stack_ptr[1];
            sum_out_b += stack_ptr[2];
            sum_in_r  -= stack_ptr[0];
            sum_in_g  -= stack_ptr[1];
            sum_in_b  -= stack_ptr[2];
        }
    }

    for (x = 0; x < w; ++x)
    {
        sum_r =    sum_g =    sum_b =
        sum_in_r = sum_in_g = sum_in_b =
        sum_out_r = sum_out_g = sum_out_b = 0;

        src_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[i * 3];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            sum_r           += src_ptr[0] * (i + 1);
            sum_g           += src_ptr[1] * (i + 1);
            sum_b           += src_ptr[2] * (i + 1);
            sum_out_r       += src_ptr[0];
            sum_out_g       += src_ptr[1];
            sum_out_b       += src_ptr[2];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= hm)
                src_ptr += w3;

            stack_ptr = &stack[3 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            sum_r += src_ptr[0] * (radius + 1 - i);
            sum_g += src_ptr[1] * (radius + 1 - i);
            sum_b += src_ptr[2] * (radius + 1 - i);
            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
        }

        sp = radius;
        yp = radius;
        if (yp > hm)
            yp = hm;

        src_ptr = data.getLinePointer (int (yp)) + (unsigned int)data.pixelStride * x;
        dst_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (y = 0; y < h; ++y)
        {
            dst_ptr[0] = (unsigned char)((sum_r * mul_sum) >> shr_sum);
            dst_ptr[1] = (unsigned char)((sum_g * mul_sum) >> shr_sum);
            dst_ptr[2] = (unsigned char)((sum_b * mul_sum) >> shr_sum);
            dst_ptr += w3;

            sum_r -= sum_out_r;
            sum_g -= sum_out_g;
            sum_b -= sum_out_b;

            stack_start = sp + div - radius;
            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[3 * stack_start];

            sum_out_r -= stack_ptr[0];
            sum_out_g -= stack_ptr[1];
            sum_out_b -= stack_ptr[2];

            if (yp < hm)
            {
                src_ptr += w3;
                ++yp;
            }

            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];

            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_r    += sum_in_r;
            sum_g    += sum_in_g;
            sum_b    += sum_in_b;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp * 3];

            sum_out_r += stack_ptr[0];
            sum_out_g += stack_ptr[1];
            sum_out_b += stack_ptr[2];
            sum_in_r  -= stack_ptr[0];
            sum_in_g  -= stack_ptr[1];
            sum_in_b  -= stack_ptr[2];
        }
    }
}

static void applyStackBlurARGB (Image& img, unsigned int radius)
{
    const unsigned int w = (unsigned int)img.getWidth();
    const unsigned int h = (unsigned int)img.getHeight();

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    radius = jlimit (2u, 254u, radius);

    unsigned char    stack[(254 * 2 + 1) * 4];

    unsigned int x, y, xp, yp, i, sp, stack_start;

    unsigned char* stack_ptr = nullptr;
    unsigned char* src_ptr = nullptr;
    unsigned char* dst_ptr = nullptr;

    unsigned long sum_r, sum_g, sum_b, sum_a, sum_in_r, sum_in_g, sum_in_b, sum_in_a,
    sum_out_r, sum_out_g, sum_out_b, sum_out_a;

    unsigned int wm = w - 1;
    unsigned int hm = h - 1;
    unsigned int w4 = (unsigned int) data.lineStride;
    unsigned int div = (unsigned int)(radius * 2) + 1;
    unsigned int mul_sum = stackblur_mul[radius];
    unsigned char shr_sum = stackblur_shr[radius];

    for (y = 0; y < h; ++y)
    {
        sum_r = sum_g = sum_b = sum_a =
        sum_in_r = sum_in_g = sum_in_b = sum_in_a =
        sum_out_r = sum_out_g = sum_out_b = sum_out_a = 0;

        src_ptr = data.getLinePointer (int (y));

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[4 * i];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];
            sum_r += src_ptr[0] * (i + 1);
            sum_g += src_ptr[1] * (i + 1);
            sum_b += src_ptr[2] * (i + 1);
            sum_a += src_ptr[3] * (i + 1);
            sum_out_r += src_ptr[0];
            sum_out_g += src_ptr[1];
            sum_out_b += src_ptr[2];
            sum_out_a += src_ptr[3];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= wm)
                src_ptr += 4;

            stack_ptr = &stack[4 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];
            sum_r += src_ptr[0] * (radius + 1 - i);
            sum_g += src_ptr[1] * (radius + 1 - i);
            sum_b += src_ptr[2] * (radius + 1 - i);
            sum_a += src_ptr[3] * (radius + 1 - i);
            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_in_a += src_ptr[3];
        }

        sp = radius;
        xp = radius;
        if (xp > wm)
            xp = wm;

        src_ptr = data.getLinePointer (int (y)) + (unsigned int)data.pixelStride * xp;
        dst_ptr = data.getLinePointer (int (y));

        for (x = 0; x < w; ++x)
        {
            dst_ptr[0] = (unsigned char)((sum_r * mul_sum) >> shr_sum);
            dst_ptr[1] = (unsigned char)((sum_g * mul_sum) >> shr_sum);
            dst_ptr[2] = (unsigned char)((sum_b * mul_sum) >> shr_sum);
            dst_ptr[3] = (unsigned char)((sum_a * mul_sum) >> shr_sum);
            dst_ptr += 4;

            sum_r -= sum_out_r;
            sum_g -= sum_out_g;
            sum_b -= sum_out_b;
            sum_a -= sum_out_a;

            stack_start = sp + div - radius;

            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[4 * stack_start];

            sum_out_r -= stack_ptr[0];
            sum_out_g -= stack_ptr[1];
            sum_out_b -= stack_ptr[2];
            sum_out_a -= stack_ptr[3];

            if (xp < wm)
            {
                src_ptr += 4;
                ++xp;
            }

            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];

            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_in_a += src_ptr[3];
            sum_r    += sum_in_r;
            sum_g    += sum_in_g;
            sum_b    += sum_in_b;
            sum_a    += sum_in_a;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp*4];

            sum_out_r += stack_ptr[0];
            sum_out_g += stack_ptr[1];
            sum_out_b += stack_ptr[2];
            sum_out_a += stack_ptr[3];
            sum_in_r  -= stack_ptr[0];
            sum_in_g  -= stack_ptr[1];
            sum_in_b  -= stack_ptr[2];
            sum_in_a  -= stack_ptr[3];
        }
    }

    for (x = 0; x < w; ++x)
    {
        sum_r =    sum_g =    sum_b =    sum_a =
        sum_in_r = sum_in_g = sum_in_b = sum_in_a =
        sum_out_r = sum_out_g = sum_out_b = sum_out_a = 0;

        src_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[i * 4];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];
            sum_r           += src_ptr[0] * (i + 1);
            sum_g           += src_ptr[1] * (i + 1);
            sum_b           += src_ptr[2] * (i + 1);
            sum_a           += src_ptr[3] * (i + 1);
            sum_out_r       += src_ptr[0];
            sum_out_g       += src_ptr[1];
            sum_out_b       += src_ptr[2];
            sum_out_a       += src_ptr[3];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= hm)
                src_ptr += w4;

            stack_ptr = &stack[4 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];
            sum_r += src_ptr[0] * (radius + 1 - i);
            sum_g += src_ptr[1] * (radius + 1 - i);
            sum_b += src_ptr[2] * (radius + 1 - i);
            sum_a += src_ptr[3] * (radius + 1 - i);
            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_in_a += src_ptr[3];
        }

        sp = radius;
        yp = radius;
        if (yp > hm)
            yp = hm;

        src_ptr = data.getLinePointer (int (yp)) + (unsigned int)data.pixelStride * x;
        dst_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (y = 0; y < h; ++y)
        {
            dst_ptr[0] = (unsigned char)((sum_r * mul_sum) >> shr_sum);
            dst_ptr[1] = (unsigned char)((sum_g * mul_sum) >> shr_sum);
            dst_ptr[2] = (unsigned char)((sum_b * mul_sum) >> shr_sum);
            dst_ptr[3] = (unsigned char)((sum_a * mul_sum) >> shr_sum);
            dst_ptr += w4;

            sum_r -= sum_out_r;
            sum_g -= sum_out_g;
            sum_b -= sum_out_b;
            sum_a -= sum_out_a;

            stack_start = sp + div - radius;
            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[4 * stack_start];

            sum_out_r -= stack_ptr[0];
            sum_out_g -= stack_ptr[1];
            sum_out_b -= stack_ptr[2];
            sum_out_a -= stack_ptr[3];

            if (yp < hm)
            {
                src_ptr += w4;
                ++yp;
            }

            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];

            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_in_a += src_ptr[3];
            sum_r    += sum_in_r;
            sum_g    += sum_in_g;
            sum_b    += sum_in_b;
            sum_a    += sum_in_a;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp * 4];

            sum_out_r += stack_ptr[0];
            sum_out_g += stack_ptr[1];
            sum_out_b += stack_ptr[2];
            sum_out_a += stack_ptr[3];
            sum_in_r  -= stack_ptr[0];
            sum_in_g  -= stack_ptr[1];
            sum_in_b  -= stack_ptr[2];
            sum_in_a  -= stack_ptr[3];
        }
    }
}

// The Stack Blur Algorithm was invented by Mario Klingemann,
// mario@quasimondo.com and described here:
// http://incubator.quasimondo.com/processing/fast_blur_deluxe.php

// Stackblur algorithm by Mario Klingemann
// Details here:
// http://www.quasimondo.com/StackBlurForCanvas/StackBlurDemo.html
// C++ implemenation base from:
// https://gist.github.com/benjamin9999/3809142
// http://www.antigrain.com/__code/include/agg_blur.h.html
void applyStackBlur (Image& img, int radius)
{
    if (img.getFormat() == Image::ARGB)          applyStackBlurARGB (img, (unsigned int)radius);
    if (img.getFormat() == Image::RGB)           applyStackBlurRGB (img, (unsigned int)radius);
    if (img.getFormat() == Image::SingleChannel) applyStackBlurBW (img, (unsigned int)radius);
}

class GUI : public bs::Snake
{
public:
    GUI (MainComponent& m) : mc (m)
    {
    }
    
    struct Choice
    {
        bs::Direction direction;
        float score = 0;
    };
    
    struct Comp
    {
        static int compareElements (const Choice& first, const Choice& second)
        {
            if (first.score < second.score)
                return -1;
            else if (first.score == second.score)
                return 0;
            else
                return +1;
        }
    };
    
    struct Grid
    {
        Grid (GUI& m, bs::Game& g)
            : game (g), me (m)
        {
            // set all to a average value
            for (int x = 0; x < w; x++)
                for (int y = 0; y < h; y++)
                    area.setPixelAt (x, y, Colour (0xff808080));
            
            // don't go near the walls if possible
            for (int x = 0; x < w; x++)
            {
                area.setPixelAt (x, 0, Colour (0xff606060));
                area.setPixelAt (x, h - 1, Colour (0xff606060));
            }
            for (int y = 0; y < h; y++)
            {
                area.setPixelAt (0, y, Colour (0xff606060));
                area.setPixelAt (w - 1, y, Colour (0xff606060));
            }
            
            // corners are no good
            area.setPixelAt (0, 0, Colour (0xff202020));
            area.setPixelAt (0, h - 1, Colour (0xff202020));
            area.setPixelAt (w - 1, 0, Colour (0xff202020));
            area.setPixelAt (w - 1, h - 1, Colour (0xff202020));

            // food is great
            for (auto f : game.getFood())
                area.setPixelAt (f->pos.x, f->pos.y, Colour (0xffffffff));
            
            // snakes are bad, mkay
            for (auto s : game.getBodies())
                for (auto p : s->pos)
                    area.setPixelAt (p.x, p.y, Colour (0xff000000));
            
            applyStackBlur (area, 5);
            
            // snakes are bad
            for (auto s : game.getBodies())
            {
                if (! s->me)
                {
                    auto h = s->pos[0];
                    area.setPixelAt (h.x - 1, h.y, Colour (0xff404040));
                    area.setPixelAt (h.x + 1, h.y, Colour (0xff404040));
                    area.setPixelAt (h.x, h.y - 1, Colour (0xff404040));
                    area.setPixelAt (h.x, h.y + 1, Colour (0xff404040));
                }
                
                for (auto p : s->pos)
                    area.setPixelAt (p.x, p.y, Colour (0xff000000));
            }
            
            seek();
        }
         
        void seek()
        {
            Array<Point<int>> seekPoints;
            
            // seek to something
            auto food = me.getNearestFood();
            if (food.has_value())
                seekPoints.add (*food);
                        
            auto prey = me.getNearestPrey (5.0);
            if (prey != nullptr && me.getHealth() > 50)
            {
                seekPoints.clear();
                seekPoints.addArray (game.possibleNextMoves (*prey));
            }
            
            if (seekPoints.size() > 0)
            {
                struct Comp
                {
                    Comp (Point<int> o) : origin (o.toDouble()) {}
                    
                    int compareElements (const Point<int>& first, const Point<int>& second)
                    {
                        double d1 = origin.getDistanceFrom (first.toDouble());
                        double d2 = origin.getDistanceFrom (second.toDouble());
                        
                        if (d1 < d2)
                            return -1;
                        else if (d1 == d2)
                            return 0;
                        else
                            return +1;
                    }
                    
                    Point<double> origin;
                };
                
                Comp cmp (me.getHead());
                seekPoints.sort (cmp);
                auto seekPoint = seekPoints[0];
                
                Image backup = area;
                
                auto head = me.getHead();
                int dx = seekPoint.x > head.x ? +1 : -1;
                int dy = seekPoint.y > head.y ? +1 : -1;
                
                int x = head.x;
                int y = head.y;
                while (x != seekPoint.x)
                {
                    x += dx;
                    
                    if (game.isSquareBlocked ({x, y}))
                    {
                        area = backup;
                        break;
                    }
                    
                    area.setPixelAt (x, y, Colour (0xffffffff));
                }
                
                while (y != seekPoint.y)
                {
                    y += dy;

                    if (game.isSquareBlocked ({x, y}))
                    {
                        area = backup;
                        break;
                    }

                    area.setPixelAt (x, y, Colour (0xffffffff));
                }
            }
        }
        
        float get (int x, int y)
        {
            return area.getPixelAt (x, y).getRed() / 255.0;
        }
        
        bs::Game& game;
        GUI& me;
        int w = game.getBoardWidth();
        int h = game.getBoardHeight();
        Image area {Image::RGB, game.getBoardWidth(), game.getBoardHeight(), true};
    };
    
    bs::Direction getMove() override
    {
        Grid grid (*this, *game);
        
        Array<Choice> choices;

        auto dirs = possibleDirections();
        for (auto d : dirs)
        {
            DBG("  " + toString (d) + ":");
           
            Choice c;
            c.direction = d;
            
            auto p = game->getPoint (getHead(), d);
            c.score = grid.get (p.x, p.y);
            
            //if (d == currentDirection)
            //    c.score += 0.05;
            
            DBG (String::formatted ("   %d %d %f", p.x, p.y, c.score));
            
            choices.add (c);
        }

        Comp cmp;
        choices.sort (cmp);

        currentDirection = choices.getLast().direction;

        DBG("  picked: " + toString (currentDirection));
        
        {
            MessageManagerLock mml (Thread::getCurrentThread());
            
            mc.images.add (grid.area);
            mc.repaint();
        }

        return currentDirection;
    }
    
    MainComponent& mc;
};
