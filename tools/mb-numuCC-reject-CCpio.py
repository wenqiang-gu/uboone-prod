#!/usr/bin/env python
import ROOT as rt
import sys
import csv 
import os


class labelDB:
  def __init__(self, filename, fields):
    self.filename = filename
    self.fields = fields # names for label
    self.label_entries = []
    self.entry = dict.fromkeys(self.fields, 0) # default 0

  def legal_field(self, field_name):
    if field_name in self.fields:
      return True
    else:
      return False

  def update_entry(self, field_name, label):
    if field_name in self.fields:
      self.entry[field_name] = label
    else:
      print("labelDB => Illegal field: %s" %field_name)

  def flush_entry(self):
    self.label_entries.append(self.entry)
    self.entry = dict.fromkeys(self.fields, 0)

  def write(self, write_header=False):
    # writing to csv file 
    with open(self.filename, 'w') as csvfile:
        # creating a csv dict writer object 
        writer = csv.DictWriter(csvfile, delimiter=' ', fieldnames = self.fields)  

        # writing headers (field names)
        if write_header:
          writer.writeheader()

        # writing data rows 
        writer.writerows(self.label_entries)


rootfilename = sys.argv[1]
rootfile = rt.TFile(rootfilename)

# event label
# label_fields = ['#entry', 'run', 'subrun', 'event', 'weight', 'status', 'post_generic', 'numuCC', 'nueBDT', 'is_FC', 'Ereco', 'Evis', 'truth_cosmic', 'truth_numuCC', 'truth_NC', 'truth_nue', 'truth_nuEnergy', 'pio_presel', 'pio_mass']
label_fields = ['run', 'subrun', 'event', 'Ereco', 'is_FC']
basename = os.path.basename(rootfilename)
pre, ext = os.path.splitext(basename)
lbldb = labelDB(pre+"_numuCC_reject_CCpio.list", label_fields)
print("creating " + pre + "_numuCC_reject_CCpio.list")

tt	= rootfile.Get("wcpselection/T_eval")
tt_pf	= rootfile.Get("wcpselection/T_PFeval")
tt_bdt	= rootfile.Get("wcpselection/T_BDTvars")
tt_kine	= rootfile.Get("wcpselection/T_KINEvars")
tt.AddFriend(tt_pf, "pf")
tt.AddFriend(tt_bdt, "bdt")
tt.AddFriend(tt_kine, "kine")

entryNum = 0
has_valid_weight = True
if not tt.GetBranchStatus('weight_spline')\
   or not tt.GetBranchStatus('weight_cv'):
  has_valid_weight = False
  print("[MC reweight] No valid TTree branch for 'weight', set to 1.0")
has_truth_info = True
if not tt.GetBranchStatus('truth_nuEnergy'):
  has_truth_info = False

for e in tt:
  # extra weight for MC event
  # if has_valid_weight:
  #   wgt_cv 	= e.weight_cv
  #   wgt_spline 	= e.weight_spline
  #   if wgt_cv>1000 or wgt_cv<=0: wgt_cv = 1.0
  #   if wgt_spline>1000 or wgt_spline<=0: wgt_spline = 1.0
  #   wgt = wgt_cv * wgt_spline
  #   lbldb.update_entry("weight", wgt)
  #   if wgt==0.0: print("entry: %d, weight 0" %entryNum)
  # else:
  #   lbldb.update_entry("weight", 1.0)


  # if has_truth_info:
  #   lbldb.update_entry("truth_nuEnergy", e.truth_nuEnergy)
  #   # lbldb.update_entry("truth_cosmic", 0)
  #   # lbldb.update_entry("truth_numuCC", 0)
  #   # lbldb.update_entry("truth_nue", 0)
  #   # lbldb.update_entry("truth_NC", 0)
  #   if not e.truth_vtxInside or e.match_completeness_energy/(e.truth_energyInside+1e-9) < 0.1:
  #     lbldb.update_entry("truth_cosmic", 1)
  #   else:
  #     if e.truth_isCC and abs(e.truth_nuPdg)==14:
  #       lbldb.update_entry("truth_numuCC", 1)
  #     elif e.truth_isCC and abs(e.truth_nuPdg)==12:
  #       lbldb.update_entry("truth_nue", 1)
  #     elif not e.truth_isCC:
  #       lbldb.update_entry("truth_NC", 1)

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
    # lbldb.update_entry("post_generic", 1)
    # production quality
    if e.reco_nuvtxX==-1:
      # pass # do nothing
      continue
    else:
      # lbldb.update_entry("status", 1)
      pass


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
    if not cosmic and numuCC:
      ## exclude CCpi0 candidates
      if e.kine_pio_flag == 1 \
      and e.kine_pio_energy_1 >15 and e.kine_pio_energy_2 >15 \
      and e.kine_pio_dis_1 < 80 and e.kine_pio_dis_2 < 80 \
      and e.kine_pio_angle > 20 \
      and e.kine_pio_vtx_dis < 1:
        continue

      # lbldb.update_entry("#entry", entryNum)
      #lbldb.update_entry("numuCC", 1)
      lbldb.update_entry("run", e.run)
      lbldb.update_entry("subrun", e.subrun)
      lbldb.update_entry("event", e.event)
      lbldb.update_entry('Ereco', e.kine_reco_Enu)
      lbldb.update_entry("is_FC", e.match_isFC)
      lbldb.flush_entry()


  entryNum += 1

lbldb.write()
