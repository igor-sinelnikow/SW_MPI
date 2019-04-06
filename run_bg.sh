#!/bin/bash

user=igor.s
limit=50

len1=125678
len2=4096

mkdir -p results
for (( i = 1; i <= 5; i++ )); do
    for t in 1 2 4; do
        for p in 1 2 4 8 16 32 64 128; do
            l=`llq -u ${user} | wc -l`
            while (( l-4 >= limit )); do
                sleep 5
                l=`llq -u ${user} | wc -l`
            done
            mpisubmit.bg -n ${p} --stdout results/${len1}x${len2}_${p}_${t}_${i}.txt simmtx -- ../data/${len1}.target ${len1} ../data/${len2}.query ${len2} 2 -1 -2 ${t}
        done

        p=256
        wtime="-w 00:10:00"
        l=`llq -u ${user} | wc -l`
        while (( l-4 >= limit )); do
            sleep 5
            l=`llq -u ${user} | wc -l`
        done
        mpisubmit.bg -n ${p} ${wtime} --stdout results/${len1}x${len2}_${p}_${t}_${i}.txt simmtx -- ../data/${len1}.target ${len1} ../data/${len2}.query ${len2} 2 -1 -2 ${t}

        p=512
        wtime="-w 00:05:00"
        l=`llq -u ${user} | wc -l`
        while (( l-4 >= limit )); do
            sleep 5
            l=`llq -u ${user} | wc -l`
        done
        mpisubmit.bg -n ${p} ${wtime} --stdout results/${len1}x${len2}_${p}_${t}_${i}.txt simmtx -- ../data/${len1}.target ${len1} ../data/${len2}.query ${len2} 2 -1 -2 ${t}

        p=1024
        wtime="-w 00:03:00"
        l=`llq -u ${user} | wc -l`
        while (( l-4 >= limit )); do
            sleep 5
            l=`llq -u ${user} | wc -l`
        done
        mpisubmit.bg -n ${p} ${wtime} --stdout results/${len1}x${len2}_${p}_${t}_${i}.txt simmtx -- ../data/${len1}.target ${len1} ../data/${len2}.query ${len2} 2 -1 -2 ${t}

        p=2048
        wtime="-w 00:03:00"
        l=`llq -u ${user} | wc -l`
        while (( l-4 >= limit )); do
            sleep 5
            l=`llq -u ${user} | wc -l`
        done
        mpisubmit.bg -n ${p} ${wtime} --stdout results/${len1}x${len2}_${p}_${t}_${i}.txt simmtx -- ../data/${len1}.target ${len1} ../data/${len2}.query ${len2} 2 -1 -2 ${t}
    done
done
