#!/bin/bash

###############################################################
#              ECE415 High Performance Computing              #
###############################################################
#                            Lab 1                            #
#                                                             #
# Project: Optimizing the performance of a grayscale Sobel    #
#          filter                                             #
#                                                             #
# File Description: Bash script to automatically create       #
#                   executables for all project source files, #
#                   based on varying optimization parameters  #
#                   and different compilers                   #
#                                                             #
# Authors: Kyritsis Spyridon     AEM: 2697                    #
#          Karaiskos Charalampos AEM: 2765                    #
###############################################################

SOURCE_DIR="source"
OUTPUT_DIR="output"

TESTNUM=0

# Run all the tests #
for EXECFILE in $(find $OUTPUT_DIR -name '*.grey' -type f | sort)
do
  ## increase number of testcases ##
  TESTNUM=`expr $TESTNUM + 1`

  echo -e "\\nRunning test" $TESTNUM
  echo -e "\\tConverting image" $(basename $EXECFILE) "from grayscale to .jpg"

  convert -depth 8 -size 4096x4096 GRAY:$EXECFILE $EXECFILE.jpg 
done