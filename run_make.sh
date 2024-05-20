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


SOURCE_DIR="source/"
OUTPUT_DIR="output/"

CC_LIST=("gcc" "icc" "icx")
OPT_LIST=("-O0" "-O1" "-O2" "-O3" "-fast")

 # "-O1" "-O2" "-O3" "-O4"

if [[ $# -eq 1 ]]
then
  DEBUG=$1
else
  DEBUG=0
fi


# Create executables using gcc #
TESTNUM=0

for i in ${!CC_LIST[@]}
do
  CCSTRING=${CC_LIST[i]}
  echo -e "\\nRunning tests for" $CCSTRING

  for j in ${!OPT_LIST[@]}
  do
    OPTSTRING=${OPT_LIST[j]}

    if [ $CCSTRING = "gcc" ] && [ $OPTSTRING = "-fast" ]
    then
      continue
    fi

    if [ $CCSTRING = "icc" ] && [ $OPTSTRING = "-O4" ]
    then
      continue
    fi

    if [ $CCSTRING = "icx" ] && [ $OPTSTRING = "-O4" ]
    then
      continue
    fi

    echo -e "\\tRunning tests with" $OPTSTRING

    for CFILE in $(find $SOURCE_DIR -type f -name '*.c' | sort)
    do
      ## increase number of testcases ##
      TESTNUM=`expr $TESTNUM + 1`

      echo -e "\\t\\tRunning test" $TESTNUM
      echo -e "\\t\\t\\tTest file is" $CFILE

      TESTDIR=$(basename $(dirname $CFILE))

      case "-fast" in
        $OPTSTRING)
          EXECUTABLE=$TESTDIR"_"$CCSTRING"-Ofast"
          ;;

        *)
          EXECUTABLE=$TESTDIR"_"$CCSTRING$OPTSTRING
          ;;
      esac

      echo -e "\\t\\t\\tmake DEBUG="$DEBUG "TEST_DIR="$TESTDIR "CC="$CCSTRING "OFLAG="$OPTSTRING "EXECUTABLE="$EXECUTABLE"\\n"
      make DEBUG=$DEBUG TEST_DIR=$TESTDIR CC=$CCSTRING OFLAG=$OPTSTRING EXECUTABLE=$EXECUTABLE
    done
  done
done
