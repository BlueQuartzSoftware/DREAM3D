#!/bin/bash


SDK_INSTALL=/Users/Shared/DREAM3D_SDK
PARALLEL_BUILD=8


if [ ! -e "$SDK_INSTALL" ];
then
  sudo mkdir -p ${SKD_INSTALL}
  chmod ugo+rwx ${SKD_INSTALL}
fi

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

if [ ! -e "$SDK_INSTALL/Qt5.4.2" ];
then
  echo "-------------------------------------------"
  echo " An installation of Qt 5.4.2 was not found."
  echo " Downloading Qt5.4.2 into ${DREAM3D_SDK}"
  echo " Please mount and run the installer when prompted."
  echo " Please set the Installation Folder to ${DREAM3D_SDK}/Qt5.4.2"
  echo " When completed, rerun this script. If you need to download Qt 5.4.2"
  echo " then the proper web link is http://download.qt.io/archive/qt/5.4/5.4.2/qt-opensource-mac-x64-clang-5.4.2.dmg"
  echo "-------------------------------------------"
  #$DOWNLOAD_PROG  "http://anychimirror101.mirrors.tds.net/pub/Qt/archive/qt/5.4/5.4.2/qt-opensource-mac-x64-clang-5.4.2.dmg" -o ${DREAM3D_SDK}/qt-opensource-mac-x64-clang-5.4.2.dmg
  #open "${DREAM3D_SDK}/qt-opensource-mac-x64-clang-5.4.2.dmg"
  exit
fi

#-------------------------------------------------
# Copy our scripts over to the SDK directory
cp Build_Boost.sh ${SDK_INSTALL}/.
cp Build_Eigen.sh ${SDK_INSTALL}/.
cp Build_HDF5.sh ${SDK_INSTALL}/.
cp Build_TBB.sh ${SDK_INSTALL}/.
cp Build_Qwt.sh ${SDK_INSTALL}/.
cp Build_ITK.sh ${SDK_INSTALL}/.
cp FixITK.sh ${SDK_INSTALL}/.

#-------------------------------------------------
# Move into the SDK directory
cd ${SDK_INSTALL}

#-------------------------------------------------
# Unpack the DREAM3D Data (including the Small IN100)
tar -xvzf ${SDK_INSTALL}/DREAM3D_Data.tar.gz

#-------------------------------------------------
# Unpack CMake
tar -xvzf ${SDK_INSTALL}/cmake-3.3.0-rc3-Darwin-x86_64.tar.gz

#-------------------------------------------------
# Get CMake on our path
export PATH=$PATH:${SDK_INSTALL}/cmake-3.3.0-rc3-Darwin-x86_64/CMake.app/Contents/bin

#-------------------------------------------------
# Start building all the packages
${SDK_INSTALL}/Build_Qwt.sh "${SDK_INSTALL}" ${PARALLEL_BUILD}
rm ${SDK_INSTALL}/Build_Qwt.sh

${SDK_INSTALL}/Build_Eigen.sh "${SDK_INSTALL}" ${PARALLEL_BUILD}
rm ${SDK_INSTALL}/Build_Eigen.sh

${SDK_INSTALL}/Build_HDF5.sh "${SDK_INSTALL}" ${PARALLEL_BUILD}
rm ${SDK_INSTALL}/Build_HDF5.sh

${SDK_INSTALL}/Build_TBB.sh "${SDK_INSTALL}" ${PARALLEL_BUILD}
rm ${SDK_INSTALL}/Build_TBB.sh

${SDK_INSTALL}/Build_Boost.sh "${SDK_INSTALL}" ${PARALLEL_BUILD}
rm ${SDK_INSTALL}/Build_Boost.sh


#-------------------------------------------------
# We are NOT going to build ITK at this time since it is so difficult
# to get correct with our HDF5. This knocks out the ImageProcessing plugin
#${SDK_INSTALL}/Build_ITK.sh "${SDK_INSTALL}" ${PARALLEL_BUILD}
echo "------------------------------------------------------------"
echo "- NOT building ITK. ImageProcessing Plugin will NOT compile."
echo "------------------------------------------------------------"
rm ${SDK_INSTALL}/Build_ITK.sh
rm ${SDK_INSTALL}/FixITK.sh
