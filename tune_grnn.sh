#!/usr/bin/env sh
export WANDB_MODE=disabled
folder="generated_datasets/src/FIRSTMM_DB"
fname="generated_datasets/src/FIRSTMM_DB/FIRSTMM_DB_graph_1.graph"
nodes=$(python get_nnodes.py $fname)
model=graphite
for config in `ls config/graphitetuning/*.yaml`
do
    sed -ri "s/([ \t]+)(num_nodes:\s[0-9]+)/\1num_nodes: $nodes/" $config
    #sed -ri "s/([ \t]+)(num_nodes:\s[0-9]+)/\1num_nodes: $nodes/" config/sbmgnn.yaml

    sed -ri "s/([ \t]*)(seed:\s[0-9]+)/\1seed: $RANDOM/" $config
    python hyper_tuning.py --phase train -i $fname -g $model --config $config

done
