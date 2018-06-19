#!/bin/bash

len1=61845
len2=34720
nproc=12
# hosts=4
# cores_per_host=20
nthreads=2
k=5

id="${len1}x${len2}_${nproc}_${nthreads}_${k}"
cores_per_host=$nproc
# let "nproc=$hosts*$cores_per_host"
# id="${len1}x${len2}_${nproc}=${hosts}*${cores_per_host}_${nthreads}_${k}"

mkdir -p lsf results
filename=lsf/$id.lsf
output=results/$id.txt

./prep2.sh $len1 $len2

cat /dev/null > $filename
echo \#BSUB -q normal >> $filename
echo \#BSUB -J SW_MPI >> $filename
echo \#BSUB -n $nproc >> $filename
echo \#BSUB -R \"span[ptile=$cores_per_host]\" >> $filename
echo \#BSUB -o $output >> $filename
echo >> $filename
echo source /polusfs/setenv/setup.SMPI >> $filename
echo >> $filename
echo export OMP_NUM_THREADS=$nthreads >> $filename
echo >> $filename
echo mpirun simmtx ../data/$len1.target $len1 ../data/$len2.query $len2 2 -1 -2 >> $filename

bsub < $filename
