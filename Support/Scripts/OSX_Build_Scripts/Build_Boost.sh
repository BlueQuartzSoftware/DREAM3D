#!/bin/bash

#------------------------------------------------------------------------------
# Read the configuration file for the SDK Build
shopt -s extglob
configfile="SDK_Configuration.conf" # set the actual path name of your (DOS or Unix) config file
tr -d '\r' < $configfile > $configfile.unix
while IFS='= ' read lhs rhs
do
    if [[ ! $lhs =~ ^\ *# && -n $lhs ]]; then
        rhs="${rhs%%\#*}"    # Del in line right comments
        rhs="${rhs%%*( )}"   # Del trailing spaces
        rhs="${rhs%\"*}"     # Del opening string quotes 
        rhs="${rhs#\"*}"     # Del closing string quotes 
        declare $lhs="$rhs"
    fi
done < $configfile.unix
rm $configfile.unix
#------------------------------------------------------------------------------


cd $SDK_INSTALL

HOST_SYSTEM=`uname`
echo "Host System: $HOST_SYSTEM"


if [ ! -e "$SDK_INSTALL/$BOOST_ARCHIVE_NAME" ];
  then
  echo "-------------------------------------------"
  echo " Downloading Boost Version $BOOST_VERSION "
  echo "-------------------------------------------"
  $DOWNLOAD_PROG  "$BOOST_DOWNLOAD_SITE/$BOOST_ARCHIVE_NAME" -o $BOOST_ARCHIVE_NAME
fi

cd $SDK_INSTALL

# Next decompress the archive
if [ -e "$SDK_INSTALL/$BOOST_ARCHIVE_NAME" ];
then
  tar -xvzf $BOOST_ARCHIVE_NAME
fi

if [ -e "$SDK_INSTALL/${BOOST_FOLDER_NAME}_source" ];
 then
  rm -rf "$SDK_INSTALL/${BOOST_FOLDER_NAME}_source"
fi

mv "$SDK_INSTALL/$BOOST_FOLDER_NAME" "$SDK_INSTALL/${BOOST_FOLDER_NAME}_source"

CPPSTD=c++11    #c++89, c++99, c++14
STDLIB=libc++   # libstdc++
COMPILER=clang++
PARALLEL_MAKE=$PARALLEL_BUILD   # how many threads to make boost with


CXX_FLAGS="-isysroot $OSX_SDK -mmacosx-version-min=$OSX_DEPLOYMENT_TARGET -std=$CPPSTD -stdlib=$STDLIB"
LINKER_FLAGS="-stdlib=$STDLIB"
cd "$SDK_INSTALL/${BOOST_FOLDER_NAME}_source"
./bootstrap.sh
./b2 headers
./b2 -j$PARALLEL_BUILD --prefix=$SDK_INSTALL/boost-$BOOST_VERSION --layout=system --build-dir=x64 --architecture=x86 address-model=64 variant=release link=shared threading=multi runtime-link=shared cxxflags="$CXX_FLAGS" linkflags="${LINKER_FLAGS}" install
mkdir -p $SDK_INSTALL/boost-$BOOST_VERSION/lib/Debug
mkdir -p $SDK_INSTALL/boost-$BOOST_VERSION/lib/Release


# http://hivelocity.dl.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz

echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Boost Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(BOOST_ROOT \"\${DREAM3D_SDK_ROOT}/boost-$BOOST_VERSION\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
