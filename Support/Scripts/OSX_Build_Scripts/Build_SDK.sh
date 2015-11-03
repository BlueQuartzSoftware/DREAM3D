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


echo "SDK_INSTALL=$SDK_INSTALL"
echo "PARALLEL_BUILD=$PARALLEL_BUILD"

if [ ! -e "$SDK_INSTALL" ];
then
  sudo mkdir -p ${SDK_INSTALL}
  chmod ugo+rwx ${SDK_INSTALL}
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

if [ ! -e "/Applications/Doxygen.app" ];
then
  echo "--------------------------------------------"
  echo "Doxygen is missing from your system."
  echo "Downloading Doxygen 1.8.10 for you."
  $DOWNLOAD_PROG  "$DOXYGEN_DOWNLOAD_SITE/$DOXYGEN_ARCHIVE_NAME" -o "${DREAM3D_SDK}/$DOXYGEN_ARCHIVE_NAME"
  open "${DREAM3D_SDK}/$DOXYGEN_ARCHIVE_NAME"
  echo "Please Copy the Doxygen.app from the mounted disk image into the /Applications directory. CMake can most"
  echo "easily find it in this location."
fi

SCRIPT_DIR=`pwd`

#-------------------------------------------------
# Move one Directory Above the SDK Folder and untar the
if [ -e "$SDK_INSTALL/../$SDK_ARCHIVE_FILENAME" ];
  then
  cd "$SDK_INSTALL/../"
  tar -xvzf $SDK_ARCHIVE_FILENAME
fi

#-------------------------------------------------
# Move into the SDK directory
cd ${SDK_INSTALL}

#-------------------------------------------------
# Unpack the DREAM3D Data (including the Small IN100)
tar -xvzf ${SDK_INSTALL}/$DREAM3D_DATA_ARCHIVE_NAME

#-------------------------------------------------
# Unpack CMake
tar -xvzf ${SDK_INSTALL}/$CMAKE_FOLDER_NAME.tar.gz

#-------------------------------------------------
# Get CMake on our path
export PATH=$PATH:${SDK_INSTALL}/$CMAKE_FOLDER_NAME/CMake.app/Contents/bin


#-------------------------------------------------
# Create the DREAM3D_SKD.cmake file, but back up any existing one first
if [ -e "$SDK_INSTALL/DREAM3D_SDK.cmake" ]
  then
  mv "$SDK_INSTALL/DREAM3D_SDK.cmake" "$SDK_INSTALL/DREAM3D_SDK.cmake.bak"
fi
echo "# This is the DREAM3D_SDK File. This file contains all the paths to the dependent libraries." > "$SDK_INSTALL/DREAM3D_SDK.cmake"

echo "if(NOT DEFINED DREAM3D_FIRST_CONFIGURE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  message(STATUS \"*******************************************************\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  message(STATUS \"* DREAM.3D First Configuration Run                    *\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  message(STATUS \"* DREAM3D_SDK Loading from \${CMAKE_CURRENT_LIST_DIR}  *\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  message(STATUS \"*******************************************************\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set(CMAKE_CXX_FLAGS \"-stdlib=libc++ -std=c++11 -Wmost -Wno-four-char-constants -Wno-unknown-pragmas -mfpmath=sse\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "endif()" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(CMAKE_CXX_STANDARD 11 CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(CMAKE_OSX_ARCHITECTURES \"x86_64\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Set our Deployment Target to match Qt" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(CMAKE_OSX_DEPLOYMENT_TARGET \"$OSX_DEPLOYMENT_TARGET\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(CMAKE_OSX_SYSROOT \"$OSX_SDK\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# These settings are specific to DREAM3D. DREAM3D needs these variables to" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# configure properly." >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(BUILD_TYPE \${CMAKE_BUILD_TYPE})" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "if(\"\${BUILD_TYPE}\" STREQUAL \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    set(BUILD_TYPE \"Release\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "endif()" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "message(STATUS \"The Current Build type being used is \${BUILD_TYPE}\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(BUILD_SHARED_LIBS ON CACHE BOOL \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(DREAM3D_SDK_ROOT \"$SDK_INSTALL\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(DREAM3D_DATA_DIR \${DREAM3D_SDK_ROOT}/DREAM3D_Data CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"

# Write out the Qt5 directory/installation
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
#echo "# Qt 5.4.x Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
#echo "set(Qt5_DIR \"\${DREAM3D_SDK_ROOT}/Qt5.4.2/5.4/clang_64/lib/cmake/Qt5\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"

echo "# Qt 5.5.x Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(Qt5_DIR \"\${DREAM3D_SDK_ROOT}/Qt-5.5.1/lib/cmake/Qt5\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"


#-------------------------------------------------
# Copy our scripts over to the SDK directory
#cp Build_Boost.sh ${SDK_INSTALL}/.
# cp Build_Eigen.sh ${SDK_INSTALL}/.
# cp Build_HDF5.sh ${SDK_INSTALL}/.
# cp Build_TBB.sh ${SDK_INSTALL}/.
# cp Build_Qwt.sh ${SDK_INSTALL}/.
# cp Build_ITK.sh ${SDK_INSTALL}/.
# cp FixITK.sh ${SDK_INSTALL}/.

# Change Directory back to our original Script Directory
cd $SCRIPT_DIR


#-------------------------------------------------
# Start building all the packages
${SCRIPT_DIR}/Build_Boost.sh 

${SCRIPT_DIR}/Build_HDF5.sh

${SCRIPT_DIR}/Build_Eigen.sh

#-------------------------------------------------
# We are going to build ITK.
# The third argument is the Version of HDF5 that we are using
${SCRIPT_DIR}/Build_ITK.sh



#${SCRIPT_DIR}/Build_TBB.sh "${SDK_INSTALL}" ${PARALLEL_BUILD}


#${SCRIPT_DIR}/Build_Qwt.sh "${SDK_INSTALL}" ${PARALLEL_BUILD}


# Continue writing the DREAM3D_SDK.cmake file after all those libraries were compiled
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Update CMake Module Path with additional paths in order to better find the libraries." >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(CMAKE_MODULE_PATH \${CMAKE_MODULE_PATH} \${ITK_DIR} \${Qt5_DIR} \${HDF5_DIR})" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Only Run this the first time when configuring DREAM.3D. After that the values" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# are cached properly and the user can add additional plugins through the normal" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# CMake GUI or CCMake programs." >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "if(NOT DEFINED DREAM3D_FIRST_CONFIGURE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set (MyPlugins" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    DDDAnalysisToolbox" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    HEDMAnalysis" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    ImageProcessing" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    TransformationPhase" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  )" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  # Loop on each plugin and generate a semi-colon separated list of values" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  foreach(plugin \${MyPlugins})" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    list(APPEND MyPluginList \"\${plugin}\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  endforeach()" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  list(LENGTH MyPluginList LIST_LENGTH)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  if(LIST_LENGTH GREATER 1)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    list(SORT MyPluginList)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  endif()" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set(DREAM3D_EXTRA_PLUGINS \"\${MyPluginList}\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set(DREAM3D_FIRST_CONFIGURE \"ON\" CACHE STRING \"Determines if DREAM3D has already been configured\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "endif()" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"


