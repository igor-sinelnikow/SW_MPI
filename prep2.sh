#!/bin/bash

./prepare ../data/M.mycoides.fasta ../data/$1.target $1
./prepare ../data/M.capricolum.fasta ../data/$2.query $2
