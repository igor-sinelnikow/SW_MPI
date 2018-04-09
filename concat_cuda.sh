#!/bin/bash

len1=16384
len2=4096

for p in 1 2 4 8 16; do
    let "maxT=16/$p"
    for (( t = 1; t <= maxT; t *= 2 )); do
        head -n 1 -q results/${len1}x${len2}_${p}_${t}_*.txt >> results/${len1}x${len2}_${p}_${t}.txt
    done
done
