// This will apply the sobel filter and return the PSNR between the golden sobel and the produced sobel
// sobelized image
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define SIZE    4096
#define INPUT_FILE  "input/input.grey"
// #define OUTPUT_FILE "output_sobel.grey"
#define GOLDEN_FILE "input/golden.grey"

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
char hor_operator[3][3] = {{-1, 0, 1},
													 {-2, 0, 2},
													 {-1, 0, 1}};
char ver_operator[3][3] = {{1, 2, 1},
													 {0, 0, 0},
													 {-1, -2, -1}};

double sobel(unsigned char *input, unsigned char *output, unsigned char *golden);

/* The arrays holding the input image, the output image and the output used *
 * as golden standard. The luminosity (intensity) of each pixel in the      *
 * grayscale image is represented by a value between 0 and 255 (an unsigned *
 * character). The arrays (and the files) contain these values in row-major *
 * order (element after element within each row and row after row.          */
unsigned char input[SIZE*SIZE], output[SIZE*SIZE], golden[SIZE*SIZE];

char *outputfilename = NULL;


/* The main computational function of the program. The input, output and *
 * golden arguments are pointers to the arrays used to store the input   *
 * image, the output produced by the algorithm and the output used as    *
 * golden standard for the comparisons.                                  */
double sobel(unsigned char *input, unsigned char *output, unsigned char *golden)
{
	double PSNR = 0, t;
	int i, j, index;
	unsigned int p;
	int res, conv_res_hor, conv_res_ver;
	struct timespec  tv1, tv2;
	FILE *f_in, *f_out, *f_golden;
	int posy_min1, posy_plus1, posy_size, posx_min1, posx_plus1;

	// outputfilename = realloc(outputfilename, (strlen(outputfilename) + strlen(OUTPUT_FILE) + 1));
	// outputfilename = strcat(outputfilename, OUTPUT_FILE);

	/* The first and last row of the output array, as well as the first  *
	 * and last element of each column are not going to be filled by the *
	 * algorithm, therefore make sure to initialize them with 0s.        */
	memset(output, 0, SIZE*sizeof(unsigned char));
	memset(&output[SIZE*(SIZE-1)], 0, SIZE*sizeof(unsigned char));
	for (i = 1; i < SIZE-1; i++) {
			output[i*SIZE] = 0;
			output[i*SIZE + SIZE - 1] = 0;
	}

	/* Open the input, output, golden files, read the input and golden    *
	 * and store them to the corresponding arrays.                        */
	f_in = fopen(INPUT_FILE, "r");
	if (f_in == NULL) {
			printf("File " INPUT_FILE " not found\n");
			exit(1);
	}

	f_out = fopen(outputfilename, "wb");
	if (f_out == NULL) {
		printf("File %s could not be created\n", outputfilename);
				fclose(f_in);
		exit(1);
	}

	f_out = fopen(outputfilename, "wb");
	if (f_out == NULL) {
		printf("File %s could not be created\n", outputfilename);
		fclose(f_in);
		exit(1);
	}

	f_golden = fopen(GOLDEN_FILE, "r");
	if (f_golden == NULL) {
			printf("File " GOLDEN_FILE " not found\n");
			fclose(f_in);
			fclose(f_out);
			exit(1);
	}

	fread(input, sizeof(unsigned char), SIZE*SIZE, f_in);
	fread(golden, sizeof(unsigned char), SIZE*SIZE, f_golden);
	fclose(f_in);
	fclose(f_golden);

	/* This is the main computation. Get the starting time. */
	clock_gettime(CLOCK_MONOTONIC_RAW, &tv1);
	/* For each pixel of the output image */
	for (i=1; i<SIZE-1; i+=1) {
			index = i << 12;
			CONV2DINITY(i)
			for (j=1; j<SIZE-1; j+=1 ) {
					/* Apply the sobel filter and calculate the magnitude *
					 * of the derivative.
					*/
					CONV2DINITX(j)
					CONVOLUTION2D_HOR(j, input, conv_res_hor)
					CONVOLUTION2D_VER(j, input, conv_res_ver)

					p = conv_res_hor * conv_res_hor + conv_res_ver * conv_res_ver;

					res = (int)sqrt(p);
					/* If the resulting value is greater than 255, clip it *
					 * to 255.                                             */
					if (res > 255)
							output[index + j] = 255;
					else
							output[index + j] = (unsigned char)res;
			}
	}

	/* Now run through the output and the golden output to calculate *
	 * the MSE and then the PSNR.                                    */
	for (i=1; i<SIZE-1; i++) {
			index = i << 12;
			for ( j=1; j<SIZE-1; j++ ) {

					t = output[index+j] - golden[index+j];
					PSNR += t*t;
			}
	}

	
	PSNR /= (double)(SIZE*SIZE);
	PSNR = 10*log10(65536/PSNR);

	/* This is the end of the main computation. Take the end time,  *
	 * calculate the duration of the computation and report it.     */
	clock_gettime(CLOCK_MONOTONIC_RAW, &tv2);

	printf ("%10g, %g\n", ((double) (tv2.tv_nsec - tv1.tv_nsec) / 1000000000.0 + (double) (tv2.tv_sec - tv1.tv_sec)), PSNR);

	// printf ("Total time = %10g seconds\n",
	//         (double) (tv2.tv_nsec - tv1.tv_nsec) / 1000000000.0 +
	//         (double) (tv2.tv_sec - tv1.tv_sec));


	/* Write the output file */
	fwrite(output, sizeof(unsigned char), SIZE*SIZE, f_out);
	fclose(f_out);

	return PSNR;
}


int main(int argc, char* argv[])
{
	const char syntax[] = "executable <outputfilename>";
	double PSNR;


	if (argc < 2)
	{
		printf("ERROR: Not enough arguments!\n");
		printf("Syntax: %s\n", syntax);

		return -1;
	}

	outputfilename = strdup(argv[1]);

	PSNR = sobel(input, output, golden);
	// printf("PSNR of original Sobel and computed Sobel image: %g\n", PSNR);
	// printf("A visualization of the sobel filter can be found at " OUTPUT_FILE ", or you can run 'make image' to get the jpg\n");

	free(outputfilename);

	return 0;
}
