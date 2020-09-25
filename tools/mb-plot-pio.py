#!/usr/bin/env python
import ROOT as rt
import sys
import math

infile1 = sys.argv[1]
infile2 = sys.argv[2]

h5 = rt.TH1F("h5","pi0 mass", 50, 0, 0.5)
h5w = rt.TH1F("h5w","weight^2", 50, 0, 0.5)

t1 = rt.TTree("t1","t1")
t1.ReadFile(infile1, "entry/I:run/I:subrun/I:event/I:weight/D:status/I:post_generic/I:numuCC/I:nueBDT/I:is_FC/I:Ereco/D:Evis/D:truth_cosmic/I:truth_numuCC/I:truth_NC/I:truth_nue/I:truth_nuEnergy/D:pio_presel/I:pio_mass/D",',')
for e in t1:
  if e.status==1 and e.post_generic==1 and e.numuCC==1: # and e.is_FC==1:

    if e.pio_presel==1:
      h5.Fill(e.pio_mass, e.weight)
      h5w.Fill(e.pio_mass, e.weight * e.weight)

for i in range(h5.GetNbinsX()):
  h5.SetBinError(i+1, math.sqrt(h5w.GetBinContent(i+1)))

h5.Scale(5E19/1.20E21)

# infile2
h6 = rt.TH1F("h6","pi0 mass", 50, 0, 0.5)
h6w = rt.TH1F("h6w","weight^2", 50, 0, 0.5)

t2 = rt.TTree("t2","t2")
t2.ReadFile(infile2, "entry/I:run/I:subrun/I:event/I:weight/D:status/I:post_generic/I:numuCC/I:nueBDT/I:is_FC/I:Ereco/D:Evis/D:truth_cosmic/I:truth_numuCC/I:truth_NC/I:truth_nue/I:truth_nuEnergy/D:pio_presel/I:pio_mass/D",',')
for e in t2:
  if e.status==1 and e.post_generic==1 and e.numuCC==1:# and e.is_FC==1:

    if e.pio_presel==1:
      h6.Fill(e.pio_mass, e.weight)
      h6w.Fill(e.pio_mass, e.weight * e.weight)

for i in range(h6.GetNbinsX()):
  h6.SetBinError(i+1, math.sqrt(h6w.GetBinContent(i+1)))

# h6.Scale(5E19/2.64E19)
h6.Scale(5E19/4.483E19)

# draw figure
h5.SetFillColor(4)
h5.SetMaximum(100)

h6.SetLineColor(1)
h6.SetMarkerColor(1)
h6.SetMarkerStyle(20)
h6.SetMarkerSize(1)

h5.Draw("hist")
h6.Draw("same")

lg = rt.TLegend(0.47,0.42,0.87,0.89)
lg.AddEntry(h6, "BNB data (%d)" %h6.Integral(), "ep")
lg.AddEntry(h5, "Overlay (%d)" %h5.Integral(), "F")
lg.Draw()


# output
# ofile = rt.TFile("pio_mass.root","update")
# h5.SetName("overlay")
# h5.Write()
# h6.SetName("bnb")
# h6.Write()
# ofile.Close()
