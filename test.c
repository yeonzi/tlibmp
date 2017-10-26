/********************************************************************************
The MIT License
Copyright (c) 2017 Yeonji
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
********************************************************************************/

#include "tlibmp.h"
#include <stdio.h>

int main(void)
{
    tlb_image_t * origin = NULL;
    tlb_image_t * image = NULL;
    tlb_print_bmp_info("test.bmp");
    origin = tlb_load_bmp("test.bmp");

    if(origin == NULL){
        printf("Unable to load this BMP\n");
        return -1;
    }

    printf("BMP Loaded\n");

    printf("First pixel color: #%02X%02X%02X\n",\
        tlb_pixel(origin, 0, 0)[CHANNEL_R],\
        tlb_pixel(origin, 0, 0)[CHANNEL_G],\
        tlb_pixel(origin, 0, 0)[CHANNEL_B]);

    image = tlb_img_copy(origin);
    tlb_img_inverse(image);
    tlb_save_bmp("./out_inverse.bmp", image);
    tlb_img_free(image);

    image = tlb_img_copy(origin);
    tlb_img_gray(image);
    tlb_save_bmp("./out_gray.bmp", image);
    tlb_img_free(image);

    image = tlb_img_copy(origin);
    tlb_img_binary(image,120);
    tlb_save_bmp("./out_binary.bmp", image);
    tlb_img_free(image);

    image = tlb_img_histogram(origin);
    tlb_save_bmp("./out_histogram.bmp", image);
    tlb_img_free(image);

    image = tlb_img_ch_histogram(origin,CHANNEL_R);
    tlb_save_bmp("./out_histogram_r.bmp", image);
    tlb_img_free(image);

    image = tlb_img_ch_histogram(origin,CHANNEL_G);
    tlb_save_bmp("./out_histogram_g.bmp", image);
    tlb_img_free(image);

    image = tlb_img_ch_histogram(origin,CHANNEL_B);
    tlb_save_bmp("./out_histogram_b.bmp", image);
    tlb_img_free(image);

    return 0;
}
