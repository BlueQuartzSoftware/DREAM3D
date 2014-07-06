#!/bin/bash

export PATH=/Applications/UniversalBinary/GMT/GMT-5.1.0.app/Contents/Resources/bin:${PATH}
gmt

DREAM3D_EXE=/Users/mjackson/Workspace/DREAM3D_Rewrite/zRel/Bin/PipelineRunner
GEN_GMT=/Users/mjackson/Workspace/DREAM3D_Rewrite/Support/Scripts/GMT/Draw_stereograms-gmt-v5.1.sh

pipelineFile=/Users/mjackson/Desktop/MultiGBCD.txt
inputDir=/Volumes/Untitled/IN100_EXTRACTED


cd /Users/mjackson/Workspace/DREAM3D_Rewrite/zRel/Bin/
dirs="0 1 2 3 4 5 10 15 20 25 50 75 100 150"

for d in $dirs;
do
	echo $d
	d=`basename $d`
	pname=`basename $pipelineFile`
	cp $pipelineFile /tmp/$pname
	# Change the install prefix to our 
	sed -i -e "s@SLICE_NUMBER@$d@g" /tmp/$pname

	$DREAM3D_EXE -p /tmp/$pname


	$GEN_GMT 1 Data/Output/SmallIN100GBCD/$d/SmallIn100GMT_${d}_ 5d rainbow 0 125 10 stereo CUBIC
	mv Data/Output/SmallIN100GBCD/$d/SmallIn100GMT_${d}_.png Data/Output/SmallIN100GBCD/SmallIn100GMT_${d}_.png 

done

