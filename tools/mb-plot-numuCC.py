#!/usr/bin/env python
import ROOT as rt
import sys
import math

def set_hs(h1, fstyle, color, width):
  h1.SetFillStyle(fstyle);
  h1.SetFillColor(color);
  h1.SetLineColor(color);
  h1.SetLineWidth(width);


infile1 = sys.argv[1]
infile2 = sys.argv[2]
infile3 = sys.argv[3]

h1 = rt.TH1F("h1","",30, 0.15, 3.15)
h1w = rt.TH1F("h1w","weight^2",30, 0.15, 3.15)
h2 = rt.TH1F("h2","numuCC",30, 0.15, 3.15)
h3 = rt.TH1F("h3","NC",30, 0.15, 3.15)
h4 = rt.TH1F("h4","cosmic",30, 0.15, 3.15)

h5 = rt.TH1F("h5","pi0 mass", 50, 0, 0.5)
h5w = rt.TH1F("h5w","weight^2", 50, 0, 0.5)

t1 = rt.TTree("t1","t1")
t1.ReadFile(infile1, "entry/I:run/I:subrun/I:event/I:weight/D:status/I:post_generic/I:numuCC/I:nueBDT/I:is_FC/I:Ereco/D:Evis/D:truth_cosmic/I:truth_numuCC/I:truth_NC/I:truth_nue/I:truth_nuEnergy/D:pio_presel/I:pio_mass/D",',')
for e in t1:
  if e.status==1 and e.post_generic==1 and e.numuCC==1 and e.is_FC==1:
    h1.Fill(e.Ereco, e.weight) 
    h1w.Fill(e.Ereco, e.weight * e.weight) 
    if e.truth_numuCC==1: h2.Fill(e.Ereco, e.weight)
    if e.truth_NC==1: h3.Fill(e.Ereco, e.weight)
    if e.truth_cosmic==1: h4.Fill(e.Ereco, e.weight)

    if e.pio_presel==1:
      h5.Fill(e.pio_mass, e.weight)
      h5w.Fill(e.pio_mass, e.weight * e.weight)

for i in range(h1.GetNbinsX()):
  h1.SetBinError(i+1, math.sqrt(h1w.GetBinContent(i+1)))



# pi0 
# for i in range(h5.GetNbinsX()):
#   h5.SetBinError(i+1, math.sqrt(h5w.GetBinContent(i+1)))
# 
# h5.Scale(5E19/3.006E19)
# h5.Draw()
# 
# ofile = rt.TFile("pio_mass.root","update")
# h5.SetName("run1_bnb")
# h5.Write()
# ofile.Close()

# h2.Draw("hist same")
# h3.Draw("hist same")
# h4.Draw("hist same")

# infile2
h6 = rt.TH1F("h6","nu reco energy",30,0.15,3.15)
h6w = rt.TH1F("h6w","weight^2: nu reco energy",30,0.15,3.15)
h6.SetLineColor(2)
h6.SetMarkerColor(2)
# h2.GetXaxis().SetRangeUser(0.1,2)
h6.GetXaxis().SetTitle('Ereco [GeV]')

t2 = rt.TTree("t2","t2")
t2.ReadFile(infile2, "entry/I:run/I:subrun/I:event/I:weight/D:status/I:post_generic/I:numuCC/I:nueBDT/I:is_FC/I:Ereco/D:Evis/D:truth_cosmic/I:truth_numuCC/I:truth_NC/I:truth_nue/I:truth_nuEnergy/D:pio_presel/I:pio_mass/D",',')
for e in t2:
  if e.status==1 and e.post_generic==1 and e.numuCC==1 and e.is_FC==1:
    h6.Fill(e.Ereco, e.weight) 
    h6w.Fill(e.Ereco, e.weight * e.weight) 

for i in range(h6.GetNbinsX()):
  h6.SetBinError(i+1, math.sqrt(h6w.GetBinContent(i+1)))

# infile3
h7 = rt.TH1F("h7","nu reco energy",30,0.15,3.15)
h7w = rt.TH1F("h7w","weight^2: nu reco energy",30,0.15,3.15)
h7.SetLineColor(2)
h7.SetMarkerColor(2)
h7.GetXaxis().SetTitle('Ereco [GeV]')

t3 = rt.TTree("t3","t3")
t3.ReadFile(infile3, "entry/I:run/I:subrun/I:event/I:weight/D:status/I:post_generic/I:numuCC/I:nueBDT/I:is_FC/I:Ereco/D:Evis/D:truth_cosmic/I:truth_numuCC/I:truth_NC/I:truth_nue/I:truth_nuEnergy/D:pio_presel/I:pio_mass/D",',')
for e in t3:
  if e.status==1 and e.post_generic==1 and e.numuCC==1 and e.is_FC==1:
    h7.Fill(e.Ereco, e.weight) 
    h7w.Fill(e.Ereco, e.weight * e.weight) 

for i in range(h7.GetNbinsX()):
  h7.SetBinError(i+1, math.sqrt(h7w.GetBinContent(i+1)))

# draw figures
# overlay
h1.Scale(5E19/5.926052E20)
h2.Scale(5E19/5.926052E20)
h3.Scale(5E19/5.926052E20)
h4.Scale(5E19/5.926052E20)

# BNB
h6.Scale(5E19/4.479E19)
h6.SetLineColor(1)
h6.SetLineWidth(2)
h6.SetMarkerStyle(4)
h6.SetMarkerSize(1)
h6.SetMarkerColor(1)
h6.Draw("hist e")

# EXTBNB
h7.Scale(5E19/5.8379E19)

hstack = rt.THStack("hs","")
set_hs(h3, 1001, rt.kOrange+1, 1)
set_hs(h2, 1001, rt.kAzure+6, 1)
set_hs(h4, 3244, rt.kMagenta, 1)
set_hs(h7, 3004, rt.kOrange+2, 1)
hstack.Add(h3) 
hstack.Add(h2)
hstack.Add(h4) 
hstack.Add(h7) 

h1.Add(h7)
h1.SetFillColor(rt.kGray+2)
h1.SetFillStyle(3002)
# h1.SetLineWidth(0)
h1.SetLineColor(1)
# h1.SetMarkerColor(1)
# h1.SetMarkerStyle(20)
h1.SetMarkerSize(0)
h1.GetXaxis().SetTitle("E_{reco} [GeV]")
h1.Draw("same E2")

hstack.Draw("hist same")

lg = rt.TLegend(0.47,0.42,0.87,0.89)
lg.AddEntry(h6, "BNB data (%d)" %h6.Integral(), "ep")
lg.AddEntry(h1, "Stat. error: EXT+MC", "eF")
lg.AddEntry(h7, "EXT (%d)" %h7.Integral(), "F")
lg.AddEntry(h4, "Cosmic (%d)" %h4.Integral(), "F")
lg.AddEntry(h2, "#nu_{#mu} CC in FV (%d)" %h2.Integral(), "F")
lg.AddEntry(h3, "#nu_{#mu} NC in FV (%d)" %h3.Integral(), "F")
lg.Draw()


ofile = rt.TFile("numuCC-reco-energy-FC.root","recreate")
h1.SetName("overlay_ext")
h1.Write()
h6.SetName("bnb")
h6.Write()
ofile.Close()


### set to interactive mode
import code
code.interact(local=locals())

