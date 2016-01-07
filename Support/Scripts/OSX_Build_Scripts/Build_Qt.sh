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


HOST_SYSTEM=`uname`
echo "Host System: $HOST_SYSTEM"


if [ ! -e "$SDK_INSTALL/${QT_ARCHIVE_NAME}" ];
  then
  echo "-------------------------------------------"
  echo " Downloading Qt Version Qt-${QT_VERSION}.tar.gz "
  echo "-------------------------------------------"
  $DOWNLOAD_PROG  "${QT_DOWNLOAD_SITE}/${QT_ARCHIVE_NAME}" -o ${QT_ARCHIVE_NAME}
fi


cd $SDK_INSTALL

# Next decompress the archive
if [ -e "$SDK_INSTALL/${QT_ARCHIVE_NAME}" ];
then
  tar -xvzf ${QT_ARCHIVE_NAME}
fi

OSX_SDKVERSION=`xcrun -sdk macosx --show-sdk-version`
DEVELOPER=`xcode-select -print-path`
XCODE_ROOT=`xcode-select -print-path`

if [ ! -d "$DEVELOPER" ]; then
  echo "xcode path is not set correctly $DEVELOPER does not exist (most likely because of xcode > 4.3)"
  echo "run"
  echo "sudo xcode-select -switch <xcode path>"
  echo "for default installation:"
  echo "sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer"
  exit 1
fi

case $DEVELOPER in  
     *\ * )
           echo "Your Xcode path contains whitespaces, which is not supported."
           exit 1
          ;;
esac

case $CURRENTPATH in  
     *\ * )
           echo "Your path contains whitespaces, which is not supported by 'make install'."
           exit 1
          ;;
esac         


#cd $SDK_INSTALL/${QT_FOLDER_NAME}/qtbase/mkspecs
# Find all the files to update the Deployment version
#updateFiles=`grep -R -l "QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7" *`
# Loop over each file and update it with 10.9 as the deployment target
#for f in $updateFiles; do
 # echo "Updating $f with new QMAKE_MACOSX_DEPLOYMENT_TARGET Target: 10.9"
 # sed -i -e "s@QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7@QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9@g" $f
#done


cd $SDK_INSTALL/${QT_FOLDER_NAME}

QT_PREFIX=$SDK_INSTALL/Qt-${QT_VERSION}
QT_BIN_DIR=$QT_PREFIX/5.5/clang_64/bin
QT_HDR_DIR=$QT_PREFIX/5.5/clang_64/include
QT_LIB_DIR=$QT_PREFIX/5.5/clang_64/lib
QT_ARCH_DIR=$QT_PREFIX/5.5/clang_64
QT_TRANS_DIR=$QT_PREFIX/5.5/clang_64/translations
QT_DOC_DIR=$QT_PREFIX/Docs
QT_SYSCONF_DIR=/Library/Preferences/Qt
QT_EXAMPLES_DIR=$QT_PREFIX/Examples

./configure -opensource -confirm-license -debug-and-release -release -nomake tests -nomake examples -no-compile-examples -qt-zlib -qt-libjpeg -qt-libpng -platform macx-clang -openssl -c++11 -sdk macosx10.9 -no-use-gold-linker -dbus -prefix $QT_PREFIX -skip qtwebkit

make -j$PARALLEL_BUILD
make -j$PARALLEL_BUILD install
make -j$PARALLEL_BUILD docs
make -j$PARALLEL_BUILD install_docs
make -j$PARALLEL_BUILD install

#--
cd qtbase
make install_global_docs


# Write out the Qt5 directory/installation
echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Qt 5.5.x Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(Qt5_DIR \"\${DREAM3D_SDK_ROOT}/Qt-${QT_VERSION}/lib/cmake/Qt5\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"



#-bindir $QT_BIN_DIR -headerdir $QT_HDR_DIR -libdir $QT_LIB_DIR -archdatadir \
#QT_ARCH_DIR -translationdir $QT_TRANS_DIR -docdir $QT_DOC_DIR -sysconfdir $QT_SYSCONF_DIR -examplesdir $QT_EXAMPLES_DIR\

# ./configure -opensource -confirm-license -debug-and-release -release -nomake tests -nomake examples -qt-zlib -qt-libjpeg -qt-libpng  -c++11 -dbus -prefix /Users/Shared/DREAM3D_SDK/Qt-5.4.2-D3D 
