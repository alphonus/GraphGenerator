import pickle, os, sys
import networkx as nx



def largest_connected_component(graph):
    return max(nx.connected_components(graph), key=len)

def load_data(path):
    if os.path.exists(path):
        with open(path, 'rb') as f:
            graph = pickle.load(f)
        #graph = nx.read_gpickle(path)
        return graph
    else:
        print("Path does not exist.")
        sys.exit(1)


def save_data(obj, name):
    with open(name, 'wb') as f:
        pickle.dump(obj, f)
    #nx.write_gpickle(obj, "{}".format(name))
    return 0

