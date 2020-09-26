#!/usr/bin/env python

import os

with open('detvar.txt') as f:
  for line in f:
    if line.startswith('#'): continue
    c = line.split()
    if len(c) != 3: continue
    filename = "%s.txt" %c[0]
    if os.path.exists(filename):continue
    cmd = './getFilelistByDefName.sh %s > %s.txt' %(c[1], c[0])
    print(cmd)
    # os.system(cmd)
