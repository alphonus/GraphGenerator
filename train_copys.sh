#!/usr/bin/env sh
folder="generated_datasets/src/FIRSTMM_DB"
model=sbm
export WANDB_MODE=disabled
declare -i counter=0

for file_loc in `ls $folder/*.graph`
do
    fname=`echo $file_loc | rev | cut -d'/' -f 1 | rev | cut -d'.' -f 1`
    nodes=$(python get_nnodes.py $file_loc)
    sed -ri "s/([ \t]+)(num_nodes:\s[0-9]+)/\1num_nodes: $nodes/" config/$model.yaml
    sed -ri "0,/[ \t]+name/{s/([ \t]+)(name:\s\w+)/\1name: $fname/}" config/$model.yaml
    for j in {1..10}
    do
        sed -ri "s/([ \t]*)(seed:\s[0-9]+)/\1seed: $RANDOM/" config/$model.yaml
        python -m GraphGenerator --phase train -i $file_loc -g $model --config config/$model.yaml

    done
    counter+=1

done
