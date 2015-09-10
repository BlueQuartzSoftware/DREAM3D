
#------------------------------------------------------------------------------
# This cmake file should be able to configure an ITK 4.7.x or newer source
# The SDK_INSTALL is the location of your DREAM3D_SDK directory that was
# installed First

include (CMakeParseArguments)

# Get our Variables
set(SDK_INSTALL $ENV{DREAM3D_SDK})

set(PARALLEL_BUILD $ENV{PARALLEL_BUILD})

set(HDFVERSION $ENV{HDF_VERSION})

set(ITK_VERSION $ENV{ITK_VERSION})

set(DEBUG_PREFIX "_D")

set(itkArchiveName "InsightToolkit-${ITK_VERSION}")

#------------------------------------------------------------------------------
# Check first to see if the ITK Source archive is available in the SDK directory
if (NOT EXISTS "${SDK_INSTALL}/${itkArchiveName}.tar.gz")
  message(STATUS  "-------------------------------------------")
  message(STATUS  " Downloading ITK Version ${version}")
  message(STATUS  "-------------------------------------------")
  file(DOWNLOAD "http://superb-dca2.dl.sourceforge.net/project/itk/itk/4.7/${itkArchiveName}.tar.gz" ${SDK_INSTALL}/${itkArchiveName}.tar.gz)
endif()

#------------------------------------------------------------------------------
# Once it is available, make sure it is unpacked from the .tar.gz
if(NOT EXISTS "${SDK_INSTALL}/${itkArchiveName}")
  message(STATUS "Unpacking InsightToolkit-${ITK_VERSION} into ${SDK_INSTALL}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${SDK_INSTALL}/${itkArchiveName}.tar.gz"
                  WORKING_DIRECTORY "${SDK_INSTALL}")
endif()


#------------------------------------------------------------------------------
# This function writes a CMakeCache.txt file to the build directory setting the
# most important variables related to HDF5
function(InitializeCacheFile)
  set(options )
  set(oneValueArgs HDF_INSTALL)
  set(multiValueArgs )
  cmake_parse_arguments(I "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if(EXISTS ${SDK_INSTALL}/ITK-${ITK_VERSION})
    file(REMOVE_RECURSE ${SDK_INSTALL}/ITK-${ITK_VERSION})
  endif()
  FILE(MAKE_DIRECTORY ${SDK_INSTALL}/ITK-${ITK_VERSION})

  set(cacheFile "${SDK_INSTALL}/ITK-${ITK_VERSION}/CMakeCache.txt")
  FILE(WRITE ${cacheFile} "")
  FILE(APPEND ${cacheFile} "CMAKE_CXX_FLAGS:STRING=/DWIN32 /D_WINDOWS /W3 /GR /EHsc /MP\n")
  FILE(APPEND ${cacheFile} "BUILD_DOCUMENTATION:BOOL=OFF\n")
  FILE(APPEND ${cacheFile} "ITK_USE_SYSTEM_HDF5:BOOL=ON\n")
  FILE(APPEND ${cacheFile} "BUILD_EXAMPLES:BOOL=OFF\n")
  FILE(APPEND ${cacheFile} "BUILD_SHARED_LIBS:BOOL=ON\n")
  FILE(APPEND ${cacheFile} "BUILD_TESTING:BOOL=OFF\n")
  if(NOT WIN32)
    FILE(APPEND ${cacheFile} "CMAKE_BUILD_TYPE:STRING=$2\n")
  endif()
  FILE(APPEND ${cacheFile} "DITK_LEGACY_REMOVE:BOOL=ON\n")
  FILE(APPEND ${cacheFile} "KWSYS_USE_MD5:BOOL=ON\n")
  FILE(APPEND ${cacheFile} "Module_ITKReview:BOOL=ON\n")
  FILE(APPEND ${cacheFile} "ITK_BUILD_DEFAULT_MODULES=OFF\n")
  FILE(APPEND ${cacheFile} "ITKGroup_Core=ON\n")
  FILE(APPEND ${cacheFile} "ITKGroup_Filtering=ON\n")
  FILE(APPEND ${cacheFile} "ITKGroup_Registration=ON\n")
  FILE(APPEND ${cacheFile} "ITKGroup_Segmentation=ON\n")
  FILE(APPEND ${cacheFile} "ITK_USE_SYSTEM_HDF5=ON\n")
  if(NOT WIN32)
    FILE(APPEND ${cacheFile} "CMAKE_SKIP_INSTALL_RPATH=ON\n")
    FILE(APPEND ${cacheFile} "CMAKE_SKIP_RPATH=ON\n")
  endif()
  FILE(APPEND ${cacheFile} "HDF5_DIR:STRING=$SDK_INSTALL/${I_HDF_INSTALL}/cmake\n")
  FILE(APPEND ${cacheFile} "HDF5_CXX_COMPILER_EXECUTABLE:FILEPATH=HDF5_CXX_COMPILER_EXECUTABLE-NOTFOUND\n")
  FILE(APPEND ${cacheFile} "HDF5_CXX_INCLUDE_DIR:PATH=${SDK_INSTALL}/${I_HDF_INSTALL}/include\n")
  FILE(APPEND ${cacheFile} "HDF5_C_COMPILER_EXECUTABLE:FILEPATH=HDF5_C_COMPILER_EXECUTABLE-NOTFOUND\n")
  FILE(APPEND ${cacheFile} "HDF5_C_INCLUDE_DIR:PATH=${SDK_INSTALL}/${I_HDF_INSTALL}/include\n")

  FILE(APPEND ${cacheFile} "HDF5_DIFF_EXECUTABLE:FILEPATH=${SDK_INSTALL}/${I_HDF_INSTALL}/bin/h5dlldiff.exe\n")
  FILE(APPEND ${cacheFile} "HDF5_DIR:PATH=${SDK_INSTALL}/${I_HDF_INSTALL}/cmake\n")
  FILE(APPEND ${cacheFile} "HDF5_Fortran_COMPILER_EXECUTABLE:FILEPATH=HDF5_Fortran_COMPILER_EXECUTABLE-NOTFOUND\n")
  FILE(APPEND ${cacheFile} "HDF5_IS_PARALLEL:BOOL=OFF\n")
  
  if("${I_BUILD_TYPE}" STREQUAL "Debug" OR WIN32)
    set(upper "DEBUG")
    FILE(APPEND ${cacheFile} "HDF5_CXX_LIBRARY:PATH=${SDK_INSTALL}/${I_HDF_INSTALL}/lib/hdf5_cpp${DEBUG_PREFIX}.lib\n")
    FILE(APPEND ${cacheFile} "HDF5_C_LIBRARY:PATH=${SDK_INSTALL}/${I_HDF_INSTALL}/lib/hdf5${DEBUG_PREFIX}.lib\n")
    FILE(APPEND ${cacheFile} "HDF5_hdf5_LIBRARY_${upper}:FILEPATH=${SDK_INSTALL}/${I_HDF_INSTALL}/lib/hdf5${DEBUG_PREFIX}.lib\n")
    FILE(APPEND ${cacheFile} "HDF5_hdf5_cpp_LIBRARY_${upper}:FILEPATH=${SDK_INSTALL}/${I_HDF_INSTALL}/lib/hdf5_cpp${DEBUG_PREFIX}.lib\n")
  endif()

  if("${I_BUILD_TYPE}" STREQUAL "Release" OR WIN32)
    set(upper "RELEASE")
    FILE(APPEND ${cacheFile} "HDF5_hdf5_LIBRARY_${upper}:FILEPATH=${SDK_INSTALL}/${I_HDF_INSTALL}/lib/hdf5.lib\n")
    FILE(APPEND ${cacheFile} "HDF5_hdf5_cpp_LIBRARY_${upper}:FILEPATH=${SDK_INSTALL}/${I_HDF_INSTALL}/lib/hdf5_cpp.lib\n")
  endif()

endfunction()

#------------------------------------------------------------------------------
# Init our Cache file
InitializeCacheFile (HDF_INSTALL hdf5-${HDFVERSION})

#------------------------------------------------------------------------------
# Have CMake generate our Visual Studio Project
execute_process(COMMAND ${CMAKE_COMMAND} -G "Visual Studio 12 2013 Win64" "${SDK_INSTALL}/${itkArchiveName}"
                  WORKING_DIRECTORY "${SDK_INSTALL}/ITK-${ITK_VERSION}")

#------------------------------------------------------------------------------
# Build the Debug version
execute_process(COMMAND ${CMAKE_COMMAND} --build "${SDK_INSTALL}/ITK-${ITK_VERSION}" --config Debug
                  WORKING_DIRECTORY "${SDK_INSTALL}/ITK-${ITK_VERSION}")

#------------------------------------------------------------------------------
# Build the Release Version
execute_process(COMMAND ${CMAKE_COMMAND} --build "${SDK_INSTALL}/ITK-${ITK_VERSION}" --config Release
                  WORKING_DIRECTORY "${SDK_INSTALL}/ITK-${ITK_VERSION}")
