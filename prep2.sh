#!/bin/bash

target="../data/$1.target"
query="../data/$2.query"

if [[ ! -f $target ]]; then
    ./prepare ../data/M.mycoides.fasta $target $1
fi

if [[ ! -f $query ]]; then
    ./prepare ../data/M.capricolum.fasta $query $2
fi
