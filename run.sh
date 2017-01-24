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
mkdir data
cd data

cp ../${PLT} .
for alg in hierarchicalfsm hierarchicalfsmdet; do
    time ../maxq_op --size $SIZE --trials $TRIALS --episodes $EPISODES \
        --train --multithreaded --$alg > ${alg}.out &
    echo "'${alg}.out' w l, \\" >> ${PLT}
done

wait
gnuplot -p ${PLT}

