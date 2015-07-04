#!/bin/bash
# This script requires 2 arguments. The root of the DREAM3D_SDK (/Users/Shared/DREAM3D_SDK
# or /opt/DREAM3D_SDK) and the number of parallel processes to use to compile. This
# is typically 2x the number of physical cores in the machine.

SDK_INSTALL=$1

cd $SDK_INSTALL

PARALLEL_BUILD=$2



if [ ! -e "$SDK_INSTALL/Qt5.4.2" ];
then
  echo "#------------------------------"
  echo " You should have installed Qt 5.4.2 in the ${SDK_INSTALL}"
  echo " The Qt5.4.2 directory is not located in the ${SDK_INSTALL}"
  echo " Please install Qt5.4.2 first before running this script."
  echo " Your SDK is incomplete and you will not be able to build DREAM3D"
  echo "--------------------------------"
  exit

fi

export PATH=$PATH:$SDK_INSTALL/Qt5.4.2/5.4/clang_64/bin


# Build the qwt libraries we need and set our Environment Variable.
qwtArchiveName="qwt-6.1.2_src"

if [ ! -e "$SDK_INSTALL/${qwtArchiveName}.tar.gz" ];
then
  echo "-------------------------------------------"
  echo " You should have a Qwt 6.1.2 archive from DREAM3D."
  echo "-------------------------------------------"
  exit
fi

if [ ! -e "$SDK_INSTALL/${qwtArchiveName}" ];
then
  tar -xvzf ${qwtArchiveName}.tar.gz
# mv qwt-1.8.15 qwt-1.8.15_source
fi

# We assume we already have downloaded the source for qwt Version 6.1.2 and have it in a folder
# called qwt-6.1.2
cd ${qwtArchiveName}
$SDK_INSTALL/Qt5.4.2/5.4/clang_64/bin/qmake qwt.pro
make -j${PARALLEL_BUILD}
make install
export qwt_INSTALL=$SDK_INSTALL/qwt-1.8.15

