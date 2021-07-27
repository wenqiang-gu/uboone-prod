#!/bin/bash
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh
setup uboonecode v08_00_00_56 -q e17:prof

### set DL needed env
setup numpy v1_14_3 -q e17:openblas:p2714b:prof
setup libtorch v1_0_1 -q e17:prof
setup SparseConvNet 8422a6f -q e17:prof

### example usage:
# source runNueApp_original_v1.sh nue_RSE_signal.list data_bnb_mcc9.1_wcp_port_TEST_v08_00_00_28_reco1.5_C1_celltree
# source runNueApp_original_v1.sh nue_RSE_signal.list data_bnb_mcc9.1_wcp_port_TEST_v08_00_00_29_reco1.5_D1_celltree
# source runNueApp_original_v1.sh nue_RSE_signal.list data_bnb_mcc9.1_wcp_port_TEST_v08_00_00_29_reco1.5_D2_celltree
# source runNueApp_original_v1.sh nue_RSE_signal.list data_bnb_mcc9.1_wcp_port_TEST_v08_00_00_29_reco1.5_E1_celltree
# source runNueApp_original_v1.sh nue_RSE_signal.list data_bnb_mcc9.1_wcp_port_TEST_v08_00_00_29_reco1.5_F_celltree
# source runNueApp_original_v1.sh nue_RSE_signal.list data_bnb_mcc9.1_wcp_port_TEST_v08_00_00_29_reco1.5_G1_celltree
# source runNueApp_original_v1.sh nue_RSE_signal.list data_bnb_mcc9.1_wcp_port_TEST_v08_00_00_29_reco1.5_G2_celltree
# source runNueApp_original_v1.sh nue_RSE_signal.list data_bnb_mcc9.1_wcp_port_TEST_v08_00_00_29_reco1.5_G2a_celltree

### generate bee links:
# python dump_json.py 'bee/nue_*.root' charge cluster flash deadarea charge simple deblob mc vtx
# source upload-to-bee.sh to_upload.zip

locality(){
  f=$1
  dir=`samweb locate-file $f | egrep 'enstore:|dcache:' | cut -d: -f2 | cut -d\( -f1`
  pushd $dir
  cat ".(get)($f)(locality)"
  popd
}

  # status_full=$(locality $filename)
  # IFS='\n'
  # read -ra status_array <<< "$status_full"
  # status="${status_array[0]}"
  # IFS=''

  # echo $status
  #  if [[ $status == "NEARLINE" ]];then # "ONLINE" or "ONLINE_AND_NEARLINE" means on disk
  #    n=$((n+1))
  #    echo $filename >> $def-nearline.txt
  #  fi

###
if [ x$1 == x -o x$2 == x ]; then
	echo "Usage: script arg1[list.txt] defname"
	exit 0
fi

# if [ -f WCP_STM.log ]; then
# 	rm WCP_STM.log
# fi

# if [ -f bookkeep.txt ]; then
# 	rm bookkeep.txt
# fi
# 
# if [ -f Errors.txt ]; then
# 	rm Errors.txt
# fi

#if [ -d bee ]; then
#	rm -r bee
#fi

# if [ -d Reco1.5_celltree ]; then
#         rm -r Reco1.5_celltree
# fi

echo "Doing WCP setup"
#source ./setup.sh  #if using local tarball, and  not needed since (including) uboonecode v08_00_00_36 
echo "Create symlink to stash dCache WCP external data files"
ln -s /cvmfs/uboone.osgstorage.org/stash/wcp_ups/wcp/releases/tag/v00_10_00/input_data_files input_data_files
ln -s /cvmfs/uboone.osgstorage.org/stash/wcp_ups/wcp/releases/tag/v00_10_00/uboone_photon_library.root uboone_photon_library.root

if [ ! -d bee ]; then
	mkdir bee
fi
if [ ! -d Reco1.5_celltree ]; then
	mkdir Reco1.5_celltree
fi


IFS=$'\n'
for line in `cat $1 | awk '{print $0}'` #list.txt
do
	#echo $line
	run=`grep $line $1 | awk '{print $1}'`
	subrun=`grep $line $1 | awk '{printf("%04d",$2)}'`
	subrun2=`grep $line $1 | awk '{print $2}'`
	event=`grep $line $1 | awk '{print $3}'`
	echo "$run $subrun $event" 
	filename=`samweb list-files "defname: $2 and run_number $run.$subrun and first_event<=$event and last_event>=$event" | xargs | awk '{print $NF}'`
	echo $filename
	if [ x$filename = x ]; then
		echo "No file matched!"
                echo "$run $subrun2 $event FileNotFound" >> Errors.txt
		continue
		#exit 1
	fi

	### full path 
	file2path=`samweb locate-file $filename`
	file2path0=${file2path#*:}
	file2path1=${file2path0%(*)}
	WCP_INFILE=$file2path1/$filename
	
	echo $WCP_INFILE
	if [ x$IFDHC_DIR = x ]; then
    		echo "Setting up ifdhc before fetching local directory."
    		setup ifdhc
  	fi

        status_full=$(locality $filename)
        IFS='\n'
        read -ra status_array <<< "$status_full"
        status="${status_array[0]}"
        IFS=''
        if [[ $status == "NEARLINE" ]];then
		echo "File not on disk!"
		exit 2		
        fi

	if [ ! -f $filename ]; then 
		ifdh cp $WCP_INFILE $filename
	fi
	if [ ! -e $filename ]; then
		echo "File copy failed!"
		exit 2		
	fi	

	entry=`check_entrynum $filename ${run}_${subrun2}_${event}`
	echo "${run}_${subrun2}_${event} $filename $entry" >> bookkeep.txt
	wire-cell-prod-nue ./input_data_files/ChannelWireGeometry_v2.txt $filename $entry -d0 -b0
	if [ ! -f nue*.root ]; then
        	echo "${run}_${subrun2}_${event} $filename $entry NoNueFileProduced" >> Errors.txt
	fi
	mv nusel*.root Reco1.5_celltree/
	mv nue*.root bee/
done
