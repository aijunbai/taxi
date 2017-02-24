#!/bin/bash

make clean
make debug

rm -f .gdbinit
while read LINE; do
    if [[ ${LINE:0:1} != "#" ]]; then
        echo "b $LINE" >> .gdbinit
    fi
done <breakpoints
cat gdbinit >> .gdbinit

cgdb -- taxi 

