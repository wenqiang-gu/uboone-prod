#!/usr/bin/env python

import sys
filename = sys.argv[1]

import ROOT as rt
infile = rt.TFile(filename)
T_pot = infile.Get("wcpselection/T_pot")

if not T_pot:
  print("Cannot get TTree with name T_pot.")

RSPv = []
for e in T_pot:
  rsp = (e.runNo, e.subRunNo, e.pot_tor875)
  if RSPv.count(rsp)==0:
    RSPv.append(rsp)
  else:
    print("Duplicated entry for run %d, subrun %d" %(rsp[0], rsp[1]))


total_pot = 0
for run, subrun, pot in RSPv:
  total_pot += pot

print("Total POT: %E" %total_pot)
