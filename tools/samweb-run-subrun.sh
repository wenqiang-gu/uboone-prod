#!/bin/bash

filename=$1
while read line; do
    input_artROOT=`basename $line`
    json=$(samweb get-metadata --json $input_artROOT)

    nSubruns=$(jq -r '.runs' <<< $json | jq length)
    i=0
    while [[ $i -lt $nSubruns ]]
    do
        run=$(jq -r ".runs[$i][0]" <<< $json)
        subrun=$(jq -r ".runs[$i][1]" <<< $json)
        echo $nSubruns $i $run $subrun
        ((i = i + 1))
    done

done < $filename
