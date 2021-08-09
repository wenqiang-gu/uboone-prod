#!/usr/bin/env python

from __future__ import print_function
import sys
import os
import subprocess
import json


'''
usage: ./batchGetDataInfo.py defname [file_list]
'''

defname = sys.argv[1]
outname = '{}_potdb.txt'.format(defname)

files = []
if len(sys.argv) > 2 :
    file_list = sys.argv[2]
    outname = '{}_potdb.txt'.format(file_list)
    with open(file_list, 'r') as infile:
        data = infile.readlines()
        for i,line in enumerate(data):
            files.append(line.split()[0])
else:
    query = "defname:{}".format(defname)
    files = subprocess.check_output(["samweb", "list-files", query]).split()

potdb = open(outname,'w')
header = "           run        subrun           EXT         Gate2        E1DCNT        tor860        tor875   E1DCNT_wcut   tor860_wcut   tor875_wcut"
print(header, file=potdb)
for i,file in enumerate(files):
    meta = subprocess.check_output(["samweb", "get-metadata", "--json", file])
    meta = json.loads(meta)
    print(i, file, len(meta["runs"]))
    for rs in meta["runs"]:
        potinfo = subprocess.check_output(["/uboone/app/users/zarko/getDataInfo.py", "--noheader", "-v2", "-r", str(rs[0]), "-s", str(rs[1])])
        print(potinfo, end='', file=potdb)

potdb.close()
