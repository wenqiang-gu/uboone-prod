#!/bin/bash

source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh

wc_workdir=`pwd`

touch wirecell.log
echo "larsoft and uboonecode version" | tee -a wirecell.log
echo $LARSOFT_VERSION | tee -a wirecell.log
echo $UBOONECODE_VERSION | tee -a wirecell.log
echo $WCP_VERSION | tee -a wirecell.log
date | tee -a wirecell.log

wc_input_celltree=`ls celltreeOVERLAY*.root | xargs | awk '{print $1}'`
if [ ! -e "$wc_input_celltree" ]; then
	echo "Celltree root not found!" | tee -a wirecell.log
	exit 201
fi

echo "contents in this directory: " | tee -a wirecell.log
ls -t | tee -a wirecell.log

wc_eventcount=`grep event_count celltreeOVERLAY*.json | awk '{print substr($2, 1, length($2)-1)}'`
echo "celltree input: $wc_input_celltree, number of events: $wc_eventcount" | tee -a wirecell.log

echo "now working in $wc_workdir" | tee -a wirecell.log
echo "will change to a new work directory " | tee -a wirecell.log
mkdir WCPwork
cp $wc_input_celltree WCPwork/

echo "set up additional environment for wirecell" | tee -a wirecell.log
cd WCPwork 
setup numpy v1_14_3 -q e17:openblas:p2714b:prof
setup libtorch v1_0_1 -q e17:prof
setup SparseConvNet 8422a6f -q e17:prof
export PYTHONPATH=$WCP_FQ_DIR/python:$PYTHONPATH #may not needed any longer; already appened in uboonecode

echo "Create symlink to stash dCache WCP external data files" | tee -a ../wirecell.log
ln -s /cvmfs/uboone.osgstorage.org/stash/wcp_ups/wcp/releases/tag/v00_10_00/input_data_files input_data_files
ln -s /cvmfs/uboone.osgstorage.org/stash/wcp_ups/wcp/releases/tag/v00_10_00/uboone_photon_library.root uboone_photon_library.root
ls | tee -a ../wirecell.log 

# event loop starts here
date | tee -a ../wirecell.log
touch WCP_STM.log
touch WCP_analysis.log
# stage1 (previously reco1.5) starts here
for ((n=0; n<$(($wc_eventcount)); n++))
do
	echo "event $n processing starts." | tee -a ../wirecell.log
	wire-cell-imaging-lmem-celltree ./input_data_files/ChannelWireGeometry_v2.txt $wc_input_celltree $n -d1 -s2 2>&1 | tee -a ../wirecell.log
	echo "event $n imaging finished." | tee -a ../wirecell.log
	###empty images
	numgoodcell=`tail -n 10 ../wirecell.log | grep "mcell" | awk '{print $5}'`
	echo "Number of good cells: $numgoodcell" | tee -a ../wirecell.log 
	if [ x$numgoodcell != x -a "$numgoodcell" = "0" ]; then
      		echo "WARNING: empty 3D image!" | tee -a ../wirecell.log
		touch nuseldummy_imaging${n}.root
		touch portdummy_imaging${n}.root
		rm result_*.root
		continue	
	fi

	echo "event $n matching starts."
	for imagingfile in result*.root
	do
		echo "$imagingfile" | tee -a ../wirecell.log
		input=$imagingfile
		input1=${input%.root}
		input2=${input1#*result_}
		echo $input2 | tee -a ../wirecell.log
		mv $imagingfile imaging_$input2.root
		#prod-nusel-eval ./input_data_files/ChannelWireGeometry_v2.txt imaging_$input2.root -d0 -p1 2>&1 | tee -a ../wirecell.log
		prod-wire-cell-matching-nusel ./input_data_files/ChannelWireGeometry_v2.txt imaging_$input2.root -d1 2>&1 | tee -a ../wirecell.log
		#mv nuselEval*.root nusel_$input2.root
		#mv port*.root porting_$input2.root
		warning=`tail -n 1 ../wirecell.log`
		echo "$warning"
		if [ "$warning" = "No points! Quit! " ]; then
        		echo "WARNING!" | tee -a ../wirecell.log
			touch nuseldummy_${input2}.root
			touch portdummy_${input2}.root
		fi
	done
	echo "event $n matching finished."
done

echo "Merge output" | tee -a ../wirecell.log
echo "contents in this directory: " | tee -a ../wirecell.log
ls -t | tee -a ../wirecell.log
hadd merge.root ./port_*.root
hadd nuselOVERLAY_WCP.root ./nuselEval_*.root

# stage2 (reco2) starts here
echo "wirecell stage2 starts here"
wc_input_celltree="nuselOVERLAY_WCP.root"
###
# The number of entries of the input file could be
# less than the total number of events in the artroot 
# when there are cases of "empty 3D imaging" from stage1.
# However, we still loop over the total number of events
# since we use the run/event from the input and the 
# wirecell apps do nothing when an entry number
# is out of the range.
##
for ((n=0; n<$(($wc_eventcount)); n++))
do
	wire-cell-prod-stm ./input_data_files/ChannelWireGeometry_v2.txt $wc_input_celltree $n -d0 -o0 -g2 2>&1 | tee -a ../wirecell.log
	stmentry=`tail -n 1 ../wirecell.log | awk '{print $2}'`
	if [ $stmentry = $n ]; then
        	echo "Warning: empty entry!" | tee -a ../wirecell.log
		touch "STMdummy_${n}.root"
		echo "event $n processing finished." | tee -a ../wirecell.log
		continue
	fi
	echo "event $n processing finished." | tee -a ../wirecell.log
	echo "event $n cosmic rejection starts." | tee -a ../wirecell.log
	for stmfile in stm*.root
	do
		echo "$stmfile" | tee -a ../wirecell.log
		stminput=$stmfile
		stminput1=${stminput%.root}
		stminput2=${stminput1#*stm_}
		echo $stminput2 | tee -a ../wirecell.log
		mv $stmfile STM_$stminput2.root
		### only if in-time match found
		test_stm STM_$stminput2.root 1>>WCP_STM.log 2>STMerror.log
		cat STMerror.log | tee -a ../wirecell.log 
	done
	echo "event $n cosmic rejection finished." | tee -a ../wirecell.log
	##### Run Wire-Cell pattern recognition ######
	echo "Input Event: $stminput2" | tee -a WCP_analysis.log
	if [ -z "$stminput2" ]; then
		echo "Warning: no stm output!" | tee -a ../wirecell.log
		continue	
	fi
	### only if pass cosmic rejction for now, later this part can be improved and implemented in WCP nue app ###
	wcreco2_runinfo=`grep -w $stminput2 WCP_STM.log | awk '{print $1}'`
        wcreco2_tag1=`grep -w $stminput2 WCP_STM.log | awk '{print $5}'`
        wcreco2_tag2=`grep -w $stminput2 WCP_STM.log | awk '{print $6}'`
        wcreco2_tag3=`grep -w $stminput2 WCP_STM.log | awk '{print $7}'`
        wcreco2_tag4=`grep -w $stminput2 WCP_STM.log | awk '{print $8}'`
        wcreco2_tag5=`grep -w $stminput2 WCP_STM.log | awk '{print $10}'`
        wcreco2_tag6=`grep -w $stminput2 WCP_STM.log | awk '{print $11}'`
        wcreco2_tag7=`grep -w $stminput2 WCP_STM.log | awk '{print $12}'`
        echo "WCP cosmic tagger: $wcreco2_runinfo $wcreco2_tag1 $wcreco2_tag2 $wcreco2_tag3 $wcreco2_tag4 $wcreco2_tag5 $wcreco2_tag6 $wcreco2_tag7" | tee -a WCP_analysis.log 
	if [ -n "$wcreco2_runinfo" ]; then 
        if [ $wcreco2_tag1 != 0 -a $wcreco2_tag2 = 0 -a $wcreco2_tag3 = 0 -a $wcreco2_tag4 = 0 -a $wcreco2_tag5 = 0 -a $wcreco2_tag6 = 0 -a $(echo "$wcreco2_tag7 > 0"|bc) = 1 ]; then
                #run=`echo $runinfo | awk -F "_" '{print $1}'`
                #subrun=`echo $runinfo | awk -F "_" '{print $2}'`
                #event=`echo $runinfo | awk -F "_" '{print $3}'`

		echo "event $n nue starts." | tee -a WCP_analysis.log	
		wire-cell-prod-nue ./input_data_files/ChannelWireGeometry_v2.txt $wc_input_celltree $n -d0 2>&1 | tee -a WCP_analysis.log
		echo "event $n nue finished." | tee -a WCP_analysis.log	
	
		if [ ! -e nue_${stminput2}.root ]; then
			echo "++> $stminput2 no output." | tee -a WCP_analysis.log
			return 203
		else	
			echo "event $n analysis starts." | tee -a WCP_analysis.log
			IFS=$'\n' #dviding symbol
			for pi0 in `tail -n 6 WCP_analysis.log | grep "Pi0"` #Pi0 found; Pi0 (displaced vertex) found
			do
				echo "==> $stminput2 $pi0" >> WCP_analysis.log
			done
			echo "event $n analysis finished." | tee -a WCP_analysis.log
		fi
        fi
	fi

done

echo "Check log" | tee -a ../wirecell.log
cat WCP_STM.log | tee -a ../wirecell.log
date | tee -a ../wirecell.log

echo "contents in this directory: " | tee -a ../wirecell.log
ls -t | tee -a ../wirecell.log

echo "Check the number of root files" | tee -a ../wirecell.log
wc_nevts1=`ls nusel*root | grep -E "Eval|dummy" | wc -l` # ignore nuselOVERLAY_WCP.root
wc_nevts2=`ls port*.root | wc -l`
wc_nevts3=`ls STM*.root | wc -l`
if [ $wc_nevts1 = $wc_nevts2 -a $wc_nevts1 = $wc_nevts3 -a $wc_nevts1 = $wc_eventcount ]; then
	echo "Good!" | tee -a ../wirecell.log
else
	echo "Bad: nusel $wc_nevts1, port $wc_nevts2, stm $wc_nevts3, input $wc_eventcount" | tee -a ../wirecell.log
 	##touch merge.root ### empty merge.root --> WCP failure	
	exit 204
fi


mv WCP_STM.log $wc_workdir
mv WCP_analysis.log $wc_workdir
# mv imaging*.root $wc_workdir
# mv nuselDATA_WCP.root $wc_workdir
# mv merge.root $wc_workdir # to not confuse production system
                            # keep it in WCPwork, see run_slimmed_port_data.fcl
# mv nue_*.root $wc_workdir # run_wcpf_port.fcl

cd $wc_workdir
rm celltreeOVERLAY*.root
echo "Done! Done! Done!" | tee -a wirecell.log
date | tee -a wirecell.log

exit 0 
