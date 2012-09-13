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
#ifndef AIM_CONSTANTS_H_
#define AIM_CONSTANTS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif



#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "DREAM3DLib/DREAM3DLib.h"

//typedef boost::shared_ptr<std::vector<int> >    IntVectorType;
//typedef boost::shared_ptr<std::vector<float> > FloatVectorType;
//typedef boost::shared_ptr<std::vector<double> > DoubleVectorType;
//typedef boost::shared_ptr<std::vector<size_t> > SizeTVectorType;

#define COPY_ARRAY_3(var, obj)\
  var[0] = obj->var[0];var[1] = obj->var[1];var[2] = obj->var[2];

#define COPY_ARRAY_4(var, obj)\
  var[0] = obj->var[0];var[1] = obj->var[1];\
  var[2] = obj->var[2]; var[3] = obj->var[3];

#define COPY_ARRAY_5(var, obj)\
  var[0] = obj->var[0];var[1] = obj->var[1];\
  var[2] = obj->var[2]; var[3] = obj->var[3];\
  var[4] = obj->var[4];


/**
 * This will perform a deep copy of the content of the shared vector from
 * target to destination. This is NOT just a simple pointer copy.
 */
#define DEEP_COPY_SHARED_VECTOR(sharedPtr, obj, VType, m_msgType)\
if (NULL != sharedPtr.get())\
{\
  sharedPtr = VType(static_cast<std::vector<m_msgType>*>(NULL));\
}\
if (NULL != obj->sharedPtr.get())\
{\
  sharedPtr = VType(new std::vector<m_msgType>(*(obj->sharedPtr.get())));\
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace DREAM3D
{

  namespace FilterGroups
  {
    const std::string GenericFilters("Generic");
    const std::string IOFilters("IO");
    const std::string ReconstructionFilters("Reconstruction");
    const std::string SamplingFilters("Sampling");
    const std::string ProcessingFilters("Processing");
    const std::string StatisticsFilters("Statistics");
    const std::string SyntheticBuilderFilters("Synthetic_Builder");
    const std::string SurfaceMeshingFilters("Surface_Meshing");
  }

  namespace CellData
  {
    const std::string GrainIds("GrainIds");
    const std::string ParentIds("ParentIds");
    const std::string Phases("Phases");
    const std::string BandContrast("BandContrast");
    const std::string EulerAngles("EulerAngles");
    const std::string SurfaceVoxels("SurfaceVoxels");
    const std::string Quats("Quats");
    const std::string GoodVoxels("GoodVoxels");
    const std::string NearestNeighbors("NearestNeighbors");
    const std::string NearestNeighborDistances("NearestNeighborDistances");
    const std::string GrainReferenceMisorientations("GrainReferenceMisorientations");
    const std::string KernelAverageMisorientations("KernelAverageMisorientations");
    const std::string ImageQuality("ImageQuality");
    const std::string IPFColor("IPFColor");
  }

  namespace FieldData
  {
    const std::string Active("Active");
    const std::string Phases("Phases");
    const std::string EulerAngles("EulerAngles");
    const std::string AxisEulerAngles("AxisEulerAngles");
    const std::string SurfaceFields("SurfaceFields");
    const std::string BiasedFields("BiasedFields");
    const std::string NumNeighbors("NumNeighbors");
    const std::string RGBs("RGBs");
    const std::string Centroids("Centroids");
    const std::string NumCells("NumCells");
    const std::string Volumes("Volumes");
    const std::string EquivalentDiameters("EquivalentDiameters");
    const std::string Schmids("Schmids");
    const std::string SlipSystems("SlipSystems");
    const std::string AspectRatios("AspectRatios");
    const std::string AxisLengths("AxisLengths");
    const std::string Omega3s("Omega3s");
    const std::string AvgQuats("AvgQuats");
    const std::string Poles("Poles");
    const std::string Neighborhoods("Neighborhoods");
    const std::string GrainAvgMisorientations("GrainAvgMisorientations");
    const std::string KernelAvgMisorientations("KernelAvgMisorientations");
    const std::string NeighborList("NeighborList");
    const std::string SharedSurfaceAreaList("SharedSurfaceAreaList");
    const std::string LMG("LMG");
  }

  namespace EnsembleData
  {
    const std::string NumFields("NumFields");
    const std::string TotalSurfaceAreas("TotalSurfaceAreas");
    const std::string CrystalStructures("CrystalStructures");
    const std::string PhaseTypes("PhaseTypes");
    const std::string PrecipitateFractions("PrecipitateFractions");
    const std::string ShapeTypes("ShapeTypes");
    const std::string Statistics("Statistics");
  }

#if 0
  namespace VTK {
     const std::string GrainIdScalarName("GrainIds");
     const std::string PhaseIdScalarName("Phases");
     const std::string EulerAnglesName("EulerAngles");
     const std::string SurfaceVoxelScalarName("SurfaceVoxels");
     const std::string NeighborsScalarName("Neighbors");
     //Quats
     //AlreadyChecked
     const std::string GoodVoxelScalarName("GoodVoxels");
     //NearestNeighbors
     //NearestNeighborDistances
     //GrainMisorientations
     //MisorientationGradients
     //KernelAverageMisorientations
     const std::string ImageQualityScalarName("ImageQuality");




     const std::string EuclideanScalarName("Euclidean");
     const std::string SchmidFactorScalarName("SchmidFactor");
     const std::string KAMScalarName("KAM");
     const std::string GAMScalarName("GAM");
     const std::string LMGScalarName("LMG");
  }

#endif

  namespace HDF5
  {

    const std::string DataContainerName("DataContainer");
    const std::string VoxelDataName("VoxelData");
    const std::string PipelineGroupName("Pipeline");
    const std::string ObjectType("ObjectType");
    const std::string NumComponents("NumComponents");

  //  const std::string Grain_ID("Grain_ID");
   // const std::string SchmidFactor ("SchmidFactor");
    const std::string Neighbor_Grain_ID_List( "Neighbor_Grain_ID_List");
    const std::string KernelAvgDisorientation( "KernelAvgDisorientation");
    const std::string GrainAvgDisorientation ("GrainAvgDisorientation");
  //  const std::string ImageQuality( "ImageQuality");

   // const std::string Phase("Phase");

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

  namespace EulerFrameRotationAxis
  {
    const unsigned int RD = 0; //!<
    const unsigned int TD = 1; //!<
    const unsigned int ND = 2; //!<
    const unsigned int None = 3; //!<
  }

  namespace EulerFrameRotationAngle
  {
    const unsigned int Ninety = 0; //!<
    const unsigned int oneEighty = 1; //!<
    const unsigned int twoSeventy = 2; //!<
    const unsigned int Zero = 3; //!<
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
    const std::string NodesFileBin("nodes.bin");
    const std::string TrianglesFileBin("triangles.bin");
    const std::string VisualizationVizFile("Visualization.vtk");
    const std::string VTKExt("vtk");

#if 0
    const std::string NodesRawFile("nodes_raw.txt");
    const std::string NodesFile("nodes.txt");
    const std::string TrianglesFileIndex("triangles");
    const std::string TrianglesFile("triangles.txt");
    const std::string EdgesFileIndex("edges");
    const std::string EdgesFile("edges.txt");

    const std::string Smooth3DIterationFile("Smooth3D");
    const std::string NodesSmoothedFile("nodes_smoothed.txt");
    const std::string STLFile("Mesh.stl");
#endif

  }


  namespace VolumeMeshing {
    /* Volume Meshing Related */
    const std::string MeshFile("volumetric_mesh_v5_1.vtk");
    const std::string MeshFile2("volumetric_mesh_v5_2.vtk");
    const std::string ElementQualityFile("element_quality_measures_v5.txt");
    const std::string VoxelsFile("voxels_v5.txt");
  } // End Namespace Representation
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


#endif /* AIM_CONSTANTS_H_ */
