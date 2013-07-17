#!/bin/bash
# This should be rung from an "MSYS Git" bash shell prompt. Cygwin may also work.
cd Source
files=`find . -type f -name "*.cpp"`

for x in $files;
do
  echo "Converting $x"
 # tr -d '\015' < "$x" > "$x.tmp"
 # mv "$x.tmp" "$x"
 dos2unix -U $x
done

files=`find . -type f -name "*.h"`

for x in $files;
do
	echo "Converting $x"
 # tr -d '\015' < "$x" > "$x.tmp"
 # mv "$x.tmp" "$x"
 dos2unix -U $x
done

files=`find . -type f -name "*.hpp"`

for x in $files;
do
	echo "Converting $x"
 # tr -d '\015' < "$x" > "$x.tmp"
 # mv "$x.tmp" "$x"
 dos2unix -U $x
done