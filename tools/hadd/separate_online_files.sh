#!/bin/bash

defname=$1

if [ -f online_path.txt ];then
  rm online_path.txt
fi

if [ -f offline_path.txt ];then
  rm offline_path.txt
fi

filelist=$(samweb list-files "defname:$defname")
for filename in $filelist;do
  #samweb locate-file $filename
  url=$(samweb get-file-access-url $filename --schema=root)
  path="${url//root:\/\/fndca1.fnal.gov:1094\/pnfs\/fnal.gov\/usr//pnfs}"
  echo $path

  checksum=$(timeout 0.5s md5sum $path)
  if [ -z "$checksum" ];
  then
    echo $path >> offline_path.txt
  else
    echo $path >> online_path.txt
  fi

done
