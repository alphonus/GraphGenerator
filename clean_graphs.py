import pickle, os
import networkx as nx

SOURCE_FOLDER="/home/martin/devel/uni/thesis/Generator/GraphGenerator/data/FIRSTMM_DB"
TARGET_FOLDER="/home/martin/devel/uni/thesis/Generator/GraphGenerator/generated_datasets/src/FIRSTMM_DB"
def check_connected(x):
    return nx.is_tree(nx.minimum_spanning_tree(x))
for file in os.listdir(SOURCE_FOLDER):
    if file.endswith("edgelist"):
        path = os.path.join(SOURCE_FOLDER, file)
        graph = nx.read_edgelist(path)
        if check_connected(graph):
            #print(file.split(".")[0])
            target_path = os.path.join(TARGET_FOLDER, file.split(".")[0]+".graph")
            #nx.write_edgelist(graph, target_path, data=False)
            #pickle.dump([graph], open("{}".format(target_path), "wb"))
            with open(target_path, 'wb') as f:
                pickle.dump([graph], f, protocol=4)
