import argparse, sys, pickle
from GraphGenerator.metrics import mmd
from GraphGenerator import train
from GraphGenerator.preprocessing import dataio, utils
import pandas as pd


def print_variables(vdict, name="args"):
    print("┌---------------------------------------")
    print("|This is the summary of {}:".format(name))
    var = vdict
    for i in var:
        if var[i] is None:
            continue
        print("|{:11}\t: {}".format(i, var[i]))
    print("└---------------------------------------")


if __name__ == '__main__':
    # get arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--phase", help="Choose phase.", default="preprocessing", type=str,
                        choices=["preprocessing", "train", "evaluate"],
                        required=True)
    parser.add_argument("-i", "--input", help="Path of input file. Example:```-i google.txt```", default=None, required=True)
    parser.add_argument("-o", "--output", help="Specify the path of output file.", default=None)
    parser.add_argument("-g", "--generator", help="choose the generator. Example:```-g sbm```", default=None)
    parser.add_argument("-e", "--evaluate", help="choose the evaluating metrics.", default=None)
    parser.add_argument("-r", "--ref", help="Path of referenced graphs(Only required in evaluate phase)", default=None)
    args = parser.parse_args()
    print_variables(vars(args))
    if args.phase == 'preprocessing':
        tmp_path = args.input
        print("# Load edgelist...")
        graph = utils.edgelist_to_graph(tmp_path)
        graphlist = [graph]
        print("# Save graphlist...")
        if args.output is None:
            output_name = "{}.graphs".format(args.input)
        else:
            output_name = args.output
        dataio.save_data(graphlist, name=output_name)

    elif args.phase == 'train':
        print("Start loading data...")
        input_data = dataio.load_data(args.input)
        print("Start (training and) inferencing graph...")
        output_data = []
        for graph in input_data:
            tmp_data = train.train_and_inference(graph, args.generator)
            if isinstance(tmp_data, list):
                output_data.extend(tmp_data)
            else:
                output_data.append(tmp_data)
        print("Start saving generated graphs...")
        if args.output is None:
            output_name = "{}_to_{}.graphs".format(args.input, args.generator)
        else:
            output_name = args.output
        dataio.save_data(output_data, name=output_name)
    elif args.phase == 'evaluate':
        print("Start evaluating the generated graphs...")
        graphs_ref = dataio.load_data(args.ref)
        graphs_pred = dataio.load_data(args.input)
        result = mmd.print_result(args.evaluate, graphs_ref, graphs_pred)
        if args.output is None:
            output_name = "{}_to_{}.csv".format(args.ref, args.input)
        else:
            output_name = args.output
        tmp_pd = pd.DataFrame(result)
        tmp_pd.to_csv(output_name)
    print("Done!")
    sys.exit(0)
