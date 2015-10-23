#!/bin/bash
# This script requires 2 arguments. The root of the DREAM3D_SDK (/Users/Shared/DREAM3D_SDK
# or /opt/DREAM3D_SDK) and the number of parallel processes to use to compile. This
# is typically 2x the number of physical cores in the machine.

SDK_INSTALL=$1

cd $SDK_INSTALL

PARALLEL_BUILD=$2


HOST_SYSTEM=`uname`
echo "Host System: $HOST_SYSTEM"

WGET=`type -P wget`
CURL=`type -P curl`

if [[ "$WGET" == "" ]];
   then
  if [[ "$CURL" == "" ]];
     then
    echo "wget and curl are NOT present on your machine. One of them is needed to download sources from the internet."
    exit 1
  fi
fi


DOWNLOAD_PROG=""
DOWNLOAD_ARGS=""

if [[ "$WGET" != "" ]];
then
  DOWNLOAD_PROG=$WGET
fi

if [[ "$CURL" != "" ]];
then
  DOWNLOAD_PROG=$CURL
  DOWNLOAD_ARGS=""
fi


if [ ! -e "$SDK_INSTALL/boost_1_58_0.tar.gz" ];
  then
  echo "-------------------------------------------"
  echo " Downloading Boost Version boost_1_58_0.tar.gz "
  echo "-------------------------------------------"
  $DOWNLOAD_PROG  "http://iweb.dl.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz" -o boost_1_58_0.tar.gz
fi

cd $SDK_INSTALL

# Next decompress the archive
if [ -e "$SDK_INSTALL/boost_1_58_0.tar.gz" ];
then
  tar -xvzf boost_1_58_0.tar.gz
  mv "$SDK_INSTALL/boost_1_58_0" "$SDK_INSTALL/boost_1_58_0_source"
fi

cd "$SDK_INSTALL/boost_1_58_0_source"
./bootstrap.sh
./b2 headers
./b2 -j$PARALLEL_BUILD --prefix=$SDK_INSTALL/boost-1.58.0  --layout=system --build-dir=x64 --architecture=x86 address-model=64 variant=release link=shared threading=multi runtime-link=shared install
mkdir -p $SDK_INSTALL/boost-1.58.0/lib/Debug
mkdir -p $SDK_INSTALL/boost-1.58.0/lib/Release


# http://hivelocity.dl.sourceforge.net/project/boost/boost/1.56.0/boost_1_56_0.tar.gz


echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Boost Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(BOOST_ROOT \"\${DREAM3D_SDK_ROOT}/boost-1.58.0\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
