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


# First check to see if we have already downloaded the archive file
cd $SDK_INSTALL
if [ ! -e "$SDK_INSTALL/${PB_ARCHIVE_NAME}" ];
  then
  echo "-------------------------------------------"
  echo " Downloading Google Protool Buffers Version ${PB_ARCHIVE_NAME} "
  echo "-------------------------------------------"
  #$DOWNLOAD_PROG  "" -o $SDK_INSTALL/tbb${TBB_VERSION}oss_src.tgz
fi


# Next decompress the archive
if [ ! -e "$SDK_INSTALL/${PB_FOLDER_NAME}" ];
then
  tar -xvzf ${PB_ARCHIVE_NAME}
fi


cd $SDK_INSTALL/${PB_FOLDER_NAME}

CPPSTD=c++11    #c++89, c++99, c++14
STDLIB=libc++   # libstdc++

export LDFLAGS=-stdlib=$STDLIB
#CFLAGS="-isysroot $OSX_SDK -mmacosx-version-min=$OSX_DEPLOYMENT_TARGET -std=$CPPSTD -stdlib=$STDLIB"
#export CXXFLAGS="-isysroot $OSX_SDK -mmacosx-version-min=$OSX_DEPLOYMENT_TARGET -std=$CPPSTD -stdlib=$STDLIB"
#export CXXFLAGS="-isysroot $OSX_SDK -mmacosx-version-min=$OSX_DEPLOYMENT_TARGET -std=$CPPSTD"
CPPFLAGS="-isysroot $OSX_SDK -mmacosx-version-min=$OSX_DEPLOYMENT_TARGET -std=$CPPSTD -stdlib=$STDLIB"

./configure --prefix=$SDK_INSTALL/${PB_INSTALL} --enable-static --disable-shared
make -j$PARALLEL_BUILD
make install


echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Protocol Buffers For TCP/IP network message handling" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(Protobuf_DIR \"\${DREAM3D_SDK_ROOT}/${PB_INSTALL}\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"

