#!/bin/bash

len1=125678
len2=4096

for p in 1 2 4 8 16 32 64 128 256 512; do
    for t in 1 2 4; do
        head -n 1 -q results/${len1}x${len2}_${p}_${t}_*.txt >> results/${len1}x${len2}_${p}_${t}.txt
    done
done
