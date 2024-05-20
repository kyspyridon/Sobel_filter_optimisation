###############################################################
#              ECE415 High Performance Computing              #
###############################################################
#                            Lab 1                            #
#                                                             #
# Project: Optimizing the performance of a grayscale Sobel    #
#          filter                                             #
#                                                             #
# File Description: Makefile to be called through the 				#
#										run_make.sh script. It produces an 				#
#  							 		executable file depending on a varying 		#
# 									set of optimization parameters. It is   	#
# 									important that the Makefile is inside the #
# 									sources subdirectory to function properly #
#                                                             #
# Authors: Kyritsis Spyridon     AEM: 2697                    #
#          Karaiskos Charalampos AEM: 2765                    #
###############################################################


# All parameters have default values in case the make is called #
# in standalone form i.e. not from the run_make.sh script 			#
# In case the Makefile is called from the run_make.sh script 		#
# the default parameters are overriden by the script's ones 		#

# Directory Variables
SOURCE_DIR=source
BUILD_DIR=build
OUTPUT_DIR=output
TEST_DIR=

#
# This is the compiler to use
CC=icc
# CC = gcc

# These are the flags passed to the compiler. Change accordingly
DFLAG = # Debug flag; start empty initialize if necessary further down
CFLAGS=-Wall
OFLAG =-O0

# These are the flags passed to the linker. Nothing in our case
LDFLAGS=-lm -fopenmp

DEBUG=0
ifeq ($(DEBUG), 1)
	DFLAG+=-g
endif

$(BUILD_DIR)/$(EXECUTABLE): $(SOURCE_DIR)/$(TEST_DIR)/sobel_orig.c
	$(CC) $(CFLAGS) $(DFLAG) $(OFLAG) $< -o $@ $(LDFLAGS)

all:
	time ./run_make.sh

run:
	time ./run_script.sh

# make image will create the output_sobel.jpg from the output_sobel.grey.
# Remember to change this rule if you change the name of the output file.
image:
	time ./convert.sh

excel:
	python csv_to_Excel.py source/ build/ output/

# make clean will remove all executables, jpg files and the
# output of previous executions.
clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(OUTPUT_DIR)/*
