/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _DREAM3DLIB_CONSTANTS_H_
#define _DREAM3DLIB_CONSTANTS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif



#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"

#define DREAM3D_STRING QString

#define DEFAULT_BLOCKSIZE 1048576

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace DREAM3D
{

  typedef uint32_t Rgb;
  const Rgb  RGB_MASK    = 0x00ffffff;                // masks RGB values
  const DREAM3D_STRING PathSep("|");
  static const uint8_t Unchecked = 0;
  static const uint8_t PartiallyChecked = 1;
  static const uint8_t Checked = 2;

  enum InfoStringFormat
  {
    HtmlFormat = 0,
//      JsonFormat,
//      TextFormat,
//      XmlFormat,
    UnknownFormat
  };

  namespace BlueQuartz
  {
    const QString VendorName("BlueQuartz Software, LLC");
    const QString URL("http://www.bluequartz.net");
    const QString Copyright("(C) 2015 BlueQuartz Software, LLC");
  }

  namespace TypeNames
  {
    const QString Bool("bool");
    const QString Float("float");
    const QString Double("double");
    const QString Int8("int8_t");
    const QString UInt8("uint8_t");
    const QString Int16("int16_t");
    const QString UInt16("uint16_t");
    const QString Int32("int32_t");
    const QString UInt32("uint32_t");
    const QString Int64("int64_t");
    const QString UInt64("uint64_t");
    const QString UnknownType("UnknownType");
    const QString Empty("");
    const QString SupportedTypeList(TypeNames::Int8 + ", " + TypeNames::UInt8 + ", " + TypeNames::Int16 + ", " + TypeNames::UInt16 + ", " + TypeNames::Int32 + ", " + TypeNames::UInt32 + ", " + TypeNames::Int64 + ", " + TypeNames::UInt64 + ", " + TypeNames::Float + ", " + TypeNames::Double);
  }

  namespace TypeEnums
  {
    static const int Int8 = 0;
    static const int UInt8 = 1;
    static const int Int16 = 2;
    static const int UInt16 = 3;
    static const int Int32 = 4;
    static const int UInt32 = 5;
    static const int Int64 = 6;
    static const int UInt64 = 7;
    static const int Float = 8;
    static const int Double = 9;
    static const int Bool = 10;

    static const int UnknownType = 11;
    const QString SupportedTypeList(TypeNames::Int8 + ", " + TypeNames::UInt8 + ", " + TypeNames::Int16 + ", " + TypeNames::UInt16 + ", " + TypeNames::Int32 + ", " + TypeNames::UInt32 + ", " + TypeNames::Int64 + ", " + TypeNames::UInt64 + ", " + TypeNames::Float + ", " + TypeNames::Double);
  }

  namespace IO
  {
    const QString DAPSettingsHeader("Path");
  }

  namespace Defaults
  {
    const DREAM3D_STRING AttributeMatrixName("AttributeMatrix");
    const DREAM3D_STRING ElementAttributeMatrixName("ElementAttributeMatrix");
    const DREAM3D_STRING FeatureAttributeMatrixName("FeatureAttributeMatrix");
    const DREAM3D_STRING EnsembleAttributeMatrixName("EnsembleAttributeMatrix");

    const DREAM3D_STRING ImageDataContainerName("ImageDataContainer");
    const DREAM3D_STRING NewImageDataContainerName("NewImageDataContainer");
    const DREAM3D_STRING TriangleDataContainerName("TriangleDataContainer");
    const DREAM3D_STRING QuadDataContainerName("QuadDataContainer");

    const DREAM3D_STRING VertexDataContainerName("VertexDataContainer");
    const DREAM3D_STRING VertexAttributeMatrixName("VertexData");
    const DREAM3D_STRING VertexFeatureAttributeMatrixName("VertexFeatureData");
    const DREAM3D_STRING VertexEnsembleAttributeMatrixName("VertexEnsembleData");

    const DREAM3D_STRING EdgeDataContainerName("EdgeDataContainer");
    const DREAM3D_STRING EdgeAttributeMatrixName("EdgeData");
    const DREAM3D_STRING EdgeFeatureAttributeMatrixName("EdgeFeatureData");
    const DREAM3D_STRING EdgeEnsembleAttributeMatrixName("EdgeEnsembleData");

    const DREAM3D_STRING SurfaceDataContainerName("SurfaceDataContainer");
    const DREAM3D_STRING FaceAttributeMatrixName("FaceData");
    const DREAM3D_STRING FaceFeatureAttributeMatrixName("FaceFeatureData");
    const DREAM3D_STRING FaceEnsembleAttributeMatrixName("FaceEnsembleData");

    const DREAM3D_STRING VolumeDataContainerName("VolumeDataContainer");
    const DREAM3D_STRING NewVolumeDataContainerName("NewVolumeDataContainer");
    const DREAM3D_STRING CellAttributeMatrixName("CellData");
    const DREAM3D_STRING NewCellAttributeMatrixName("NewCellData");
    const DREAM3D_STRING CellFeatureAttributeMatrixName("CellFeatureData");
    const DREAM3D_STRING NewCellFeatureAttributeMatrixName("NewCellFeatureData");
    const DREAM3D_STRING CellEnsembleAttributeMatrixName("CellEnsembleData");

    const DREAM3D_STRING VoxelDataName("VoxelData");

    const DREAM3D_STRING SyntheticVolumeDataContainerName("SyntheticVolumeDataContainer");
    const DREAM3D_STRING StatsGenerator("StatsGeneratorDataContainer");

    const DREAM3D_STRING SomePath("SomeDataContainer|SomeAttributeMatrix|SomeDataArray");

    const DREAM3D_STRING GenericBundleName("GenericBundle");
    const DREAM3D_STRING TimeSeriesBundleName("TimeSeriesBundle");

    const DREAM3D_STRING GenericBundleAttributeMatrixName("GenericBundleAttributeMatrix");
    const DREAM3D_STRING TimeSeriesBundleAttributeMatrixName("TimeSeriesBundleAttributeMatrix");

    const DREAM3D_STRING DataContainerName("DataContainer");
    const DREAM3D_STRING NewDataContainerName("NewDataContainer");
    const DREAM3D_STRING NewAttributeMatrixName("NewAttributeMatrixName");
  }

  namespace FilterGroups
  {
    const DREAM3D_STRING GenericFilters("Generic");
    const DREAM3D_STRING OrientationAnalysisFilters("Orientation Analysis");
    const DREAM3D_STRING IOFilters("IO");
    const DREAM3D_STRING ProcessingFilters("Processing");
    const DREAM3D_STRING ReconstructionFilters("Reconstruction");
    const DREAM3D_STRING SamplingFilters("Sampling");
    const DREAM3D_STRING StatisticsFilters("Statistics");
    const DREAM3D_STRING CustomFilters("Custom");
    const DREAM3D_STRING SyntheticBuildingFilters("Synthetic Building");
    const DREAM3D_STRING SurfaceMeshingFilters("Surface Meshing");
    const DREAM3D_STRING SolidMeshingFilters("Solid Meshing");
    const DREAM3D_STRING TestFilters("Test");
    const DREAM3D_STRING CoreFilters("Core");
    const DREAM3D_STRING Unsupported("Unsupported");
  }

  namespace FilterSubGroups
  {
    const DREAM3D_STRING EnsembleStatsFilters("Ensemble");
    const DREAM3D_STRING MemoryManagementFilters("Memory/Management");
    const DREAM3D_STRING SpatialFilters("Spatial");
    const DREAM3D_STRING StatisticsFilters("Statistics");
    const DREAM3D_STRING FeatureIdentificationFilters("FeatureIdentification");
    const DREAM3D_STRING OutputFilters("Output");
    const DREAM3D_STRING InputFilters("Input");
    const DREAM3D_STRING ImageFilters("Image");
    const DREAM3D_STRING CleanupFilters("Cleanup");
    const DREAM3D_STRING ThresholdFilters("Threshold");
    const DREAM3D_STRING RegularizationFilters("Regularization");
    const DREAM3D_STRING ConversionFilters("Conversion");
    const DREAM3D_STRING FusionFilters("Fusion");
    const DREAM3D_STRING WarpingFilters("Warping");
    const DREAM3D_STRING AlignmentFilters("Alignment");
    const DREAM3D_STRING SegmentationFilters("Segmentation");
    const DREAM3D_STRING GroupingFilters("Grouping");
    const DREAM3D_STRING CropCutFilters("Croping/Cutting");
    const DREAM3D_STRING RotationTransformationFilters("Rotating/Transforming");
    const DREAM3D_STRING ResolutionFilters("Resolution");
    const DREAM3D_STRING MorphologicalFilters("Morphological");
    const DREAM3D_STRING CrystallographicFilters("Crystallographic");
    const DREAM3D_STRING PackingFilters("Packing");
    const DREAM3D_STRING CrystallographyFilters("Crystallography");
    const DREAM3D_STRING GenerationFilters("Generation");
    const DREAM3D_STRING SmoothingFilters("Smoothing");
    const DREAM3D_STRING CurvatureFilters("Curvature");
    const DREAM3D_STRING ConnectivityArrangementFilters("Connectivity/Arrangement");
    const DREAM3D_STRING MappingFilters("Mapping");
    const DREAM3D_STRING MiscFilters("Misc");
  }

  namespace GeneralData
  {
    const DREAM3D_STRING ThresholdArray("ThresholdArray");
    const DREAM3D_STRING Mask("Mask");
  }

  namespace CellData
  {
    const DREAM3D_STRING AxisAngles("AxisAngles");
    const DREAM3D_STRING BC("BandContrasts");
    const DREAM3D_STRING BandContrast("BandContrast");
    const DREAM3D_STRING CellPhases("Phases");
    const DREAM3D_STRING ConfidenceIndex("Confidence Index");
    const DREAM3D_STRING CAxisLocation("CAxisLocation");
    const DREAM3D_STRING ConfidenceIndexNoSpace("ConfidenceIndex");
    const DREAM3D_STRING Current("Current");
    const DREAM3D_STRING DislocationTensors("DislocationTensors");
    const DREAM3D_STRING EulerAngles("EulerAngles");
    const DREAM3D_STRING EulerColor("EulerColor");
    const DREAM3D_STRING FarFeatureQuats("FarFeatureQuats");
    const DREAM3D_STRING FarFeatureZoneIds("FarFeatureZoneIds");
    const DREAM3D_STRING FitQuality("FitQuality");
    const DREAM3D_STRING FlatImageData("FlatImageData");
    const DREAM3D_STRING GBEuclideanDistances("GBEuclideanDistances");
    const DREAM3D_STRING GlobAlpha("GlobAlpha");
    const DREAM3D_STRING GoodVoxels("GoodVoxels");
    const DREAM3D_STRING FeatureIds("FeatureIds");
    const DREAM3D_STRING FeatureReferenceCAxisMisorientations("FeatureReferenceCAxisMisorientations");
    const DREAM3D_STRING FeatureReferenceMisorientations("FeatureReferenceMisorientations");
    const DREAM3D_STRING IPFColor("IPFColor");
    const DREAM3D_STRING ImageData("ImageData");
    const DREAM3D_STRING ImageQuality("Image Quality");
    const DREAM3D_STRING ImageQualityNoSpace("ImageQuality");
    const DREAM3D_STRING KernelAverageMisorientations("KernelAverageMisorientations");
    const DREAM3D_STRING Mask("Mask");
    const DREAM3D_STRING MotionDirection("MotionDirection");
    const DREAM3D_STRING MicroTexVolFrac("MicroTexVolFrac");
    const DREAM3D_STRING MisorientationColor("MisorientationColor");
    const DREAM3D_STRING ParentDensity("ParentDensity");
    const DREAM3D_STRING MTRgKAM("MTRgKAM");
    const DREAM3D_STRING NearestNeighbors("NearestNeighbors");
    const DREAM3D_STRING ParentIds("ParentIds");
    const DREAM3D_STRING Phases("Phases");
    const DREAM3D_STRING ProjectedImageMin("ProjectedImageMin");
    const DREAM3D_STRING ProjectedImageMax("ProjectedImageMax");
    const DREAM3D_STRING ProjectedImageAvg("ProjectedImageAvg");
    const DREAM3D_STRING ProjectedImageStd("ProjectedImageStd");
    const DREAM3D_STRING ProjectedImageVar("ProjectedImageVar");
    const DREAM3D_STRING QPEuclideanDistances("QPEuclideanDistances");
    const DREAM3D_STRING Quats("Quats");
    const DREAM3D_STRING RodriguesColor("RodriguesColor");
    const DREAM3D_STRING RodriguesVectors("RodriguesVectors");
    const DREAM3D_STRING SolidMeshNodes("SolidMeshNodes");
    const DREAM3D_STRING SineParams("SineParams");
    const DREAM3D_STRING SolidMeshTetrahedrons("SolidMeshTetrahedrons");
    const DREAM3D_STRING Speed("Speed");
    const DREAM3D_STRING SurfaceMeshCells("SurfaceMeshCells");
    const DREAM3D_STRING BoundaryCells("BoundaryCells");
    const DREAM3D_STRING TJEuclideanDistances("TJEuclideanDistances");
    const DREAM3D_STRING VectorColor("VectorColor");
    const DREAM3D_STRING VectorData("VectorData");
    const DREAM3D_STRING Histogram("Histogram");
  }

  namespace FeatureData
  {
    const DREAM3D_STRING FeatureID("Feature_ID");
    const DREAM3D_STRING Active("Active");
    const DREAM3D_STRING AspectRatios("AspectRatios");
    const DREAM3D_STRING AvgCAxes("AvgCAxes");
    const DREAM3D_STRING AvgCAxisMisalignments("AvgCAxisMisalignments");
    const DREAM3D_STRING LocalCAxisMisalignments("LocalCAxisMisalignments");
    const DREAM3D_STRING UnbiasedLocalCAxisMisalignments("UnbiasedLocalCAxisMisalignments");
    const DREAM3D_STRING AvgQuats("AvgQuats");
    const DREAM3D_STRING AxisEulerAngles("AxisEulerAngles");
    const DREAM3D_STRING AxisLengths("AxisLengths");
    const DREAM3D_STRING BasalLoadingFactor("BasalLoadingFactor");
    const DREAM3D_STRING BiasedFeatures("BiasedFeatures");
    const DREAM3D_STRING CAxisMisalignmentList("CAxisMisalignmentList");
    const DREAM3D_STRING Centroids("Centroids");
    const DREAM3D_STRING ClusteringList("ClusteringList");
    const DREAM3D_STRING ElasticStrains("ElasticStrains");
    const DREAM3D_STRING EquivalentDiameters("EquivalentDiameters");
    const DREAM3D_STRING SaltykovEquivalentDiameters("SaltykovEquivalentDiameters");
    const DREAM3D_STRING EulerAngles("EulerAngles");
    const DREAM3D_STRING AvgEulerAngles("AvgEulerAngles");
    const DREAM3D_STRING F1List("F1List");
    const DREAM3D_STRING F1sptList("F1sptList");
    const DREAM3D_STRING F7List("F7List");
    const DREAM3D_STRING FarFeatureOrientations("FarFeatureOrientations");
    const DREAM3D_STRING FeaturePhases("Phases");
    const DREAM3D_STRING GoodFeatures("GoodFeatures");
    const DREAM3D_STRING FeatureAvgCAxisMisorientations("FeatureAvgCAxisMisorientations");
    const DREAM3D_STRING FeatureAvgMisorientations("FeatureAvgMisorientations");
    const DREAM3D_STRING FeatureStdevCAxisMisorientations("FeatureStdevCAxisMisorientations");
    const DREAM3D_STRING KernelAvgMisorientations("KernelAvgMisorientations");
    const DREAM3D_STRING LMG("LMG");
    const DREAM3D_STRING LargestCrossSections("LargestCrossSections");
    const DREAM3D_STRING Mask("Mask");
    const DREAM3D_STRING MTRdensity("MTRdensity");
    const DREAM3D_STRING MTRgKAM("MTRgKAM");
    const DREAM3D_STRING MicroTextureRegionFractionOccupied("MicroTextureRegionFractionOccupied");
    const DREAM3D_STRING MicroTextureRegionNumCells("MicroTextureRegionNumCells");
    const DREAM3D_STRING MisorientationList("MisorientationList");
    const DREAM3D_STRING NeighborList("NeighborList");
    const DREAM3D_STRING NeighborhoodList("NeighborhoodList");
    const DREAM3D_STRING Neighborhoods("Neighborhoods");
    const DREAM3D_STRING NumCells("NumCells");
    const DREAM3D_STRING NumFeaturesPerParent("NumFeaturesPerParent");
    const DREAM3D_STRING NumNeighbors("NumNeighbors");
    const DREAM3D_STRING Omega3s("Omega3s");
    const DREAM3D_STRING ParentIds("ParentIds");
    const DREAM3D_STRING Phases("Phases");
    const DREAM3D_STRING Poles("Poles");
    const DREAM3D_STRING RGBs("RGBs");
    const DREAM3D_STRING Schmids("Schmids");
    const DREAM3D_STRING SharedSurfaceAreaList("SharedSurfaceAreaList");
    const DREAM3D_STRING SlipSystems("SlipSystems");
    const DREAM3D_STRING SurfaceAreaVol("SurfaceAreaVolumeRatio");
    const DREAM3D_STRING SurfaceFeatures("SurfaceFeatures");
    const DREAM3D_STRING SurfaceElementFractions("SurfaceElementFractions");
    const DREAM3D_STRING Volumes("Volumes");
    const DREAM3D_STRING AvgMisorientations("AvgMisorientations");
    const DREAM3D_STRING mPrimeList("mPrimeList");
    const DREAM3D_STRING NumBins("NumBins");
    const DREAM3D_STRING ScalarAverages("ScalarAverages");

  }

  namespace EnsembleData
  {
    const DREAM3D_STRING NumFeatures("NumFeatures");
    const DREAM3D_STRING VolFractions("VolFractions");
    const DREAM3D_STRING TotalSurfaceAreas("TotalSurfaceAreas");
    const DREAM3D_STRING CrystalStructures("CrystalStructures");
    const DREAM3D_STRING PhaseTypes("PhaseTypes");
    const DREAM3D_STRING BravaisLattice("BravaisLattice");
    const DREAM3D_STRING PrecipitateFractions("PrecipitateFractions");
    const DREAM3D_STRING ShapeTypes("ShapeTypes");
    const DREAM3D_STRING Statistics("Statistics");
    const DREAM3D_STRING PhaseName("PhaseName");
    const DREAM3D_STRING MaterialName("MaterialName");
    const DREAM3D_STRING LatticeConstants("LatticeConstants");
    const DREAM3D_STRING GBCD("GBCD");
    const DREAM3D_STRING GBCDdimensions("GBCDdimensions");
    const DREAM3D_STRING FitParameters("FitParameters");
  }


  namespace VertexData
  {
    const DREAM3D_STRING AtomVelocities("AtomVelocities");
    const DREAM3D_STRING AtomTypes("AtomTypes");
    const DREAM3D_STRING AtomFeatureLabels("AtomFeatureLabels");
    const DREAM3D_STRING NumberOfArms("NumberOfArms");
    const DREAM3D_STRING NodeConstraints("NodeConstraints");
    const DREAM3D_STRING SurfaceMeshNodes("Nodes");
    const DREAM3D_STRING SurfaceMeshNodeType("NodeType");
    const DREAM3D_STRING SurfaceMeshNodeNormals("NodeNormals");
    const DREAM3D_STRING SurfaceMeshNodeFaces("NodeFaces");
  }

  namespace FaceData
  {
    const DREAM3D_STRING SurfaceMeshFaces("Faces");
    const DREAM3D_STRING SurfaceMeshFaceIPFColors("IPFColors");
    const DREAM3D_STRING SurfaceMeshFaceMisorientationColors("MisorientationColors");
    const DREAM3D_STRING SurfaceMeshFaceSchuhMisorientationColors("SchuhMisorientationColors");
    const DREAM3D_STRING SurfaceMeshFaceLabels("FaceLabels");
    const DREAM3D_STRING SurfaceMeshFacePhases("Phases");
    const DREAM3D_STRING SurfaceMeshF1s("F1s");
    const DREAM3D_STRING SurfaceMeshF1spts("F1spts");
    const DREAM3D_STRING SurfaceMeshF7s("F7s");
    const DREAM3D_STRING SurfaceMeshmPrimes("mPrimes");
    const DREAM3D_STRING SurfaceMeshVoxels("SurfaceMeshVoxels");
    const DREAM3D_STRING SurfaceMeshFaceCentroids("FaceCentroids");
    const DREAM3D_STRING SurfaceMeshFaceAreas("FaceAreas");
    const DREAM3D_STRING SurfaceMeshTwinBoundary("TwinBoundary");
    const DREAM3D_STRING SurfaceMeshTwinBoundaryIncoherence("TwinBoundaryIncoherence");
    const DREAM3D_STRING SurfaceMeshTwinBoundarySchmidFactors("TwinBoundarySchmidFactors");
    const DREAM3D_STRING SurfaceMeshFaceDihedralAngles("FaceDihedralAngles");
    const DREAM3D_STRING SurfaceMeshFaceNormals("FaceNormals");
    const DREAM3D_STRING SurfaceMeshFeatureFaceId("FeatureFaceId");
    const DREAM3D_STRING SurfaceMeshGaussianCurvatures("GaussianCurvatures");
    const DREAM3D_STRING SurfaceMeshMeanCurvatures("MeanCurvatures");
    const DREAM3D_STRING SurfaceMeshPrincipalCurvature1("PrincipalCurvature1");
    const DREAM3D_STRING SurfaceMeshPrincipalCurvature2("PrincipalCurvature2");
    const DREAM3D_STRING SurfaceMeshPrincipalDirection1("PrincipalDirection1");
    const DREAM3D_STRING SurfaceMeshPrincipalDirection2("PrincipalDirection2");
  }

  namespace EdgeData
  {
    const DREAM3D_STRING DislocationIds("DislocationIds");
    const DREAM3D_STRING BurgersVectors("BurgersVectors");
    const DREAM3D_STRING SlipPlaneNormals("SlipPlaneNormals");
    const DREAM3D_STRING SurfaceMeshEdges("SurfaceMeshEdges");
    const DREAM3D_STRING SurfaceMeshUniqueEdges("SurfaceMeshUniqueEdges");
    const DREAM3D_STRING SurfaceMeshInternalEdges("SurfaceMeshInternalEdges");
    const DREAM3D_STRING SurfaceMeshTriangleEdges("SurfaceMeshTriangleEdges");
    const DREAM3D_STRING SurfaceMeshEdgeFaces("SurfaceMeshEdgeFaces");
  }

  namespace Settings
  {
    const DREAM3D_STRING Library("Filter Library");
    const DREAM3D_STRING PrebuiltPipelines("Prebuilt Pipelines");
    const DREAM3D_STRING FavoritePipelines("Favorite Pipelines");
    const DREAM3D_STRING PipelineBuilderGroup("PipelineBuilder");
    const DREAM3D_STRING FavoriteConfig("favorite_config");
    const DREAM3D_STRING NumFilters("Number_Filters");
    const DREAM3D_STRING PipelineName("Name");
    const DREAM3D_STRING FilterName("Filter_Name");
    const DREAM3D_STRING HumanLabel("Filter_Human_Label");
    const DREAM3D_STRING Version("Version");
    const DREAM3D_STRING PipelineBuilderGeomertry("PipelineBuilderGeometry");
  }


  namespace HDF5
  {
    const DREAM3D_STRING FileVersion("7.0");
    const DREAM3D_STRING FileVersionName("FileVersion");
    const DREAM3D_STRING DREAM3DVersion("DREAM3D Version");
    const DREAM3D_STRING ObjectType("ObjectType");
    const DREAM3D_STRING Rank("Rank");
    const DREAM3D_STRING TupleDimensions("TupleDimensions");
    const DREAM3D_STRING ComponentDimensions("ComponentDimensions");
    const DREAM3D_STRING AxisDimensions("Tuple Axis Dimensions");
    const DREAM3D_STRING DataArrayVersion("DataArrayVersion");
  }

  namespace StringConstants
  {
    const DREAM3D_STRING Alpha("Alpha");
    const DREAM3D_STRING Angle("Angle");

    const DREAM3D_STRING Average("Average");
    const DREAM3D_STRING Axis("Axis");
    const DREAM3D_STRING AxisODFWeights("AxisODF-Weights");
    const DREAM3D_STRING AxisOrientation("AxisOrientation");
    const DREAM3D_STRING Beta("Beta");
    const DREAM3D_STRING BetaDistribution("Beta Distribution");
    const DREAM3D_STRING BinNumber("BinNumber");
    const DREAM3D_STRING BoundaryArea("BoundaryArea");
    const DREAM3D_STRING BoundaryStatsData("BoundaryStatsData");
    const DREAM3D_STRING CellNeighbors("CellNeighbors");

    const DREAM3D_STRING CellsContainingVert("CellsContainingVert");
    const DREAM3D_STRING CellsName("Cells");
    const DREAM3D_STRING CrystalStructure("CrystalStructure");
    const DREAM3D_STRING DataContainerGroupName("DataContainers");
    const DREAM3D_STRING DataContainerBundleGroupName("DataContainerBundles");
    const DREAM3D_STRING DataContainerNames("DataContainerNames");
    const DREAM3D_STRING MetaDataArrays("MetaDataArrays");
    const DREAM3D_STRING DataContainerType("DataContainerType");
    const DREAM3D_STRING AttributeMatrixType("AttributeMatrixType");

    const DREAM3D_STRING Dims("Dims");
    const DREAM3D_STRING DistributionType("Distribution Type");

    const DREAM3D_STRING EdgeNeighbors("EdgeNeighbors");
    const DREAM3D_STRING EdgesContainingVert("EdgesContainingVert");
    const DREAM3D_STRING EdgesName("Edges");
    const DREAM3D_STRING EdgeCentroids("EdgeCentroids");
    const DREAM3D_STRING Euler1("Euler 1");
    const DREAM3D_STRING Euler2("Euler 2");
    const DREAM3D_STRING Euler3("Euler 3");
    const DREAM3D_STRING Exp_k("K");

    const DREAM3D_STRING FaceNeighbors("FaceNeighbors");
    const DREAM3D_STRING FacesContainingVert("FacesContainingVert");
    const DREAM3D_STRING FacesName("Faces");
    const DREAM3D_STRING TrianglesName("Triangles");
    const DREAM3D_STRING TriangleNeighbors("TriangleNeighbors");
    const DREAM3D_STRING TrianglesContainingVert("TrianglesContainingVert");
    const DREAM3D_STRING TriangleCentroids("TriangleCentroids");
    const DREAM3D_STRING Frequencies("Frequencies");

    const DREAM3D_STRING QuadsName("Quadrilaterals");
    const DREAM3D_STRING QuadNeighbors("QuadrilateralNeighbors");
    const DREAM3D_STRING QuadsContainingVert("QuadrilateralsContainingVerts");
    const DREAM3D_STRING QuadCentroids("QuadrilateralCentroids");

    const DREAM3D_STRING GBCD("GBCD");
    const DREAM3D_STRING FeatureAvgDisorientation ("FeatureAvgDisorientation");
    const DREAM3D_STRING Feature_Diameter_Info("Feature_Diameter_Info");
    const DREAM3D_STRING Feature_SizeVBoverA_Distributions("FeatureSize Vs B Over A Distributions");
    const DREAM3D_STRING Feature_SizeVCoverA_Distributions("FeatureSize Vs C Over A Distributions");
    const DREAM3D_STRING Feature_SizeVNeighbors_Distributions("FeatureSize Vs Neighbors Distributions");
    const DREAM3D_STRING Feature_SizeVOmega3_Distributions("FeatureSize Vs Omega3 Distributions");
    const DREAM3D_STRING Feature_SizeVClustering_Distributions("FeatureSize Vs Clustering Distributions");
    const DREAM3D_STRING Feature_Size_Distribution("FeatureSize Distribution");
    const DREAM3D_STRING KernelAvgDisorientation( "KernelAvgDisorientation");
    const DREAM3D_STRING LogNormalDistribution("Log Normal Distribution");
    const DREAM3D_STRING MDFWeights("MDF-Weights");
    const DREAM3D_STRING MatrixStatsData("MatrixStatsData");
    const DREAM3D_STRING MeshFaceNeighborLists("MeshFaceNeighborLists");
    const DREAM3D_STRING MicroTextureBins("MicroTextureBins");
    const DREAM3D_STRING MinimumValue("Minimum Value");
    const DREAM3D_STRING MinMaxNoOfBins("MinMaxNoOfBins");
    const DREAM3D_STRING MisorientationBins("MisorientationBins");
    const DREAM3D_STRING Neighbor_Feature_ID_List( "Neighbor_Feature_ID_List");

    const DREAM3D_STRING ODF("ODF");
    const DREAM3D_STRING ODFWeights("ODF-Weights");

    const DREAM3D_STRING ParentPhase("Parent Phase");
    const DREAM3D_STRING PhaseFraction("PhaseFraction");
    const DREAM3D_STRING PhaseType("PhaseType");
    const DREAM3D_STRING PipelineGroupName("Pipeline");
    const DREAM3D_STRING PowerLawDistribution("Power Law Distribution");
    const DREAM3D_STRING PrecipitateBoundaryFraction("Precipitate Boundary Fraction");
    const DREAM3D_STRING PrecipitateStatsData("PrecipitateStatsData");
    const DREAM3D_STRING PrimaryStatsData("PrimaryStatsData");

    const DREAM3D_STRING RadialDistFunc("Radial Distribution Function");
    const DREAM3D_STRING RdfMinDistance("Min");
    const DREAM3D_STRING RdfMaxDistance("Max");

    const DREAM3D_STRING Sigma("Sigma");
    const DREAM3D_STRING StandardDeviation("Standard Deviation");
    const DREAM3D_STRING Statistics("Statistics");
    const DREAM3D_STRING Stats("Stats");
    const DREAM3D_STRING StatsData("StatsData");
    const DREAM3D_STRING StatsType("StatsType");

    const DREAM3D_STRING TransformationStatsData("TransformationStatsData");
    const DREAM3D_STRING UnknownDistribution("Unknown Distribution");

    const DREAM3D_STRING VerticesName("Vertices");
    const DREAM3D_STRING VertsName("Verts");
    const DREAM3D_STRING ImageName("Image");

    const DREAM3D_STRING Weight("Weight");

    const DREAM3D_STRING MetaData("_META_DATA");
  }

  namespace CellType
  {
    const DREAM3D_STRING Quadrilateral("Quadrilateral");
    const DREAM3D_STRING Tetrahedron("Tetrahedron");
    const DREAM3D_STRING Triangle("Triangle");
    const DREAM3D_STRING Face("Face");
    const DREAM3D_STRING Edge("Edge");
    const DREAM3D_STRING Vertex("Vertex");
    const DREAM3D_STRING Node("Node");
  }

  namespace PhaseType
  {
    const unsigned int PrimaryPhase = 0;              //!<
    const unsigned int PrecipitatePhase = 1;          //!<
    const unsigned int TransformationPhase = 2;       //!<
    const unsigned int MatrixPhase = 3;              //!<
    const unsigned int BoundaryPhase = 4;              //!<
    const unsigned int UnknownPhaseType = 999;    //!<

    const DREAM3D_STRING Primary("Primary");
    const DREAM3D_STRING Precipitate("Precipitate");
    const DREAM3D_STRING Transformation("Transformation");
    const DREAM3D_STRING Matrix("Matrix");
    const DREAM3D_STRING Boundary("Boundary");
    const DREAM3D_STRING UnknownPhase("UnknownPhaseType");
  }

  namespace AlignmentMethod
  {
    const unsigned int OuterBoundary = 0;        //!<
    const unsigned int Misorientation = 1;       //!<
    const unsigned int MutualInformation = 2;    //!<
    const unsigned int Count = 3;                //!<
    const unsigned int UnknownAlignmentMethod = 999;    //!<
  }

  namespace StatisticsType
  {
    const unsigned int Feature_SizeVBoverA = 0;    //!<
    const unsigned int Feature_SizeVCoverA = 1;        //!<
    const unsigned int Feature_SizeVNeighbors = 2;     //!<
    const unsigned int Feature_SizeVOmega3 = 3;        //!<
    const unsigned int Feature_SizeVClustering = 4;        //!<
    const unsigned int UnknownStatisticsGroup = 999;    //!<

  }

  namespace DistributionType
  {
    const unsigned int Beta = 0;         //!<
    const unsigned int LogNormal = 1;    //!<
    const unsigned int Power = 2;        //!<
    const unsigned int RDFFrequency = 3; //!<
    const unsigned int RDFMaxMin = 4; //!<
    const unsigned int UnknownDistributionType = 5;    //!<
    const unsigned int Count = 5; //!<

    enum ColumnCount
    {
      BetaColumnCount = 2,       //!<
      LogNormalColumnCount = 2,  //!<
      PowerLawColumnCount = 2,   //!<
      RawDistDataColumnCount = 1,   //!<
      UnknownColumCount = 0      //!<
    };

  }

  namespace EulerAngleConversionType
  {
    const unsigned int DegreesToRadians = 0; //!<
    const unsigned int RadiansToDegrees = 1; //!<
  }

  namespace FlattenImageMethod
  {
    const unsigned int Lightness = 0; //!<
    const unsigned int Average = 1; //!<
    const unsigned int Luminosity = 2; //!<
  }

  namespace EulerFrameRotationAxis
  {
    const unsigned int RD = 0; //!<
    const unsigned int TD = 1; //!<
    const unsigned int ND = 2; //!<
    const unsigned int None = 3; //!<
  }

  namespace SampleFrameRotationAxis
  {
    const unsigned int X = 0; //!<
    const unsigned int Y = 1; //!<
    const unsigned int Z = 2; //!<
    const unsigned int None = 3; //!<
  }

  namespace RefFrameRotationAngle
  {
    const unsigned int Ninety = 0; //!<
    const unsigned int oneEighty = 1; //!<
    const unsigned int twoSeventy = 2; //!<
    const unsigned int Mirror = 3; //!<
    const unsigned int Zero = 4; //!<
  }

  namespace Overlap
  {
    const unsigned int Rigid = 0; //!<
    const unsigned int Progressive = 1; //!<
    const unsigned int UnknownOverlap = 999; //!<
  }

  namespace DataContainerType
  {
    const unsigned int VolumeDataContainer = 0; //!<
    const unsigned int SurfaceDataContainer = 1; //!<
    const unsigned int EdgeDataContainer = 2; //!<
    const unsigned int VertexDataContainer = 3; //!<
    const unsigned int UnknownDataContainer = 999; //!<
  }

  namespace AttributeMatrixType
  {
    const unsigned int Vertex = 0; //!<
    const unsigned int Edge = 1; //!<
    const unsigned int Face = 2; //!<
    const unsigned int Cell = 3; //!<
    const unsigned int VertexFeature = 4; //!<
    const unsigned int EdgeFeature = 5; //!<
    const unsigned int FaceFeature = 6; //!<
    const unsigned int CellFeature = 7; //!<
    const unsigned int VertexEnsemble = 8; //!<
    const unsigned int EdgeEnsemble = 9; //!<
    const unsigned int FaceEnsemble = 10; //!<
    const unsigned int CellEnsemble = 11; //!<
    const unsigned int MetaData = 12; //!<
    const unsigned int Generic = 13; //!<
    const unsigned int Unknown = 999; //!<
  }

  namespace AttributeMatrixObjectType
  {
    const unsigned int Element = 0;
    const unsigned int Feature = 1;
    const unsigned int Ensemble = 2;
    const unsigned int Unknown = 3;
  }

  namespace Geometry
  {
    const DREAM3D_STRING Geometry("_SIMPL_GEOMETRY");
    const DREAM3D_STRING GeometryName("GeometryName");
    const DREAM3D_STRING GeometryType("GeometryType");
    const DREAM3D_STRING GeometryTypeName("GeometryTypeName");
    const DREAM3D_STRING NumberOfTuples("NumberOfTuples");
    const DREAM3D_STRING UnitDimensionality("UnitDimensionality");
    const DREAM3D_STRING SpatialDimensionality("SpatialDimensionality");

    const DREAM3D_STRING UnknownGeometry("UnkownGeometry");
    const DREAM3D_STRING ImageGeometry("ImageGeometry");
    const DREAM3D_STRING RectGridGeometry("RectGridGeometry");
    const DREAM3D_STRING VertexGeometry("VertexGeometry");
    const DREAM3D_STRING EdgeGeometry("EdgeGeometry");
    const DREAM3D_STRING TriangleGeometry("TriangleGeometry");
    const DREAM3D_STRING QuadGeometry("QuadrilateralGeometry");

    const DREAM3D_STRING xBoundsList("xBounds");
    const DREAM3D_STRING yBoundsList("yBounds");
    const DREAM3D_STRING zBoundsList("zBounds");
    const DREAM3D_STRING SharedVertexList("SharedVertexList");
    const DREAM3D_STRING SharedEdgeList("SharedEdgeList");
    const DREAM3D_STRING SharedTriList("SharedTriList");
    const DREAM3D_STRING SharedQuadList("SharedQuadList");
    const DREAM3D_STRING UnsharedEdgeList("UnsharedEdgeList");
  }

  namespace GeometryType
  {
    const unsigned int ImageGeometry = 11;
    const unsigned int RectGridGeometry = 11;
    const unsigned int VertexGeometry = 1;
    const unsigned int EdgeGeometry = 3;
    const unsigned int TriangleGeometry = 5;
    const unsigned int QuadGeometry = 9;
    const unsigned int UnknownGeometry = 999;
  }

  namespace XdmfGridType
  {
    const unsigned int PolyData = 0;
    const unsigned int RectilinearGrid = 1;
  }

  namespace XdmfCenterType
  {
    const DREAM3D_STRING Node("Node");
    const DREAM3D_STRING Edge("Edge");
    const DREAM3D_STRING Face("Face");
    const DREAM3D_STRING Cell("Cell");
    const DREAM3D_STRING Grid("Grid");
  }

  namespace ShapeType
  {
    const unsigned int EllipsoidShape = 0; //!<
    const unsigned int SuperEllipsoidShape = 1; //!<
    const unsigned int CubeOctahedronShape = 2; //!<
    const unsigned int CylinderAShape = 3; //!<
    const unsigned int CylinderBShape = 4; //!<
    const unsigned int CylinderCShape = 5; //!<
    const unsigned int ShapeTypeEnd = 6;
    const unsigned int UnknownShapeType = 999; //!<
  }

  namespace Precipitates
  {
    const unsigned int NoPrecipitates = 0; //!<
    const unsigned int BoundaryPrecipitates = 1; //!<
    const unsigned int BulkPrecipitates = 2; //!<
    const unsigned int UnknownPrecipitates = 999; //!<
  }

  namespace Reconstruction
  {
    /*    Reconstruction related */
    const DREAM3D_STRING H5VoxelFile("VoxelData.h5voxel");

    const DREAM3D_STRING VisualizationVizFile("Visualization.vtk");//11
    const DREAM3D_STRING DownSampledVizFile("DownSampled_Visualization.vtk");//11
    const DREAM3D_STRING HDF5FeatureFile("Features.h5feature");
  }

  namespace SyntheticBuilder
  {
    const DREAM3D_STRING FeatureDataFile("FeatureData.csv");
    const DREAM3D_STRING H5VoxelFile("VoxelData.h5voxel");

    const DREAM3D_STRING VisualizationVizFile("Visualization.vtk");
    const DREAM3D_STRING HDF5FeatureFile("Features.h5feature");

    const DREAM3D_STRING ErrorFile("Error.txt");
    const DREAM3D_STRING VtkFile("Test.vtk");
  }

  /*   Surface Meshing Related   */
  namespace SurfaceMesh
  {
    namespace NodeType
    {
      const signed char Unused = 0;
      const signed char Default = 2;
      const signed char TriplePoint = 3;
      const signed char QuadPoint = 4;
      const signed char SurfaceDefault = 12;
      const signed char SurfaceTriplePoint = 13;
      const signed char SurfaceQuadPoint = 14;
    }
    namespace NodeId
    {
      const signed char Unused = -1;
    }
  }

  namespace SolidMeshing
  {
    /* Solid Meshing Related */
    const DREAM3D_STRING MeshFile("solid_mesh_v5_1.vtk");
    const DREAM3D_STRING MeshFile2("solid_mesh_v5_2.vtk");
    const DREAM3D_STRING ElementQualityFile("element_quality_measures_v5.txt");
    const DREAM3D_STRING VoxelsFile("voxels_v5.txt");
  }

  namespace Comparison
  {
    namespace Strings
    {
      const DREAM3D_STRING LessThan("<");
      const DREAM3D_STRING GreaterThan(">");
      const DREAM3D_STRING Equal("=");
    }
    enum Enumeration
    {
      Operator_LessThan = 0,
      Operator_GreaterThan,
      Operator_Equal,
      Operator_Unknown
    };

  }

  namespace Layout
  {
    const signed int Horizontal = 0;
    const signed int Vertical = 1;
    const signed int Square = 2;
  }

}

namespace Core
{
  const QString CoreBaseName("Core");
}

namespace Test
{
  const QString TestPluginFile("TestPlugin");
  const QString TestPluginDisplayName("TestPlugin");
  const QString TestBaseName("TestPlugin");
}


#if 0
namespace Generic
{
  const QString GenericPluginFile("GenericPlugin");
  const QString GenericPluginDisplayName("GenericPlugin");
  const QString GenericBaseName("GenericPlugin");
}

namespace IO
{
  const QString IOPluginFile("IOPlugin");
  const QString IOPluginDisplayName("IOPlugin");
  const QString IOBaseName("IOPlugin");
}
#endif

#endif /* _DREAM3DLIB_CONSTANTS_H_ */

