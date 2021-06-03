#!/bin/bash
#setup uboonecode v08_00_00_34 -q e17:prof
#setup wcp v00_13_03 -q e17:prof

if [ x$1 == x -o x$2 == x ]; then
	echo "Usage: generateFileList.sh RSE.list defname"
	exit 0
fi

if [ -f FileList.txt ]; then
	rm FileList.txt
fi

if [ -f SamDef.txt ]; then
	rm SamDef.txt
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
                echo "$run $subrun2 $event FileNotFound"
		continue
		#exit 1
	fi

	### full path 
	file2path=`samweb locate-file $filename`
	file2path0=${file2path#*:}
	file2path1=${file2path0%(*)}
	WCP_INFILE=$file2path1/$filename

	echo "file_name $filename or" >> SamDef.txt	
	echo $WCP_INFILE >> FileList.txt
done

#remove duplicate lines
awk '!x[$0]++' FileList.txt > sampleList.txt
awk '!y[$0]++' SamDef.txt > SamDefinition.txt

rm FileList.txt
rm SamDef.txt
