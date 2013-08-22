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



#include <string>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace DREAM3D
{

  namespace FilterGroups
  {
    const std::string GenericFilters("Generic");
    const std::string IOFilters("IO");
    const std::string ProcessingFilters("Processing");
    const std::string ReconstructionFilters("Reconstruction");
    const std::string SamplingFilters("Sampling");
    const std::string StatisticsFilters("Statistics");
    const std::string CustomFilters("Custom");
    const std::string SyntheticBuildingFilters("SyntheticBuilding");
    const std::string SurfaceMeshingFilters("SurfaceMeshing");
    const std::string SolidMeshingFilters("SolidMeshing");
  }

  namespace FilterSubGroups
  {
    const std::string EnsembleStatsFilters("Ensemble");
    const std::string MemoryManagementFilters("Memory/Management");
    const std::string SpatialFilters("Spatial");
    const std::string OutputFilters("Output");
    const std::string InputFilters("Input");
    const std::string ImageFilters("Image");
    const std::string CleanupFilters("Cleanup");
    const std::string ThresholdFilters("Threshold");
    const std::string RegularizationFilters("Regularization");
    const std::string ConversionFilters("Conversion");
    const std::string AlignmentFilters("Alignment");
    const std::string SegmentationFilters("Segmentation");
    const std::string GroupingFilters("Grouping");
    const std::string CropCutFilters("Croping/Cutting");
    const std::string RotationTransformationFilters("Rotating/Transforming");
    const std::string ResolutionFilters("Resolution");
    const std::string MorphologicalFilters("Morphological");
    const std::string CrystallographicFilters("Crystallographic");
    const std::string PackingFilters("Packing");
    const std::string CrystallographyFilters("Crystallography");
    const std::string GenerationFilters("Generation");
    const std::string SmoothingFilters("Smoothing");
    const std::string CurvatureFilters("Curvature");
    const std::string ConnectivityArrangementFilters("Connectivity/Arrangement");
    const std::string MiscFilters("Misc");
  }

  namespace CellData
  {
    const std::string GrainIds("GrainIds");
    const std::string FarFieldZoneIds("FarFieldZoneIds");
    const std::string FarFieldQuats("FarFieldQuats");
    const std::string ParentIds("ParentIds");
    const std::string Phases("Phases");
    const std::string CellPhases("Phases");
    const std::string BandContrast("BandContrast");
    const std::string EulerAngles("EulerAngles");
 //   const std::string CellEulerAngles("CellEulerAngles");
    const std::string SurfaceVoxels("SurfaceVoxels");
    const std::string Quats("Quats");
    const std::string GoodVoxels("GoodVoxels");
    const std::string NearestNeighbors("NearestNeighbors");
    const std::string GBEuclideanDistances("GBEuclideanDistances");
    const std::string TJEuclideanDistances("TJEuclideanDistances");
    const std::string QPEuclideanDistances("QPEuclideanDistances");
    const std::string GrainReferenceMisorientations("GrainReferenceMisorientations");
    const std::string GrainReferenceCAxisMisorientations("GrainReferenceCAxisMisorientations");
    const std::string KernelAverageMisorientations("KernelAverageMisorientations");
    const std::string ImageQualityNoSpace("ImageQuality");
    const std::string ImageQuality("Image Quality");
    const std::string ConfidenceIndexNoSpace("ConfidenceIndex");
    const std::string ConfidenceIndex("Confidence Index");
    const std::string IPFColor("IPFColor");
    const std::string RodriguesColor("RodriguesColor");
    const std::string EulerColor("EulerColor");
    const std::string GlobAlpha("GlobAlpha");
    const std::string BC("BandContrasts");
    const std::string ImageData("ImageData");
    const std::string FlatImageData("FlatImageData");

    const std::string SolidMeshNodes("SolidMeshNodes");
    const std::string SolidMeshTetrahedrons("SolidMeshTetrahedrons");
  }

  namespace FieldData
  {
    const std::string Active("Active");
    const std::string GoodFields("GoodFields");
    const std::string Phases("Phases");
    const std::string FieldPhases("Phases");
    const std::string F1("F1");
    const std::string F1spt("F1spt");
    const std::string F7("F7");
    const std::string mPrime("mPrime");
    const std::string EulerAngles("EulerAngles");
  //  const std::string FieldEulerAngles("FieldEulerAngles");
    const std::string SurfaceVoxelFractions("SurfaceVoxelFractions");
    const std::string AxisEulerAngles("AxisEulerAngles");
    const std::string SurfaceFields("SurfaceFields");
    const std::string BiasedFields("BiasedFields");
    const std::string NumNeighbors("NumNeighbors");
    const std::string RGBs("RGBs");
    const std::string ElasticStrains("ElasticStrains");
    const std::string FarFieldOrientations("FarFieldOrientations");
    const std::string Centroids("Centroids");
    const std::string NumCells("NumCells");
    const std::string ParentIds("ParentIds");
    const std::string Volumes("Volumes");
    const std::string EquivalentDiameters("EquivalentDiameters");
    const std::string Schmids("Schmids");
    const std::string SlipSystems("SlipSystems");
    const std::string LargestCrossSections("LargestCrossSections");
    const std::string AspectRatios("AspectRatios");
    const std::string AxisLengths("AxisLengths");
    const std::string Omega3s("Omega3s");
    const std::string AvgCAxes("AvgCAxes");
    const std::string AvgQuats("AvgQuats");
    const std::string Poles("Poles");
    const std::string Neighborhoods("Neighborhoods");
    const std::string GrainAvgMisorientations("GrainAvgMisorientations");
    const std::string GrainAvgCAxisMisorientations("GrainAvgCAxisMisorientations");
    const std::string KernelAvgMisorientations("KernelAvgMisorientations");
    const std::string NeighborList("NeighborList");
    const std::string MisorientationList("MisorientationList");
    const std::string SharedSurfaceAreaList("SharedSurfaceAreaList");
    const std::string LMG("LMG");
  }

  namespace EnsembleData
  {
    const std::string NumFields("NumFields");
    const std::string VolFractions("VolFractions");
    const std::string TotalSurfaceAreas("TotalSurfaceAreas");
    const std::string CrystalStructures("CrystalStructures");
    const std::string PhaseTypes("PhaseTypes");
    const std::string BravaisLattice("BravaisLattice");
    const std::string PrecipitateFractions("PrecipitateFractions");
    const std::string ShapeTypes("ShapeTypes");
    const std::string Statistics("Statistics");
    const std::string PhaseName("PhaseName");
    const std::string MaterialName("MaterialName");
    const std::string LatticeConstants("LatticeConstants");
    const std::string GBCD("GBCD");
    const std::string GBCDdimensions("GBCDdimensions");
  }


  namespace VertexData
  {
    const std::string SurfaceMeshNodes("SurfaceMeshNodes");
    const std::string SurfaceMeshNodeType("SurfaceMeshNodeType");
    const std::string SurfaceMeshNodeNormals("SurfaceMeshNodeNormals");
    const std::string SurfaceMeshNodeFaces("SurfaceMeshNodeFaces");
  }

  namespace FaceData
  {
    const std::string SurfaceMeshFaces("SurfaceMeshFaces");
    const std::string SurfaceMeshFaceIPFColors("SurfaceMeshFaceIPFColors");
    const std::string SurfaceMeshFaceMisorientationColors("SurfaceMeshFaceMisorientationColors");
    const std::string SurfaceMeshFaceLabels("SurfaceMeshFaceLabels");
    const std::string SurfaceMeshF1s("SurfaceMeshF1s");
    const std::string SurfaceMeshF1spts("SurfaceMeshF1spts");
    const std::string SurfaceMeshF7s("SurfaceMeshF7s");
    const std::string SurfaceMeshmPrimes("SurfaceMeshmPrimes");
    const std::string SurfaceMeshVoxels("SurfaceMeshVoxels");
    const std::string SurfaceMeshFaceCentroids("SurfaceMeshFaceCentroids");
    const std::string SurfaceMeshFaceAreas("SurfaceMeshFaceAreas");
    const std::string SurfaceMeshTwinBoundary("SurfaceMeshTwinBoundary");
    const std::string SurfaceMeshTwinBoundaryIncoherence("SurfaceMeshTwinBoundaryIncoherence");
    const std::string SurfaceMeshTwinBoundarySchmidFactors("SurfaceMeshTwinBoundarySchmidFactors");
    const std::string SurfaceMeshFaceDihedralAngles("SurfaceMeshFaceDihedralAngles");
    const std::string SurfaceMeshFaceNormals("SurfaceMeshFaceNormals");
    const std::string SurfaceMeshGrainFaceId("SurfaceMeshGrainFaceId");
    const std::string SurfaceMeshGaussianCurvatures("SurfaceMeshGaussianCurvatures");
    const std::string SurfaceMeshMeanCurvatures("SurfaceMeshMeanCurvatures");
    const std::string SurfaceMeshPrincipalCurvature1("PrincipalCurvature1");
    const std::string SurfaceMeshPrincipalCurvature2("PrincipalCurvature2");
    const std::string SurfaceMeshPrincipalDirection1("PrincipalDirection1");
    const std::string SurfaceMeshPrincipalDirection2("PrincipalDirection2");
  }

  namespace EdgeData
  {
    const std::string SurfaceMeshEdges("SurfaceMeshEdges");
    const std::string SurfaceMeshUniqueEdges("SurfaceMeshUniqueEdges");
    const std::string SurfaceMeshInternalEdges("SurfaceMeshInternalEdges");
    const std::string SurfaceMeshTriangleEdges("SurfaceMeshTriangleEdges");
    const std::string SurfaceMeshEdgeFaces("SurfaceMeshEdgeFaces");
  }


  namespace HDF5
  {
    const std::string FileVersionName("FileVersion");
    const std::string FileVersion("4.0");
    const std::string DataContainerName("DataContainer");
    const std::string VoxelDataContainerName("VoxelDataContainer");
    const std::string SurfaceMeshDataContainerName("SurfaceMeshDataContainer");
    const std::string SolidMeshDataContainerName("SolidMeshDataContainer");
    const std::string VoxelDataName("VoxelData");
    const std::string PipelineGroupName("Pipeline");
    const std::string ObjectType("ObjectType");
    const std::string NumComponents("NumComponents");

    const std::string VerticesName("Vertices");
    const std::string FacesName("Faces");
    const std::string EdgesName("Edges");
    const std::string MeshVertLinksName("MeshVertLinks");
    const std::string MeshFaceNeighborLists("MeshFaceNeighborLists");

    //  const std::string Grain_ID("Grain_ID");
    // const std::string SchmidFactor ("SchmidFactor");
    const std::string Neighbor_Grain_ID_List( "Neighbor_Grain_ID_List");
    const std::string KernelAvgDisorientation( "KernelAvgDisorientation");
    const std::string GrainAvgDisorientation ("GrainAvgDisorientation");
    //  const std::string ImageQuality( "ImageQuality");

    // const std::string Phase("Phase");
    const std::string GBCD("GBCD");
    const std::string Statistics("Statistics");
    const std::string AxisOrientation("AxisOrientation");
    const std::string AxisODFWeights("AxisODF-Weights");
    const std::string ODF("ODF");
    const std::string ODFWeights("ODF-Weights");
    const std::string Euler1("Euler 1");
    const std::string Euler2("Euler 2");
    const std::string Euler3("Euler 3");
    const std::string Weight("Weight");
    const std::string Sigma("Sigma");
    const std::string MisorientationBins("MisorientationBins");
    const std::string MDFWeights("MDF-Weights");
    const std::string Angle("Angle");
    const std::string Axis("Axis");
    const std::string MicroTextureBins("MicroTextureBins");
    const std::string Stats("Stats");
    const std::string BoundaryArea("BoundaryArea");
    const std::string PhaseFraction("PhaseFraction");
    const std::string CrystalStructure("CrystalStructure");
    const std::string PhaseType("PhaseType");
    const std::string PrecipitateBoundaryFraction("Precipitate Boundary Fraction");
    const std::string ParentPhase("Parent Phase");
    const std::string Grain_Diameter_Info("Grain_Diameter_Info");
    const std::string Grain_Size_Distribution("GrainSize Distribution");
    const std::string Grain_SizeVBoverA_Distributions("GrainSize Vs B Over A Distributions");
    const std::string Grain_SizeVCoverA_Distributions("GrainSize Vs C Over A Distributions");
    const std::string Grain_SizeVNeighbors_Distributions("GrainSize Vs Neighbors Distributions");
    const std::string Grain_SizeVOmega3_Distributions("GrainSize Vs Omega3 Distributions");
    const std::string StatsType("StatsType");

    const std::string StatsData("StatsData");
    const std::string PrimaryStatsData("PrimaryStatsData");
    const std::string PrecipitateStatsData("PrecipitateStatsData");
    const std::string BoundaryStatsData("BoundaryStatsData");
    const std::string MatrixStatsData("MatrixStatsData");
    const std::string TransformationStatsData("TransformationStatsData");


    const std::string BinNumber("BinNumber");

    const std::string Average("Average");
    const std::string StandardDeviation("Standard Deviation");

    const std::string Alpha("Alpha");
    const std::string Beta("Beta");
    const std::string Exp_k("K");
    const std::string MinimumValue("Minimum Value");

    const std::string UnknownDistribution("Unknown Distribution");
    const std::string DistributionType("Distribution Type");
    const std::string BetaDistribution("Beta Distribution");
    const std::string LogNormalDistribution("Log Normal Distribution");
    const std::string PowerLawDistribution("Power Law Distribution");

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

    const std::string Primary("Primary");
    const std::string Precipitate("Precipitate");
    const std::string Transformation("Transformation");
    const std::string Matrix("Matrix");
    const std::string Boundary("Boundary");
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
    const std::string H5VoxelFile("VoxelData.h5voxel");

    const std::string VisualizationVizFile("Visualization.vtk");//11
    const std::string DownSampledVizFile("DownSampled_Visualization.vtk");//11
    const std::string HDF5GrainFile("Grains.h5grain");
  }

  namespace GrainData
  {
    const std::string GrainID("Grain_ID");
    const std::string PhaseID("Phase_ID");
    const std::string Phi1("Phi1");
    const std::string PHI("PHI");
    const std::string Phi2("Phi2");
    const std::string EquivDiam("Equiv_Diameter");
    const std::string B_Over_A("b/a");
    const std::string C_Over_A("c/a");
    const std::string Omega3("Omega3");
    const std::string SurfaceGrain("Surface_Grain");
    const std::string OutsideBoundingBox("Outside_Bounding_Box");
    const std::string NumNeighbors("No_Neighbors");
    const char Delimiter = ',';
  }

  namespace SyntheticBuilder
  {
    const std::string GrainDataFile("GrainData.csv");
    const std::string H5VoxelFile("VoxelData.h5voxel");

    const std::string VisualizationVizFile("Visualization.vtk");
    const std::string HDF5GrainFile("Grains.h5grain");

    const std::string ErrorFile("Error.txt");
    const std::string VtkFile("Test.vtk");
  }

  namespace MicroStats {
    const std::string H5StatisticsFile("Results.h5stats");
    const std::string GrainDataFile("GrainData.csv");//14
    const std::string DeformationStatsFile("Deformation_Stats.txt");
    const std::string IPFDeformVTKFile("IPF_DeformationData.vtk");
    const std::string VoxelDataName("VoxelData");
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
    const std::string MeshFile("solid_mesh_v5_1.vtk");
    const std::string MeshFile2("solid_mesh_v5_2.vtk");
    const std::string ElementQualityFile("element_quality_measures_v5.txt");
    const std::string VoxelsFile("voxels_v5.txt");
  }

  namespace Comparison
  {
    namespace Strings
    {
      const std::string LessThan("<");
      const std::string GreaterThan(">");
      const std::string Equal("=");
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


#if 0
const double Textures[15][3] =
{
  { 0.610865,0.785398,0.0},
  { 1.029744,0.645772,1.099557},
  { 1.570796,0.610865,0.785398},
  { 1.029744,0.506145,1.099557},
  { 0.820305,0.645772,1.099557},
  { 0.0,0.610865,0.785398},
  { 0.0,0.785398,0.0},
  { 0.0,0.0,0.0},
  { 0.0,0.349066,0.0},
  { 0.0,0.610865,0.0},
  { 0.349066,0.0,0.0},
  { 0.610865,0.0,0.0},
  { 1.22173,0.785398,0.0},
  { 0.959931,0.349066,0.0},
  { 0.959931,1.308997,0.436332}};



// Keep these to remember the names of the texture components in order
//  const int Brassbin = {0.610865,0.785398,0.0};
//  const int Sbin = {0.959931,0.610865,1.134464};
//  const int Copperbin = {1.570796,0.523599,0.785398};
//  const int Shear1bin = {0.0,0.0,0.785398};
//  const int Shear2bin = {0.0,0.959931,0.785398};
//  const int Shear3bin = {0.0,0.610865,0.785398};
//  const int Gossbin = {0.0,0.785398,0.0};
//  const int Cubebin = {0.0,0.0,0.0};
//  const int RC1bin = {0.0,0.349066,0.0};
//  const int RC2bin = {0.0,0.610865,0.0};
//  const int RC3bin = {0.349066,0.0,0.0};
//  const int RC4bin = {0.610865,0.0,0.0};
//  const int Pbin = {1.22173,0.785398,0.0};
//  const int Qbin = {0.959931,0.349066,0.0};
//  const int Rbin = {0.959931,1.308997,0.436332};
#endif


#endif /* _DREAM3D_CONSTANTS_H_ */
