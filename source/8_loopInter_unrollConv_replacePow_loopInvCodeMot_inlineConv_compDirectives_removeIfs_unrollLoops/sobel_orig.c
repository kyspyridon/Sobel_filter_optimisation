// This will apply the sobel filter and return the PSNR between the golden sobel and the produced sobel
// sobelized image
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define SIZE    4096
#define SIZE_SQUARED 16777216
#define INPUT_FILE  "input/input.grey"
// #define OUTPUT_FILE "output_sobel.grey"
#define GOLDEN_FILE "input/golden.grey"

#define SQUARE255 65025
/* Implement a 2D convolution of the matrix with the operator */
/* posy and posx correspond to the vertical and horizontal disposition of the *
* pixel we process in the original image, input is the input image and       *
* operator the operator we apply (horizontal or vertical). The function ret. *
* value is the convolution of the operator with the neighboring pixels of the*
* pixel we process.
*/

#define CONV2DINITY(posy)                                                      \
posy_min1 = (posy - 1) << 12;                                              \
posy_plus1 = (posy + 1) << 12;                                             \
posy_size = posy << 12;                                                    \

#define CONV2DINITX(posx)                                                      \
posx_min1 = (posx - 1);                                                    \
posx_plus1 = (posx + 1);                                                   \

#define CONVOLUTION2D_HOR(posx, input, result)                                 \
                                        \
result = (input[posy_min1 + posx_min1] * hor_operator[0][0]                \
    + input[posy_min1 + posx_plus1] * hor_operator[0][2]               \
    + input[posy_size + posx_min1] * hor_operator[1][0]                \
    + input[posy_size + posx_plus1] * hor_operator[1][2]               \
    + input[posy_plus1 + posx_min1] * hor_operator[2][0]               \
    + input[posy_plus1 + posx_plus1] * hor_operator[2][2]);            \

#define CONVOLUTION2D_VER(posx, input, result)                                 \
                                        \
result = (input[posy_min1 + posx_min1] * ver_operator[0][0]                \
    + input[posy_min1 + posx] * ver_operator[0][1]                     \
    + input[posy_min1 + posx_plus1] * ver_operator[0][2]               \
    + input[posy_plus1 + posx_min1] * ver_operator[2][0]               \
    + input[posy_plus1 + posx] * ver_operator[2][1]                    \
    + input[posy_plus1 + posx_plus1] * ver_operator[2][2]);            \

/* The horizontal and vertical operators to be used in the sobel filter */
const char hor_operator[3][3] = {{-1, 0, 1},
                {-2, 0, 2},
                {-1, 0, 1}};
const char ver_operator[3][3] = {{1, 2, 1},
                {0, 0, 0},
                {-1, -2, -1}};

double sobel(register const unsigned char *restrict input, unsigned char *restrict output, register const unsigned char *restrict golden);

/* The arrays holding the input image, the output image and the output used *
* as golden standard. The luminosity (intensity) of each pixel in the      *
* grayscale image is represented by a value between 0 and 255 (an unsigned *
* character). The arrays (and the files) contain these values in row-major *
* order (element after element within each row and row after row.          */
unsigned char input[SIZE_SQUARED], output[SIZE_SQUARED], golden[SIZE_SQUARED];
//register unsigned char input[SIZE_SQUARED], output[SIZE_SQUARED], golden[SIZE_SQUARED];
//register unsigned char input[SIZE_SQUARED], output[SIZE_SQUARED], golden[SIZE_SQUARED];


char *outputfilename = NULL;


/* The main computational function of the program. The input, output and *
* golden arguments are pointers to the arrays used to store the input   *
* image, the output produced by the algorithm and the output used as    *
* golden standard for the comparisons.                                  */
double sobel(register const unsigned char *restrict input, unsigned char *restrict output, register const unsigned char *restrict golden)
{
    double PSNR = 0, t;
    int i, j, index;
    unsigned int p;
    int res, conv_res_hor, conv_res_ver;
    struct timespec  tv1, tv2;
    FILE *f_out;
    int posy_min1, posy_plus1, posy_size, posx_min1, posx_plus1;

    //outputfilename = realloc(outputfilename, (strlen(outputfilename) + strlen(OUTPUT_FILE) + 1));
    //outputfilename = strcat(outputfilename, OUTPUT_FILE);

    /* The first and last row of the output array, as well as the first  *
     * and last element of each column are not going to be filled by the *
     * algorithm, therefore make sure to initialize them with 0s.        */
    memset(output, 0, SIZE*sizeof(unsigned char));
    memset(&output[SIZE*(SIZE-1)], 0, SIZE*sizeof(unsigned char));
    for (i = 1; i < SIZE-1; i++) {
        output[i*SIZE] = 0;
        output[i*SIZE + SIZE - 1] = 0;
    }

    /* Open the output file       */

    f_out = fopen(outputfilename, "wb");
    if (f_out == NULL) {
        printf("File %s could not be created\n", outputfilename);
        // fclose(f_in);
        exit(1);
    }
  

    /* This is the main computation. Get the starting time. */
    clock_gettime(CLOCK_MONOTONIC_RAW, &tv1);
    /* For each pixel of the output image */
    for (i=1; i<SIZE-1; i+=1) {
        index = i << 12;
        CONV2DINITY(i)
        for (j=1; j<SIZE-1; j+=178) {

            CONV2DINITX(j)
            CONVOLUTION2D_HOR(j, input, conv_res_hor)
            CONVOLUTION2D_VER(j, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j] = 255;
                }
            }

            CONV2DINITX(j+1)
            CONVOLUTION2D_HOR(j + 1, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 1, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +1] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 1] = 255;
                }
            }

            CONV2DINITX(j+2)
            CONVOLUTION2D_HOR(j + 2, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 2, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +2] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 2] = 255;
                }
            }

            CONV2DINITX(j+3)
            CONVOLUTION2D_HOR(j + 3, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 3, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +3] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 3] = 255;
                }
            }

            CONV2DINITX(j+4)
            CONVOLUTION2D_HOR(j + 4, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 4, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +4] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 4] = 255;
                }
            }

            CONV2DINITX(j+5)
            CONVOLUTION2D_HOR(j + 5, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 5, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +5] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 5] = 255;
                }
            }

            CONV2DINITX(j+6)
            CONVOLUTION2D_HOR(j + 6, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 6, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +6] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 6] = 255;
                }
            }

            CONV2DINITX(j+7)
            CONVOLUTION2D_HOR(j + 7, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 7, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +7] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 7] = 255;
                }
            }

            CONV2DINITX(j+8)
            CONVOLUTION2D_HOR(j + 8, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 8, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +8] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 8] = 255;
                }
            }

            CONV2DINITX(j+9)
            CONVOLUTION2D_HOR(j + 9, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 9, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +9] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 9] = 255;
                }
            }

            CONV2DINITX(j+10)
            CONVOLUTION2D_HOR(j + 10, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 10, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +10] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 10] = 255;
                }
            }

            CONV2DINITX(j+11)
            CONVOLUTION2D_HOR(j + 11, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 11, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +11] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 11] = 255;
                }
            }

            CONV2DINITX(j+12)
            CONVOLUTION2D_HOR(j + 12, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 12, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +12] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 12] = 255;
                }
            }

            CONV2DINITX(j+13)
            CONVOLUTION2D_HOR(j + 13, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 13, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +13] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 13] = 255;
                }
            }

            CONV2DINITX(j+14)
            CONVOLUTION2D_HOR(j + 14, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 14, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +14] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 14] = 255;
                }
            }

            CONV2DINITX(j+15)
            CONVOLUTION2D_HOR(j + 15, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 15, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +15] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 15] = 255;
                }
            }

            CONV2DINITX(j+16)
            CONVOLUTION2D_HOR(j + 16, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 16, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +16] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 16] = 255;
                }
            }

            CONV2DINITX(j+17)
            CONVOLUTION2D_HOR(j + 17, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 17, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +17] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 17] = 255;
                }
            }

            CONV2DINITX(j+18)
            CONVOLUTION2D_HOR(j + 18, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 18, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +18] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 18] = 255;
                }
            }

            CONV2DINITX(j+19)
            CONVOLUTION2D_HOR(j + 19, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 19, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +19] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 19] = 255;
                }
            }

            CONV2DINITX(j+20)
            CONVOLUTION2D_HOR(j + 20, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 20, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +20] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 20] = 255;
                }
            }

            CONV2DINITX(j+21)
            CONVOLUTION2D_HOR(j + 21, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 21, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +21] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 21] = 255;
                }
            }

            CONV2DINITX(j+22)
            CONVOLUTION2D_HOR(j + 22, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 22, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +22] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 22] = 255;
                }
            }

            CONV2DINITX(j+23)
            CONVOLUTION2D_HOR(j + 23, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 23, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +23] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 23] = 255;
                }
            }

            CONV2DINITX(j+24)
            CONVOLUTION2D_HOR(j + 24, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 24, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +24] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 24] = 255;
                }
            }

            CONV2DINITX(j+25)
            CONVOLUTION2D_HOR(j + 25, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 25, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +25] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 25] = 255;
                }
            }

            CONV2DINITX(j+26)
            CONVOLUTION2D_HOR(j + 26, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 26, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +26] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 26] = 255;
                }
            }

            CONV2DINITX(j+27)
            CONVOLUTION2D_HOR(j + 27, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 27, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +27] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 27] = 255;
                }
            }

            CONV2DINITX(j+28)
            CONVOLUTION2D_HOR(j + 28, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 28, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +28] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 28] = 255;
                }
            }

            CONV2DINITX(j+29)
            CONVOLUTION2D_HOR(j + 29, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 29, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +29] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 29] = 255;
                }
            }

            CONV2DINITX(j+30)
            CONVOLUTION2D_HOR(j + 30, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 30, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +30] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 30] = 255;
                }
            }

            CONV2DINITX(j+31)
            CONVOLUTION2D_HOR(j + 31, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 31, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +31] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 31] = 255;
                }
            }

            CONV2DINITX(j+32)
            CONVOLUTION2D_HOR(j + 32, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 32, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +32] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 32] = 255;
                }
            }

            CONV2DINITX(j+33)
            CONVOLUTION2D_HOR(j + 33, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 33, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +33] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 33] = 255;
                }
            }

            CONV2DINITX(j+34)
            CONVOLUTION2D_HOR(j + 34, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 34, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +34] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 34] = 255;
                }
            }

            CONV2DINITX(j+35)
            CONVOLUTION2D_HOR(j + 35, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 35, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +35] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 35] = 255;
                }
            }

            CONV2DINITX(j+36)
            CONVOLUTION2D_HOR(j + 36, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 36, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +36] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 36] = 255;
                }
            }

            CONV2DINITX(j+37)
            CONVOLUTION2D_HOR(j + 37, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 37, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +37] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 37] = 255;
                }
            }

            CONV2DINITX(j+38)
            CONVOLUTION2D_HOR(j + 38, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 38, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +38] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 38] = 255;
                }
            }

            CONV2DINITX(j+39)
            CONVOLUTION2D_HOR(j + 39, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 39, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +39] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 39] = 255;
                }
            }

            CONV2DINITX(j+40)
            CONVOLUTION2D_HOR(j + 40, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 40, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +40] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 40] = 255;
                }
            }

            CONV2DINITX(j+41)
            CONVOLUTION2D_HOR(j + 41, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 41, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +41] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 41] = 255;
                }
            }

            CONV2DINITX(j+42)
            CONVOLUTION2D_HOR(j + 42, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 42, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +42] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 42] = 255;
                }
            }

            CONV2DINITX(j+43)
            CONVOLUTION2D_HOR(j + 43, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 43, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +43] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 43] = 255;
                }
            }

            CONV2DINITX(j+44)
            CONVOLUTION2D_HOR(j + 44, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 44, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +44] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 44] = 255;
                }
            }

            CONV2DINITX(j+45)
            CONVOLUTION2D_HOR(j + 45, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 45, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +45] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 45] = 255;
                }
            }

            CONV2DINITX(j+46)
            CONVOLUTION2D_HOR(j + 46, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 46, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +46] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 46] = 255;
                }
            }

            CONV2DINITX(j+47)
            CONVOLUTION2D_HOR(j + 47, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 47, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +47] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 47] = 255;
                }
            }

            CONV2DINITX(j+48)
            CONVOLUTION2D_HOR(j + 48, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 48, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +48] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 48] = 255;
                }
            }

            CONV2DINITX(j+49)
            CONVOLUTION2D_HOR(j + 49, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 49, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +49] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 49] = 255;
                }
            }

            CONV2DINITX(j+50)
            CONVOLUTION2D_HOR(j + 50, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 50, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +50] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 50] = 255;
                }
            }

            CONV2DINITX(j+51)
            CONVOLUTION2D_HOR(j + 51, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 51, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +51] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 51] = 255;
                }
            }

            CONV2DINITX(j+52)
            CONVOLUTION2D_HOR(j + 52, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 52, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +52] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 52] = 255;
                }
            }

            CONV2DINITX(j+53)
            CONVOLUTION2D_HOR(j + 53, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 53, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +53] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 53] = 255;
                }
            }

            CONV2DINITX(j+54)
            CONVOLUTION2D_HOR(j + 54, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 54, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +54] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 54] = 255;
                }
            }

            CONV2DINITX(j+55)
            CONVOLUTION2D_HOR(j + 55, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 55, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +55] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 55] = 255;
                }
            }

            CONV2DINITX(j+56)
            CONVOLUTION2D_HOR(j + 56, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 56, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +56] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 56] = 255;
                }
            }

            CONV2DINITX(j+57)
            CONVOLUTION2D_HOR(j + 57, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 57, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +57] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 57] = 255;
                }
            }

            CONV2DINITX(j+58)
            CONVOLUTION2D_HOR(j + 58, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 58, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +58] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 58] = 255;
                }
            }

            CONV2DINITX(j+59)
            CONVOLUTION2D_HOR(j + 59, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 59, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +59] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 59] = 255;
                }
            }

            CONV2DINITX(j+60)
            CONVOLUTION2D_HOR(j + 60, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 60, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +60] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 60] = 255;
                }
            }

            CONV2DINITX(j+61)
            CONVOLUTION2D_HOR(j + 61, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 61, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +61] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 61] = 255;
                }
            }

            CONV2DINITX(j+62)
            CONVOLUTION2D_HOR(j + 62, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 62, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +62] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 62] = 255;
                }
            }

            CONV2DINITX(j+63)
            CONVOLUTION2D_HOR(j + 63, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 63, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +63] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 63] = 255;
                }
            }

            CONV2DINITX(j+64)
            CONVOLUTION2D_HOR(j + 64, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 64, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +64] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 64] = 255;
                }
            }

            CONV2DINITX(j+65)
            CONVOLUTION2D_HOR(j + 65, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 65, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +65] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 65] = 255;
                }
            }

            CONV2DINITX(j+66)
            CONVOLUTION2D_HOR(j + 66, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 66, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +66] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 66] = 255;
                }
            }

            CONV2DINITX(j+67)
            CONVOLUTION2D_HOR(j + 67, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 67, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +67] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 67] = 255;
                }
            }

            CONV2DINITX(j+68)
            CONVOLUTION2D_HOR(j + 68, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 68, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +68] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 68] = 255;
                }
            }

            CONV2DINITX(j+69)
            CONVOLUTION2D_HOR(j + 69, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 69, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +69] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 69] = 255;
                }
            }

            CONV2DINITX(j+70)
            CONVOLUTION2D_HOR(j + 70, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 70, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +70] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 70] = 255;
                }
            }

            CONV2DINITX(j+71)
            CONVOLUTION2D_HOR(j + 71, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 71, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +71] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 71] = 255;
                }
            }

            CONV2DINITX(j+72)
            CONVOLUTION2D_HOR(j + 72, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 72, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +72] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 72] = 255;
                }
            }

            CONV2DINITX(j+73)
            CONVOLUTION2D_HOR(j + 73, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 73, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +73] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 73] = 255;
                }
            }

            CONV2DINITX(j+74)
            CONVOLUTION2D_HOR(j + 74, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 74, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +74] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 74] = 255;
                }
            }

            CONV2DINITX(j+75)
            CONVOLUTION2D_HOR(j + 75, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 75, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +75] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 75] = 255;
                }
            }

            CONV2DINITX(j+76)
            CONVOLUTION2D_HOR(j + 76, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 76, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +76] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 76] = 255;
                }
            }

            CONV2DINITX(j+77)
            CONVOLUTION2D_HOR(j + 77, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 77, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +77] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 77] = 255;
                }
            }

            CONV2DINITX(j+78)
            CONVOLUTION2D_HOR(j + 78, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 78, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +78] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 78] = 255;
                }
            }

            CONV2DINITX(j+79)
            CONVOLUTION2D_HOR(j + 79, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 79, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +79] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 79] = 255;
                }
            }

            CONV2DINITX(j+80)
            CONVOLUTION2D_HOR(j + 80, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 80, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +80] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 80] = 255;
                }
            }

            CONV2DINITX(j+81)
            CONVOLUTION2D_HOR(j + 81, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 81, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +81] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 81] = 255;
                }
            }

            CONV2DINITX(j+82)
            CONVOLUTION2D_HOR(j + 82, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 82, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +82] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 82] = 255;
                }
            }

            CONV2DINITX(j+83)
            CONVOLUTION2D_HOR(j + 83, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 83, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +83] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 83] = 255;
                }
            }

            CONV2DINITX(j+84)
            CONVOLUTION2D_HOR(j + 84, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 84, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +84] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 84] = 255;
                }
            }

            CONV2DINITX(j+85)
            CONVOLUTION2D_HOR(j + 85, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 85, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +85] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 85] = 255;
                }
            }

            CONV2DINITX(j+86)
            CONVOLUTION2D_HOR(j + 86, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 86, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +86] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 86] = 255;
                }
            }

            CONV2DINITX(j+87)
            CONVOLUTION2D_HOR(j + 87, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 87, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +87] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 87] = 255;
                }
            }

            CONV2DINITX(j+88)
            CONVOLUTION2D_HOR(j + 88, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 88, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +88] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 88] = 255;
                }
            }

            CONV2DINITX(j+89)
            CONVOLUTION2D_HOR(j + 89, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 89, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +89] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 89] = 255;
                }
            }

            CONV2DINITX(j+90)
            CONVOLUTION2D_HOR(j + 90, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 90, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +90] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 90] = 255;
                }
            }

            CONV2DINITX(j+91)
            CONVOLUTION2D_HOR(j + 91, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 91, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +91] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 91] = 255;
                }
            }

            CONV2DINITX(j+92)
            CONVOLUTION2D_HOR(j + 92, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 92, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +92] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 92] = 255;
                }
            }

            CONV2DINITX(j+93)
            CONVOLUTION2D_HOR(j + 93, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 93, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +93] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 93] = 255;
                }
            }

            CONV2DINITX(j+94)
            CONVOLUTION2D_HOR(j + 94, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 94, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +94] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 94] = 255;
                }
            }

            CONV2DINITX(j+95)
            CONVOLUTION2D_HOR(j + 95, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 95, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +95] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 95] = 255;
                }
            }

            CONV2DINITX(j+96)
            CONVOLUTION2D_HOR(j + 96, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 96, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +96] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 96] = 255;
                }
            }

            CONV2DINITX(j+97)
            CONVOLUTION2D_HOR(j + 97, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 97, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +97] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 97] = 255;
                }
            }

            CONV2DINITX(j+98)
            CONVOLUTION2D_HOR(j + 98, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 98, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +98] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 98] = 255;
                }
            }

            CONV2DINITX(j+99)
            CONVOLUTION2D_HOR(j + 99, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 99, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +99] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 99] = 255;
                }
            }

            CONV2DINITX(j+100)
            CONVOLUTION2D_HOR(j + 100, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 100, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +100] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 100] = 255;
                }
            }

            CONV2DINITX(j+101)
            CONVOLUTION2D_HOR(j + 101, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 101, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +101] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 101] = 255;
                }
            }

            CONV2DINITX(j+102)
            CONVOLUTION2D_HOR(j + 102, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 102, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +102] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 102] = 255;
                }
            }

            CONV2DINITX(j+103)
            CONVOLUTION2D_HOR(j + 103, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 103, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +103] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 103] = 255;
                }
            }

            CONV2DINITX(j+104)
            CONVOLUTION2D_HOR(j + 104, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 104, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +104] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 104] = 255;
                }
            }

            CONV2DINITX(j+105)
            CONVOLUTION2D_HOR(j + 105, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 105, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +105] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 105] = 255;
                }
            }

            CONV2DINITX(j+106)
            CONVOLUTION2D_HOR(j + 106, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 106, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +106] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 106] = 255;
                }
            }

            CONV2DINITX(j+107)
            CONVOLUTION2D_HOR(j + 107, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 107, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +107] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 107] = 255;
                }
            }

            CONV2DINITX(j+108)
            CONVOLUTION2D_HOR(j + 108, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 108, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +108] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 108] = 255;
                }
            }

            CONV2DINITX(j+109)
            CONVOLUTION2D_HOR(j + 109, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 109, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +109] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 109] = 255;
                }
            }

            CONV2DINITX(j+110)
            CONVOLUTION2D_HOR(j + 110, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 110, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +110] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 110] = 255;
                }
            }

            CONV2DINITX(j+111)
            CONVOLUTION2D_HOR(j + 111, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 111, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +111] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 111] = 255;
                }
            }

            CONV2DINITX(j+112)
            CONVOLUTION2D_HOR(j + 112, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 112, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +112] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 112] = 255;
                }
            }

            CONV2DINITX(j+113)
            CONVOLUTION2D_HOR(j + 113, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 113, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +113] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 113] = 255;
                }
            }

            CONV2DINITX(j+114)
            CONVOLUTION2D_HOR(j + 114, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 114, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +114] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 114] = 255;
                }
            }

            CONV2DINITX(j+115)
            CONVOLUTION2D_HOR(j + 115, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 115, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +115] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 115] = 255;
                }
            }

            CONV2DINITX(j+116)
            CONVOLUTION2D_HOR(j + 116, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 116, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +116] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 116] = 255;
                }
            }

            CONV2DINITX(j+117)
            CONVOLUTION2D_HOR(j + 117, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 117, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +117] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 117] = 255;
                }
            }

            CONV2DINITX(j+118)
            CONVOLUTION2D_HOR(j + 118, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 118, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +118] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 118] = 255;
                }
            }

            CONV2DINITX(j+119)
            CONVOLUTION2D_HOR(j + 119, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 119, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +119] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 119] = 255;
                }
            }

            CONV2DINITX(j+120)
            CONVOLUTION2D_HOR(j + 120, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 120, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +120] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 120] = 255;
                }
            }

            CONV2DINITX(j+121)
            CONVOLUTION2D_HOR(j + 121, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 121, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +121] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 121] = 255;
                }
            }

            CONV2DINITX(j+122)
            CONVOLUTION2D_HOR(j + 122, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 122, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +122] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 122] = 255;
                }
            }

            CONV2DINITX(j+123)
            CONVOLUTION2D_HOR(j + 123, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 123, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +123] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 123] = 255;
                }
            }

            CONV2DINITX(j+124)
            CONVOLUTION2D_HOR(j + 124, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 124, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +124] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 124] = 255;
                }
            }

            CONV2DINITX(j+125)
            CONVOLUTION2D_HOR(j + 125, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 125, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +125] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 125] = 255;
                }
            }

            CONV2DINITX(j+126)
            CONVOLUTION2D_HOR(j + 126, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 126, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +126] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 126] = 255;
                }
            }

            CONV2DINITX(j+127)
            CONVOLUTION2D_HOR(j + 127, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 127, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +127] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 127] = 255;
                }
            }

            CONV2DINITX(j+128)
            CONVOLUTION2D_HOR(j + 128, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 128, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +128] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 128] = 255;
                }
            }

            CONV2DINITX(j+129)
            CONVOLUTION2D_HOR(j + 129, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 129, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +129] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 129] = 255;
                }
            }

            CONV2DINITX(j+130)
            CONVOLUTION2D_HOR(j + 130, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 130, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +130] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 130] = 255;
                }
            }

            CONV2DINITX(j+131)
            CONVOLUTION2D_HOR(j + 131, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 131, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +131] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 131] = 255;
                }
            }

            CONV2DINITX(j+132)
            CONVOLUTION2D_HOR(j + 132, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 132, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +132] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 132] = 255;
                }
            }

            CONV2DINITX(j+133)
            CONVOLUTION2D_HOR(j + 133, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 133, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +133] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 133] = 255;
                }
            }

            CONV2DINITX(j+134)
            CONVOLUTION2D_HOR(j + 134, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 134, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +134] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 134] = 255;
                }
            }

            CONV2DINITX(j+135)
            CONVOLUTION2D_HOR(j + 135, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 135, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +135] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 135] = 255;
                }
            }

            CONV2DINITX(j+136)
            CONVOLUTION2D_HOR(j + 136, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 136, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +136] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 136] = 255;
                }
            }

            CONV2DINITX(j+137)
            CONVOLUTION2D_HOR(j + 137, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 137, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +137] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 137] = 255;
                }
            }

            CONV2DINITX(j+138)
            CONVOLUTION2D_HOR(j + 138, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 138, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +138] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 138] = 255;
                }
            }

            CONV2DINITX(j+139)
            CONVOLUTION2D_HOR(j + 139, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 139, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +139] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 139] = 255;
                }
            }

            CONV2DINITX(j+140)
            CONVOLUTION2D_HOR(j + 140, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 140, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +140] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 140] = 255;
                }
            }

            CONV2DINITX(j+141)
            CONVOLUTION2D_HOR(j + 141, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 141, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +141] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 141] = 255;
                }
            }

            CONV2DINITX(j+142)
            CONVOLUTION2D_HOR(j + 142, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 142, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +142] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 142] = 255;
                }
            }

            CONV2DINITX(j+143)
            CONVOLUTION2D_HOR(j + 143, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 143, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +143] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 143] = 255;
                }
            }

            CONV2DINITX(j+144)
            CONVOLUTION2D_HOR(j + 144, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 144, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +144] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 144] = 255;
                }
            }

            CONV2DINITX(j+145)
            CONVOLUTION2D_HOR(j + 145, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 145, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +145] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 145] = 255;
                }
            }

            CONV2DINITX(j+146)
            CONVOLUTION2D_HOR(j + 146, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 146, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +146] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 146] = 255;
                }
            }

            CONV2DINITX(j+147)
            CONVOLUTION2D_HOR(j + 147, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 147, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +147] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 147] = 255;
                }
            }

            CONV2DINITX(j+148)
            CONVOLUTION2D_HOR(j + 148, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 148, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +148] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 148] = 255;
                }
            }

            CONV2DINITX(j+149)
            CONVOLUTION2D_HOR(j + 149, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 149, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +149] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 149] = 255;
                }
            }

            CONV2DINITX(j+150)
            CONVOLUTION2D_HOR(j + 150, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 150, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +150] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 150] = 255;
                }
            }

            CONV2DINITX(j+151)
            CONVOLUTION2D_HOR(j + 151, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 151, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +151] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 151] = 255;
                }
            }

            CONV2DINITX(j+152)
            CONVOLUTION2D_HOR(j + 152, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 152, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +152] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 152] = 255;
                }
            }

            CONV2DINITX(j+153)
            CONVOLUTION2D_HOR(j + 153, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 153, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +153] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 153] = 255;
                }
            }

            CONV2DINITX(j+154)
            CONVOLUTION2D_HOR(j + 154, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 154, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +154] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 154] = 255;
                }
            }

            CONV2DINITX(j+155)
            CONVOLUTION2D_HOR(j + 155, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 155, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +155] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 155] = 255;
                }
            }

            CONV2DINITX(j+156)
            CONVOLUTION2D_HOR(j + 156, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 156, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +156] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 156] = 255;
                }
            }

            CONV2DINITX(j+157)
            CONVOLUTION2D_HOR(j + 157, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 157, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +157] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 157] = 255;
                }
            }

            CONV2DINITX(j+158)
            CONVOLUTION2D_HOR(j + 158, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 158, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +158] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 158] = 255;
                }
            }

            CONV2DINITX(j+159)
            CONVOLUTION2D_HOR(j + 159, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 159, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +159] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 159] = 255;
                }
            }

            CONV2DINITX(j+160)
            CONVOLUTION2D_HOR(j + 160, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 160, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +160] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 160] = 255;
                }
            }

            CONV2DINITX(j+161)
            CONVOLUTION2D_HOR(j + 161, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 161, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +161] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 161] = 255;
                }
            }

            CONV2DINITX(j+162)
            CONVOLUTION2D_HOR(j + 162, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 162, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +162] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 162] = 255;
                }
            }

            CONV2DINITX(j+163)
            CONVOLUTION2D_HOR(j + 163, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 163, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +163] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 163] = 255;
                }
            }

            CONV2DINITX(j+164)
            CONVOLUTION2D_HOR(j + 164, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 164, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +164] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 164] = 255;
                }
            }

            CONV2DINITX(j+165)
            CONVOLUTION2D_HOR(j + 165, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 165, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +165] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 165] = 255;
                }
            }

            CONV2DINITX(j+166)
            CONVOLUTION2D_HOR(j + 166, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 166, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +166] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 166] = 255;
                }
            }

            CONV2DINITX(j+167)
            CONVOLUTION2D_HOR(j + 167, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 167, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +167] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 167] = 255;
                }
            }

            CONV2DINITX(j+168)
            CONVOLUTION2D_HOR(j + 168, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 168, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +168] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 168] = 255;
                }
            }

            CONV2DINITX(j+169)
            CONVOLUTION2D_HOR(j + 169, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 169, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +169] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 169] = 255;
                }
            }

            CONV2DINITX(j+170)
            CONVOLUTION2D_HOR(j + 170, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 170, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +170] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 170] = 255;
                }
            }

            CONV2DINITX(j+171)
            CONVOLUTION2D_HOR(j + 171, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 171, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +171] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 171] = 255;
                }
            }

            CONV2DINITX(j+172)
            CONVOLUTION2D_HOR(j + 172, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 172, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +172] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 172] = 255;
                }
            }

            CONV2DINITX(j+173)
            CONVOLUTION2D_HOR(j + 173, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 173, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +173] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 173] = 255;
                }
            }

            CONV2DINITX(j+174)
            CONVOLUTION2D_HOR(j + 174, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 174, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +174] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 174] = 255;
                }
            }

            CONV2DINITX(j+175)
            CONVOLUTION2D_HOR(j + 175, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 175, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +175] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 175] = 255;
                }
            }

            CONV2DINITX(j+176)
            CONVOLUTION2D_HOR(j + 176, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 176, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +176] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 176] = 255;
                }
            }

            CONV2DINITX(j+177)
            CONVOLUTION2D_HOR(j + 177, input, conv_res_hor)
            CONVOLUTION2D_VER(j + 177, input, conv_res_ver)
            p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;
            res = ((int)p) > SQUARE255;
            switch(res){
                case 0:{
                    output[index + j +177] = (unsigned char)((int)sqrt(p));
                    break;
                }
                default:{
                    output[index + j + 177] = 255;
                }
            }


        }
    }

    /* Now run through the output and the golden output to calculate *
     * the MSE and then the PSNR.                                    */
    for (i=1; i<SIZE-1; i++) {
        index = i << 12;
        for ( j=1; j<SIZE-1; j+=178) {
            t = output[index+j] - golden[index+j];
            PSNR += t*t;

            t = output[index+j+1] - golden[index+j+1];
            PSNR += t*t;

            t = output[index+j+2] - golden[index+j+2];
            PSNR += t*t;

            t = output[index+j+3] - golden[index+j+3];
            PSNR += t*t;

            t = output[index+j+4] - golden[index+j+4];
            PSNR += t*t;

            t = output[index+j+5] - golden[index+j+5];
            PSNR += t*t;

            t = output[index+j+6] - golden[index+j+6];
            PSNR += t*t;

            t = output[index+j+7] - golden[index+j+7];
            PSNR += t*t;

            t = output[index+j+8] - golden[index+j+8];
            PSNR += t*t;

            t = output[index+j+9] - golden[index+j+9];
            PSNR += t*t;

            t = output[index+j+10] - golden[index+j+10];
            PSNR += t*t;

            t = output[index+j+11] - golden[index+j+11];
            PSNR += t*t;

            t = output[index+j+12] - golden[index+j+12];
            PSNR += t*t;

            t = output[index+j+13] - golden[index+j+13];
            PSNR += t*t;

            t = output[index+j+14] - golden[index+j+14];
            PSNR += t*t;

            t = output[index+j+15] - golden[index+j+15];
            PSNR += t*t;

            t = output[index+j+16] - golden[index+j+16];
            PSNR += t*t;

            t = output[index+j+17] - golden[index+j+17];
            PSNR += t*t;

            t = output[index+j+18] - golden[index+j+18];
            PSNR += t*t;

            t = output[index+j+19] - golden[index+j+19];
            PSNR += t*t;

            t = output[index+j+20] - golden[index+j+20];
            PSNR += t*t;

            t = output[index+j+21] - golden[index+j+21];
            PSNR += t*t;

            t = output[index+j+22] - golden[index+j+22];
            PSNR += t*t;

            t = output[index+j+23] - golden[index+j+23];
            PSNR += t*t;

            t = output[index+j+24] - golden[index+j+24];
            PSNR += t*t;

            t = output[index+j+25] - golden[index+j+25];
            PSNR += t*t;

            t = output[index+j+26] - golden[index+j+26];
            PSNR += t*t;

            t = output[index+j+27] - golden[index+j+27];
            PSNR += t*t;

            t = output[index+j+28] - golden[index+j+28];
            PSNR += t*t;

            t = output[index+j+29] - golden[index+j+29];
            PSNR += t*t;

            t = output[index+j+30] - golden[index+j+30];
            PSNR += t*t;

            t = output[index+j+31] - golden[index+j+31];
            PSNR += t*t;

            t = output[index+j+32] - golden[index+j+32];
            PSNR += t*t;

            t = output[index+j+33] - golden[index+j+33];
            PSNR += t*t;

            t = output[index+j+34] - golden[index+j+34];
            PSNR += t*t;

            t = output[index+j+35] - golden[index+j+35];
            PSNR += t*t;

            t = output[index+j+36] - golden[index+j+36];
            PSNR += t*t;

            t = output[index+j+37] - golden[index+j+37];
            PSNR += t*t;

            t = output[index+j+38] - golden[index+j+38];
            PSNR += t*t;

            t = output[index+j+39] - golden[index+j+39];
            PSNR += t*t;

            t = output[index+j+40] - golden[index+j+40];
            PSNR += t*t;

            t = output[index+j+41] - golden[index+j+41];
            PSNR += t*t;

            t = output[index+j+42] - golden[index+j+42];
            PSNR += t*t;

            t = output[index+j+43] - golden[index+j+43];
            PSNR += t*t;

            t = output[index+j+44] - golden[index+j+44];
            PSNR += t*t;

            t = output[index+j+45] - golden[index+j+45];
            PSNR += t*t;

            t = output[index+j+46] - golden[index+j+46];
            PSNR += t*t;

            t = output[index+j+47] - golden[index+j+47];
            PSNR += t*t;

            t = output[index+j+48] - golden[index+j+48];
            PSNR += t*t;

            t = output[index+j+49] - golden[index+j+49];
            PSNR += t*t;

            t = output[index+j+50] - golden[index+j+50];
            PSNR += t*t;

            t = output[index+j+51] - golden[index+j+51];
            PSNR += t*t;

            t = output[index+j+52] - golden[index+j+52];
            PSNR += t*t;

            t = output[index+j+53] - golden[index+j+53];
            PSNR += t*t;

            t = output[index+j+54] - golden[index+j+54];
            PSNR += t*t;

            t = output[index+j+55] - golden[index+j+55];
            PSNR += t*t;

            t = output[index+j+56] - golden[index+j+56];
            PSNR += t*t;

            t = output[index+j+57] - golden[index+j+57];
            PSNR += t*t;

            t = output[index+j+58] - golden[index+j+58];
            PSNR += t*t;

            t = output[index+j+59] - golden[index+j+59];
            PSNR += t*t;

            t = output[index+j+60] - golden[index+j+60];
            PSNR += t*t;

            t = output[index+j+61] - golden[index+j+61];
            PSNR += t*t;

            t = output[index+j+62] - golden[index+j+62];
            PSNR += t*t;

            t = output[index+j+63] - golden[index+j+63];
            PSNR += t*t;

            t = output[index+j+64] - golden[index+j+64];
            PSNR += t*t;

            t = output[index+j+65] - golden[index+j+65];
            PSNR += t*t;

            t = output[index+j+66] - golden[index+j+66];
            PSNR += t*t;

            t = output[index+j+67] - golden[index+j+67];
            PSNR += t*t;

            t = output[index+j+68] - golden[index+j+68];
            PSNR += t*t;

            t = output[index+j+69] - golden[index+j+69];
            PSNR += t*t;

            t = output[index+j+70] - golden[index+j+70];
            PSNR += t*t;

            t = output[index+j+71] - golden[index+j+71];
            PSNR += t*t;

            t = output[index+j+72] - golden[index+j+72];
            PSNR += t*t;

            t = output[index+j+73] - golden[index+j+73];
            PSNR += t*t;

            t = output[index+j+74] - golden[index+j+74];
            PSNR += t*t;

            t = output[index+j+75] - golden[index+j+75];
            PSNR += t*t;

            t = output[index+j+76] - golden[index+j+76];
            PSNR += t*t;

            t = output[index+j+77] - golden[index+j+77];
            PSNR += t*t;

            t = output[index+j+78] - golden[index+j+78];
            PSNR += t*t;

            t = output[index+j+79] - golden[index+j+79];
            PSNR += t*t;

            t = output[index+j+80] - golden[index+j+80];
            PSNR += t*t;

            t = output[index+j+81] - golden[index+j+81];
            PSNR += t*t;

            t = output[index+j+82] - golden[index+j+82];
            PSNR += t*t;

            t = output[index+j+83] - golden[index+j+83];
            PSNR += t*t;

            t = output[index+j+84] - golden[index+j+84];
            PSNR += t*t;

            t = output[index+j+85] - golden[index+j+85];
            PSNR += t*t;

            t = output[index+j+86] - golden[index+j+86];
            PSNR += t*t;

            t = output[index+j+87] - golden[index+j+87];
            PSNR += t*t;

            t = output[index+j+88] - golden[index+j+88];
            PSNR += t*t;

            t = output[index+j+89] - golden[index+j+89];
            PSNR += t*t;

            t = output[index+j+90] - golden[index+j+90];
            PSNR += t*t;

            t = output[index+j+91] - golden[index+j+91];
            PSNR += t*t;

            t = output[index+j+92] - golden[index+j+92];
            PSNR += t*t;

            t = output[index+j+93] - golden[index+j+93];
            PSNR += t*t;

            t = output[index+j+94] - golden[index+j+94];
            PSNR += t*t;

            t = output[index+j+95] - golden[index+j+95];
            PSNR += t*t;

            t = output[index+j+96] - golden[index+j+96];
            PSNR += t*t;

            t = output[index+j+97] - golden[index+j+97];
            PSNR += t*t;

            t = output[index+j+98] - golden[index+j+98];
            PSNR += t*t;

            t = output[index+j+99] - golden[index+j+99];
            PSNR += t*t;

            t = output[index+j+100] - golden[index+j+100];
            PSNR += t*t;

            t = output[index+j+101] - golden[index+j+101];
            PSNR += t*t;

            t = output[index+j+102] - golden[index+j+102];
            PSNR += t*t;

            t = output[index+j+103] - golden[index+j+103];
            PSNR += t*t;

            t = output[index+j+104] - golden[index+j+104];
            PSNR += t*t;

            t = output[index+j+105] - golden[index+j+105];
            PSNR += t*t;

            t = output[index+j+106] - golden[index+j+106];
            PSNR += t*t;

            t = output[index+j+107] - golden[index+j+107];
            PSNR += t*t;

            t = output[index+j+108] - golden[index+j+108];
            PSNR += t*t;

            t = output[index+j+109] - golden[index+j+109];
            PSNR += t*t;

            t = output[index+j+110] - golden[index+j+110];
            PSNR += t*t;

            t = output[index+j+111] - golden[index+j+111];
            PSNR += t*t;

            t = output[index+j+112] - golden[index+j+112];
            PSNR += t*t;

            t = output[index+j+113] - golden[index+j+113];
            PSNR += t*t;

            t = output[index+j+114] - golden[index+j+114];
            PSNR += t*t;

            t = output[index+j+115] - golden[index+j+115];
            PSNR += t*t;

            t = output[index+j+116] - golden[index+j+116];
            PSNR += t*t;

            t = output[index+j+117] - golden[index+j+117];
            PSNR += t*t;

            t = output[index+j+118] - golden[index+j+118];
            PSNR += t*t;

            t = output[index+j+119] - golden[index+j+119];
            PSNR += t*t;

            t = output[index+j+120] - golden[index+j+120];
            PSNR += t*t;

            t = output[index+j+121] - golden[index+j+121];
            PSNR += t*t;

            t = output[index+j+122] - golden[index+j+122];
            PSNR += t*t;

            t = output[index+j+123] - golden[index+j+123];
            PSNR += t*t;

            t = output[index+j+124] - golden[index+j+124];
            PSNR += t*t;

            t = output[index+j+125] - golden[index+j+125];
            PSNR += t*t;

            t = output[index+j+126] - golden[index+j+126];
            PSNR += t*t;

            t = output[index+j+127] - golden[index+j+127];
            PSNR += t*t;

            t = output[index+j+128] - golden[index+j+128];
            PSNR += t*t;

            t = output[index+j+129] - golden[index+j+129];
            PSNR += t*t;

            t = output[index+j+130] - golden[index+j+130];
            PSNR += t*t;

            t = output[index+j+131] - golden[index+j+131];
            PSNR += t*t;

            t = output[index+j+132] - golden[index+j+132];
            PSNR += t*t;

            t = output[index+j+133] - golden[index+j+133];
            PSNR += t*t;

            t = output[index+j+134] - golden[index+j+134];
            PSNR += t*t;

            t = output[index+j+135] - golden[index+j+135];
            PSNR += t*t;

            t = output[index+j+136] - golden[index+j+136];
            PSNR += t*t;

            t = output[index+j+137] - golden[index+j+137];
            PSNR += t*t;

            t = output[index+j+138] - golden[index+j+138];
            PSNR += t*t;

            t = output[index+j+139] - golden[index+j+139];
            PSNR += t*t;

            t = output[index+j+140] - golden[index+j+140];
            PSNR += t*t;

            t = output[index+j+141] - golden[index+j+141];
            PSNR += t*t;

            t = output[index+j+142] - golden[index+j+142];
            PSNR += t*t;

            t = output[index+j+143] - golden[index+j+143];
            PSNR += t*t;

            t = output[index+j+144] - golden[index+j+144];
            PSNR += t*t;

            t = output[index+j+145] - golden[index+j+145];
            PSNR += t*t;

            t = output[index+j+146] - golden[index+j+146];
            PSNR += t*t;

            t = output[index+j+147] - golden[index+j+147];
            PSNR += t*t;

            t = output[index+j+148] - golden[index+j+148];
            PSNR += t*t;

            t = output[index+j+149] - golden[index+j+149];
            PSNR += t*t;

            t = output[index+j+150] - golden[index+j+150];
            PSNR += t*t;

            t = output[index+j+151] - golden[index+j+151];
            PSNR += t*t;

            t = output[index+j+152] - golden[index+j+152];
            PSNR += t*t;

            t = output[index+j+153] - golden[index+j+153];
            PSNR += t*t;

            t = output[index+j+154] - golden[index+j+154];
            PSNR += t*t;

            t = output[index+j+155] - golden[index+j+155];
            PSNR += t*t;

            t = output[index+j+156] - golden[index+j+156];
            PSNR += t*t;

            t = output[index+j+157] - golden[index+j+157];
            PSNR += t*t;

            t = output[index+j+158] - golden[index+j+158];
            PSNR += t*t;

            t = output[index+j+159] - golden[index+j+159];
            PSNR += t*t;

            t = output[index+j+160] - golden[index+j+160];
            PSNR += t*t;

            t = output[index+j+161] - golden[index+j+161];
            PSNR += t*t;

            t = output[index+j+162] - golden[index+j+162];
            PSNR += t*t;

            t = output[index+j+163] - golden[index+j+163];
            PSNR += t*t;

            t = output[index+j+164] - golden[index+j+164];
            PSNR += t*t;

            t = output[index+j+165] - golden[index+j+165];
            PSNR += t*t;

            t = output[index+j+166] - golden[index+j+166];
            PSNR += t*t;

            t = output[index+j+167] - golden[index+j+167];
            PSNR += t*t;

            t = output[index+j+168] - golden[index+j+168];
            PSNR += t*t;

            t = output[index+j+169] - golden[index+j+169];
            PSNR += t*t;

            t = output[index+j+170] - golden[index+j+170];
            PSNR += t*t;

            t = output[index+j+171] - golden[index+j+171];
            PSNR += t*t;

            t = output[index+j+172] - golden[index+j+172];
            PSNR += t*t;

            t = output[index+j+173] - golden[index+j+173];
            PSNR += t*t;

            t = output[index+j+174] - golden[index+j+174];
            PSNR += t*t;

            t = output[index+j+175] - golden[index+j+175];
            PSNR += t*t;

            t = output[index+j+176] - golden[index+j+176];
            PSNR += t*t;

            t = output[index+j+177] - golden[index+j+177];
            PSNR += t*t;


        }
    }
  
    PSNR /= (double)(SIZE_SQUARED);
    PSNR = 10*log10(65536/PSNR);


    /* This is the end of the main computation. Take the end time,  *
     * calculate the duration of the computation and report it.     */
    clock_gettime(CLOCK_MONOTONIC_RAW, &tv2);
    printf ("%10g, %g\n", ((double) (tv2.tv_nsec - tv1.tv_nsec) / 1000000000.0 + (double) (tv2.tv_sec - tv1.tv_sec)), PSNR);

  
    /* Write the output file */
    fwrite(output, sizeof(unsigned char), SIZE_SQUARED, f_out);
    fclose(f_out);
  
    return PSNR;
}


int main(int argc, char* argv[])
{
    const char syntax[] = "executable <outputfilename>";
    // double PSNR;
    FILE *f_in,  *f_golden;


    if (argc < 2)
    {
        printf("ERROR: Not enough arguments!\n");
        printf("Syntax: %s\n", syntax);

        return -1;
    }


    /* Open the input, golden files, read the input and golden            *
    * and store them to the corresponding arrays.                        */
    f_in = fopen(INPUT_FILE, "r");
    if (f_in == NULL) {
        printf("File " INPUT_FILE " not found\n");
        exit(1);
    }

    f_golden = fopen(GOLDEN_FILE, "r");
    if (f_golden == NULL) {
        printf("File " GOLDEN_FILE " not found\n");
        fclose(f_in);
        exit(1);
    }

    fread(input, sizeof(unsigned char), SIZE_SQUARED, f_in);
    fread(golden, sizeof(unsigned char), SIZE_SQUARED, f_golden);
    fclose(f_in);
    fclose(f_golden);

    outputfilename = strdup(argv[1]);

    sobel(input, output, golden);
    // printf("PSNR of original Sobel and computed Sobel image: %g\n", PSNR);
    // printf("A visualization of the sobel filter can be found at " OUTPUT_FILE ", or you can run 'make image' to get the jpg\n");

    free(outputfilename);

    return 0;
}

