#!/usr/bin/env python
# coding: utf-8

from GraphGenerator.preprocessing import dataio
import sys


def main(argv):
    graph = dataio.load_data(argv[0])
    print(len(graph[0].nodes))
if __name__ == "__main__":
   main(sys.argv[1:])


