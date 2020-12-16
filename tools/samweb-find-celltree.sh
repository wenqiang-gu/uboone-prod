#!/bin/bash


def_reco1d5=$1
WCP_celltree_dataset=$2

filename=$def_reco1d5.txt
samweb list-files "defname:$def_reco1d5" > $filename

ntotal=$(wc -l < $filename)
n=0
while read line; do
    input_artROOT=`basename $line`
    WCP_parent_file=`samweb get-metadata $input_artROOT | grep 'Parents:' | awk '{print $2}'`
    input_celltree=`samweb list-files "defname:$WCP_celltree_dataset and ischildof:(file_name $WCP_parent_file)" | xargs | awk '{print $NF}'`
    if [[ $input_celltree == "nusel"* ]]; then
        echo $input_celltree
    fi
    n=$((n+1))
    echo $n / $ntotal
done < $filename
