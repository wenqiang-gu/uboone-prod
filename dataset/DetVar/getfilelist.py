#!/usr/bin/env python

import os

with open('DetVarSample.txt') as f:
  for line in f:
    c = line.split()
    if len(c) != 2: continue
    cmd = './getFilelistByDefName.sh %s > %s.txt' %(c[1], c[0])
    print(cmd)
    os.system(cmd)
