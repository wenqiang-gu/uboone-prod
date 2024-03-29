#!/usr/bin/env python

import sys
filename = sys.argv[1]

import ROOT as rt
infile = rt.TFile(filename)

## 
# CAVEAT: T_pot tree is more precise for subrun look up
#         In some rare case, a subrun has no triggerred events
#
# T_eval = infile.Get("wcpselection/T_eval")
# if not T_eval:
#   print("Cannot get TTree with name T_eval.")
# RSPv = []
# for e in T_eval:
#   rsp = (e.run, e.subrun)
#   if RSPv.count(rsp)==0:
#     RSPv.append(rsp)
#   # else:
#   #   print("Duplicated entry for run %d, subrun %d" %(rsp[0], rsp[1]))
# 
# print("# of subruns (T_eval): %d" %len(RSPv))

T_pot = infile.Get("wcpselection/T_pot")
if not T_pot:
  print("Cannot get TTree with name T_pot.")
RSPv = []
for e in T_pot:
  rsp = (e.runNo, e.subRunNo)
  if RSPv.count(rsp)==0:
    RSPv.append(rsp)
  # else:
  #   print("Duplicated entry for run %d, subrun %d" %(rsp[0], rsp[1]))
# print("# of subruns (T_pot): %d" %len(RSPv2))

print("Save %d lines to rslist.txt" %len(RSPv))
rslistfile = open('rslist.txt','w')
for run, subrun in RSPv:
  rslistfile.write('%d %d\n' %(run, subrun))
rslistfile.close()

print('Run comamnd "/uboone/app/users/zarko/getDataInfo.py -v2 --run-subrun-list rslist.txt"')
print('===')
print('How to read off the POT information?')
print('* BNB data: column ``tor875`` is the POT')
print("* BNB EXT: scale POT from any BNB data with ``EXT`` and BNB data ``E1DCNT``")
print("  Per 1e18 POT, the E1DCNT is about 230,000")
print('===')

import os
os.system('/uboone/app/users/zarko/getDataInfo.py -v2 --run-subrun-list rslist.txt')
