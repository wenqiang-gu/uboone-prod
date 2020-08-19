#!/usr/bin/env python
from string import Template

###
release = "WCP-0817"
file_type = "overlay"
name = "prodgenie_bnb_nu_overlay_WCP_DetVar_CV_run1-1"
inputlist = "/uboone/app/users/wgu/uboone-prod/DetVar/prodgenie_bnb_nu_overlay_WCP_DetVar_CV_run1.txt"
celltree_dataset = "prodgenie_bnb_nu_uboone_overlay_detvar_cv_wcp_reco1.5_run1_reco1.5_celltree"
WCP_tarball = "/pnfs/uboone/resilient/users/hanyuwei/WCPport/nueDev/WCP_20200817.tar"
larsoft_tarball = "/pnfs/uboone/resilient/users/hanyuwei/WCPport/nueDev/uboonecode_v08_00_00_42_Aug18.tar"
larsoft_tag = "v08_00_00_42"
larsoft_qual = "e17:prof"
FHiCLs = ["run_wcpplus_port.fcl", "run_wcpf_port.fcl", "run_eventweight_microboone_mar18.fcl", "/uboone/app/users/wgu/NuSel/Validation/spool/run_WCPcheckout.fcl"]
initscript = "/uboone/app/users/wgu/NuSel/Validation/spool/initscript_wcpplus_inputlist.sh"
numjobs = -1 # all jobs

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
  <stage name="port2">
    $FHiCLs
    <outdir>/pnfs/uboone/scratch/users/&user;/&release;/&name;/port/reco</outdir>
    <logdir>/pnfs/uboone/scratch/users/&user;/&release;/&name;/port/log</logdir>
    <workdir>/pnfs/uboone/scratch/users/&user;/&release;/&name;/port/work</workdir>
    <inputlist>$inputlist</inputlist>
    <maxfilesperjob>1</maxfilesperjob>
    <numjobs>$numjobs</numjobs>
    <memory>4000</memory> 
    <schema>root</schema>
    <jobsub>--expected-lifetime=8h -e WCP_celltree_dataset=$celltree_dataset --tar_file_name=dropbox://$WCP_tarball --append_condor_requirements='(TARGET.HAS_CVMFS_uboone_opensciencegrid_org==true)&amp;&amp;(TARGET.HAS_CVMFS_uboone_osgstorage_org==true)'</jobsub>
    <jobsub_start>--expected-lifetime=short --append_condor_requirements='(TARGET.HAS_CVMFS_uboone_opensciencegrid_org==true)'</jobsub_start>
    <initscript>$initscript</initscript>
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
                       celltree_dataset=celltree_dataset,
                       WCP_tarball=WCP_tarball,
                       initscript=initscript)

with open("%s.xml"%name,'w') as f:
  f.write(content) 
