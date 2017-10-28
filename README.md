# tlibmp - tiny libbmp

[![license](https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000)](http://opensource.org/licenses/MIT)
![build](https://img.shields.io/badge/build-success-brightgreen.svg)
![language](https://img.shields.io/badge/language-C-green.svg)
![platform](https://img.shields.io/badge/platform-POSIX--compatible-lightgray.svg)

A tiny libary that can read/write/edit .BMP files from disk

[View project homepage](http://www.short-circuits.org/tlibmp/index.html)

## Basic

It was a lib that can create bmp file, and print in it.

these is some demo with this lib:

### Color print
![color](http://www.short-circuits.org/tlibmp/out_color.bmp)

### Draw lines
![lines](http://www.short-circuits.org/tlibmp/out_lines.bmp)

### Draw triangles
![lines](http://www.short-circuits.org/tlibmp/out_triangle.bmp)

## Image processing

it also contain some basic	image processing functions.

I use a picture from pixiv by
[(ˉ﹃ˉ)EM1234](https://www.pixiv.net/member.php?id=8467971) as following:

[![origin](http://www.short-circuits.org/tlibmp/test.bmp)](https://www.pixiv.net/member_illust.php?mode=medium&amp;illust_id=61057871)

I have transform it to a bmp file named test.bmp,

Use this lib, I have made the following images:

### Gray

![gray](http://www.short-circuits.org/tlibmp/out_gray.bmp)

### Binary

![binary](http://www.short-circuits.org/tlibmp/out_binary.bmp)

### Channel R

![channel R](http://www.short-circuits.org/tlibmp/out_channel_r.bmp)

### Channel G

![channel G](http://www.short-circuits.org/tlibmp/out_channel_g.bmp)

### Channel B

![channel B](http://www.short-circuits.org/tlibmp/out_channel_b.bmp)

### Mosaic

![mosaic](http://www.short-circuits.org/tlibmp/out_block_mosaic.bmp)

![mosaic](http://www.short-circuits.org/tlibmp/out_mosaic.bmp)

### Histogram graph

![histogram_r](http://www.short-circuits.org/tlibmp/out_histogram_r.bmp)

![histogram_g](http://www.short-circuits.org/tlibmp/out_histogram_g.bmp)

![histogram_b](http://www.short-circuits.org/tlibmp/out_histogram_b.bmp)

![histogram](http://www.short-circuits.org/tlibmp/out_histogram.bmp)

### Convolution

In mathematics convolution is a mathematical operation on two functions to produce a third function. 

As picture can treated as a function, we can run convolution operation on a picture, and this will produce magical effect

Firstly, let's try a Gaussian function, the Matrix(convolution core) is:

|     |     |     |     |     |     |     |
|-----|-----|-----|-----|-----|-----|-----|
|     |  1.0|  4.0|  7.0|  4.0|  1.0|     |
|     |  4.0| 16.0| 26.0| 16.6|  4.0|     |
|     |  7.0| 26.0| 41.0| 26,0|  7.0|     |
|     |  4.0| 16.0| 26.0| 16.6|  4.0|     |
|     |  1.0|  4.0|  7.0|  4.0|  1.0|     |
|     |     |     |     |     |     |     |

and this matrix produce this picture:

![gaussian](http://www.short-circuits.org/tlibmp/out_conv0.bmp)

and then, let's try a matrix like this:

|     |     |     |     |     |
|-----|-----|-----|-----|-----|
|     | -1.0| -1.0|  0.0|     |
|     | -1.0|  0.0|  1.0|     |
|     |  0.0|  1.0|  1.0|     |
|     |     |     |     |     |

easily to find that this matrix will produce a gray picture, just like a sculpture:

![gaussian](http://www.short-circuits.org/tlibmp/out_conv1.bmp)

a matrix like this will stress the edge of the picture:

|     |     |     |     |     |
|-----|-----|-----|-----|-----|
|     |  0.0| -1.0|  0.0|     |
|     | -1.0|  5.0| -1.0|     |
|     |  0.0| -1.0|  0.0|     |
|     |     |     |     |     |


![stress](http://www.short-circuits.org/tlibmp/out_conv2.bmp)

To make a color inverse, you can use a matrix with the summary small than zero:

![inverse](http://www.short-circuits.org/tlibmp/out_inverse.bmp)
