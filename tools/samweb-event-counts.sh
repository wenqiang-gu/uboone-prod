#!/bin/bash

filename=$1
m=0
n=0
while read line; do
    input_artROOT=`basename $line`
    event_count=`samweb get-metadata $input_artROOT | grep 'Event Count' | awk '{print $3}'`
    echo $event_count
done < $filename
