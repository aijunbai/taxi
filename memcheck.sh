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

make clean
make debug

valgrind ./maxq_op -n 5 -Mt -HS 2>&1 | tee -a valgrind.log

