#!/bin/bash

cd /Users/mjackson/Workspace/DREAM3D/Source

files=`find . -type f -name "*.cpp"`

for x in $files;
  do
      echo "Converting $x"
      tr -d '\015' < "$x" > "$x.tmp"
      mv "$x.tmp" "$x"
  done
  
  files=`find . -type f -name "*.h"`

  for x in $files;
    do
        echo "Converting $x"
        tr -d '\015' < "$x" > "$x.tmp"
        mv "$x.tmp" "$x"
    done

