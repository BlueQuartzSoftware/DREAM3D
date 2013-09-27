/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _DREAM3D_CONSTANTS_H_
#define _DREAM3D_CONSTANTS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif



#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace DREAM3D
{

  typedef uint32_t Rgb;
  const Rgb  RGB_MASK    = 0x00ffffff;                // masks RGB values

  namespace FilterGroups
  {
    const QString GenericFilters("Generic");
    const QString IOFilters("IO");
    const QString ProcessingFilters("Processing");
    const QString ReconstructionFilters("Reconstruction");
    const QString SamplingFilters("Sampling");
    const QString StatisticsFilters("Statistics");
    const QString CustomFilters("Custom");
    const QString SyntheticBuildingFilters("SyntheticBuilding");
    const QString SurfaceMeshingFilters("SurfaceMeshing");
    const QString SolidMeshingFilters("SolidMeshing");
  }

  namespace FilterSubGroups
  {
    const QString EnsembleStatsFilters("Ensemble");
    const QString MemoryManagementFilters("Memory/Management");
    const QString SpatialFilters("Spatial");
    const QString OutputFilters("Output");
    const QString InputFilters("Input");
    const QString ImageFilters("Image");
    const QString CleanupFilters("Cleanup");
    const QString ThresholdFilters("Threshold");
    const QString RegularizationFilters("Regularization");
    const QString ConversionFilters("Conversion");
    const QString AlignmentFilters("Alignment");
    const QString SegmentationFilters("Segmentation");
    const QString GroupingFilters("Grouping");
    const QString CropCutFilters("Croping/Cutting");
    const QString RotationTransformationFilters("Rotating/Transforming");
    const QString ResolutionFilters("Resolution");
    const QString MorphologicalFilters("Morphological");
    const QString CrystallographicFilters("Crystallographic");
    const QString PackingFilters("Packing");
    const QString CrystallographyFilters("Crystallography");
    const QString GenerationFilters("Generation");
    const QString SmoothingFilters("Smoothing");
    const QString CurvatureFilters("Curvature");
    const QString ConnectivityArrangementFilters("Connectivity/Arrangement");
    const QString MiscFilters("Misc");
  }

  namespace CellData
  {
    const QString SurfaceMeshCells("SurfaceMeshCells");
    const QString GrainIds("GrainIds");
    const QString FarFieldZoneIds("FarFieldZoneIds");
    const QString FarFieldQuats("FarFieldQuats");
    const QString ParentIds("ParentIds");
    const QString Phases("Phases");
    const QString CellPhases("Phases");
    const QString BandContrast("BandContrast");
    const QString EulerAngles("EulerAngles");
 //   const QString CellEulerAngles("CellEulerAngles");
    const QString SurfaceVoxels("SurfaceVoxels");
    const QString Quats("Quats");
    const QString GoodVoxels("GoodVoxels");
    const QString NearestNeighbors("NearestNeighbors");
    const QString GBEuclideanDistances("GBEuclideanDistances");
    const QString TJEuclideanDistances("TJEuclideanDistances");
    const QString QPEuclideanDistances("QPEuclideanDistances");
    const QString GrainReferenceMisorientations("GrainReferenceMisorientations");
    const QString GrainReferenceCAxisMisorientations("GrainReferenceCAxisMisorientations");
    const QString KernelAverageMisorientations("KernelAverageMisorientations");
    const QString ImageQualityNoSpace("ImageQuality");
    const QString ImageQuality("Image Quality");
    const QString ConfidenceIndexNoSpace("ConfidenceIndex");
    const QString ConfidenceIndex("Confidence Index");
    const QString IPFColor("IPFColor");
    const QString MisorientationColor("MisorientationColor");
    const QString RodriguesColor("RodriguesColor");
    const QString EulerColor("EulerColor");
    const QString GlobAlpha("GlobAlpha");
    const QString BC("BandContrasts");
    const QString ImageData("ImageData");
    const QString FlatImageData("FlatImageData");

    const QString SolidMeshNodes("SolidMeshNodes");
    const QString SolidMeshTetrahedrons("SolidMeshTetrahedrons");
  }

  namespace FieldData
  {
    const QString Active("Active");
    const QString GoodFields("GoodFields");
    const QString Phases("Phases");
    const QString FieldPhases("Phases");
    const QString F1("F1");
    const QString F1spt("F1spt");
    const QString F7("F7");
    const QString mPrime("mPrime");
    const QString EulerAngles("EulerAngles");
  //  const QString FieldEulerAngles("FieldEulerAngles");
    const QString SurfaceVoxelFractions("SurfaceVoxelFractions");
    const QString AxisEulerAngles("AxisEulerAngles");
    const QString SurfaceFields("SurfaceFields");
    const QString BiasedFields("BiasedFields");
    const QString NumNeighbors("NumNeighbors");
    const QString RGBs("RGBs");
    const QString ElasticStrains("ElasticStrains");
    const QString FarFieldOrientations("FarFieldOrientations");
    const QString Centroids("Centroids");
    const QString NumCells("NumCells");
    const QString ParentIds("ParentIds");
    const QString Volumes("Volumes");
    const QString EquivalentDiameters("EquivalentDiameters");
    const QString Schmids("Schmids");
    const QString SlipSystems("SlipSystems");
    const QString LargestCrossSections("LargestCrossSections");
    const QString AspectRatios("AspectRatios");
    const QString AxisLengths("AxisLengths");
    const QString Omega3s("Omega3s");
    const QString AvgCAxes("AvgCAxes");
    const QString AvgQuats("AvgQuats");
    const QString Poles("Poles");
    const QString Neighborhoods("Neighborhoods");
    const QString GrainAvgMisorientations("GrainAvgMisorientations");
    const QString GrainAvgCAxisMisorientations("GrainAvgCAxisMisorientations");
    const QString KernelAvgMisorientations("KernelAvgMisorientations");
    const QString NeighborList("NeighborList");
    const QString NeighborhoodList("NeighborhoodList");
    const QString MisorientationList("MisorientationList");
    const QString SharedSurfaceAreaList("SharedSurfaceAreaList");
    const QString LMG("LMG");
    const QString MicroTextureRegionNumCells("MicroTextureRegionNumCells");
    const QString MicroTextureRegionFractionOccupied("MicroTextureRegionFractionOccupied");

  }

  namespace EnsembleData
  {
    const QString NumFields("NumFields");
    const QString VolFractions("VolFractions");
    const QString TotalSurfaceAreas("TotalSurfaceAreas");
    const QString CrystalStructures("CrystalStructures");
    const QString PhaseTypes("PhaseTypes");
    const QString BravaisLattice("BravaisLattice");
    const QString PrecipitateFractions("PrecipitateFractions");
    const QString ShapeTypes("ShapeTypes");
    const QString Statistics("Statistics");
    const QString PhaseName("PhaseName");
    const QString MaterialName("MaterialName");
    const QString LatticeConstants("LatticeConstants");
    const QString GBCD("GBCD");
    const QString GBCDdimensions("GBCDdimensions");
  }


  namespace VertexData
  {
    const QString SurfaceMeshNodes("SurfaceMeshNodes");
    const QString SurfaceMeshNodeType("SurfaceMeshNodeType");
    const QString SurfaceMeshNodeNormals("SurfaceMeshNodeNormals");
    const QString SurfaceMeshNodeFaces("SurfaceMeshNodeFaces");
  }

  namespace FaceData
  {
    const QString SurfaceMeshFaces("SurfaceMeshFaces");
    const QString SurfaceMeshFaceIPFColors("SurfaceMeshFaceIPFColors");
    const QString SurfaceMeshFaceMisorientationColors("SurfaceMeshFaceMisorientationColors");
    const QString SurfaceMeshFaceLabels("SurfaceMeshFaceLabels");
    const QString SurfaceMeshF1s("SurfaceMeshF1s");
    const QString SurfaceMeshF1spts("SurfaceMeshF1spts");
    const QString SurfaceMeshF7s("SurfaceMeshF7s");
    const QString SurfaceMeshmPrimes("SurfaceMeshmPrimes");
    const QString SurfaceMeshVoxels("SurfaceMeshVoxels");
    const QString SurfaceMeshFaceCentroids("SurfaceMeshFaceCentroids");
    const QString SurfaceMeshFaceAreas("SurfaceMeshFaceAreas");
    const QString SurfaceMeshTwinBoundary("SurfaceMeshTwinBoundary");
    const QString SurfaceMeshTwinBoundaryIncoherence("SurfaceMeshTwinBoundaryIncoherence");
    const QString SurfaceMeshTwinBoundarySchmidFactors("SurfaceMeshTwinBoundarySchmidFactors");
    const QString SurfaceMeshFaceDihedralAngles("SurfaceMeshFaceDihedralAngles");
    const QString SurfaceMeshFaceNormals("SurfaceMeshFaceNormals");
    const QString SurfaceMeshGrainFaceId("SurfaceMeshGrainFaceId");
    const QString SurfaceMeshGaussianCurvatures("SurfaceMeshGaussianCurvatures");
    const QString SurfaceMeshMeanCurvatures("SurfaceMeshMeanCurvatures");
    const QString SurfaceMeshPrincipalCurvature1("PrincipalCurvature1");
    const QString SurfaceMeshPrincipalCurvature2("PrincipalCurvature2");
    const QString SurfaceMeshPrincipalDirection1("PrincipalDirection1");
    const QString SurfaceMeshPrincipalDirection2("PrincipalDirection2");
  }

  namespace EdgeData
  {
    const QString SurfaceMeshEdges("SurfaceMeshEdges");
    const QString SurfaceMeshUniqueEdges("SurfaceMeshUniqueEdges");
    const QString SurfaceMeshInternalEdges("SurfaceMeshInternalEdges");
    const QString SurfaceMeshTriangleEdges("SurfaceMeshTriangleEdges");
    const QString SurfaceMeshEdgeFaces("SurfaceMeshEdgeFaces");
  }


  namespace HDF5
  {
    const QString FileVersionName("FileVersion");
    const QString FileVersion("6.0");
    const QString DataContainerName("DataContainers");
    const QString DataContainerType("DataContainerType");
    const QString VolumeDataContainerName("VolumeDataContainer");
    const QString SurfaceDataContainerName("SurfaceDataContainer");
    const QString VertexDataContainerName("VertexDataContainer");
    const QString EdgeDataContainerName("EdgeDataContainer");
    const QString VoxelDataName("VoxelData");
    const QString PipelineGroupName("Pipeline");
    const QString ObjectType("ObjectType");
    const QString NumComponents("NumComponents");

    const QString VerticesName("Vertices");
    const QString FacesName("Faces");
    const QString EdgesName("Edges");
    const QString CellsName("Cells");
    const QString EdgesContainingVert("EdgesContainingVert");
    const QString EdgeNeighbors("EdgeNeighbors");
    const QString FacesContainingVert("FacesContainingVert");
    const QString FaceNeighbors("FaceNeighbors");
    const QString CellsContainingVert("CellsContainingVert");
    const QString CellNeighbors("CellNeighbors");

    //  const QString Grain_ID("Grain_ID");
    // const QString SchmidFactor ("SchmidFactor");
    const QString Neighbor_Grain_ID_List( "Neighbor_Grain_ID_List");
    const QString KernelAvgDisorientation( "KernelAvgDisorientation");
    const QString GrainAvgDisorientation ("GrainAvgDisorientation");
    //  const QString ImageQuality( "ImageQuality");

    // const QString Phase("Phase");
    const QString GBCD("GBCD");
    const QString Statistics("Statistics");
    const QString AxisOrientation("AxisOrientation");
    const QString AxisODFWeights("AxisODF-Weights");
    const QString ODF("ODF");
    const QString ODFWeights("ODF-Weights");
    const QString Euler1("Euler 1");
    const QString Euler2("Euler 2");
    const QString Euler3("Euler 3");
    const QString Weight("Weight");
    const QString Sigma("Sigma");
    const QString MisorientationBins("MisorientationBins");
    const QString MDFWeights("MDF-Weights");
    const QString Angle("Angle");
    const QString Axis("Axis");
    const QString MicroTextureBins("MicroTextureBins");
    const QString Stats("Stats");
    const QString BoundaryArea("BoundaryArea");
    const QString PhaseFraction("PhaseFraction");
    const QString CrystalStructure("CrystalStructure");
    const QString PhaseType("PhaseType");
    const QString PrecipitateBoundaryFraction("Precipitate Boundary Fraction");
    const QString ParentPhase("Parent Phase");
    const QString Grain_Diameter_Info("Grain_Diameter_Info");
    const QString Grain_Size_Distribution("GrainSize Distribution");
    const QString Grain_SizeVBoverA_Distributions("GrainSize Vs B Over A Distributions");
    const QString Grain_SizeVCoverA_Distributions("GrainSize Vs C Over A Distributions");
    const QString Grain_SizeVNeighbors_Distributions("GrainSize Vs Neighbors Distributions");
    const QString Grain_SizeVOmega3_Distributions("GrainSize Vs Omega3 Distributions");
    const QString StatsType("StatsType");

    const QString StatsData("StatsData");
    const QString PrimaryStatsData("PrimaryStatsData");
    const QString PrecipitateStatsData("PrecipitateStatsData");
    const QString BoundaryStatsData("BoundaryStatsData");
    const QString MatrixStatsData("MatrixStatsData");
    const QString TransformationStatsData("TransformationStatsData");


    const QString BinNumber("BinNumber");

    const QString Average("Average");
    const QString StandardDeviation("Standard Deviation");

    const QString Alpha("Alpha");
    const QString Beta("Beta");
    const QString Exp_k("K");
    const QString MinimumValue("Minimum Value");

    const QString UnknownDistribution("Unknown Distribution");
    const QString DistributionType("Distribution Type");
    const QString BetaDistribution("Beta Distribution");
    const QString LogNormalDistribution("Log Normal Distribution");
    const QString PowerLawDistribution("Power Law Distribution");

    enum ColumnCount
    {
      BetaColumnCount = 2,       //!<
      LogNormalColumnCount = 2,  //!<
      PowerLawColumnCount = 2,   //!<
      UnknownColumCount = 0      //!<
    };
  }

  namespace PhaseType
  {
    const unsigned int PrimaryPhase = 0;              //!<
    const unsigned int PrecipitatePhase = 1;          //!<
    const unsigned int TransformationPhase = 2;       //!<
    const unsigned int MatrixPhase = 3;              //!<
    const unsigned int BoundaryPhase = 4;              //!<
    const unsigned int UnknownPhaseType = 999;    //!<

    const QString Primary("Primary");
    const QString Precipitate("Precipitate");
    const QString Transformation("Transformation");
    const QString Matrix("Matrix");
    const QString Boundary("Boundary");
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
    const unsigned int Grain_SizeVBoverA = 0;    //!<
    const unsigned int Grain_SizeVCoverA = 1;        //!<
    const unsigned int Grain_SizeVNeighbors = 2;     //!<
    const unsigned int Grain_SizeVOmega3 = 3;        //!<
    const unsigned int UnknownStatisticsGroup = 999;    //!<
  }

  namespace DistributionType
  {
    const unsigned int Beta = 0;         //!<
    const unsigned int LogNormal = 1;    //!<
    const unsigned int Power = 2;        //!<
    const unsigned int UnknownDistributionType = 3;    //!<
    const unsigned int Count = 3; //!<
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

  namespace ShapeType
  {
    const unsigned int EllipsoidShape = 0; //!<
    const unsigned int SuperEllipsoidShape = 1; //!<
    const unsigned int CubeOctahedronShape = 2; //!<
    const unsigned int CylinderShape = 3; //!<
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
    const QString H5VoxelFile("VoxelData.h5voxel");

    const QString VisualizationVizFile("Visualization.vtk");//11
    const QString DownSampledVizFile("DownSampled_Visualization.vtk");//11
    const QString HDF5GrainFile("Grains.h5grain");
  }

  namespace GrainData
  {
    const QString GrainID("Grain_ID");
    const QString PhaseID("Phase_ID");
    const QString Phi1("Phi1");
    const QString PHI("PHI");
    const QString Phi2("Phi2");
    const QString EquivDiam("Equiv_Diameter");
    const QString B_Over_A("b/a");
    const QString C_Over_A("c/a");
    const QString Omega3("Omega3");
    const QString SurfaceGrain("Surface_Grain");
    const QString OutsideBoundingBox("Outside_Bounding_Box");
    const QString NumNeighbors("No_Neighbors");
    const char Delimiter = ',';
  }

  namespace SyntheticBuilder
  {
    const QString GrainDataFile("GrainData.csv");
    const QString H5VoxelFile("VoxelData.h5voxel");

    const QString VisualizationVizFile("Visualization.vtk");
    const QString HDF5GrainFile("Grains.h5grain");

    const QString ErrorFile("Error.txt");
    const QString VtkFile("Test.vtk");
  }

  namespace MicroStats {
    const QString H5StatisticsFile("Results.h5stats");
    const QString GrainDataFile("GrainData.csv");//14
    const QString DeformationStatsFile("Deformation_Stats.txt");
    const QString IPFDeformVTKFile("IPF_DeformationData.vtk");
    const QString VoxelDataName("VoxelData");
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

  namespace SolidMeshing {
    /* Solid Meshing Related */
    const QString MeshFile("solid_mesh_v5_1.vtk");
    const QString MeshFile2("solid_mesh_v5_2.vtk");
    const QString ElementQualityFile("element_quality_measures_v5.txt");
    const QString VoxelsFile("voxels_v5.txt");
  }

  namespace Comparison
  {
    namespace Strings
    {
      const QString LessThan("<");
      const QString GreaterThan(">");
      const QString Equal("=");
    }
    enum Enumeration
    {
      Operator_LessThan = 0,
      Operator_GreaterThan,
      Operator_Equal,
      Operator_Unknown
    };

  }


}

#endif /* _DREAM3D_CONSTANTS_H_ */
