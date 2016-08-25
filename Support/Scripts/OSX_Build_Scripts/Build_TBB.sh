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

#https://www.threadingbuildingblocks.org/sites/default/files/software_releases/mac/tbb${TBB_VERSION}oss_src.tgz
# First check to see if we have already downloaded the archive file
cd $SDK_INSTALL
if [ ! -e "$SDK_INSTALL/tbb${TBB_VERSION}oss_src.tgz" ];
  then
  echo "-------------------------------------------"
  echo " Downloading TBB Version tbb${TBB_VERSION} "
  echo "-------------------------------------------"
  $DOWNLOAD_PROG  "https://www.threadingbuildingblocks.org/sites/default/files/software_releases/source/tbb${TBB_VERSION}oss_src.tgz" -o $SDK_INSTALL/tbb${TBB_VERSION}oss_src.tgz
fi


# Next decompress the archive
if [ ! -e "$SDK_INSTALL/tbb${TBB_VERSION}oss" ];
then
  tar -xvzf tbb${TBB_VERSION}oss_src.tgz
fi

# To build on OS X 10.6.8 some minor changes for the latest TBB 20140601 need to be made to the
# build/macosx.inc file to change from clang to gcc since the 10.6.8 clang does not work
# correctly for that combination.

cd $SDK_INSTALL/tbb${TBB_VERSION}oss

# Add the proper link flags
sed -i -e "s@LIB_LINK_FLAGS = -dynamiclib@LIB_LINK_FLAGS = -dynamiclib -headerpad_max_install_names@g" build/macos.clang.inc

# compile the source
make lambdas=1 cpp0x=1 compiler=clang stdlib=libc++ -j$PARALLEL_BUILD

rm -rf $SDK_INSTALL/tbb${TBB_VERSION}oss/lib
mkdir -p $SDK_INSTALL/tbb${TBB_VERSION}oss/lib

libs=`find . -type f -name "*tbb*.dylib"`
for lib in $libs;
do
  echo "Moving Library $lib"
  name=`basename $lib`
  mv $lib $SDK_INSTALL/tbb${TBB_VERSION}oss/lib/.
done


# If we are on OS X we need to update the "install_name" of each of the TBB libraries so CMake can properly include them into the app bundle.
if [[ "$HOST_SYSTEM" = "Darwin" ]];
  then
  tbbInstallDir="$SDK_INSTALL/tbb${TBB_VERSION}oss"
  cd "$tbbInstallDir/lib"
  tbbLibs="libtbb.dylib libtbbmalloc.dylib libtbb_debug.dylib libtbbmalloc_debug.dylib"
  for l in $tbbLibs;
  do
    echo "Updating install_name for $tbbInstallDir/lib/$l"
    install_name_tool -id $tbbInstallDir/lib/$l $l
  done

fi

mkdir -p $SDK_INSTALL/tbb${TBB_VERSION}oss/bin

chmod ugo+rwx $SDK_INSTALL/tbb${TBB_VERSION}oss/build/generate_tbbvars.sh
(cd $SDK_INSTALL/tbb${TBB_VERSION}oss/bin && exec $SDK_INSTALL/tbb${TBB_VERSION}oss/build/generate_tbbvars.sh)



echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Intel Threading Building Blocks Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(SIMPL_USE_MULTITHREADED_ALGOS ON CACHE BOOL \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(TBB_INSTALL_DIR  \"\${DREAM3D_SDK_ROOT}/tbb${TBB_VERSION}oss\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(TBB_ARCH_TYPE \"intel64\" CACHE STRING \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"


