#!/bin/bash

# workdir
workdir=/uboone/app/users/wgu/NuSel/Validation/spool

# num of jobs to trigger a new job submission
MAX_NJOBS=5

# setup environment for jobsub_q
setup_env() {
    source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh;
    setup uboonecode v08_00_00_40 -q e17:prof;
    source /uboone/app/users/wgu/larsoft/jun17/localProducts_larsoft_v08_05_00_13_e17_prof/setup;
    source /cvmfs/larsoft.opensciencegrid.org/products/mrb/v4_01_03/bin/setup_local_products # i.e. mrbslp
}

# return num of jobs
check_njobs() {
  nlines=$(jobsub_q --user=wgu | wc -l)
  njobs=$((nlines - 3))
  echo $njobs 
}

#submit one job
submit_one() {
  cd $workdir
  first_xml=`ls new | head -n 1`
  if [ ! -z "$first_xml" ]; then
    echo project.py --xml new/$first_xml --stage port2 --submit
    echo mv new/$first_xml complete/
  else
    echo "No more new xml for project.py"
  fi
}

# main body
date
setup_env
njobs=$(check_njobs)
echo "number of remaining jobs: " $njobs
if [ $njobs -lt $MAX_NJOBS ]; then
  submit_one
else
  echo "More than $MAX_NJOBS jobs running, do not submit new jobs"
fi

# please add this script to crontab: crontab -e
# */1 * * * * /path/to/workdir/spool.sh >> /tmp/spool.x0726
