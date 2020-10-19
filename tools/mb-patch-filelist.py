#!/usr/bin/env python
'''
Usage: mb-patch-filelist.py #list1 #list2 #outputName

Description: list2 is from a production based on list1. However, some files failed, we
would like to re-run the prodcution for these files. So we need to find them from
the #list1 by assuming the file names from #list1 and #list2 can have some match.
'''

import os,sys

def find_best_match_name(set1, name1):
  best_match_name = ''
  best_match_length = 0
  for name in set1:
    common_prefix = os.path.commonprefix([name, name1])
    if len(common_prefix) > best_match_length:
      best_match_name = name
      best_match_length = len(common_prefix)
  return (best_match_name, best_match_length)

infile1 = sys.argv[1]
infile2 = sys.argv[2]
ofile = sys.argv[3]

basename2path_1 = dict()
basename_1 = list()
with open(infile1) as f:
  for line in f:
    path = line.strip('\n')
    # dirname = os.path.dirname(path)
    basename = os.path.basename(path)
    basename2path_1[basename] = path
    basename_1.append(basename)

basename2path_2 = dict()
basename_2 = list()
with open(infile2) as f2:
  for line in f2:
    path = line.strip('\n')
    # dirname = os.path.dirname(path)
    basename = os.path.basename(path)
    basename2path_2[basename] = path
    basename_2.append(basename)


bm_names = list()
for name in basename_2:
  name1, length1 = find_best_match_name(basename_1, name)
  bm_names.append(name1)
  if length1<75: print(name1, name)

with open(ofile,'w') as f:
  for name in basename_1:
    if name in bm_names: continue
    f.write(basename2path_1[name] + '\n')

