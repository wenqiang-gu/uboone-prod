#! /bin/bash
jobstat=/tmp/jobstat.txt
jobsub_q --user=uboonepro | awk '/jobsub/{print $6,$9}' | sed 's/\(.*\)\.sh_.*/\1/' > $jobstat
echo
echo N R I H X Job | awk '{printf "%6s%6s%6s%6s%6s  %s\n",$1,$2,$3,$4,$5,$6}'
echo _________________________________________________________
cat $jobstat | cut -d' ' -f2 | sort -u | while read job
do
  nj=`cat $jobstat | grep " $job\$"  | wc -l`
  njr=`cat $jobstat | grep " $job\$" | grep '^R ' | wc -l`
  nji=`cat $jobstat | grep " $job\$" | grep '^I ' | wc -l`
  njh=`cat $jobstat | grep " $job\$" | grep '^H ' | wc -l`
  njx=$(( $nj - $njr - $nji - $njh ))
  echo $nj $njr $nji $njh $njx $job | awk '{printf "%6d%6d%6d%6d%6d  %s\n",$1,$2,$3,$4,$5,$6}'
done
echo _________________________________________________________
nj=`cat $jobstat | wc -l`
njr=`cat $jobstat | grep '^R ' | wc -l`
nji=`cat $jobstat | grep '^I ' | wc -l`
njh=`cat $jobstat | grep '^H ' | wc -l`
njx=$(( $nj - $njr - $nji - $njh ))
echo $nj $njr $nji $njh $njx | awk '{printf "%6d%6d%6d%6d%6d  Total\n",$1,$2,$3,$4,$5}'
