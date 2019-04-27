#!/bin/bash

# K=`bjobs -l $1 | grep -m 1 -n "RESOURCE REQUIREMENT DETAILS" | cut -d : -f 1`
# bjobs -l $1 | tail -n +$K

files="simmtx_gpu.67106x64000-p2-dev2.296814.out"
files+=" simmtx_gpu.67106x64000-p2-dev2.299676.out"
for file in $files; do
    job=`echo $file | cut -d . -f 3`
    bhist -l -aff $job >> $file
done

files="simmtx_gpu.64113x62000-p1-dev1.296880.txt"
files+=" simmtx_gpu.64113x62000-p1-dev1.297092.txt"
files+=" simmtx_gpu.64113x62000-p1-dev1.297122.txt"
files+=" simmtx_gpu.64113x62000-p1-dev1.297152.txt"
files+=" simmtx_gpu.64113x62000-p1-dev1.299679.txt"
files+=" simmtx_gpu.64113x62000-p2-dev2.296881.txt"
files+=" simmtx_gpu.64113x62000-p2-dev2.299678.txt"
files+=" simmtx_gpu.64113x62000-p2-dev2.299824.txt"
files+=" simmtx_gpu.64113x62000-p2-dev2.299851.txt"
files+=" simmtx_gpu.64113x62000-p2-dev2.299854.txt"
files+=" simmtx_gpu.67106x64000-p2-dev2.299857.txt"
files+=" simmtx_gpu.67106x64000-p2-dev2.299860.txt"
for file in $files; do
    job=`echo $file | cut -d . -f 3`
    out=${file/.txt/.out}
    bhist -l -aff $job > $out
    echo >> $out
    echo "The output (if any) follows:" >> $out
    echo >> $out
    cat $file >> $out
done
