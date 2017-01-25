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

VERSION="release"
SIZE="5"
TRIALS="4"
EPISODES="1000000"
PLT="plot.gnuplot"

ulimit -c unlimited

make clean
make $VERSION
mkdir -p data
cd data

cp ../${PLT} plot.gnuplot
cp ../${PLT} cplot.gnuplot

for alg in hierarchicalfsm hierarchicalfsmdet; do
    time ../maxq_op --size $SIZE --trials $TRIALS --episodes $EPISODES \
        --train --multithreaded --$alg > ${alg}.out
    echo "'${alg}.out' u 1:2 w l, \\" >> plot.gnuplot
    echo "'${alg}.out' u 1:3 w l, \\" >> cplot.gnuplot
done

gnuplot -p plot.gnuplot &
gnuplot -p cplot.gnuplot &

wait

