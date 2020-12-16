#!/bin/bash

filename=$1
WCP_celltree_dataset=$2
m=0
n=0
while read line; do
    input_artROOT=`basename $line`
    WCP_parent_file=`samweb get-metadata $input_artROOT | grep 'Parents:' | awk '{print $2}'`
    input_celltree=`samweb list-files "defname:$WCP_celltree_dataset and ischildof:(file_name $WCP_parent_file)" | xargs | awk '{print $NF}'`
    #echo "$n : $input_artROOT $WCP_parent_file $input_celltree"
    if [[ $input_celltree == "nusel"* ]]; then
        echo $line
    	m=$((m+1))
    fi
    n=$((n+1))
done < $filename

echo "total # of files: $n"
echo "total # of valid files: $m"

# samweb list-files --summary "defname: prod_uboone_nu2020_fakedata_set5_reco1d5_v08_00_00_41_run1_reco1.5_celltree and file_name nuselOVERLAY%"
