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

int color_test(const char * bmpfile){
    tlb_image_t * image = NULL;
    int32_t x,y;
    /* drow colors */
    image = tlb_img_new(256,256,tlb_rgb(0,0,0));
    for(x = 0; x<256; x++){
        for(y = 0; y<256; y++){
            tlb_pixel_set(image, x, y, tlb_rgb(x,0,y));
        }
    }
    tlb_save_bmp(bmpfile, image);
    tlb_img_free(image);
    return 0;
}

int line_test(const char * bmpfile){
    tlb_image_t * image = NULL;
    int32_t x,y;
    /* drow lines */
    image = tlb_img_new(510,510,tlb_rgb(0,0,255));
    y = 505;
    for(x = 5; x < 505; x+=50){
        tlb_draw_line(image,5,5,x,y,tlb_rgb(255,255,0));
    }
    for(y = 505; y >= 5; y-=50){
        tlb_draw_line(image,5,5,x,y,tlb_rgb(255,255,0));
    }
    y = 5;
    for(x = 5; x < 505; x+=50){
        tlb_draw_line(image, 5, 505, x, y, tlb_rgb(255,255,0));
    }
    for(y = 505; y >= 5; y-=50){
        tlb_draw_line(image, 5, 505, x, y, tlb_rgb(255,255,0));
    }
    y = 5;
    for(x = 505; x > 5; x-=50){
        tlb_draw_line(image, 505, 505, x, y, tlb_rgb(255,255,0));
    }
    for(y = 5; y <= 505; y+=50){
        tlb_draw_line(image, 505, 505, x, y, tlb_rgb(255,255,0));
    }
    y = 505;
    for(x = 505; x > 5; x-=50){
        tlb_draw_line(image, 505, 5, x, y, tlb_rgb(255,255,0));
    }
    for(y = 505; y >= 5; y-=50){
        tlb_draw_line(image, 505, 5, x, y, tlb_rgb(255,255,0));
    }
    tlb_save_bmp(bmpfile, image);
    tlb_img_free(image);
    return 0;
}

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

    image = tlb_img_channel(origin,CHANNEL_R);
    tlb_save_bmp("./out_channel_r.bmp", image);
    tlb_img_free(image);

    image = tlb_img_channel(origin,CHANNEL_G);
    tlb_save_bmp("./out_channel_g.bmp", image);
    tlb_img_free(image);

    image = tlb_img_channel(origin,CHANNEL_B);
    tlb_save_bmp("./out_channel_b.bmp", image);
    tlb_img_free(image);

    image = tlb_img_mosaic(origin,5);
    tlb_save_bmp("./out_mosaic.bmp", image);
    tlb_img_free(image);

    image = tlb_block_mosaic(origin, 175, 125, 185, 125, 5);
    tlb_save_bmp("./out_block_mosaic.bmp", image);
    tlb_img_free(image);

    color_test("out_color.bmp");

    line_test("out_lines.bmp");

    return 0;
}
