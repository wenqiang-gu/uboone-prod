#! /bin/bash

locality(){
  f=$1
  dir=`samweb locate-file $f | egrep 'enstore:|dcache:' | cut -d: -f2 | cut -d\( -f1`
  cd $dir
  cat ".(get)($f)(locality)"
}

def=$1
filelist=$(samweb list-files "defname:$def")
filecounts=$(samweb list-files --summary "defname: $def" | egrep "File count:" | cut -f2)
n=0 # no of files nearline (on tape)
m=0 # no of checked files 
for filename in $filelist;do
  # echo $filename
  # url=$(samweb get-file-access-url $filename --schema=root)
  # path="${url//root:\/\/fndca1.fnal.gov:1094\/pnfs\/fnal.gov\/usr//pnfs}"
  # echo $path

  status_full=$(locality $filename)
  IFS='\n'
  read -ra status_array <<< "$status_full"
  status="${status_array[0]}"
  IFS=''

  # echo $status
  if [[ $status == "NEARLINE" ]];then # "ONLINE" or "ONLINE_AND_NEARLINE" means on disk
    n=$((n+1))
  fi
  m=$((m+1))

  echo $n, $m, $filecounts [UNSTAGED, CHECKED, TOTAL]
done

