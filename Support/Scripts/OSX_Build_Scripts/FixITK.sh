#! /bin/bash
# This script requires a single argument which is the current install path


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

DEBUG=0
# -----------------------------------------------------------------------------
#  Conditional Printing
# -----------------------------------------------------------------------------
function printStatus()
{
    if [ "x$DEBUG" = "x1" ]; then
        echo "${1}"
    fi  
}

printStatus "************** Fixing up ITK 4.9.1 **********************"


InstallPrefix="${1}"
printStatus "InstallPrefix = ${InstallPrefix}"

InstallPrefix="${1}"
PackageInstallDest="${1}"
ProjectBinaryDir=@PROJECT_INSTALL_DIR@
ApplicationName="@osx_app_name@"
ApplicationExe="${InstallPrefix}/${ProjectBinaryDir}/${ApplicationName}"

LibDirName="lib"
LibPath="${InstallPrefix}/${LibDirName}"
#InstallNameLib="@executable_path/../${LibDirName}"
InstallNameLib=${LibPath}


TmpDir="/tmp"




# -----------------------------------------------------------------------------
#  $1 is the path to a framework
#  $2 is the target file to update
# -----------------------------------------------------------------------------
function CorrectLinkedFrameworkInstallName()
{
    printStatus "      |-  CorrectLinkedFrameworkInstallName"
    printStatus "          |- 1: ${1}"
    printStatus "          |- 2: ${2}"
    oldPath="${1}"
    libName=`basename ${oldPath}`
    frameworkName="${libName}.framework"
    frameworkRootDir="${1%${frameworkName}*}"
    frameworkBundle="${frameworkRootDir}${frameworkName}"
    newPath="${InstallNameFramework}/${1#${frameworkRootDir}*}" 
    printStatus "          |- oldPath: ${oldPath}"
    printStatus "          |- newPath: ${newPath}"
    install_name_tool -change ${oldPath} ${newPath} ${2}
}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct all the DEPENDENCIES of an input file
#  $1 is an install_name to check. This was generated from a call to otool -L
#  $2 is the Library in its final installed location
# -----------------------------------------------------------------------------
function CorrectLibraryDependency()
{
  pattern=" \(*\)"            # everything between '(' and ')'
  oldPath="${1%$pattern}"
  # Filter out System Libraries or those located in /Libary/Frameworks
  isSystem=`expr  "${oldPath}" : '/System'`
  isUsrLib=`expr "${oldPath}" : '/usr/lib'`
  isToolkitsLib=`expr "${oldPath}" : '/Users/Shared/DREAM3D_SDK/hdf5-1.8.16'`
  #isLibFrwk=`expr "${oldPath}" : '/Libraray/Frameworks'`
  isEmbeddedPathExe=`expr "${oldPath}" : '@executable_path/'`
  isEmbeddedPathLdr=`expr "${oldPath}" : '@loader_path/'`
  if [[ "$isSystem" = "0" \
    && "$isUsrLib" = "0" \
    && "$isToolkitsLib" = "0" \
    && "$isEmbeddedPathExe" = "0" \
    && "$isEmbeddedPathLdr" = "0" ]]; then
      printStatus "    |-  CorrectLibraryDependency"
      printStatus "      |-  ${1}"
      printStatus "      |-  ${2}"
      libName=`basename ${oldPath}`
      frameworkName="${libName}.framework"
      isFramework=` echo ${oldPath} | grep "${frameworkName}"`
      if [[ "${#isFramework}" = "0" ]]; then
        libName=`basename ${oldPath}`
        newPath="${InstallNameLib}/${libName}"
        install_name_tool -change "${oldPath}" "${newPath}" "${2}"
      else
        CorrectLinkedFrameworkInstallName "${oldPath}" "${2}"
      fi
   fi
}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct this library's dependencies
#  $1 is a linked library to update
# -----------------------------------------------------------------------------
function UpdateLibraryDependencies()
{
      printStatus "    |- UpdateLibraryDependencies ${1}"
      local libFile
      local libOutFile
      local i
      
      libFile="${1}"
      libOutFile="${TmpDir}/otool-library.out"
      otool -L "${1}" > "${libOutFile}"
      i=0
      exec 10<"${libOutFile}"
      while read -u 10 line
        do
        if [[ ${i} -gt 1 ]]; then
            CorrectLibraryDependency "${line}" "${1}"
            let i=i+1
        fi
        let i=i+1
      done
      exec 10<&-
}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct this framework
#  $1 is a linked framework
#  $2 is the executable
# -----------------------------------------------------------------------------
function UpdateFrameworkInstallName()
{
    printStatus "  |- UpdateFrameworkInstallName"
    printStatus "     |  ${1}"
    printStatus "     |  ${2}"
    local frameworkBundle
    local frameworkName
    local frameworkRootDir
    local libName
    local frameworkRootPath
    if [[ -e ${1} ]]; then
        #-- Copy the Framework using the current "install_name" as the path
        printStatus "     |  Copying Framework '${1}' into app bundle"
        
        libName=`basename ${1}`
        
        frameworkName="${libName}.framework"
        frameworkRootDir="${1%${frameworkName}*}"
        frameworkBundle="${frameworkRootDir}${frameworkName}"
        frameworkRootPath=`dirname ${1#${frameworkRootDir}*}`
        printStatus "     |  frameworkRootDir: ${frameworkRootDir}"
        printStatus "     |  frameworkBundle: ${frameworkBundle}"
        printStatus "     |  frameworkRootPath: ${frameworkRootPath}"
        printStatus "     |  ${FrameworkPath}/${1#${frameworkRootDir}*}"

        # mkdir -p "${FrameworkPath}/${frameworkRootPath}"
        # cp "${1}" "${FrameworkPath}/${frameworkRootPath}/"
        
        # Update the Executables link path to this library
        oldPath="${1}"
        libName=`basename ${oldPath}`
        newPath="${InstallNameFramework}/${1#${frameworkRootDir}*}" 
        printStatus "     | oldPath: ${oldPath}"
        printStatus "     | newPath: ${newPath}"
        printStatus "     | 2: ${2}"
        install_name_tool -change ${oldPath} ${newPath} ${2}
        
        # Update the install_name of the library itself
        install_name_tool -id ${newPath} "${FrameworkPath}/${1#${frameworkRootDir}*}"
        
        UpdateLibraryDependencies "${FrameworkPath}/${1#${frameworkRootDir}*}"

    fi
}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct this library
#  $1 is a linked library
#  $2 is the executable
# -----------------------------------------------------------------------------
function UpdateDylibInstallName()
{
    printStatus "  |- UpdateDylibInstallName $1"
    if [[ -e ${1} ]]; then
        #-- Copy the library using the current "install_name" as the path
        #printStatus "  |- Copying Library '${1}' into app bundle"
        #cp "${1}" "${LibPath}/."
        
        # Update the Executables link path to this library
        oldPath="${1}"
        libName=`basename ${oldPath}`
        newPath="${InstallNameLib}/${libName}" 
        #echo "   ${oldPath} \n   ${newPath}\n   ${2}"
        install_name_tool -change ${oldPath} ${newPath} ${2}
        
        # Update the install_name of the library itself
        install_name_tool -id ${newPath} "${LibPath}/${libName}"
        
        UpdateLibraryDependencies "${LibPath}/${libName}"
    fi
}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct all the DEPENDENCIES of an input file
#  $1 is a linked library
#  $2 is the executable in its final installed location
# -----------------------------------------------------------------------------
function UpdateExecutableDependencies()
{
  pattern=" \(*\)"            # everything between '(' and ')'
  oldPath="${1%$pattern}"
  # Filter out System Libraries or those located in /Libary/Frameworks
  isSystem=`expr  "${oldPath}" : '/System'`
  isUsrLib=`expr "${oldPath}" : '/usr/lib'`
  isToolkitsLib=`expr "${oldPath}" : '/Users/Shared/DREAM3D_SDK/hdf5-1.8.16'`
  #isLibFrwk=`expr "${oldPath}" : '/Libraray/Frameworks'`
  isEmbeddedPathExe=`expr "${oldPath}" : '@executable_path/'`
  isEmbeddedPathLdr=`expr "${oldPath}" : '@loader_path/'`
  if [[ "$isSystem" = "0" \
    && "$isUsrLib" = "0" \
    && "$isToolkitsLib" = "0" \
    && "$isEmbeddedPathExe" = "0" \
    && "$isEmbeddedPathLdr" = "0" ]]; then
      printStatus "- UpdateExecutableDependencies"
      #printStatus "      ${1}"
      #printStatus "      ${2}"
      libName=`basename ${oldPath}`
      frameworkName="${libName}.framework"
      isFramework=` echo ${oldPath} | grep "${frameworkName}"`
      if [[ "${#isFramework}" = "0" ]]; then
        UpdateDylibInstallName "${oldPath}" "${2}"
      else
        UpdateFrameworkInstallName "${oldPath}" "${2}"
      fi
   fi
}


# -----------------------------------------------------------------------------
#  Script really starts here
# -----------------------------------------------------------------------------
tmpFile="${TmpDir}/${RANDOM}"
mkdir -p ${tmpFile}
tmpFile="${tmpFile}/itk_otool.out"

cd ${LibPath}


libs=`ls *.dylib`
# Get the total number of libs to update
total=`echo $libs | wc -w`
# Remove leading spaces in the 'total' variable
total=`echo ${total} | sed -e 's/^[ \t]*//'`
# Get a counter
let k=0
for l in $libs
do
    #echo "------------------"
    echo "[${k}/${total}] Changing " $l
    let k=k+1
    ApplicationExe=$l

    otool -L "${LibPath}/${ApplicationExe}" > "${tmpFile}"

    i=0
    exec 9<"${tmpFile}"
    while read -u 9 line
    do
        if [[ ${i} -gt 0 ]]; then
            UpdateExecutableDependencies "${line}" "${ApplicationExe}"     
        fi
        let i=i+1
    done
    exec 9<&-
done


