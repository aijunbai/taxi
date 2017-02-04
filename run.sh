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
EPISODES="1000000"
PLT="plot.gnuplot"
OPT=""

NPROC="`nproc`"
TRIALS="`expr $NPROC / 4`"

if [ $VERSION = "debug" ]; then
    OPT="--debug $OPT"
fi

ulimit -c unlimited

make clean
make $VERSION
mkdir -p data
cd data

cp ../${PLT} plot.gnuplot
cp ../${PLT} cplot.gnuplot
cp ../plot.sh .

for alg in hierarchicalfsm hierarchicalfsmdet maxq0 maxqq qlearning; do
    time ../maxq_op $OPT --size $SIZE --trials $TRIALS --episodes $EPISODES \
        --train --multithreaded --$alg > ${alg}.out &
    echo "'${alg}.out' u 1:2 w l, \\" >> plot.gnuplot
    echo "'${alg}.out' u 1:3 w l, \\" >> cplot.gnuplot
done

wait

./plot.sh

