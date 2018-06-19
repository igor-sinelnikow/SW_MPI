#!/bin/bash

len1=61845
len2=34720

for p in 1 2 4 8 12 16 20; do
    for (( t = 1; t <= 8; t *= 2 )); do
        for (( i = 1; i <= 4; i++ )); do
            tail -n 2 results/${len1}x${len2}_${p}_${t}_${i}.txt | head -n 1 >> results/${len1}x${len2}_${p}_${t}.txt
        done
    done
done
