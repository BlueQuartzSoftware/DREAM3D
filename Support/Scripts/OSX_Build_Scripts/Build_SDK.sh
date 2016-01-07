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

HOST_SYSTEM=`uname`
echo "Host System: $HOST_SYSTEM"


SCRIPT_DIR=`pwd`

if [ ! -e "$SDK_PARENT/$SDK_ARCHIVE_FILENAME" ];
  then
  echo "-------------------------------------------"
  echo " Downloading DREAM.3D SDK Source Archive "
  echo "-------------------------------------------"
  $DOWNLOAD_PROG  "$SDK_DOWNLOAD_SITE/$SDK_ARCHIVE_FILENAME" -o $SDK_PARENT/$SDK_ARCHIVE_FILENAME
fi



#-------------------------------------------------
# Move one Directory Above the SDK Folder and untar the
if [ -e "$SDK_PARENT/$SDK_ARCHIVE_FILENAME" ];
  then
  echo "Decompressing Archive $SDK_PARENT/$SDK_ARCHIVE_FILENAME"
  sudo mkdir -p ${SDK_INSTALL}
  sudo chmod ugo+rwx ${SDK_INSTALL}
  cd "$SDK_INSTALL/../"
  sudo tar -xvzf $SDK_ARCHIVE_FILENAME
  sudo chmod ugo+rwx $SDK_INSTALL
  USER=`whoami`
  sudo chown -R ${USER} "$SDK_INSTALL"
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
echo "# This was generated for Version 6.3 Development of DREAM.3D. This SDK has C++11 Support ENABLED" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "if(NOT DEFINED DREAM3D_FIRST_CONFIGURE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  message(STATUS \"*******************************************************\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  message(STATUS \"* DREAM.3D First Configuration Run                    *\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  message(STATUS \"* DREAM3D_SDK Loading from \${CMAKE_CURRENT_LIST_DIR}  *\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  message(STATUS \"*******************************************************\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set(CMAKE_CXX_FLAGS \"-stdlib=libc++ -std=c++11 -Wmost -Wno-four-char-constants -Wno-unknown-pragmas -mfpmath=sse\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set(CMAKE_CXX_STANDARD 11 CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set(CMAKE_OSX_ARCHITECTURES \"x86_64\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  # Set our Deployment Target to match Qt" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set(CMAKE_OSX_DEPLOYMENT_TARGET \"$OSX_DEPLOYMENT_TARGET\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set(CMAKE_OSX_SYSROOT \"$OSX_SDK\" CACHE STRING \"\" FORCE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "endif()" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
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




#------------------------------------------------------------------------------
# Ensure the user has a valid Qt 5.5.1 installation
if [ ! -e "$SDK_PARENT/$SDK_FOLDER_NAME/$QT_INSTALL_DIR/$QT_BIN_DIR/qmake" ];
  then
  echo "qmake was NOT found in $SDK_PARENT/$QT_INSTALL_DIR/$QT_BIN_DIR/ directory."
  echo "This indicates a potentially broken DREAM3D_SDK installation. Have you installed"
  echo "Qt 5.5.1 from the following location:"
  echo "$QT_PREBUILT_BINARY_DOWNLOAD"
  echo "During installation please set the installation directory to:"
  echo "/Users/Shared/DREAM3D_SDK/Qt5.5.1"
  exit 1
fi

# Write out the Qt5 directory/installation
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Qt $QT_VERSION Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(Qt5_DIR \"\${DREAM3D_SDK_ROOT}/$QT_INSTALL_DIR/lib/cmake/Qt5\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"



# Change Directory back to our original Script Directory
cd $SCRIPT_DIR

#-------------------------------------------------
# Start building all the packages
# Build Protocol Buffers
${SCRIPT_DIR}/Build_ProtoBuf.sh 

# Build Boost
${SCRIPT_DIR}/Build_Boost.sh 

# Build HDF5
${SCRIPT_DIR}/Build_HDF5.sh

# Builds Eigen
${SCRIPT_DIR}/Build_Eigen.sh

# Build TBB
${SCRIPT_DIR}/Build_TBB.sh

# Build ITK.
${SCRIPT_DIR}/Build_ITK.sh
# Update the "install_name" on all the ITK libraries.
$SCRIPT_DIR/FixITK.sh $SDK_INSTALL/${ITK_INSTALL}-Debug 
$SCRIPT_DIR/FixITK.sh $SDK_INSTALL/${ITK_INSTALL}-Release 



# Build Qwt
${SCRIPT_DIR}/Build_Qwt.sh


# Continue writing the DREAM3D_SDK.cmake file after all those libraries were compiled
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Update CMake Module Path with additional paths in order to better find the libraries." >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(CMAKE_MODULE_PATH \${CMAKE_MODULE_PATH} \${ITK_DIR} \${Qt5_DIR} \${HDF5_DIR} \${Protobuf_DIR})" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Only Run this the first time when configuring DREAM.3D. After that the values" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# are cached properly and the user can add additional plugins through the normal" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# CMake GUI or CCMake programs." >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "if(NOT DEFINED DREAM3D_FIRST_CONFIGURE)" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "  set (MyPlugins" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    # DDDAnalysisToolbox" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    # HEDMAnalysis" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    # ImageProcessing" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "    # TransformationPhase" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
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
