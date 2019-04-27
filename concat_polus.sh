#!/bin/bash

# len1=67106
# len2=64000
len1=64113
len2=62000

dir=results

exec=simmtx_omp
for p in 1 2 4 8 10 20; do
    for (( t = 1; t <= 8; t *= 2 )); do
        tail -n 1 -q ${dir}/${exec}.${len1}x${len2}-p${p}-t${t}.*.txt >> ${dir}/${exec}.${len1}x${len2}-p${p}-t${t}.txt
    done
done

exec=simmtx_gpu
for dev in 1 2; do
    p=${dev}
    tail -n 1 -q ${dir}/${exec}.${len1}x${len2}-p${p}-dev${dev}.*.txt >> ${dir}/${exec}.${len1}x${len2}-p${p}-dev${dev}.txt
done
