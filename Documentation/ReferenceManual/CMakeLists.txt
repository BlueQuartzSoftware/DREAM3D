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
  ${PROJECT_SOURCE_DIR}/Index.dox
  ${PROJECT_SOURCE_DIR}/UserManual/UserManual.md
  ${PROJECT_SOURCE_DIR}/Filters/FilterReference.dox
#--- Filter Group GenericFilters

  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/CopyFieldArrayToCellArray.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/FindBoundingBoxGrains.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/FindCellQuats.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/FindGrainCentroids.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/FindGrainPhases.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/FindSurfaceCells.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/FindSurfaceGrains.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/GenerateIPFColors.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/RemoveArrays.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/RenumberGrains.md
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters/GenericFilters.dox
  
#--- Filter Group IOFilters
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/AvizoRectilinearCoordinateWriter.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/AvizoUniformCoordinateWriter.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/DataContainerReader.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/DataContainerWriter.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/DxReader.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/DxWriter.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/EbsdToH5Ebsd.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/EnsembleInfoReader.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/FieldDataCSVWriter.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/FieldInfoReader.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/H5VoxelFileReader.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/INLWriter.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/NodesTrianglesToStl.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/NodesTrianglesToVtk.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/PhReader.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/PhWriter.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/ReadH5Ebsd.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToNodesTrianglesEdges.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToStl.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToVtk.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/SurfaceMeshToNonconformalVtk.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/VtkRectilinearGridWriter.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/YSChoiAbaqusReader.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/RawBinaryReader.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/VtkGrainIdReader.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/SolidMeshToVtk.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/SPParksWriter.md
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters/IOFilters.dox
  
#--- Filter Group ProcessingFilters
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/ProcessingFilters.dox
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/MinNeighbors.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/FlattenImage.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/MinSize.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/ClearData.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/PerPhaseMinSize.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/FillBadData.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/OpenCloseBadData.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/OpenCloseCoordinationNumber.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/ConvertEulerAngles.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/RotateEulerRefFrame.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/IdentifySample.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/SingleThresholdCells.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/MultiThresholdCells.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/SingleThresholdFields.md
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters/MultiThresholdFields.md
##--- Filter Group ReconstructionFilters
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/ReconstructionFilters.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsMisorientation.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsMutualInformation.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsFeature.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsFeatureCentroid.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/AlignSectionsList.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/EBSDSegmentGrains.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/ScalarSegmentGrains.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/SegmentBetaGrains.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/CAxisSegmentGrains.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/MergeTwins.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/MergeColonies.md
#  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters/GroupMicroTextureRegions.md
##--- Filter Group SamplingFilters
#  ${PROJECT_SOURCE_DIR}/Filters/SamplingFilters/SamplingFilters.md
#  ${PROJECT_SOURCE_DIR}/Filters/SamplingFilters/ChangeResolution.md
#  ${PROJECT_SOURCE_DIR}/Filters/SamplingFilters/RegularizeZSpacing.md
#  ${PROJECT_SOURCE_DIR}/Filters/SamplingFilters/RotateSampleRefFrame.md
#  ${PROJECT_SOURCE_DIR}/Filters/SamplingFilters/CropVolume.md
#  ${PROJECT_SOURCE_DIR}/Filters/SamplingFilters/Hex2SqrConverter.md
##--- Filter Group StatisticsFilters
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/StatisticsFilters.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindAvgOrientations.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindAvgCAxes.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindBoundaryStrengths.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindDeformationStatistics.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindEuclideanDistMap.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindGrainReferenceMisorientations.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindGrainReferenceCAxisMisorientations.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindKernelAvgMisorientations.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/GenerateEnsembleStatistics.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindMisorientations.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindNeighborhoods.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindNumFields.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindSchmids.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindShapes.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindSizes.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindNeighbors.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindSlicetoSliceRotations.md
#  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters/FindSlipTransmissionMetrics.md
##--- Filter Group SurfaceMeshingFilters
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/SurfaceMeshingFilters.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/GenerateSurfaceMeshConnectivity.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/GenerateTriangleIPFColoring.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/LaplacianSmoothing.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/M3CSliceBySlice.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/MovingFiniteElementSmoothing.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/QuickSurfaceMesh.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/ReverseTriangleWinding.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/SharedGrainFaceFilter.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/TriangleCentroidFilter.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/TriangleNormalFilter.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/VerifyTriangleWinding.md
#  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters/GrainFaceCurvatureFilter.md
##--- Filter Group SyntheticBuildingFilters
#  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters/SyntheticBuildingFilters.md
#  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters/PackPrimaryPhases.md
#  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters/MatchCrystallography.md
#  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters/JumbleOrientations.md
#  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters/InsertPrecipitatePhases.md
#  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters/EstablishMatrixPhase.md
#  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters/AddBadData.md
#  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters/AddOrientationNoise.md
#  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters/InitializeSyntheticVolume.md
)

set(LATEX_EPS_FILES

)

set(DOXYGEN_IMAGE_DIRS
  ${PROJECT_SOURCE_DIR}/UserManual/Images
  ${PROJECT_SOURCE_DIR}/Filters/GenericFilters
  ${PROJECT_SOURCE_DIR}/Filters/IOFilters
  ${PROJECT_SOURCE_DIR}/Filters/ProcessingFilters
  ${PROJECT_SOURCE_DIR}/Filters/ReconstructionFilters
  ${PROJECT_SOURCE_DIR}/Filters/SamplingFilters
  ${PROJECT_SOURCE_DIR}/Filters/StatisticsFilters
  ${PROJECT_SOURCE_DIR}/Filters/SurfaceMeshingFilters
  ${PROJECT_SOURCE_DIR}/Filters/SyntheticBuildingFilters
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


#-------------------------------------------------------------------------------
# Make Sure this target is built so that the DREAM3D Application has access to
# the HTML help files
add_dependencies(DREAM3D DREAM3DReferenceManual_HTML)
  
