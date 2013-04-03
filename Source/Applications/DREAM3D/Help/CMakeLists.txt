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
  ${DOCUMENTATION_SOURCE_DIR}/UserManual/UserManual.dox
#--- Filter Group GenericFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/GenericFilters.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindGrainPhases.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindSurfaceCells.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindCellQuats.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindGrainCentroids.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindSurfaceGrains.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/FindBoundingBoxGrains.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/CopyFieldArrayToCellArray.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/LinkFieldMapToCellArray.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/GenerateIPFColors.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/RenumberGrains.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/RemoveArrays.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/RenameCellArray.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/GenericFilters/RenameFieldArray.dox
#--- Filter Group IOFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/IOFilters.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/AvizoRectilinearCoordinateWriter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/AvizoUniformCoordinateWriter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/DataContainerReader.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/DataContainerWriter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/DxReader.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/DxWriter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/EbsdToH5Ebsd.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/EnsembleInfoReader.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/FieldDataCSVWriter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/FieldInfoReader.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/H5VoxelFileReader.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/INLWriter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/NodesTrianglesToStl.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/NodesTrianglesToVtk.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/PhReader.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/PhWriter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/ReadH5Ebsd.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToNodesTrianglesEdges.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToStl.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToVtk.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToNonconformalVtk.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/VtkRectilinearGridWriter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/YSChoiAbaqusReader.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/RawBinaryReader.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/VtkGrainIdReader.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SolidMeshToVtk.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/IOFilters/SPParksWriter.dox
#--- Filter Group ProcessingFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/ProcessingFilters.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/MinNeighbors.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/FlattenImage.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/MinSize.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/ClearData.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/PerPhaseMinSize.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/FillBadData.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/OpenCloseBadData.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/OpenCloseCoordinationNumber.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/ConvertEulerAngles.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/RotateEulerRefFrame.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/IdentifySample.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/SingleThresholdCells.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/MultiThresholdCells.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/SingleThresholdFields.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ProcessingFilters/MultiThresholdFields.dox
#--- Filter Group ReconstructionFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/ReconstructionFilters.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsMisorientation.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsMutualInformation.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsFeature.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsFeatureCentroid.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsList.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/EBSDSegmentGrains.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/ScalarSegmentGrains.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/SegmentBetaGrains.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/CAxisSegmentGrains.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/MergeTwins.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/MergeColonies.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/ReconstructionFilters/GroupMicroTextureRegions.dox
#--- Filter Group SamplingFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/SamplingFilters.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/ChangeResolution.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/RegularizeZSpacing.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/RotateSampleRefFrame.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/CropVolume.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SamplingFilters/Hex2SqrConverter.dox
#--- Filter Group StatisticsFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/StatisticsFilters.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindAvgOrientations.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindAvgCAxes.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindBoundaryStrengths.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindDeformationStatistics.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindEuclideanDistMap.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindGrainReferenceMisorientations.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindGrainReferenceCAxisMisorientations.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindKernelAvgMisorientations.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/GenerateEnsembleStatistics.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindMisorientations.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindNeighborhoods.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindNumFields.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindSchmids.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindShapes.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindSizes.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindNeighbors.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindSlicetoSliceRotations.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/StatisticsFilters/FindSlipTransmissionMetrics.dox
#--- Filter Group SurfaceMeshingFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/SurfaceMeshingFilters.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/GenerateSurfaceMeshConnectivity.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/GenerateTriangleIPFColoring.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/LaplacianSmoothing.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/M3CSliceBySlice.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/MovingFiniteElementSmoothing.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/QuickSurfaceMesh.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/ReverseTriangleWinding.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/SharedGrainFaceFilter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/TriangleCentroidFilter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/TriangleNormalFilter.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/VerifyTriangleWinding.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SurfaceMeshingFilters/GrainFaceCurvatureFilter.dox
#--- Filter Group SyntheticBuildingFilters
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/SyntheticBuildingFilters.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/PackPrimaryPhases.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/MatchCrystallography.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/JumbleOrientations.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/InsertPrecipitatePhases.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/EstablishMatrixPhase.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/AddBadData.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/AddOrientationNoise.dox
  ${DOCUMENTATION_SOURCE_DIR}/Filters/SyntheticBuildingFilters/InitializeSyntheticVolume.dox
)

set(LATEX_EPS_FILES

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
                     IMAGE_DIR "${DOCUMENTATION_SOURCE_DIR}/Images"
                     OUTPUT_DIR "${DREAM3DProj_BINARY_DIR}/Documentation/${PROJECT_NAME}"
                     OUTPUT_SUB_DIR "latex")
endif()

set(DOCUMENTATION_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/index.dox
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
                     IMAGE_DIR "${DOCUMENTATION_SOURCE_DIR}/UserManual/Images"
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
                     IMAGE_DIR "${PROJECT_SOURCE_DIR}/Images"
                     OUTPUT_DIR "${PROJECT_BINARY_DIR}/DREAM3D_RTF")
endif()

