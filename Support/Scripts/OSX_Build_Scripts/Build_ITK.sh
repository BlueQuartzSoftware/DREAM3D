#!/bin/bash
# This script requires 2 arguments. The root of the DREAM3D_SDK (/Users/Shared/DREAM3D_SDK
# or /opt/DREAM3D_SDK) and the number of parallel processes to use to compile. This
# is typically 2x the number of physical cores in the machine.

SDK_INSTALL=$1

cd $SDK_INSTALL

PARALLEL_BUILD=$2

HDFVERSION=$3


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



CMAKE=`type -P cmake`
if [[ $CMAKE == "" ]];
  then
  echo "CMake is needed for this script. Please install it on your system and be sure it is on your path."
  exit 1
fi


# Build the ITK libraries we need and set our Environment Variable.
version=4.7.2
archiveName="InsightToolkit-${version}"

if [ ! -e "${SDK_INSTALL}/${archiveName}.tar.gz" ];
then
  echo "-------------------------------------------"
  echo " Downloading ITK Version ${version}"
  echo "-------------------------------------------"
  
  $DOWNLOAD_PROG  "http://superb-dca2.dl.sourceforge.net/project/itk/itk/4.7/${archiveName}.tar.gz" -o ${archiveName}.tar.gz
fi

if [ ! -e "$SDK_INSTALL/${archiveName}" ];
then
  tar -xvzf ${archiveName}.tar.gz
fi



function ConfigureITK()
{
  # $1 is the Build Type
  # $2 is the HDF5 Installation location
  # Write an initial Cache File
  mkdir ITK-${version}-$2
  cd ITK-${version}-$2
  lower=`echo $2 | tr '[:upper:]' '[:lower:]'`
  upper=`echo $2 | tr '[:lower:]' '[:upper:]'`
  cacheFile=CMakeCache.txt
  echo "BUILD_DOCUMENTATION:BOOL=OFF" > $cacheFile
  echo "ITK_USE_SYSTEM_HDF5:BOOL=ON" >> $cacheFile
  echo "BUILD_EXAMPLES:BOOL=OFF" >> $cacheFile
  echo "BUILD_SHARED_LIBS:BOOL=ON" >> $cacheFile
  echo "BUILD_TESTING:BOOL=OFF" >> $cacheFile
  echo "CMAKE_BUILD_TYPE:STRING=$2" >> $cacheFile
  echo "DITK_LEGACY_REMOVE:BOOL=ON" >> $cacheFile
  echo "KWSYS_USE_MD5:BOOL=ON" >> $cacheFile
  echo "Module_ITKReview:BOOL=ON" >> $cacheFile
  echo "ITK_BUILD_DEFAULT_MODULES=OFF" >> $cacheFile
  echo "ITKGroup_Core=ON" >> $cacheFile
  echo "ITKGroup_Filtering=ON" >> $cacheFile
  echo "ITKGroup_Registration=ON" >> $cacheFile
  echo "ITKGroup_Segmentation=ON" >> $cacheFile
  echo "ITK_USE_SYSTEM_HDF5=ON" >> $cacheFile
  echo "CMAKE_SKIP_INSTALL_RPATH=ON" >> $cacheFile
  echo "CMAKE_SKIP_RPATH=ON" >> $cacheFile

  echo "HDF5_DIR:STRING=$SDK_INSTALL/$1-$2/share/cmake" >> $cacheFile
  echo "HDF5_CXX_COMPILER_EXECUTABLE:FILEPATH=HDF5_CXX_COMPILER_EXECUTABLE-NOTFOUND" >> $cacheFile
  echo "HDF5_CXX_INCLUDE_DIR:PATH=/Users/Shared/DREAM3D_SDK/$1-$2/include" >> $cacheFile
  echo "HDF5_C_COMPILER_EXECUTABLE:FILEPATH=HDF5_C_COMPILER_EXECUTABLE-NOTFOUND" >> $cacheFile
  echo "HDF5_C_INCLUDE_DIR:PATH=/Users/Shared/DREAM3D_SDK/$1-$2/include" >> $cacheFile

  echo "HDF5_DIFF_EXECUTABLE:FILEPATH=/Users/Shared/DREAM3D_SDK/$1-$2/bin/h5diff" >> $cacheFile
  echo "HDF5_DIR:PATH=/Users/Shared/DREAM3D_SDK/$1-$2/share/cmake" >> $cacheFile
  echo "HDF5_Fortran_COMPILER_EXECUTABLE:FILEPATH=HDF5_Fortran_COMPILER_EXECUTABLE-NOTFOUND" >> $cacheFile
  echo "HDF5_IS_PARALLEL:BOOL=OFF" >> $cacheFile
  if [ "$2" = "Debug" ]; then
    echo "HDF5_CXX_LIBRARY:PATH=/Users/Shared/DREAM3D_SDK/$1-$2/lib/libhdf5_cpp_${lower}.1.8.15.dylib" >> $cacheFile
    echo "HDF5_C_LIBRARY:PATH=/Users/Shared/DREAM3D_SDK/$1-$2/lib/libhdf5_${lower}.1.8.15.dylib" >> $cacheFile
    echo "HDF5_hdf5_LIBRARY_${upper}:FILEPATH=/Users/Shared/DREAM3D_SDK/$1-$2/lib/libhdf5_${lower}.dylib" >> $cacheFile
    echo "HDF5_hdf5_cpp_LIBRARY_${upper}:FILEPATH=/Users/Shared/DREAM3D_SDK/$1-$2/lib/libhdf5_cpp_${lower}.dylib" >> $cacheFile
  fi

  if [ "$2" = "Release" ]; then
    echo "HDF5_hdf5_LIBRARY_${upper}:FILEPATH=/Users/Shared/DREAM3D_SDK/$1-$2/lib/libhdf5.dylib" >> $cacheFile
    echo "HDF5_hdf5_cpp_LIBRARY_${upper}:FILEPATH=/Users/Shared/DREAM3D_SDK/$1-$2/lib/libhdf5_cpp.dylib" >> $cacheFile
  fi

  cmake -G Ninja ../$archiveName
  ninja
  cd ../
}

ConfigureITK hdf5-${HDFVERSION} Debug
./FixITK.sh $SDK_INSTALL/ITK-${version}-Debug 

ConfigureITK hdf5-${HDFVERSION} Release
./FixITK.sh $SDK_INSTALL/ITK-${version}-Release 



echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# ITK (www.itk.org) For Image Processing Plugin" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(ITK_DIR \"\${DREAM3D_SDK_ROOT}/ITK-${version}-\${BUILD_TYPE}\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
