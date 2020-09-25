#!/bin/bash
defname=$1

# echo
# echo Finding dataset with defname: 
# echo $defname
# echo

# samweb list-files "defname:data_bnb_mcc9.1_wcp_port_v08_00_00_28_reco1.5_C1_5e19_reco1.5 AND event_count>19"
filelist=$(samweb list-files "defname:$defname AND event_count>19")
# filelist=$(samweb list-files "defname:$defname")
for filename in $filelist;do
  #samweb locate-file $filename
  url=$(samweb get-file-access-url $filename --schema=root)
  path="${url//root:\/\/fndca1.fnal.gov:1094\/pnfs\/fnal.gov\/usr//pnfs}"
  echo $path
done
