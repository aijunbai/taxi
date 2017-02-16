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
PROFILE=""
OPT=""

declare -a ALGS=(
        "hierarchicalfsmint" 
        "hierarchicalfsm" 
        "maxq0" 
        "maxqq" 
    )

TRIALS="4"

if [ $VERSION = "debug" ]; then
    OPT="--debug $OPT"
fi

ulimit -c unlimited

make cleanall
make $VERSION
mkdir -p data
cd data

cp ../${PLT} plot.gnuplot
cp ../${PLT} cplot.gnuplot
cp ../plot.sh .

echo "set output \"reward.png\"" >> plot.gnuplot
echo "set output \"cumulativereward.png\"" >> cplot.gnuplot
echo "set title \"averaged reward\"" >> plot.gnuplot
echo "set title \"averaged cumulative reward\"" >> cplot.gnuplot
echo "plot \\" >> plot.gnuplot
echo "plot \\" >> cplot.gnuplot

LS="1"
for alg in "${ALGS[@]}"; do
    time ../maxq_op $OPT --size $SIZE --trials $TRIALS --episodes $EPISODES \
        --train $PROFILE --multithreaded --$alg > ${alg}.out
    echo "'${alg}.out' u 1:2 t \"${alg}\" ls $LS smooth bezier, \\" >> plot.gnuplot
    echo "'${alg}.out' u 1:4 t \"${alg}\" ls $LS smooth bezier, \\" >> cplot.gnuplot
    LS="`expr $LS + 1`"
done

./plot.sh

