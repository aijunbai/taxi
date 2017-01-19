#!/bin/bash - 
#===============================================================================
#
#          FILE: run.sh
# 
#         USAGE: ./run.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Aijun Bai (), 
#  ORGANIZATION: 
#       CREATED: 01/15/2017 10:37
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

SIZE="5"

make clean
make release
mkdir data
cd data

time ../maxq_op -n $SIZE -Mt -H > H.out &
time ../maxq_op -n $SIZE -Mt -HS > HS.out &

wait

gnuplot -p ../plot.gnuplot

