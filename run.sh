#!/bin/bash

len1=16384
len2=4096
mkdir -p results
for (( i = 1; i <= 5; i++ )); do
    for p in 1 2 4 8 16; do
        let "maxT=16/$p"
        for (( t = 1; t <= maxT; t *= 2 )); do
            mpirun -np ${p} ./simmtx ../data/${len1}.target ${len1} ../data/${len2}.query ${len2} 2 -1 -2 ${t} > results/${len1}x${len2}_${p}_${t}_${i}.txt
        done
    done
done
