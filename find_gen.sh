#!/usr/bin/env sh
export WANDB_MODE=disabled
folder="generated_datasets/src/COLLAB"
#fname="generated_datasets/src/FIRSTMM_DB/FIRSTMM_DB_graph_1.graph"


for fname in `ls $folder | shuf -n 10`
do
    nodes=$(python get_nnodes.py $folder/$fname)
    echo "$fname, $nodes" >> hyper_tune.csv
    for config in dcsbm sbm bigg graphrnn netgan rmat sbmgnn vgae graphite
    do
        sed -ri "s/([ \t]+)(num_nodes:\s[0-9]+)/\1num_nodes: $nodes/" config/$config.yaml
        #sed -ri "s/([ \t]+)(num_nodes:\s[0-9]+)/\1num_nodes: $nodes/" config/sbmgnn.yaml

        sed -ri "s/([ \t]*)(seed:\s[0-9]+)/\1seed: $RANDOM/" config/$config.yaml
        bench=`time -f "'%C',%e" -q -a -o bench.csv python hyper_tuning.py --phase train -i $folder/$fname -g $config --config config/$config.yaml`


    done
done
