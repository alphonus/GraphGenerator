#!/usr/bin/env sh
folder="generated_datasets/src/COLLAB"
graph="generated_datasets/src/COLLAB/COLLAB_graph_1348.edgelist.graph"
model=w-s
export WANDB_MODE=disabled
declare -i counter=0


    #fname=`echo $file_loc | rev | cut -d'/' -f 1 | rev | cut -d'.' -f 1`
    #nodes=$(python get_nnodes.py $file_loc)
    #sed -ri "s/([ \t]+)(num_nodes:\s[0-9]+)/\1num_nodes: $nodes/" config/sbmgnn.yaml
for k in 8 14 20
do
    echo $k
    sed -ri "s/([ \t]+)(k:\s[0-9]+)/\1k: $k/" config/$model.yaml
    for beta in 0.2 0.4 0.6 0.8
    do
        sed -ri "s/([ \t]+)(beta:\s0.[0-9])/\1beta: $beta/" config/$model.yaml
        for j in {1..3}
        do
            sed -ri "s/([ \t]*)(seed:\s[0-9]+)/\1seed: $RANDOM/" config/$model.yaml
            python -m GraphGenerator --phase train -i $graph -g $model --config config/$model.yaml
        done
    done
done


