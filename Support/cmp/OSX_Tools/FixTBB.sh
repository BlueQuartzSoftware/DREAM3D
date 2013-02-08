#!/bin/bash

TBB_ROOT=/Users/Shared/Toolkits/tbb41_20121003oss

cd $TBB_ROOT/lib

libs=`ls *.dylib`
for l in $libs
do
echo "------------------"
echo 'Changing '$l

install_name_tool -id $TBB_ROOT/lib/$l $l
otool -L $TBB_ROOT/lib/$l

done

