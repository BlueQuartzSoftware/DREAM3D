#!/bin/bash

cd ../Source/

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

    files=`find . -type f -name "*.hpp"`

    for x in $files;
      do
          echo "Converting $x"
          tr -d '\015' < "$x" > "$x.tmp"
          mv "$x.tmp" "$x"
      done
      
cd ../Documentation/

    files=`find . -type f -name "*.md"`

    for x in $files;
      do
          echo "Converting $x"
          tr -d '\015' < "$x" > "$x.tmp"
          mv "$x.tmp" "$x"
      done
