#!/usr/bin/env python
import ROOT as rt
import sys
import math

infile1 = sys.argv[1]


t1 = rt.TTree("t1","t1")
t1.ReadFile(infile1, "entry/I:run/I:subrun/I:event/I:weight/D:status/I:post_generic/I:numuCC/I:nueBDT/I:is_FC/I:Ereco/D:Evis/D:truth_cosmic/I:truth_numuCC/I:truth_NC/I:truth_nue/I:truth_nuEnergy/D:pio_presel/I:pio_mass/D",',')

N_total = 0
N_failed_production = 0
N_post_generic = 0
N_numuCC = 0
for e in t1:
  N_total += 1
  if e.post_generic==1:
    N_post_generic += 1 

    if e.status==0:
      N_failed_production += 1

    if e.numuCC ==1:
      N_numuCC += 1

print('# events: %d' %N_total)
print('=== passing rate ===')
print('pre-generic: 1')
print('failed production: %f' %(N_failed_production*1.0/N_total))
print('post-generic: %f' %(N_post_generic*1.0/N_total))
print('numuCC: %f' %(N_numuCC*1.0/N_total))
