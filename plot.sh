#!/bin/bash - 
#===============================================================================
#
#          FILE: plot.sh
# 
#         USAGE: ./plot.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Aijun Bai (), 
#  ORGANIZATION: 
#       CREATED: 01/25/2017 12:52
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

gnuplot -p plot.gnuplot &
gnuplot -p cplot.gnuplot &

wait

eog reward.png &
eog cumulativereward.png &

wait
