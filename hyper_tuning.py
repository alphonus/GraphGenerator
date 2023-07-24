import argparse, sys, pickle, warnings, os, torch, logging, random

# import torch.cuda

warnings.filterwarnings("ignore")
from GraphGenerator.preprocessing import dataio
from GraphGenerator.utils.arg_utils import get_config, set_device
import pandas as pd
import networkx as nx
import numpy as np
import wandb


logging.basicConfig(
    handlers=[
        logging.FileHandler(f"tune.log"),
        logging.StreamHandler(),
    ],
    level=logging.INFO,
    format="%(asctime)s %(message)s",
    datefmt="%d/%m/%Y %I:%M:%S",
)

def disconnect_node_ratio(graph):
    components = [len(c) for c in sorted(nx.connected_components(graph), key=len, reverse=True)]
    return (1-components[0]/sum(components))

if __name__=='__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--phase", help="Choose phase.", default="preprocessing", type=str,
                        choices=["preprocessing", "train", "evaluate", "test"],
                        required=True)
    parser.add_argument("-i", "--input", help="Path of input file. Example:```-i google.txt```", default=None)
    parser.add_argument("-o", "--output", help="Specify the name of output file.", default=None)
    parser.add_argument("-c", "--config", help="Specify the path of config file.", default=None)
    parser.add_argument("-g", "--generator", help="choose the generator. Example:```-g sbm```", default="vgae",
                        choices=["e-r", "b-a", "w-s", "rtg", "bter", "sbm", "dcsbm", "rmat", "kronecker",
                                 "mmsb", "vgae", "graphite", "sbmgnn", "graphrnn", "gran", "bigg", "arvga",
                                 "netgan", "condgen", "sgae"])
    parser.add_argument("-e", "--evaluate", help="choose the evaluating metrics.", default=None)
    parser.add_argument("-r", "--ref", help="Path of referenced graphs(Only required in evaluate phase)", default=None)
    args = parser.parse_args()

    wandb.init(
        project="Graph-generator"

        )
    if args.phase == 'train':

        config = get_config(args.config)
        set_device(config)
        from GraphGenerator.train import train_base as train



        print("Start loading data...")
        input_data = dataio.load_data(args.input)


        if args.config is None:
            args.config = "config/{}.yaml".format(args.generator)
        # os.environ["CUDA_VISIBLE_DEVICES"] = str(config.gpu)
        print("Start (training and) inferencing graph...")
        output_data = []
        fault_counter = 0
        _len = 0
        ratio = []
        nnodes = []

        logging.info(f"Doing {args.generator}")
        while True:
            if isinstance(input_data, list):
                for graph in input_data:
                    components = list(nx.connected_components(graph))
                    if len(components) > 1:
                        graph = graph.subgraph(max(components, key=len))
                        with open("faulty_graphs.csv", 'a') as f:
                            f.write(f"'{args.input}', too many components\n")
                    tmp_data = train.train_and_inference(graph, args.generator, config=config)

                    if isinstance(tmp_data, list):
                        for G in tmp_data:
                            ratio.append(disconnect_node_ratio(G))
                            nnodes.append(G.number_of_nodes())
                        output_data.extend(tmp_data)
                        _len += len(tmp_data)
                        #print("{} out of {} faulty for config: {}.".format(fault_counter, _len, args.config))
                    else:
                        ratio.append(disconnect_node_ratio(tmp_data))
                        nnodes.append(tmp_data.number_of_nodes())
                        output_data.append(tmp_data)
                        _len += 1
                        #print("{} out of {} faulty for config: {}.".format(fault_counter, 1, args.config))
            else:
                input_data = input_data.subgraph(max(nx.connected_components(input_data), key=len))
                tmp_data = train.train_and_inference(input_data, args.generator)
                if isinstance(tmp_data, list):
                    for G in tmp_data:
                        ratio.append(disconnect_node_ratio(G))
                        nnodes.append(G.number_of_nodes())
                    output_data.extend(tmp_data)
                    _len += len(tmp_data)
                    #print("{} out of {} graph faulty for config: {}.".format(fault_counter, len(tmp_data), args.config))
                else:
                    ratio.append(disconnect_node_ratio(tmp_data))
                    nnodes.append(tmp_data.number_of_nodes())
                    output_data.append(tmp_data)
                    _len += 1
            config.seed = random.randrange(10_000)
            if _len >= 10:
                break
        mean = sum(ratio)/len(ratio)
        logging.warning("From {} Graphs; on average are {}% Nodes dissconected for config: {}.".format(len(ratio), mean, args.config))

        config_name = args.config.split(".")[-2].split("/")[-1]
        mean_nnodes = sum(nnodes)/len(nnodes)
        with open("hyper_tune.csv", "a") as f:
            f.write(f"{config_name}, {mean}, {len(ratio)}, {mean_nnodes}\n")

        logging.debug("Evaluating graph..")
        if args.output is None:

            output_name = "{}.graphs".format(config_name)
        else:
            output_name = args.output

        dataio.save_data(output_data, name=os.path.join("exp/tuning", output_name))
