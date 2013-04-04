#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
PROJECT(DREAM3DReferenceManual)
cmake_minimum_required(VERSION 2.8.9)

if(NOT DEFINED PROJECT_RESOURCES_DIR)
set(PROJECT_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/Resources)
endif()

set(DOCUMENTATION_RESOURCES_DIR ${PROJECT_RESOURCES_DIR}/DocConfigFiles)
set(DOCUMENTATION_SOURCE_DIR ${DREAM3DProj_SOURCE_DIR}/Documentation)

FIND_PACKAGE(Doxygen)
if(NOT DOXYGEN_FOUND)
  message(FATAL_ERROR "Doxygen is needed to build the documentation but was not found on your system. Please install it correctly and rerun cmake.")
endif()

set(DOX_GENERATE_RTF "NO")
option(DREAM3D_GENERATE_RTF "Generate RTF Documentation" OFF)
if(DREAM3D_GENERATE_RTF STREQUAL "ON")
    set(DOX_GENERATE_RTF "YES")
endif()


set(DOX_GENERATE_HTML "YES")
option(DREAM3D_GENERATE_HTML "Generate HTML Documentation" ON)
if(DREAM3D_GENERATE_HTML STREQUAL "ON")
    set(DOX_GENERATE_HTML "YES")
endif()


set(DOX_GENERATE_LATEX "NO")
option(DREAM3D_GENERATE_LATEX "Generate HTML Documentation" OFF)
if(DREAM3D_GENERATE_LATEX STREQUAL "ON")
    set(DOX_GENERATE_LATEX "YES")
    # Check for existence of LaTex executable files
    INCLUDE( ${DOCUMENTATION_RESOURCES_DIR}/CMake/UseLATEX.cmake)
    if(APPLE)
        option(AUTO_OPEN_PDF "Automatically open generated PDF files with default PDF reading application" ON)
    endif()
endif()


include(${DOCUMENTATION_RESOURCES_DIR}/CMake/Macros.cmake)

#---- Set some LATEX Varaibles
set(LATEX_TITLE "DREAM.3D\\\\[0.45cm]User Manual and Filter Reference")
set(LATEX_AUTHOR "DREAM3D Development Team")

# The order of the files as they appear here is the order they will appear in the
# Latex/PDF output
set(DOCUMENTATION_SOURCE_FILES
  ${DOCUMENTATION_SOURCE_DIR}/Index.dox
  ${DOCUMENTATION_SOURCE_DIR}/UserManual/UserManual.md
#--- Filter Group GenericFilters

  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/CopyFieldArrayToCellArray.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindBoundingBoxGrains.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindCellQuats.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindGrainCentroids.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindGrainPhases.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindSurfaceCells.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindSurfaceGrains.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/GenerateIPFColors.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/RemoveArrays.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/RenumberGrains.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/GenericFilters.dox
#--- Filter Group IOFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/AvizoRectilinearCoordinateWriter.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/AvizoUniformCoordinateWriter.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/DataContainerReader.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/DataContainerWriter.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/DxReader.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/DxWriter.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/EbsdToH5Ebsd.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/EnsembleInfoReader.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/FieldDataCSVWriter.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/FieldInfoReader.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/H5VoxelFileReader.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/INLWriter.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/NodesTrianglesToStl.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/NodesTrianglesToVtk.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/PhReader.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/PhWriter.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/ReadH5Ebsd.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToNodesTrianglesEdges.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToStl.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToVtk.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToNonconformalVtk.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/VtkRectilinearGridWriter.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/YSChoiAbaqusReader.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/RawBinaryReader.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/VtkGrainIdReader.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SolidMeshToVtk.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SPParksWriter.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/IOFilters.dox
#--- Filter Group ProcessingFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/ProcessingFilters.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/MinNeighbors.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/FlattenImage.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/MinSize.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/ClearData.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/PerPhaseMinSize.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/FillBadData.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/OpenCloseBadData.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/OpenCloseCoordinationNumber.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/ConvertEulerAngles.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/RotateEulerRefFrame.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/IdentifySample.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/SingleThresholdCells.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/MultiThresholdCells.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/SingleThresholdFields.md
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/MultiThresholdFields.md
##--- Filter Group ReconstructionFilters
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/ReconstructionFilters.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsMisorientation.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsMutualInformation.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsFeature.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsFeatureCentroid.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsList.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/EBSDSegmentGrains.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/ScalarSegmentGrains.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/SegmentBetaGrains.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/CAxisSegmentGrains.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/MergeTwins.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/MergeColonies.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/GroupMicroTextureRegions.md
##--- Filter Group SamplingFilters
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/SamplingFilters.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/ChangeResolution.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/RegularizeZSpacing.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/RotateSampleRefFrame.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/CropVolume.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/Hex2SqrConverter.md
##--- Filter Group StatisticsFilters
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/StatisticsFilters.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindAvgOrientations.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindAvgCAxes.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindBoundaryStrengths.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindDeformationStatistics.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindEuclideanDistMap.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindGrainReferenceMisorientations.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindGrainReferenceCAxisMisorientations.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindKernelAvgMisorientations.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/GenerateEnsembleStatistics.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindMisorientations.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindNeighborhoods.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindNumFields.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindSchmids.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindShapes.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindSizes.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindNeighbors.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindSlicetoSliceRotations.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindSlipTransmissionMetrics.md
##--- Filter Group SurfaceMeshingFilters
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/SurfaceMeshingFilters.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/GenerateSurfaceMeshConnectivity.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/GenerateTriangleIPFColoring.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/LaplacianSmoothing.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/M3CSliceBySlice.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/MovingFiniteElementSmoothing.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/QuickSurfaceMesh.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/ReverseTriangleWinding.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/SharedGrainFaceFilter.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/TriangleCentroidFilter.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/TriangleNormalFilter.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/VerifyTriangleWinding.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/GrainFaceCurvatureFilter.md
##--- Filter Group SyntheticBuildingFilters
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/SyntheticBuildingFilters.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/PackPrimaryPhases.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/MatchCrystallography.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/JumbleOrientations.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/InsertPrecipitatePhases.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/EstablishMatrixPhase.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/AddBadData.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/AddOrientationNoise.md
#  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/InitializeSyntheticVolume.md
)

set(LATEX_EPS_FILES

)

set(DOXYGEN_IMAGE_DIRS
  ${DOCUMENTATION_SOURCE_DIR}/UserManual/Images
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters
)

if(DOX_GENERATE_LATEX)
  ADD_DOXYGEN_TARGETS(PDF_FILENAME "DREAM3D_Reference_Manual"
                     PROJECT_NAME  "${PROJECT_NAME}_LATEX"
                     PROJECT_BINARY_DIR "${DREAM3DProj_BINARY_DIR}/Documentation/${PROJECT_NAME}"
                     SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}"
                     EPS_FILES "${LATEX_EPS_FILES}"
                     TITLE "${LATEX_TITLE}"
                     AUTHOR "${LATEX_AUTHOR}"
                     GENERATE_HTML "NO"
                     GENERATE_LATEX "YES"
                     GENERATE_RTF "NO"
                     TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                     IMAGE_DIR "${DOXYGEN_IMAGE_DIRS}"
                     OUTPUT_DIR "${DREAM3DProj_BINARY_DIR}/Documentation/${PROJECT_NAME}"
                     OUTPUT_SUB_DIR "latex")
endif()

set(DOCUMENTATION_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/index.md
        ${DOCUMENTATION_SOURCE_FILES})


if(DOX_GENERATE_HTML)
  ADD_DOXYGEN_TARGETS(PDF_FILENAME "DREAM3D Reference Manual"
                     PROJECT_NAME  "${PROJECT_NAME}_HTML"
                     PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}"
                     SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}"
                     EPS_FILES "${LATEX_EPS_FILES}"
                     TITLE "${LATEX_TITLE}"
                     AUTHOR "${LATEX_AUTHOR}"
                     GENERATE_HTML "YES"
                     GENERATE_LATEX "NO"
                     GENERATE_RTF "NO"
                     TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                     IMAGE_DIR "${DOXYGEN_IMAGE_DIRS}"
                     OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Help"
                     OUTPUT_SUB_DIR "DREAM3D")
endif()

if(DOX_GENERATE_RTF)
  ADD_DOXYGEN_TARGETS(PDF_FILENAME ${PROJECT_NAME}
                     PROJECT_NAME  "${PROJECT_NAME}_RTF"
                     PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}"
                     SOURCE_FILES "${DOCUMENTATION_SOURCE_FILES}"
                     EPS_FILES "${LATEX_EPS_FILES}"
                     TITLE "${LATEX_TITLE}"
                     AUTHOR "${LATEX_AUTHOR}"
                     GENERATE_HTML "NO"
                     GENERATE_LATEX "NO"
                     GENERATE_RTF "YES"
                     TEMPLATE_DIR "${DOCUMENTATION_RESOURCES_DIR}"
                     IMAGE_DIR "${DOXYGEN_IMAGE_DIRS}"
                     OUTPUT_DIR "${PROJECT_BINARY_DIR}/DREAM3D_RTF")
endif()

