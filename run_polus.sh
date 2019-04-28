#!/bin/bash

limit=1
task_dist=pack
queue="-q normal"

mkdir -p results

len1=67106
len2=64000
# len1=64113
# len2=62000
exec=simmtx_omp
for (( i = 1; i <= 1; i++ )); do
    for p in 1 2; do
        let "t=160/$p"
        l=`bjobs 2> /dev/null | wc -l`
        while (( l-1 >= limit )); do
            sleep 7
            l=`bjobs 2> /dev/null | wc -l`
        done
        bsub -n ${p} -a "p8aff(${t},8,8,${task_dist})" -R "select[(maxmem==256G) && (type==any)] same[nthreads]" -env "all, OMP_DISPLAY_ENV=FALSE, OMP_DYNAMIC=FALSE, OMP_SCHEDULE=STATIC" ${queue} -e results/${exec}.${len1}x${len2}-p${p}-t${t}.%J.txt -J "SW_MPI" mpiexec ./${exec} ../data/${len1}.target ${len1} ../data/${len2}.query ${len2} 2 -1 -2
    done

    # exec=simmtx_gpu
    # for dev in 1 2; do
    #     l=`bjobs 2> /dev/null | wc -l`
    #     while (( l-1 >= limit )); do
    #         sleep 7
    #         l=`bjobs 2> /dev/null | wc -l`
    #     done
    #     p=${dev}
    #     bsub -n ${p} -gpu "num=${dev}:mode=exclusive_process" -R "select[(maxmem==256G) && (type==any)]" ${queue} -e results/${exec}.${len1}x${len2}-p${p}-dev${dev}.%J.txt -J "SW_GPU" mpiexec ./${exec} ../data/${len1}.target ${len1} ../data/${len2}.query ${len2} 2 -1 -2
    # done
done
