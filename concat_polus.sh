#!/bin/bash

len1=67106
len2=64000

dir=results/2019-04-18

for p in 1 2 4 8 10 20; do
    for (( t = 1; t <= 8; t *= 2 )); do
        for (( i = 1; i <= 3; i++ )); do
            tail -n 2 ${dir}/sw-p${p}-t${t}_${i}.txt | head -n 1 >> ${dir}/simmtx_omp.${len1}x${len2}-p${p}-t${t}.txt
        done
    done
done

tail -n 1 -q ${dir}/simmtx_gpu.${len1}x${len2}-p2-dev2.*.out >> ${dir}/simmtx_gpu.${len1}x${len2}-p2-dev2.txt
