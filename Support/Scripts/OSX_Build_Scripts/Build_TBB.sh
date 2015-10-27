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
fi


TBB_VERS=tbb43_20150611

#https://www.threadingbuildingblocks.org/sites/default/files/software_releases/mac/${TBB_VERS}oss_src.tgz
# First check to see if we have already downloaded the archive file
cd $SDK_INSTALL
if [ ! -e "$SDK_INSTALL/${TBB_VERS}oss_src.tgz" ];
  then
  echo "-------------------------------------------"
  echo " Downloading TBB Version ${TBB_VERS} "
  echo "-------------------------------------------"
  $DOWNLOAD_PROG  "https://www.threadingbuildingblocks.org/sites/default/files/software_releases/source/${TBB_VERS}oss_src.tgz" -o ${TBB_VERS}oss_src.tgz
fi


# Next decompress the archive
if [ ! -e "$SDK_INSTALL/${TBB_VERS}oss" ];
then
  tar -xvzf ${TBB_VERS}oss_src.tgz
fi

# To build on OS X 10.6.8 some minor changes for the latest TBB 20140601 need to be made to the
# build/macosx.inc file to change from clang to gcc since the 10.6.8 clang does not work
# correctly for that combination.

cd $SDK_INSTALL/${TBB_VERS}oss

# Add the proper link flags
sed -i -e "s@LIB_LINK_FLAGS = -dynamiclib@LIB_LINK_FLAGS = -dynamiclib -headerpad_max_install_names@g" build/macos.clang.inc

# compile the source
make -j8

rm -rf $SDK_INSTALL/${TBB_VERS}oss/lib
mkdir -p $SDK_INSTALL/${TBB_VERS}oss/lib

libs=`find . -type f -name "*tbb*.dylib"`
for lib in $libs;
do
  echo "Moving Library $lib"
  name=`basename $lib`
  mv $lib $SDK_INSTALL/${TBB_VERS}oss/lib/.
done


# If we are on OS X we need to update the "install_name" of each of the TBB libraries so CMake can properly include them into the app bundle.
if [[ "$HOST_SYSTEM" = "Darwin" ]];
  then
  tbbInstallDir="$SDK_INSTALL/${TBB_VERS}oss"
  cd "$tbbInstallDir/lib"
  tbbLibs="libtbb.dylib libtbbmalloc.dylib libtbb_debug.dylib libtbbmalloc_debug.dylib"
  for l in $tbbLibs;
  do
    echo "Updating install_name for $tbbInstallDir/lib/$l"
    install_name_tool -id $tbbInstallDir/lib/$l $l
  done

fi

mkdir -p $SDK_INSTALL/${TBB_VERS}oss/bin

chmod ugo+rwx $SDK_INSTALL/${TBB_VERS}oss/build/generate_tbbvars.sh
(cd $SDK_INSTALL/${TBB_VERS}oss/bin && exec $SDK_INSTALL/${TBB_VERS}oss/build/generate_tbbvars.sh)



echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Intel Threading Building Blocks Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(DREAM3D_USE_MULTITHREADED_ALGOS ON CACHE BOOL \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(TBB_INSTALL_DIR  \"\${DREAM3D_SDK_ROOT}/${TBB_VERS}oss\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(TBB_ARCH_TYPE \"intel64\" CACHE STRING \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"


