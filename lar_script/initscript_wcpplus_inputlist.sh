#!/bin/bash
#source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh

workdir=`pwd`
if [ -f wirecell.log ]; then
        exit 0 #only run once for the first fcl
else
        touch wirecell.log
fi


echo "################# Init of job" | tee -a wirecell.log

echo "larsoft and uboonecode version" | tee -a wirecell.log
echo $LARSOFT_VERSION | tee -a wirecell.log
echo $UBOONECODE_VERSION | tee -a wirecell.log
date | tee -a wirecell.log

WCP_parent_file=""
input_celltree=""
input_artROOT=""
WCP_XROOTD_URI=""

if [ -f condor_lar_input.list ]; then
	input_artROOT=`cat condor_lar_input.list | xargs basename`
else 
	echo "Input file not found!" | tee -a wirecell.log
	exit 201
fi
echo "Input reco1.5: $input_artROOT" | tee -a wirecell.log	

WCP_parent_file=`samweb get-metadata $input_artROOT | grep 'Parents:' | awk '{print $2}'`
# eventcount=`samweb get-metadata $input_artROOT | grep 'Event Count' | awk '{print $3}'`
echo "Parent file: $WCP_parent_file" | tee -a wirecell.log
echo "Celltree dataset: $WCP_celltree_dataset" | tee -a wirecell.log
input_celltree=`samweb list-files "defname:$WCP_celltree_dataset and ischildof:(file_name $WCP_parent_file)" | xargs | awk '{print $NF}'`
echo "celltree input: $input_celltree" | tee -a wirecell.log
eventcount=`samweb get-metadata $input_celltree | grep 'Event Count' | awk '{print $3}'`

### full path 
file2path=`samweb locate-file $input_celltree`
file2path0=${file2path#*:}
file2path1=${file2path0%(*)}
WCP_INFILE=$file2path1/$input_celltree

### use WCP_XROOTD_URI=`file_to_url.sh $WCP_INFILE` later when TFile::Open is used in API
echo "Copying $WCP_INFILE" | tee -a wirecell.log
ifdh cp $WCP_INFILE $input_celltree
if [ ! -e "$input_celltree" ]; then
	echo "Celltree root not found!" | tee -a wirecell.log
	exit 202
fi

echo "Make WCP work directory" | tee -a wirecell.log
echo `pwd`
mkdir WCPwork
cp $input_celltree WCPwork/

echo "Checking tarball extraction" | tee -a wirecell.log
tar -xf $INPUT_TAR_FILE -C WCPwork

echo "Doing WCP setup" | tee -a wirecell.log
cd WCPwork 
source ./setup.sh
#setup glpk v4_65 -q e17:prof
#setup wcp $WCP_VERSION -q e17:prof now part of uboonecode

echo "Create symlink to stash dCache WCP external data files" | tee -a ../wirecell.log
ln -s /cvmfs/uboone.osgstorage.org/stash/wcp_ups/wcp/releases/tag/v00_10_00/input_data_files input_data_files
ln -s /cvmfs/uboone.osgstorage.org/stash/wcp_ups/wcp/releases/tag/v00_10_00/uboone_photon_library.root uboone_photon_library.root

ls | tee -a ../wirecell.log 

#echo "Job process num"
#echo $PROCESS
date | tee -a ../wirecell.log
touch WCP_STM.log
touch WCP_analysis.log
#for ((n=0; n<20; n++))
for ((n=0; n<$(($eventcount)); n++))
do
	echo "$n event processing starts." | tee -a ../wirecell.log
	wire-cell-prod-stm ./input_data_files/ChannelWireGeometry_v2.txt $input_celltree $n -d0 -o0 -g2 2>&1 | tee -a ../wirecell.log
	echo "$n event processing finished." | tee -a ../wirecell.log
	echo "$n event cosmic rejection starts." | tee -a ../wirecell.log
	for stmfile in stm*.root
	do
		echo "$stmfile" | tee -a ../wirecell.log
		input=$stmfile
		input1=${input%.root}
		input2=${input1#*stm_}
		echo $input2 | tee -a ../wirecell.log
		mv $stmfile STM_$input2.root
		test_stm STM_$input2.root 1>>WCP_STM.log 2>STMerror.log
		cat STMerror.log | tee -a ../wirecell.log 
	done
	echo "$n event cosmic rejection finished." | tee -a ../wirecell.log
	##### Run Wire-Cell pattern recognition ######
	echo "Input Event: $input2" | tee -a WCP_analysis.log

	### only if pass cosmic rejction for now, later this part can be improved and implemented in WCP nue app ###
	runinfo=`grep -w $input2 WCP_STM.log | awk '{print $1}'`
        tag1=`grep -w $input2 WCP_STM.log | awk '{print $5}'`
        tag2=`grep -w $input2 WCP_STM.log | awk '{print $6}'`
        tag3=`grep -w $input2 WCP_STM.log | awk '{print $7}'`
        tag4=`grep -w $input2 WCP_STM.log | awk '{print $8}'`
        tag5=`grep -w $input2 WCP_STM.log | awk '{print $10}'`
        tag6=`grep -w $input2 WCP_STM.log | awk '{print $11}'`
        tag7=`grep -w $input2 WCP_STM.log | awk '{print $12}'`
        echo "WCP cosmic tagger: $runinfo $tag1 $tag2 $tag3 $tag4 $tag5 $tag6 $tag7" | tee -a WCP_analysis.log 
	if [ -n "$runinfo" ]; then 
        if [ $tag1 != 0 -a $tag2 = 0 -a $tag3 = 0 -a $tag4 = 0 -a $tag5 = 0 -a $tag6 = 0 -a $(echo "$tag7 > 0"|bc) = 1 ]; then
                #run=`echo $runinfo | awk -F "_" '{print $1}'`
                #subrun=`echo $runinfo | awk -F "_" '{print $2}'`
                #event=`echo $runinfo | awk -F "_" '{print $3}'`

		echo "$n event nue starts." | tee -a WCP_analysis.log	
		wire-cell-prod-nue ./input_data_files/ChannelWireGeometry_v2.txt $input_celltree $n -d0 2>&1 | tee -a WCP_analysis.log
		echo "$n event nue finished." | tee -a WCP_analysis.log	
	
		if [ ! -e nue_${input2}.root ]; then
			echo "++> $input2 no output." | tee -a WCP_analysis.log
		else	
			echo "$n event analysis starts." | tee -a WCP_analysis.log
			IFS=$'\n' #dviding symbol
			for pi0 in `tail -n 6 WCP_analysis.log | grep "Pi0"` #Pi0 found; Pi0 (displaced vertex) found
			do
				echo "==> $input2 $pi0" >> WCP_analysis.log
			done
			echo "$n event analysis finished." | tee -a WCP_analysis.log
		fi
        fi
	fi
	### END ###

done
echo "Check log" | tee -a ../wirecell.log
cat WCP_STM.log | tee -a ../wirecell.log
date | tee -a ../wirecell.log

echo "Check the number of root files" | tee -a ../wirecell.log
nevts1=`ls STM*.root | wc -l`
if [ $nevts1 = $eventcount ]; then
	echo "Good!" | tee -a ../wirecell.log
else
	echo "Bad: nusel $nevts1, input $eventcount" | tee -a ../wirecell.log
 	##touch merge.root ### empty merge.root --> WCP failure	
	exit 203
fi

mv WCP_STM.log $workdir
mv WCP_analysis.log $workdir
mv nue_*.root $workdir

cd $workdir
mkdir out
cp WCP_STM.log out/
cp WCP_analysis.log out/
echo "Done! Done! Done!" | tee -a wirecell.log
echo "+++++++++++++++++++++++++++++"
rm $input_celltree
date | tee -a wirecell.log

exit 0 
