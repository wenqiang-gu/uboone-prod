#!/usr/bin/env python

import sys
filename = sys.argv[1]

import ROOT as rt
infile = rt.TFile(filename)
T_pot = infile.Get("wcpselection/T_pot")

if not T_pot:
  print("Cannot get TTree with name T_pot.")

RSPv = dict()
for e in T_pot:
  if (e.runNo, e.subRunNo) not in RSPv:
    RSPv[(e.runNo, e.subRunNo)] = [e.pot_tor875]
  else:
    RSPv[(e.runNo, e.subRunNo)].append(e.pot_tor875)
    print("Duplicated entry for run %d, subrun %d, pot %E" %(e.runNo, e.subRunNo, e.pot_tor875))


total_pot = 0
for run, subrun in RSPv:
  potvx = RSPv[(run,subrun)]
  non_zero_potvx = [x for x in potvx if x > 0]
  if len(non_zero_potvx)==0:
    print("No valid POT entry for run %d, subrun %d" %(run,subrun))
  elif len(non_zero_potvx)>1:
    print("More than one valid POT entry for run %d, subrun %d: (1st & 2nd) %f %f" %(run, subrun, non_zero_potvx[0]/1E19, non_zero_potvx[1]/1E19))
    total_pot += non_zero_potvx[0]
  else: # only one non-zero POT, good case
    total_pot += non_zero_potvx[0]

print("Total POT: %E" %total_pot)
