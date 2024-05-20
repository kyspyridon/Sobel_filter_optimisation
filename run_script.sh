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


BUILD_DIR="build"
OUTPUT_DIR="output"

TESTNUM=0

# Run all the tests #
for EXECFILE in $(find $BUILD_DIR -type f | sort)
do
  ## increase number of testcases ##
  TESTNUM=`expr $TESTNUM + 1`

  echo -e "\\nRunning test" $TESTNUM
  echo "Test file is" $EXECFILE

  # Build the output directory structure #
  mkdir output/$(basename $EXECFILE)

  OUTPUTIMAGENAME=$OUTPUT_DIR/$(basename $EXECFILE)/output_sobel.grey

  echo -e "Execution time, PSNR" >> $OUTPUT_DIR/$(basename $EXECFILE)/$(basename $EXECFILE).runlog

  for i in {1..22}
  do
    echo -e "\\tRun" $i

    echo $EXECFILE $OUTPUTIMAGENAME ">>" $OUTPUT_DIR/$(basename $EXECFILE)".runlog"
    $EXECFILE $OUTPUTIMAGENAME >> $OUTPUT_DIR/$(basename $EXECFILE)/$(basename $EXECFILE).runlog
  done
done