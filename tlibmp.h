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

#ifndef _TLIBMP_H_
#define _TLIBMP_H_ 1

#define _TLIBMP_VER_ "0.0.1"

#include <stdint.h>

#define TLB_OK      0
#define TLB_ERROR   -1

/* BMP INFO Struct */
typedef struct {
    uint16_t bfType;
    uint16_t biType;
}bmp_info_t;

/* BMP Header Struct */
typedef struct {
    unsigned char bfType[2];
    unsigned char bfSize[4];
    unsigned char bfReserved1[2];
    unsigned char bfReserved2[2];
    unsigned char bfOffBits[4];
}bmp_header_t;

/* Magic Number in BMP Header (bfType) */
#define WINDOWS_BMP         0X424D  /*BM*/
#define OS2_BITMAP_ARRAY    0X4241  /*BA*/
#define OS2_COLOR_ICON      0X4349  /*CI*/
#define OS2_COLOR_POINTER   0X4350  /*CP*/
#define OS2_ICON            0X4943  /*IC*/
#define OS2_POINTER         0X5054  /*PT*/

/* DIB Header Struct */
typedef struct {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
}bmp_stddib_t;

#define DIB_BITMAPCOREHEADER_SIZE   12
#define DIB_OS22XBITMAPHEADER_SIZE  16
#define DIB_BITMAPINFOHEADER_SIZE   40
#define DIB_BITMAPV2INFOHEADER_SIZE 52
#define DIB_BITMAPV3INFOHEADER_SIZE 56
#define DIB_OS22XBITMAPHEADER2_SIZE 64
#define DIB_BITMAPV4HEADER_SIZE     108
#define DIB_BITMAPV5HEADER_SIZE     124

#define COMPRESS_BI_RGB             0
#define COMPRESS_BI_RLE8            1
#define COMPRESS_BI_RLE4            2
#define COMPRESS_BI_BITFIELDS       3
#define COMPRESS_BI_JPEG            4
#define COMPRESS_BI_PNG             5
#define COMPRESS_BI_ALPHABITFIELDS  6
#define COMPRESS_BI_CMYK            11
#define COMPRESS_BI_CMYKRLE8        12
#define COMPRESS_BI_CMYKRLE4        13

#define CHANNEL_R 0
#define CHANNEL_G 1
#define CHANNEL_B 2
#define CHANNEL_A 3
#define CHANNEL_CNT 4

/* tlibmp bitmap struct */
typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t  *data;             /* Data format: [RGBA]    */
}tlb_image_t;

#define tlb_rgb(R,G,B)              (uint32_t)(((uint8_t)R<<0)+((uint8_t)G<<8)+((uint8_t)B<<16))
#define tlb_rgba(R,G,B,A)           (uint32_t)(((uint8_t)R<<0)+((uint8_t)G<<8)+((uint8_t)B<<16)+((uint8_t)A<<24))

/* BMP operation APIs */

/* load a bmp frome file */
tlb_image_t * tlb_load_bmp(const char *file_name);

/* save bmp to file by default settings */
int tlb_save_bmp(const char *file_name, tlb_image_t * image);

/* print basic info of a bmp file */
int tlb_print_bmp_info(const char *file_name);

/* pixel operation APIs */

/* #define tlb_pixel(image, x, y, ch)  ((image->data + 4*(x * image->width + y))[ch]) */

/* get a array of the pixel */
uint8_t * tlb_pixel(tlb_image_t * image, uint32_t x, uint32_t y);

/* print a pixel with specific color */
int tlp_print_pixel(tlb_image_t * image, uint32_t x, uint32_t y, uint32_t color);

/* print a pixel with specific color */
int tlp_print_pixel_ch(tlb_image_t * image, uint32_t x, uint32_t y, uint8_t channel, uint8_t val);

/* Image operation APIs */

/* new image */
tlb_image_t * tlb_img_new(uint32_t width, uint32_t height, uint32_t bgcolor);

/* delete image */
void tlb_img_free(tlb_image_t * image);

/* make a copy of a image */
tlb_image_t * tlb_img_copy(tlb_image_t * image);

int tlb_img_inverse(tlb_image_t * image);
int tlb_img_gray(tlb_image_t * image);
int tlb_img_binary(tlb_image_t * image, uint8_t threshold);

int tlb_img_color_replace(tlb_image_t * image, uint32_t find, uint32_t replace);

tlb_image_t * tlb_img_ch_histogram(tlb_image_t * image, uint8_t channel);
tlb_image_t * tlb_img_histogram(tlb_image_t * image);


#endif
