#!/usr/bin/env python

import os

with open('detvar.txt') as f:
  for line in f:
    if line.startswith('#'): continue
    c = line.split()
    if len(c) != 3: continue

    name = c[0]
    celltree_dataset = c[2]

    ## query filelist
    # cmd = './getFilelistByDefName.sh %s > %s.txt' %(c[1], c[0])
    # print(cmd)
    # os.system(cmd)


    ## print projects json format
    inputlist = name + '.txt'
    # inputlist = '/uboone/app/users/wgu/uboone-prod/dataset/DetVar/' + name + '.txt'
#     print(\
# '''
#     {
#       "name": "%s",
#       "inputlist" : "%s",
#       "celltree_dataset" : "%s"
#     },
# ''' %(name, inputlist, celltree_dataset))

    print(\
'''
    {
      "name": "%s",
      "inputlist" : "%s"
    },
''' %(name, inputlist))
