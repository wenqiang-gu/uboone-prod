#!/usr/bin/env python
'''
In some cases, the production provides multiple identical files given a single input.
When a job gets held and resumes, it has some probability to start over again.

This script will read a file list and keep only one file in the same directory.
Here, we assume the file list is given in absolute path.

Usage: mb-prune-replica.py [file list] > [new file list]
'''

import os,sys

infile = sys.argv[1]

dir2file = dict()
with open(infile) as f:
  for line in f:
    path = line.strip('\n')
    dirname = os.path.dirname(path)
    # basename = os.path.basename(path)
    dir2file[dirname] = path

for dirname in dir2file:
  print(dir2file[dirname])
