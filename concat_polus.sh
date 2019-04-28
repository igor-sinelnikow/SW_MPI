#!/bin/bash

dir=results

# len1=67106
# len2=64000
len1=64113
len2=62000

exec=simmtx_omp
for p in 1 2 4 10; do
    let "t=160/$p"
    rm -f ${dir}/${exec}.${len1}x${len2}-p${p}-t${t}.txt
    tail -n 1 -q ${dir}/${exec}.${len1}x${len2}-p${p}-t${t}.*.txt >> ${dir}/${exec}.${len1}x${len2}-p${p}-t${t}.txt
done

# exec=simmtx_gpu
# for dev in 1 2; do
#     p=${dev}
#     tail -n 1 -q ${dir}/${exec}.${len1}x${len2}-p${p}-dev${dev}.*.txt >> ${dir}/${exec}.${len1}x${len2}-p${p}-dev${dev}.txt
# done
