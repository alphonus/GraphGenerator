#!/usr/bin/env sh

for graph in `ls data/DD_Clean/*.edgelist`
do
    fname=`echo $graph | rev |cut -d'/' -f 1 | rev | cut -d'.' -f 1`

    python -m GraphGenerator --phase preprocessing -i $graph -o "generated_datasets/src/DD_clean/$fname.graph"
done
