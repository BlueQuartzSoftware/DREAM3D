
if(EXISTS "${ITK_DIR}")
  set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" ${ITK_DIR})
endif()

# --------------------------------------------------------------------
# find ITK libararies
find_package(ITK COMPONENTS
  #Group Core
  ITKCommon

  #Group IO  ---No HDF5 Modules---
  ITKIOImageBase
  ITKIOBMP
  ITKIOJPEG
  ITKIOPNG
  ITKIOTIFF

  #Group Filtering
  ITKImageCompare
  ITKImageIntensity
  #ITKDistanceMap
  #ITKThresholding
  #ITKImageGradient
  #ITKImageFilterBase
  #ITKSmoothing
  #ITKDenoising
  ITKConvolution
  #ITKStatistics
  #ITKImageFeature

  #Group Registration
  ITKRegistrationCommon

  #Group Segmentation
  #ITKVoronoi
  #ITKLevelSets
  ITKWatersheds
  ITKClassifiers
  #ITKConnectedComponents

  #Other
  #ITKReview

  REQUIRED
)

include(${ITK_USE_FILE})

IF(ITK_DIR)
  SET(DREAM3D_ITK_DIR "${ITK_DIR}" CACHE INTERNAL "Path to store itk binaries")
  MESSAGE(STATUS "${PROJECT_NAME}: Using ITK available at: ${ITK_DIR}")

  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    IF("${ITK_LIBRARY_DIRS}" STREQUAL "")
      # ITKv4 does not seem to specify ITK_LIBRARY_DIRS any more, so we generate the binary directory from the main ITK directory
      SET(ITK_LIBRARY_DIRS "${ITK_DIR}/bin")
    ENDIF()

    FILE(COPY
      ${ITK_LIBRARY_DIRS}/Release/
      DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    FILE(COPY
      ${ITK_LIBRARY_DIRS}/Debug/
      DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ELSE()
    IF("${ITK_LIBRARY_DIRS}" STREQUAL "")
      # ITKv4 does not seem to specify ITK_LIBRARY_DIRS any more, so we generate the binary directory from the main ITK directory
      SET(ITK_LIBRARY_DIRS "${ITK_DIR}/lib/")
    ENDIF()
    #message(STATUS "CMAKE_SHARED_LIBRARY_SUFFIX: ${CMAKE_SHARED_LIBRARY_SUFFIX}")
    #message(STATUS "ITK_LIBRARY_DIRS: ${ITK_LIBRARY_DIRS}")
    FILE(GLOB itkLibs
        "${ITK_LIBRARY_DIRS}*${CMAKE_SHARED_LIBRARY_SUFFIX}.*")
    FILE(COPY
        ${ITK_LIBRARY_DIRS}/
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        FILES_MATCHING PATTERN "*${CMAKE_SHARED_LIBRARY_SUFFIX}.*"
      )

    #message(STATUS "itkLibs: ${itkLibs}")
    install( FILES ${itkLibs}
      DESTINATION "lib"
      COMPONENT Applications
      )
  ENDIF()


endif()



