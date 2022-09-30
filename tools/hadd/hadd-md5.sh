#!/bin/bash
###
# A wrapper of ROOT's hadd command that requires a md5sum of input files
# It serves as a fast way to filter out unstaged (i.e. taped) files
#
# Usage: ./hadd-md5.sh <input txt> <output root>
# Input format: a text file with absolute paths
##

inputfile=$1
outputrootfile=$2
m=0
n=0
ntotal=$(wc -l $inputfile | awk '{print $1}')

if [ -f $inputfile.1 ]; then
  rm $inputfile.1
fi

if [ -f $inputfile.2 ]; then
  rm $inputfile.2
fi

while read line; do
  md5=$(timeout 0.5s md5sum $line)
  if [ -z "$md5" ];
  then # no valid md5sum
    echo $line >> $inputfile.1 
    m=$((m+1))
  else
    echo $line >> $inputfile.2
  fi
  n=$((n+1))
  echo -en "\r total: $ntotal on-disk: $((n-m)) on-tape: $m"
done < $inputfile

hadd -f $outputrootfile @$inputfile.2
# mv $inputfile.1 $inputfile
