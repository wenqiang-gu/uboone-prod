#!/usr/bin/env python
from string import Template
import json, sys


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
  # FHiCLs = ["run_wcpplus_port.fcl", "run_wcpf_port.fcl", "run_eventweight_microboone_mar18.fcl", "run_eventweight_microboone_LEE.fcl"] #, "/uboone/app/users/wgu/NuSel/Validation/spool/run_WCPcheckout.fcl"]
  # endscript = "/uboone/app/users/wgu/NuSel/Validation/spool/initscript_wcpplus_inputlist.sh" # FIXME: give a "end" script
  # numjobs = -1 # -1: all jobs

  release = obj["release"]
  file_type = obj["file_type"]
  name = obj["name"]
  inputlist = obj["inputlist"]
  # celltree_dataset = obj["celltree_dataset"]  
  WCP_tarball = obj["WCP_tarball"]  
  larsoft_tarball = obj["larsoft_tarball"]  
  larsoft_tag = obj["larsoft_tag"]  
  larsoft_qual =  obj["larsoft_qual"]
  FHiCLs =  obj["FHiCLs"]
  if "WireMod".lower() in name.lower():
    FHiCLs = obj["WireModFHiCLs"]
  endscript =  obj["endscript"]
  numjobs =  obj["numjobs"]
  
  
  # set numjobs
  non_blank_count = 0
  with open(inputlist) as f:
    for line in f:
      if line.strip():
        non_blank_count += 1
  if numjobs<0 or numjobs>non_blank_count:
    numjobs = non_blank_count
  
  # set fhicls
  fhicls = ""
  for fcl in FHiCLs:
    fhicls += "<fcl>%s</fcl>\n" %fcl
  
  # check celltree def name
  # if not celltree_dataset.endswith("celltree"):
  #   print("Error! CellTree dataset name should end with ``celltree``")
  #   exit()
  
  # template for project xml
  temp_content =  \
'''<?xml version="1.0"?>

<!-- Production Project -->
<!DOCTYPE project [
<!ENTITY release "$release">
<!ENTITY file_type "$file_type">
<!ENTITY run_type "physics">
<!ENTITY user "wgu">
<!ENTITY name "$name">
]>


<job>

<project name="&name;">

  <!-- Project size -->
  <numevents>200000</numevents>

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
  <stage name="portAll">
    $FHiCLs
    <outdir>/pnfs/uboone/scratch/users/&user;/&release;/&name;/port/reco</outdir>
    <logdir>/pnfs/uboone/scratch/users/&user;/&release;/&name;/port/log</logdir>
    <workdir>/pnfs/uboone/scratch/users/&user;/&release;/&name;/port/work</workdir>
    <inputlist>$inputlist</inputlist>
    <maxfilesperjob>1</maxfilesperjob>
    <numjobs>$numjobs</numjobs>
    <memory>4000</memory> 
    <schema>root</schema>
    <jobsub>--expected-lifetime=8h -f /pnfs/uboone/resilient/users/wgu/run_slimmed_port_overlay.fcl --tar_file_name=dropbox://$WCP_tarball --append_condor_requirements='(TARGET.HAS_CVMFS_uboone_opensciencegrid_org==true)&amp;&amp;(TARGET.HAS_CVMFS_uboone_osgstorage_org==true)'</jobsub>
    <jobsub_start>--expected-lifetime=short --append_condor_requirements='(TARGET.HAS_CVMFS_uboone_opensciencegrid_org==true)'</jobsub_start>
    <endscript>$endscript</endscript>
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
                         FHiCLs=fhicls,
                         inputlist=inputlist,
                         numjobs=numjobs,
                         # celltree_dataset=celltree_dataset,
                         WCP_tarball=WCP_tarball,
                         endscript=endscript)
  
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

