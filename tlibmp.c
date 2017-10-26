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
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define uint16_LE(str)   (uint16_t)((uint16_t)(str[1] << 0) + (uint16_t)(str[0] << 8))

#define uint32_LE(str)   (uint32_t)((uint32_t)(str[3] << 0) +\
                                    (uint32_t)(str[2] << 8) +\
                                    (uint32_t)(str[1] << 16)+\
                                    (uint32_t)(str[0] << 24))

#define uint16_BE(str)   (uint16_t)((uint16_t)(str[0] << 0) + (uint16_t)(str[1] << 8))

#define uint32_BE(str)   (uint32_t)((uint32_t)(str[0] << 0) +\
                                    (uint32_t)(str[1] << 8) +\
                                    (uint32_t)(str[2] << 16)+\
                                    (uint32_t)(str[3] << 24))

#define str2_LE(str,content) {\
    str[1] = (uint8_t)((content&0x00FF) >> 0);\
    str[0] = (uint8_t)((content&0xFF00) >> 8);\
}
#define str4_LE(str,content) {\
    str[3] = (uint8_t)((content&0x000000FF) >> 0);\
    str[2] = (uint8_t)((content&0x0000FF00) >> 8);\
    str[1] = (uint8_t)((content&0x00FF0000) >> 16);\
    str[0] = (uint8_t)((content&0xFF000000) >> 24);\
}

#define str2_BE(str,content) {\
    str[0] = (uint8_t)((content&0x00FF) >> 0);\
    str[1] = (uint8_t)((content&0xFF00) >> 8);\
}
#define str4_BE(str,content) {\
    str[0] = (uint8_t)((content&0x000000FF) >> 0);\
    str[1] = (uint8_t)((content&0x0000FF00) >> 8);\
    str[2] = (uint8_t)((content&0x00FF0000) >> 16);\
    str[3] = (uint8_t)((content&0xFF000000) >> 24);\
}

#define s_free(ptr)  if(ptr!=NULL){free(ptr);};

const char * string_unknown           = "Unknown";

const char * string_windows_bmp       = "Windows 3.1x bitmap (424D)";
const char * string_os2_bitmap_array  = "OS/2 struct bitmap array (4241)";
const char * string_os2_color_icon    = "OS/2 struct color icon (4349)";
const char * string_os2_color_pointer = "OS/2 const color pointer (4350)";
const char * string_os2_icon          = "OS/2 struct icon (4943)";
const char * string_os2_pointer       = "OS/2 pointer (5054)";

const char * string_bitmapcoreheader   = "BMP core header";
const char * string_os22xbitmapheader  = "OS/2 BMP header";
const char * string_bitmapinfoheader   = "Standard BMP header";
const char * string_bitmapv2infoheader = "Adobe Photoshop Externed BMP header ver.2";
const char * string_bitmapv3infoheader = "Adobe Photoshop Externed BMP header ver.3";
const char * string_os22xbitmapheader2 = "OS/2 BMP header ver.2";
const char * string_bitmapv4header     = "Standard BMP header ver.4";
const char * string_bitmapv5header     = "Standard BMP header ver.5";

const char * string_compress_bi_rgb             = "None";
const char * string_compress_bi_rle8            = "RLE 8-bit/pixel";
const char * string_compress_bi_rle4            = "RLE 4-bit/pixel";
const char * string_compress_bi_bitfields       = "RGBA (Perhaps Huffman 1D)";
const char * string_compress_bi_jpeg            = "JPEG image for printing";
const char * string_compress_bi_png             = "PNG image for printing";
const char * string_compress_bi_alphabitfields  = "RGBA bit field masks on Windows CE 5.0 with .NET 4.0 or later";
const char * string_compress_bi_cmyk            = "Windows Metafile CMYK";
const char * string_compress_bi_cmykrle8        = "Windows Metafile CMYK with RLE 8-bit/pixel";
const char * string_compress_bi_cmykrle4        = "Windows Metafile CMYK with RLE 4-bit/pixel";

const char * get_bmp_type_string(const unsigned char * type_word);
const char * get_bmp_dib_string(uint32_t size);
const char * get_bmp_comp_string(uint32_t method);

int convert_from_raw(tlb_image_t * image, uint32_t depth);
uint8_t * convert_to_raw(tlb_image_t * image, uint32_t depth);

tlb_image_t * tlb_load_bmp(const char *file_name)
{
    /* image struct */
    tlb_image_t * image = NULL;

    /* image file pointer */
    FILE *fp = NULL;

    /* bmp header struct */
    bmp_header_t header;

    /* bmp information header */
    bmp_stddib_t dib_info;

    /* data offset */
    uint32_t data_offset = 0;

    /* color depth */
    uint32_t color_depth = 0;

    /* pixel count */
    uint32_t pixel_cnt = 0;

    /* open file */
    fp = fopen(file_name,"rb");

    /* open exceptional */
    if(fp == NULL){
        fprintf(stderr, "Cannot open file.\n");
        return NULL;
    }

    /* read bmp header */
    if(fread(&header, sizeof(header), 1, fp) != 1){
        fprintf(stderr, "Broken File: Can not read header.\n");
        goto destory;
    }

    /* unsupported bmp format */
    if(uint16_LE(header.bfType)!=WINDOWS_BMP){
        fprintf(stderr, "Unsupported Format: %s.\n", get_bmp_type_string(header.bfType));
        goto destory;
    }

    /* transform to uint32_t */
    data_offset = uint32_BE(header.bfOffBits);

    /* get bmp information header */
    fread(&dib_info, sizeof(dib_info), 1, fp);

    /**********************************/
    /* begin of file legitimacy check */
    /**********************************/

    /* unsupported bmp format */
    if(dib_info.biSize < DIB_BITMAPINFOHEADER_SIZE){
        fprintf(stderr, "Unsupported DIB Format: %s.\n", get_bmp_dib_string(dib_info.biSize));
        goto destory;
    }

    /* set bytes_per_pixel and read image */
    if(dib_info.biBitCount == 16){
        /* 16-bit RGB use two byte every pixel */
        color_depth=2;
    }else if(dib_info.biBitCount == 24){
        /* 24-bit RGB use three byte every pixel */
        color_depth=3;
    }else if(dib_info.biBitCount == 32){
        /* 32-bit RGB use four byte every pixel */
        color_depth=4;
    }else{
        /* unsupported number of bits per pixel */
        fprintf(stderr, "Unsupported Format: Can not parse %d-bit color.\n", dib_info.biBitCount);
        goto destory;
    }

    /* unsupported compression method */
    if(dib_info.biCompression != COMPRESS_BI_RGB &&\
       dib_info.biCompression != COMPRESS_BI_BITFIELDS){
        fprintf(stderr, "Unsupported Format: Can not parse %s compression method.\n", get_bmp_comp_string(dib_info.biCompression));
        goto destory;
    }

    /* unsupported compression method */
    /* COMPRESS_BI_BITFIELDS marked, but color depth is not 4 */
    /* this will appear when use Huffman 1D */
    if(dib_info.biCompression == COMPRESS_BI_BITFIELDS && color_depth != 4){
        fprintf(stderr, "Unsupported Format: Can not parse with Huffman 1D.\n");
        goto destory;
    }

    /* get data size in theory */
    pixel_cnt = dib_info.biWidth * dib_info.biHeight;

    if(uint32_BE(header.bfSize)<(pixel_cnt*color_depth+data_offset)){
        fprintf(stderr, "Broken File: Size in theory more then that in record.\n");
        goto destory;
    }

    /********************************/
    /* end of file legitimacy check */
    /********************************/

    /* locate file stream */
    if(fseek(fp, data_offset, SEEK_SET)!=0){
        fprintf(stderr, "Error: Can not locate file stream.\n");
        goto destory;
    }

    /* get memory for image */
    image = malloc(sizeof(tlb_image_t));

    /* malloc exceptional */
    if(image == NULL){
        fprintf(stderr, "Error: Can not malloc for new object.\n");
        goto destory;
    }

    /* set width of struct */
    image->width  = dib_info.biWidth;

    /* set height of struct */
    image->height = dib_info.biHeight;

    /* malloc for cooked img data */
    image->data = malloc((4*image->width*image->height)*sizeof(char));
    if(image->data == NULL){
        fprintf(stderr, "Error: Can not malloc for new object.\n");
        goto destory;
    }

    /* set memory to zero */
    memset(image->data, 0, (4*image->width*image->height));

    /* read bmp data */
    if(fread(image->data, (pixel_cnt*color_depth), 1, fp) != 1){
        fprintf(stderr, "Broken File: Can not read data.\n");
        goto destory;
    }

    /* convert to cooked format */
    convert_from_raw(image, color_depth);

    return image;

destory:
    if(image != NULL){
        s_free(image->data);
        s_free(image);
    }
    fclose(fp);
    return NULL;
}

/* save bmp to file by default settings */
/* bmp_format = Windows 3.1x bitmap (424D) */
/* dib_format = Standard BMP header */
/* bit_pre_pixel = 24  */
int tlb_save_bmp(const char *file_name, tlb_image_t * image)
{
    /* bmp header struct */
    bmp_header_t header;

    /* bmp information header */
    bmp_stddib_t dib_info;

    uint8_t  data_offset;
    uint32_t data_size;
    uint32_t file_size;

    uint8_t *bmp_data = NULL;

    FILE * fp = NULL;

    data_offset = sizeof(bmp_header_t) + sizeof(bmp_stddib_t);
    data_size   = image->width * image->height * 3; /* 3 means use 3 byte pre pixel */
    file_size   = data_offset + data_size;

    /* bmp header magic */
    memset(&header, 0, sizeof(bmp_header_t));

    str2_LE(header.bfType,    WINDOWS_BMP);
    str4_BE(header.bfSize,    file_size);
    str4_BE(header.bfOffBits, data_offset);

    dib_info.biSize          = DIB_BITMAPINFOHEADER_SIZE;
    dib_info.biWidth         = image->width;
    dib_info.biHeight        = image->height;
    dib_info.biPlanes        = 1; /* This shoule always be 1 */
    dib_info.biBitCount      = 24;
    dib_info.biCompression   = COMPRESS_BI_RGB;
    dib_info.biSizeImage     = data_size;
    dib_info.biXPelsPerMeter = 72;
    dib_info.biYPelsPerMeter = 72;
    dib_info.biClrUsed       = 0;
    dib_info.biClrImportant  = 0;

    /* convert to bmp data */
    bmp_data = convert_to_raw(image, 3);

    /* convert exceptional */
    if(bmp_data == NULL){
        fprintf(stderr, "Cannot convert this file.\n");
        return TLB_ERROR;
    }

    /* open file */
    fp = fopen(file_name,"wb");

    /* open exceptional */
    if(fp == NULL){
        fprintf(stderr, "Cannot open file.\n");
        return TLB_ERROR;
    }

    fwrite(&header, sizeof(header), 1, fp);
    fwrite(&dib_info, sizeof(dib_info), 1, fp);
    fwrite(bmp_data, data_size, 1, fp);

    fclose(fp);
    s_free(bmp_data);

    return 0;

}

int tlb_print_bmp_info(const char *file_name)
{
    bmp_header_t header;
    bmp_stddib_t dib_info;
    const char * type_string = NULL;
    FILE * fp;

    uint32_t offset;

    fp = fopen(file_name,"rb");
    if(fp == NULL){
        fprintf(stderr, "\tCannot open this file.\n");
        return TLB_ERROR;
    }
    if( fread(&header, sizeof(header), 1, fp) == 1){
        /* check file type */
        type_string = get_bmp_type_string(header.bfType);

        fprintf(stderr, "%s\n", type_string);
        if (type_string == string_unknown) return TLB_ERROR;

        fprintf(stderr, "File Size: %d Byte\n", uint32_BE(header.bfSize));

        offset = uint32_BE(header.bfOffBits);
        fprintf(stderr, "Data Offset: %d Byte\n", offset);

        fread(&dib_info, sizeof(dib_info), 1, fp);

        type_string = get_bmp_dib_string(dib_info.biSize);
        fprintf(stderr, "DIB Type: %s (%d Byte)\n",type_string, dib_info.biSize);
        if (type_string == string_unknown) return TLB_ERROR;

        fprintf(stderr, "Image Width: %d pix\n", dib_info.biWidth);
        fprintf(stderr, "Image Width: %d pix\n", dib_info.biHeight);
        fprintf(stderr, "Color Planes: %d\n", dib_info.biPlanes);
        fprintf(stderr, "Color Depth: %d-bit per pixel\n", dib_info.biBitCount);

        type_string = get_bmp_comp_string(dib_info.biCompression);
        fprintf(stderr, "Compression Type: %s (%d)\n",type_string, dib_info.biCompression);

    }else{
        fprintf(stderr, "Bad File\n");
    }

    fclose(fp);
    return TLB_OK;
}

/***************/
/* Image utils */
/***************/
tlb_image_t * tlb_img_new(uint32_t width, uint32_t height, uint32_t bgcolor)
{
    tlb_image_t * image = NULL;

    uint32_t * data;
    uint32_t index;

    /* get memory for image */
    image = malloc(sizeof(tlb_image_t));

    /* malloc exceptional */
    if(image == NULL){
        fprintf(stderr, "Error: Can not malloc for new object.\n");
        return NULL;
    }

    /* set width of struct */
    image->width  = width;

    /* set height of struct */
    image->height = height;

    /* malloc for cooked img data */
    image->data = malloc((4*image->width*image->height)*sizeof(char));
    if(image->data == NULL){
        fprintf(stderr, "Error: Can not malloc for new object.\n");
        return NULL;
    }

    data = (uint32_t*)(image->data);

    for(index = 0; index < width*height; index++)
    {
        data[index] = bgcolor;
    }

    return image;
}

void tlb_img_free(tlb_image_t * image)
{
    if(image != NULL){
        s_free(image->data);
        s_free(image);
    }
}

tlb_image_t * tlb_img_copy(tlb_image_t * src_image)
{
    tlb_image_t * image = NULL;

    /* get memory for image */
    image = malloc(sizeof(tlb_image_t));

    /* malloc exceptional */
    if(image == NULL){
        fprintf(stderr, "Error: Can not malloc for new object.\n");
        return NULL;
    }

    /* set width of struct */
    image->width  = src_image->width;

    /* set height of struct */
    image->height = src_image->height;

    /* malloc for cooked img data */
    image->data = malloc((4*image->width*image->height)*sizeof(char));
    if(image->data == NULL){
        fprintf(stderr, "Error: Can not malloc for new object.\n");
        return NULL;
    }

    memcpy(image->data, src_image->data, (4*image->width*image->height));

    return image;
}

/*******************/
/* pixel operation */
/*******************/
/* get a array of the pixel */
uint8_t * tlb_pixel(tlb_image_t * image, uint32_t x, uint32_t y){
    /* Check the input legitimacy */
    if(x > image->width){
        x = image->width;
    }
    if(y > image->height){
        y = image->height;
    }
    return (image->data + 4*(y * image->width + x));
}

/* print a pixel with specific color */
int tlp_print_pixel(tlb_image_t * image, uint32_t x, uint32_t y, uint32_t color){
    if(x > image->width){
        return TLB_ERROR;
    }
    if(y > image->height){
        return TLB_ERROR;
    }
    *(uint32_t*)(image->data + 4*(y * image->width + x)) = color;
    return TLB_OK;
}

/* print a pixel with specific color */
int tlp_print_pixel_ch(tlb_image_t * image, uint32_t x, uint32_t y, uint8_t channel, uint8_t val){
    if(x > image->width){
        return TLB_ERROR;
    }
    if(y > image->height){
        return TLB_ERROR;
    }
    (image->data + 4*(y * image->width + x))[channel] = val;
    return TLB_OK;
}

/*******************/
/* image operation */
/*******************/

int tlb_img_inverse(tlb_image_t * image)
{
    uint32_t size;
    uint8_t  *now;
    uint8_t  *last;

    size = image->width * image->height * CHANNEL_CNT;

    last = image->data + size - CHANNEL_CNT;

    for(now = image->data; now<=last; now+=CHANNEL_CNT){
        now[CHANNEL_R] = 0xFF - now[CHANNEL_R];
        now[CHANNEL_G] = 0xFF - now[CHANNEL_G];
        now[CHANNEL_B] = 0xFF - now[CHANNEL_B];
    }
    return 0;
}

int tlb_img_gray(tlb_image_t * image)
{
    uint32_t size;
    uint8_t  *now;
    uint8_t  *last;

    uint8_t gray;

    size = image->width * image->height * CHANNEL_CNT;

    last = image->data + size - CHANNEL_CNT;

    for(now = image->data; now<=last; now+=CHANNEL_CNT){
        gray = (now[CHANNEL_R] + now[CHANNEL_G] + now[CHANNEL_B])/3;
        now[CHANNEL_R] = gray;
        now[CHANNEL_G] = gray;
        now[CHANNEL_B] = gray;
    }
    return 0;
}

int tlb_img_binary(tlb_image_t * image, uint8_t threshold){
    uint32_t size;
    uint8_t  *now;
    uint8_t  *last;

    uint8_t gray;

    size = image->width * image->height * CHANNEL_CNT;

    last = image->data + size - CHANNEL_CNT;

    for(now = image->data; now<=last; now+=CHANNEL_CNT){
        gray = (now[CHANNEL_R] + now[CHANNEL_G] + now[CHANNEL_B])/3;
        if(gray > threshold){
            *(uint32_t*)now |= 0x00FFFFFF;
        }else{
            *(uint32_t*)now &= 0xFF000000;
        }
    }
    return 0;
}

int tlb_img_color_replace(tlb_image_t * image, uint32_t find, uint32_t replace)
{
    uint32_t size = image->width * image->height;
    uint32_t *data = (uint32_t*)image->data;
    uint32_t i;

    for (i = 0; i < size; ++i){
        if(data[i]==find) data[i]=replace;
    }
    return 0;
}

int tlb_img_color_switch(tlb_image_t * image, uint32_t color1, uint32_t color2)
{
    uint32_t size = image->width * image->height;
    uint32_t *data = (uint32_t*)image->data;
    uint32_t i;

    for (i = 0; i < size; ++i){
        if(data[i]==color1){
            data[i]=color2;
        }else if(data[i]==color2){
            data[i]=color1;
        }
    }
    return 0;
}

tlb_image_t * tlb_img_ch_histogram(tlb_image_t * image, uint8_t channel)
{
    tlb_image_t * histogram = NULL;

    /* ver summary */
    uint32_t summary[256] = {0};

    /* image_size */
    uint32_t size = image->width * image->height;

    /* element index */
    long int i,j;

    uint32_t cnt_max = 0;

    for (i = channel; i < 4 * size; i += CHANNEL_CNT){
        summary[image->data[i]]++;
    }

    for (i = 0; i < 256; i++){
        cnt_max = (summary[i] > cnt_max) ? summary[i] : cnt_max;
    }

    for (i = 0; i < 256; i++){
        summary[i] = (summary[i]*100)/cnt_max;
    }

    histogram = tlb_img_new(256, 100, tlb_rgba(0,0,0,0));

    for(i = 0; i < 256; i++){
        for(j = summary[i]; j>=0; j--){
            tlp_print_pixel_ch(histogram, i, j, channel, 0xFF);
        }
    }
    return histogram;
}

tlb_image_t * tlb_img_histogram(tlb_image_t * image)
{
    tlb_image_t * histogram = NULL;

    uint8_t channel = 0;

    /* image_size */
    uint32_t size = image->width * image->height;

    histogram = tlb_img_new(512, 200, tlb_rgba(255,255,255,0));

    for (channel = 0; channel < CHANNEL_CNT; ++channel){
        /* ver summary */
        uint32_t summary[256] = {0};

        /* element index */
        long int i,j;

        uint32_t cnt_max = 0;

        for (i = channel; i < 4 * size; i += CHANNEL_CNT){
            summary[image->data[i]]++;
        }

        for (i = 0; i < 255; i++){
            cnt_max = (summary[i] > cnt_max) ? summary[i] : cnt_max;
        }

        for (i = 0; i < 256; i++){
            summary[i] = (summary[i]*200)/cnt_max;
            if(summary[i]>199) summary[i]=199;
        }

        for(i = 0; i < 256; i++){
            for(j = summary[i]; j>=0; j--){
                tlp_print_pixel_ch(histogram, (2*i), j, channel, 0x00);
            }
            for(j = summary[i]; j>=0; j--){
                tlp_print_pixel_ch(histogram, (2*i)+1, j, channel, 0x00);
            }
        }
    }

    tlb_img_inverse(histogram);
    tlb_img_color_replace(histogram, tlb_rgba(255,0,0,0), tlb_rgba(255,160,160,0));
    tlb_img_color_replace(histogram, tlb_rgba(0,255,0,0), tlb_rgba(160,255,160,0));
    tlb_img_color_replace(histogram, tlb_rgba(0,0,255,0), tlb_rgba(160,160,255,0));

    tlb_img_color_replace(histogram, tlb_rgba(255,255,0,0), tlb_rgba(210,255,150,0));
    tlb_img_color_replace(histogram, tlb_rgba(255,0,255,0), tlb_rgba(210,150,255,0));
    tlb_img_color_replace(histogram, tlb_rgba(0,255,255,0), tlb_rgba(180,210,255,0));

    tlb_img_color_replace(histogram, tlb_rgba(0,0,0,0), tlb_rgba(230,230,230,0));
    tlb_img_color_replace(histogram, tlb_rgba(255,255,255,0), tlb_rgba(150,150,250,0));

    return histogram;
}

/*********************/
/* Private Functions */
/*********************/

const char * get_bmp_type_string(const unsigned char * type_word)
{
    switch(uint16_LE(type_word))
    {
        case WINDOWS_BMP       : return string_windows_bmp       ; break;
        case OS2_BITMAP_ARRAY  : return string_os2_bitmap_array  ; break;
        case OS2_COLOR_ICON    : return string_os2_color_icon    ; break;
        case OS2_COLOR_POINTER : return string_os2_color_pointer ; break;
        case OS2_ICON          : return string_os2_icon          ; break;
        case OS2_POINTER       : return string_os2_pointer       ; break;
        default                : return string_unknown;
    };
    return string_unknown;
}

const char * get_bmp_dib_string(uint32_t size)
{
    switch(size)
    {
        case DIB_BITMAPCOREHEADER_SIZE   : return string_bitmapcoreheader   ; break;
        case DIB_OS22XBITMAPHEADER_SIZE  : return string_os22xbitmapheader  ; break;
        case DIB_BITMAPINFOHEADER_SIZE   : return string_bitmapinfoheader   ; break;
        case DIB_BITMAPV2INFOHEADER_SIZE : return string_bitmapv2infoheader ; break;
        case DIB_BITMAPV3INFOHEADER_SIZE : return string_bitmapv3infoheader ; break;
        case DIB_OS22XBITMAPHEADER2_SIZE : return string_os22xbitmapheader2 ; break;
        case DIB_BITMAPV4HEADER_SIZE     : return string_bitmapv4header     ; break;
        case DIB_BITMAPV5HEADER_SIZE     : return string_bitmapv5header     ; break;
        default                          : return string_unknown;
    }
    return string_unknown;
}

const char * get_bmp_comp_string(uint32_t method){
    switch(method)
    {
        case COMPRESS_BI_RGB            : return string_compress_bi_rgb            ; break;
        case COMPRESS_BI_RLE8           : return string_compress_bi_rle8           ; break;
        case COMPRESS_BI_RLE4           : return string_compress_bi_rle4           ; break;
        case COMPRESS_BI_BITFIELDS      : return string_compress_bi_bitfields      ; break;
        case COMPRESS_BI_JPEG           : return string_compress_bi_jpeg           ; break;
        case COMPRESS_BI_PNG            : return string_compress_bi_png            ; break;
        case COMPRESS_BI_ALPHABITFIELDS : return string_compress_bi_alphabitfields ; break;
        case COMPRESS_BI_CMYK           : return string_compress_bi_cmyk           ; break;
        case COMPRESS_BI_CMYKRLE8       : return string_compress_bi_cmykrle8       ; break;
        case COMPRESS_BI_CMYKRLE4       : return string_compress_bi_cmykrle4       ; break;
        default                         : return string_unknown;
    }
    return string_unknown;
}

int convert_from_raw(tlb_image_t * image, uint32_t depth)
{
    uint8_t * head = image->data;
    uint8_t * src;
    uint8_t * dst;
    uint8_t pixel[4] = {0};

    src = image->data + (image->width * image->height * depth - depth);
    dst = image->data + (image->width * image->height * CHANNEL_CNT - CHANNEL_CNT);

    if(depth == 3){
        /* 24-bit RGB color mode */
        while(src>head){
            /* Mapping matrix */
            dst[CHANNEL_R] = src[2];
            dst[CHANNEL_G] = src[1];
            dst[CHANNEL_B] = src[0];
            dst[CHANNEL_A] = 0;

            src -= depth;
            dst -= CHANNEL_CNT;
        }
        /* deal with the last bit */
        pixel[CHANNEL_R] = src[2];
        pixel[CHANNEL_G] = src[1];
        pixel[CHANNEL_B] = src[0];
        pixel[CHANNEL_A] = 0;
        dst[CHANNEL_R] = pixel[CHANNEL_R];
        dst[CHANNEL_G] = pixel[CHANNEL_G];
        dst[CHANNEL_B] = pixel[CHANNEL_B];
        dst[CHANNEL_A] = pixel[CHANNEL_A];

    }else if(depth == 4){
        /* 32-bit RGBA color mode */
        do{
            /* Mapping matrix */
            pixel[CHANNEL_R] = src[3];
            pixel[CHANNEL_G] = src[2];
            pixel[CHANNEL_B] = src[1];
            pixel[CHANNEL_A] = src[0];

            dst[CHANNEL_R] = pixel[CHANNEL_R];
            dst[CHANNEL_G] = pixel[CHANNEL_G];
            dst[CHANNEL_B] = pixel[CHANNEL_B];
            dst[CHANNEL_A] = pixel[CHANNEL_A];

            src -= depth;
            dst -= CHANNEL_CNT;
        }while(src>=head);

    }
    return TLB_OK;
}

uint8_t * convert_to_raw(tlb_image_t * image, uint32_t depth)
{
    /* converted data */
    uint8_t * data = NULL;

    uint8_t * head = image->data;
    uint8_t * src;
    uint8_t * dst;

    /* malloc memory for converted data */
    data = malloc( image->width * image->height * depth );

    /* malloc exceptional */
    if(data == NULL){
        fprintf(stderr, "Error: Can not malloc for new object.\n");
        return NULL;
    }

    dst = data + (image->width * image->height * depth - depth);
    src = image->data + (image->width * image->height * CHANNEL_CNT - CHANNEL_CNT);

    if(depth == 3){
        /* 24-bit RGB color mode */
        fprintf(stderr, "Warning: All data in Alpha area will LOST.\n");
        do{
            /* Mapping matrix */
            dst[CHANNEL_R] = src[2];
            dst[CHANNEL_G] = src[1];
            dst[CHANNEL_B] = src[0];

            src -= CHANNEL_CNT;
            dst -= depth;
        }while(src>=head);
    }
    return data;
}
