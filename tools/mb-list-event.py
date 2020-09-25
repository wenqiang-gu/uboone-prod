#!/usr/bin/env python
import ROOT as rt
import sys
import csv 
import os


rootfilename = sys.argv[1]
rootfile = rt.TFile(rootfilename)

tt	= rootfile.Get("wcpselection/T_eval")
tt_pf	= rootfile.Get("wcpselection/T_PFeval")
tt_bdt	= rootfile.Get("wcpselection/T_BDTvars")
tt_kine	= rootfile.Get("wcpselection/T_KINEvars")
tt.AddFriend(tt_pf, "pf")
tt.AddFriend(tt_bdt, "bdt")
tt.AddFriend(tt_kine, "kine")

numuCClst = open("numuCC.txt","w")
nuelst = open("nue.txt","w")

hall = rt.TH2F("hall","",100,0,1,100,0,10)
hlee = rt.TH2F("hlee","",100,0,1,100,0,10)

entryNum = 0
total_wgt_lee = 0
for e in tt:
  # extra weight for MC event
  wgt_cv 	= e.weight_cv
  wgt_spline 	= e.weight_spline
  if wgt_cv>1000 or wgt_cv<=0: wgt_cv = 1.0
  if wgt_spline>1000 or wgt_spline<=0: wgt_spline = 1.0
  wgt = wgt_cv * wgt_spline

  wgt_lee       = e.weight_lee
  total_wgt_lee += (wgt_lee*wgt)

  hall.Fill(e.truth_nuEnergy*0.001, wgt_lee*wgt)
  hlee.Fill(e.truth_nuEnergy*0.001, wgt_lee)

  ### generic selection starts here
  if e.flash_found \
     and e.match_found \
     and not e.match_isTgm \
     and not e.stm_TGM \
     and not e.stm_STM \
     and not e.stm_lowenergy \
     and not e.stm_LM \
     and not e.stm_FullDead \
     and e.stm_clusterlength > 15:
    ### post-generic selection starts here

    # production quality
    status = True
    if e.reco_nuvtxX==-1:
      status = False

    neutrino_type = e.neutrino_type
    cosmic = ( neutrino_type >> 1 ) & 1 
    numuCC = ( neutrino_type >> 2 ) & 1
    NC     = ( neutrino_type >> 3 ) & 1
    nueCC  = ( neutrino_type >> 5 ) & 1
    # print("neutrino bit: %d %d %d %d" %(cosmic, numuCC, NC, nueCC))

    ### numuCC
    # variables for energy
    # T_KINEvars::kine_reco_Enu  => "neutrino energy"
    # T_eval::match_energy	 => Evis
    # T_BDTvars::mip_energy      => primary shower energy
    ###
    if status and not cosmic and numuCC:
      numuCClst.write("%d %d %d %f\n" %(e.run, e.subrun, e.event, e.kine_reco_Enu))

    ### nueCC
    if status and e.nue_score>6 and wgt_lee>0:
      nuelst.write("%d %d %d %f\n" %(e.run, e.subrun, e.event, e.kine_reco_Enu))

  entryNum += 1

numuCClst.close()
nuelst.close()

print("total LEE weight: %f" %total_wgt_lee)

hall.Draw("colz")
