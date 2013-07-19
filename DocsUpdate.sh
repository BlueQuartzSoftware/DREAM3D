#!/bin/bash

cd Documentation/ReferenceManual/Filters

files=`find . -type f -name "*.md"`
for f in $files;
do

echo "" >> $f
echo "See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)" >> $f
echo "" >> $f

done
