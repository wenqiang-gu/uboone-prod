#!/bin/bash
###
# Modified from Hanyu's end- and init-script by Wenqiang
# Instruction: 1) set the variables "input_artROOT" and "INPUT_TAR_FILE"
#              2) when it's necessary change `run_celltreeub_overlay_port_prod.fcl`
#                 to `run_celltreeub_overlay_wiremod_port_prod.fcl` entirely
#

source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh
#setup sam_web_client


workdir=`pwd`
touch wirecell.log

echo "################# Init of job" | tee -a wirecell.log

echo "larsoft and uboonecode version" | tee -a wirecell.log
echo $LARSOFT_VERSION | tee -a wirecell.log
echo $UBOONECODE_VERSION | tee -a wirecell.log
date | tee -a wirecell.log


# input_celltree=""
input_artROOT="/pnfs/uboone/data/uboone/reconstructed/prod_v08_00_00_28/data_bnb_mcc9.1_wcp_port_TEST/reco1.5_C1/00/00/53/28/PhysicsRun-2016_3_6_3_49_2-0005328-00048_20160306T193701_bnb_20160306T214055_merged_20181106T073556_optfilter_20181221T164410_reco1_postwcct_postdl_20181221T172022_postwcct.root"
INPUT_TAR_FILE="/pnfs/uboone/resilient/users/wgu/WCP_v00_14_07.tar"
# XROOTD_URI=""

cp $input_artROOT .
input_artROOT=`basename $input_artROOT`


lar -n -1 -c filter_data_beamdata_beamdataquality.fcl $input_artROOT | tee -a wirecell.log

input_artROOT0=`ls -t *filter.root | xargs | awk '{print $1}'` # find the last artROOT file
ls -t *.root | tee -a wirecell.log
echo $input_artROOT0 | tee -a wirecell.log
mv $input_artROOT0 $input_artROOT

cp /pnfs/uboone/resilient/users/wgu/run_celltreeub_overlay_port_prod.fcl .
cat <<EOT >>  run_celltreeub_overlay_port_prod.fcl
services.FileCatalogMetadata.applicationVersion: "v08_00_00_42"
services.FileCatalogMetadata.fileType: "overlay"
services.FileCatalogMetadata.runType: "physics"
services.FileCatalogMetadataMicroBooNE: {
FCLName: "" # "run_celltreeub_overlay_wiremod_port_prod.fcl"
FCLVersion: "" # "v08_00_00_42"
ProjectName: "" # "DetVar_nu_overlay_WireModThetaYZ_run3b_reco1.5-reco2"
ProjectStage: "" # "portAll"
ProjectVersion: "" # "v08_00_00_42"
}
services.TFileMetadataMicroBooNE: {
  JSONFileName:          [ "celltreeOVERLAY.root.json", "nuselOVERLAY_WCP.root.json"]
  GenerateTFileMetadata: [ true, true ]
  dataTier:              [ "celltree", "celltree" ]
  fileFormat:            [ "root", "root" ]
}
EOT
lar -n -1 -c run_celltreeub_overlay_port_prod.fcl $input_artROOT | tee -a wirecell.log

input_celltree=`ls celltreeOVERLAY*.root | xargs | awk '{print $1}'`
if [ ! -e "$input_celltree" ]; then
	echo "Celltree root not found!" | tee -a wirecell.log
	exit 201 
fi
echo "celltree input: $input_celltree" | tee -a wirecell.log
#if [ -e "$input_celltree" ]; then
#	#### the last matched file
#	input_artROOT0=`samweb list-files "defname: $RECO1_DATASET and isparentof:(file_name $input_celltree)" | xargs | awk '{print $NF}'`
#	echo "Matched reco1: $input_artROOT0" | tee -a wirecell.log	
#fi

# if there's naming issue (max length) this won't work.
# A general method should be tried
#input_artROOT0=`ls *_postwcct*.root | xargs | awk '{print $1}'`
#input_artROOT=${input_artROOT0%_detsim*}".root" #change parent file name. Be aware of how many fcls/stages run
#mv $input_artROOT0 $input_artROOT

# # This is a better method
# CPID=`cat cpid.txt`
# input_artROOT=($(ifdh translateConstraints "consumer_process_id=$CPID and consumed_status consumed"))
# stat=$?
# if [ $stat -ne 0 ]; then
# 	if [ -f condor_lar_input.list ]; then
#         	input_artROOT=`cat condor_lar_input.list | xargs basename`
# 	else
# 		echo "Failed to determine inputfile name!" | tee -a wirecell.log
#      		exit 202
# 	fi
# fi
# echo "Input artROOT: $input_artROOT found!" | tee -a wirecell.log

input_artROOT0=`ls -t *.root | xargs | awk '{print $2}'` # the 2nd last one is the artROOT (since the last one is celltree.root)
ls -t *.root | tee -a wirecell.log
echo $input_artROOT0 | tee -a wirecell.log
mv $input_artROOT0 $input_artROOT

if [ ! -e "$input_artROOT" ]; then
	echo "Reco1 artROOT not found!" | tee -a wirecell.log
	exit 203
fi

# eventcount=5
eventcount=`grep event_count celltreeOVERLAY*.json | awk '{print substr($2, 1, length($2)-1)}'`
echo "eventcount:" $eventcount | tee -a wirecell.log
if [ $eventcount -eq 0 ]; then
	exit 0
fi

echo "Make WCP work directory" | tee -a wirecell.log
echo `pwd`
mkdir WCPwork
cp $input_celltree WCPwork/

#echo "Checking tarball extraction" | tee -a wirecell.log
tar -xf $INPUT_TAR_FILE -C WCPwork

echo "Doing WCP setup" | tee -a wirecell.log
cd WCPwork 
source ./setup.sh
#setup wcp $WCP_VERSION -q e17:prof
echo "Create symlink to stash dCache WCP external data files" | tee -a ../wirecell.log
ln -s /cvmfs/uboone.osgstorage.org/stash/wcp_ups/wcp/releases/tag/v00_10_00/input_data_files input_data_files
ln -s /cvmfs/uboone.osgstorage.org/stash/wcp_ups/wcp/releases/tag/v00_10_00/uboone_photon_library.root uboone_photon_library.root
ls | tee -a ../wirecell.log 

#echo "Job process num"
#echo $PROCESS
date | tee -a ../wirecell.log
#for ((n=0; n<20; n++))
for ((n=0; n<$(($eventcount)); n++))
do
	echo "$n event processing starts." | tee -a ../wirecell.log
	wire-cell-imaging-lmem-celltree ./input_data_files/ChannelWireGeometry_v2.txt $input_celltree $n -d1 -s2 2>&1 | tee -a ../wirecell.log
	echo "$n event imaging finished." | tee -a ../wirecell.log
	echo "$n event matching starts."
	for imagingfile in result*.root
	do
		echo "$imagingfile" | tee -a ../wirecell.log
		input=$imagingfile
		input1=${input%.root}
		input2=${input1#*result_}
		echo $input2 | tee -a ../wirecell.log
		mv $imagingfile imaging_$input2.root
		#prod-nusel-eval ./input_data_files/ChannelWireGeometry_v2.txt imaging_$input2.root -d1 -p1 2>&1 | tee -a ../wirecell.log
		prod-wire-cell-matching-nusel ./input_data_files/ChannelWireGeometry_v2.txt imaging_$input2.root -d1 -z1 2>&1 | tee -a ../wirecell.log
		### Exception: "No points! Quit!" -- this is a "warning" instead of "error"!
		warning=`tail -n 1 ../wirecell.log`
		echo "$warning"
		if [ "$warning" = "No points! Quit! " ]; then
        		echo "WARNING!" | tee -a ../wirecell.log
			touch nuseldummy_${input2}.root
			touch portdummy_${input2}.root
		fi
	done
	echo "$n event matching finished."
done
date | tee -a ../wirecell.log

echo "Check the number of root files" | tee -a ../wirecell.log
nevts1=`ls nusel*.root | wc -l`
nevts2=`ls port*.root | wc -l`
if [ $nevts1 = $nevts2 -a $nevts1 = $eventcount ]; then
	echo "Good!" | tee -a ../wirecell.log
else
	echo "Bad: nusel $nevts1, port $nevts2, input $eventcount" | tee -a ../wirecell.log
 	##touch merge.root ### empty merge.root --> WCP failure	
	exit 204
fi

inputfilebase=`basename $input_artROOT`
bookkeep=${inputfilebase%.*}
echo "Merge output" | tee -a ../wirecell.log
ls *.root
hadd merge.root ./port_*.root
hadd nuselOVERLAY_WCP.root ./nuselEval_*.root

echo "++++++++++++++++++++++++++++"
echo "start reco 2"
input_celltree="nuselOVERLAY_WCP.root" # not a celltree any more

# update the eventcount when it exists dummy files from reco 1.5
N_nuseldummy=$(ls nulseldummy*root | wc -l)
eventcount=$((eventcount-N_nuseldummy))

date | tee -a ../wirecell.log
touch WCP_STM.log
touch WCP_analysis.log
for ((n=0; n<$(($eventcount)); n++))
do
	echo "$n event processing starts." | tee -a ../wirecell.log
	wire-cell-prod-stm ./input_data_files/ChannelWireGeometry_v2.txt $input_celltree $n -d0 -o0 -g2 -z1 2>&1 | tee -a ../wirecell.log
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
		wire-cell-prod-nue ./input_data_files/ChannelWireGeometry_v2.txt $input_celltree $n -d0 -z1 2>&1 | tee -a WCP_analysis.log
		echo "$n event nue finished." | tee -a WCP_analysis.log	
	
		if [ ! -e nue_${input2}.root ]; then
			echo "++> $input2 no output." | tee -a WCP_analysis.log
			exit 205
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

echo "end reco 2"
echo "+++++++++++++++++++++++++++++"


#mv imaging*.root $workdir
mv nuselOVERLAY_WCP.root $workdir
mv merge.root $workdir

cd $workdir
#cp merge.root ${bookkeep}_WCPport.root
echo "Done! Done! Done!" | tee -a wirecell.log
date | tee -a wirecell.log


echo "+++++++++++++++++++++++++++++"
echo "art port"
# change nuselEval json file
#sed -i "s/_detsim_mix.root/.root/g" nusel*.root.json

#make a new wrapper.fcl (Stage?.fcl)
## find the last stage
# laststagefcl=`ls -l Stage*.fcl | xargs | awk '{print $NF}'`
# sed -e "s/run_celltreeub\(.*\).fcl/run_slimmed_port_overlay.fcl/g" -e "s/FCLName:.*\"/FCLName: \"run_slimmed_port_overlay.fcl\"/g" $laststagefcl > port.fcl #multiple fhicls/stages, use the celltree one (last one) to generate the port stage fcl including all necessary metadata

cp /pnfs/uboone/resilient/users/wgu/run_slimmed_port_overlay.fcl .
lar -c run_slimmed_port_overlay.fcl -n -1 -s $input_artROOT | tee -a wirecell.log
rm $input_artROOT


echo "+++++++++++++++++++++++++++++"
echo "start reco 2 (2nd time)"
input_artROOT0=`ls -t *overlayWCP.root | xargs | awk '{print $1}'` # find the last artROOT file
ls -t *.root | tee -a wirecell.log
echo $input_artROOT0 | tee -a wirecell.log
mv $input_artROOT0 $input_artROOT
lar -n -1 -c run_wcpplus_port.fcl -s $input_artROOT | tee -a wirecell.log

input_artROOT0=`ls -t *WCPplus.root | xargs | awk '{print $1}'` # find the last artROOT file
ls -t *.root | tee -a wirecell.log
echo $input_artROOT0 | tee -a wirecell.log
mv $input_artROOT0 $input_artROOT
lar -n -1 -c run_wcpf_port.fcl -s $input_artROOT | tee -a wirecell.log


input_artROOT0=`ls -t *WCPF.root | xargs | awk '{print $1}'` # find the last artROOT file
ls -t *.root | tee -a wirecell.log
echo $input_artROOT0 | tee -a wirecell.log
mv $input_artROOT0 $input_artROOT
lar -n -1 -c run_eventweight_microboone_mar18.fcl -s $input_artROOT | tee -a wirecell.log

echo "end reco 2 (2nd time)"
echo "+++++++++++++++++++++++++++++"


rm $input_artROOT # duplicated artROOT
# rm $input_celltree 
# rm merge.root
rm reco_stage_*_hist.root
rm detsim_hist.root
rm DataOverlayMixer_hist.root

mkdir out
cp WCP_STM.log out/
cp WCP_analysis.log out/
cp wirecell.log out/

exit 0 
