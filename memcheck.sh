#!/bin/bash - 
#===============================================================================
#
#          FILE: memcheck.sh
# 
#         USAGE: ./memcheck.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Aijun Bai (), 
#  ORGANIZATION: 
#       CREATED: 01/23/2017 13:12
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

ulimit -c unlimited

make clean
make debug

valgrind ./maxq_op --size 5 --multithreaded --trials 8 --episodes 1000 --train --hierarchicalfsmdet 2>&1 | tee -a valgrind.log

