#!/bin/bash

user=igor.s
limit=1

mkdir -p lsf results
for (( i = 1; i <= 4; i++ )); do
    for p in 20 16 12 8 4 2 1; do
        for (( t = 8; t >= 1; t /= 2 )); do
            l=`bjobs -u ${user} 2> /dev/null | wc -l`
            while (( l > limit )); do
                sleep 7
                l=`bjobs -u ${user} 2> /dev/null | wc -l`
            done
            ./submit.sh $p $t $i
        done
    done
done
