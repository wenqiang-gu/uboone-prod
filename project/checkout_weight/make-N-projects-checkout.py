#!/usr/bin/env python
from string import Template
import json, sys
import os


# parst object to project xml
def execute(obj):
  # release = "WCP-0819"
  # file_type = "overlay" # data / overlay
  # name = "prodgenie_bnb_intrinsic_nue_overlay_WCP_DetVar_SCE_run3b"
  # inputlist = "/uboone/app/users/wgu/uboone-prod/dataset/DetVar/prodgenie_bnb_intrinsic_nue_overlay_WCP_DetVar_SCE_run3b.txt" 
  # celltree_dataset = "prodgenie_bnb_intrinsic_nue_uboone_overlay_detvar_sce_wcp_reco1.5_fixed_run3b_reco1.5_celltree"
  # WCP_tarball = "/pnfs/uboone/resilient/users/hanyuwei/WCPport/nueDev/WCP_20200819.tar"
  # larsoft_tarball = "/pnfs/uboone/resilient/users/hanyuwei/WCPport/nueDev/uboonecode_v08_00_00_42_Aug19.tar"
  # larsoft_tag = "v08_00_00_42"
  # larsoft_qual = "e17:prof"
  # initscript = "/uboone/app/users/wgu/NuSel/Validation/spool/initscript_wcpplus_inputlist.sh"
  # numjobs = -1 # -1: all jobs

  release = obj["release"]
  file_type = obj["file_type"]
  name = obj["name"]
  inputlist = obj["inputlist"]
  larsoft_tarball = obj["larsoft_tarball"]  
  larsoft_tag = obj["larsoft_tag"]  
  larsoft_qual =  obj["larsoft_qual"]
  numjobs =  obj["numjobs"]
  
  POT_inputTag = "generator"
  if file_type == "data":
    POT_inputTag = "beamdata:bnbETOR875"
  MC = obj["MC"]
  SaveWeights = obj["SaveWeights"]
  SaveLeeWeights = obj["SaveLeeWeights"]
  SaveFullWeights = obj["SaveFullWeights"]

  # set numjobs
  non_blank_count = 0
  with open(inputlist) as f:
    for line in f:
      if line.strip():
        non_blank_count += 1
  if numjobs<0 or numjobs>non_blank_count:
    numjobs = non_blank_count
  
  # template for run_WCPcheckoutWgt.fcl
  temp_content1 =  \
'''#include "time_memory_tracker_microboone.fcl"
#include "services_microboone.fcl"
process_name: WCPcheckoutWgt

services:
{
  TFileService: { fileName: "WCP_checkout_with_weights.root" }
  TimeTracker:             @local::microboone_time_tracker
  MemoryTracker:           @local::microboone_memory_tracker
  #FileCatalogMetadata:     @local::art_file_catalog_mc
  @table::microboone_services
}

source:
{
  module_type: RootInput
  fileNames:   ["dummy.root"]
  maxEvents:   -1
}

physics:
{
  analyzers:
  {
    wcpselection:
    {
      module_type:      "WCPcheckout"
      ContainmentLabel: "nuselMetrics"
      ChargeLabel:      "nuselMetrics"
      TruthLabel:       "nuselMetrics"
      MatchLabel:       "nuselMetrics"
      STMLabel:       	"nuselMetricsSTM"
      FileType:		"$name"

      MC:               $MC
      SaveWeights:	$SaveWeights
      SaveLeeWeights:	$SaveLeeWeights
      SaveFullWeights:	false

      POT_counting:	true
      POT_inputTag:	"generator" #this if for overlay. For data e.g. "beamdata:bnbETOR875"

      ## Wire-Cell particle flow
      wirecellPF:	true	
      BDTvars:		true
      KINEvars:		true
      PF_validation: 	true
      PF_inputtag:	"wirecellPF"
      PFtruth_inputtag: "largeant"
      Threshold_showerKE: 0.070 # units: GeV

    }

    wcpweights:
    {
      module_type:      "WCPweights"
      STMLabel:       	"nuselMetricsSTM"
      FileType:		"$name"

      SaveWeights:	$SaveWeights
      SaveLeeWeights:	$SaveLeeWeights
      SaveFullWeights:	$SaveFullWeights

    }

  }

  ana: [ wcpselection, wcpweights ]
  end_paths: [ ana ]
}

outputs:
{
 out1:
 {
   module_type: RootOutput
   dataTier: "reconstructed"
   compressionLevel: 1
   saveMemoryObjectThreshold: 0
   fileName: "%ifb_%tc_WCPsl.root"
 }
}

services.SpaceCharge.CalibrationInputFilename: "SpaceCharge/SCEoffsets_dataDriven_combined_bkwd_Jan18.root"
services.SpaceCharge.EnableCalEfieldSCE: true
services.SpaceCharge.EnableCalSpatialSCE: true
services.SpaceCharge.EnableCorrSCE: true
services.SpaceCharge.EnableSimSpatialSCE: true
services.SpaceCharge.EnableSimEfieldSCE: true
services.SpaceCharge.InputFilename: "SpaceCharge/SCEoffsets_dataDriven_combined_fwd_Jan18.root"
services.SpaceCharge.ResponseType: "Voxelized_TH3"
services.SpaceCharge.service_provider: "SpaceChargeServiceMicroBooNE"
services.DetectorPropertiesService.NumberTimeSamples: 6400
services.DetectorPropertiesService.ReadOutWindowSize: 6400
services.DetectorClocksService.TriggerOffsetTPC: -0.400e3
services.DetectorClocksService.InheritClockConfig:  false
#services.BackTrackerService:  @local::microboone_backtrackerservice
#services.ParticleInventoryService: @local::standard_particleinventoryservice
'''

  t = Template(temp_content1)
  content = t.substitute(name=name,
                         MC=MC,
                         SaveWeights=SaveWeights,
                         SaveLeeWeights=SaveLeeWeights,
                         SaveFullWeights=SaveFullWeights)
  with open("run_%s.fcl"%name,'w') as f:
    f.write(content) 



  # template for project xml
  temp_content =  \
'''<?xml version="1.0"?>

<!-- Production Project -->
<!DOCTYPE project [
<!ENTITY release "$release">
<!ENTITY file_type "$file_type">
<!ENTITY run_type "physics">
<!ENTITY user "wgu">
<!ENTITY name "$name-chkout">
]>


<job>

<project name="&name;">

  <!-- Project size -->
  <numevents>2000000</numevents>

  <!-- Operating System -->
  <os>SL7</os>

  <!-- Batch resources -->
  <resource>DEDICATED,OPPORTUNISTIC</resource>

  <!-- Larsoft information -->
  <larsoft>
    <tag>$larsoft_tag</tag>
    <qual>$larsoft_qual</qual>
    <local>$larsoft_tarball</local>
  </larsoft>
  
  <check>1</check>
  <!--copy>1</copy-->

  <!-- Project stages -->
  <stage name="port2">
    <fcl>$cwd/run_$name.fcl</fcl>
    <outdir>/pnfs/uboone/scratch/users/&user;/&release;/&name;/port/reco</outdir>
    <logdir>/pnfs/uboone/scratch/users/&user;/&release;/&name;/port/log</logdir>
    <workdir>/pnfs/uboone/scratch/users/&user;/&release;/&name;/port/work</workdir>
    <inputlist>$inputlist</inputlist>
    <maxfilesperjob>1</maxfilesperjob>
    <numjobs>$numjobs</numjobs>
    <memory>4000</memory> 
    <schema>root</schema>
    <jobsub>--expected-lifetime=8h --append_condor_requirements='(TARGET.HAS_CVMFS_uboone_opensciencegrid_org==true)&amp;&amp;(TARGET.HAS_CVMFS_uboone_osgstorage_org==true)'</jobsub>
    <jobsub_start>--expected-lifetime=short --append_condor_requirements='(TARGET.HAS_CVMFS_uboone_opensciencegrid_org==true)'</jobsub_start>
  </stage>

  <!-- file type -->
  <filetype>&file_type;</filetype>

  <!-- run type -->
  <runtype>&run_type;</runtype>

</project>


</job>
'''
  
  t = Template(temp_content)
  content = t.substitute(release=release,
                         file_type=file_type,
                         name=name,
                         larsoft_tag=larsoft_tag,
                         larsoft_qual=larsoft_qual,
                         larsoft_tarball=larsoft_tarball,
                         inputlist=inputlist,
                         numjobs=numjobs,
                         cwd=os.getcwd())
  
  with open("%s.xml"%name,'w') as f:
    f.write(content) 




# # parse json context to project list
# import copy, pprint
# pp = pprint.PrettyPrinter(indent=4)
# def parse_projects(context):
#   ret = []
#   default = copy.deepcopy(context)
#   del default["projects"]
#   for cfg in context["projects"]:
#     one = copy.deepcopy(default)
#     one.update(cfg)  
#     print('===')
#     pp.pprint(one)


# parse json context to project list
import copy, pprint
pp = pprint.PrettyPrinter(indent=4)
def parse_projects(context):
  ret = []
  default = copy.deepcopy(context)
  del default["projects"]
  for cfg in context["projects"]:
    default.update(cfg)  
    print('==='); pp.pprint(default)
    execute(default)

if __name__=='__main__':
  injson = sys.argv[1]
  with open(injson) as f:
    proj_context = json.load(f)
    projects = parse_projects(proj_context)

