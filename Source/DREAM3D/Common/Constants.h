/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef AIM_CONSTANTS_H_
#define AIM_CONSTANTS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include "DREAM3D/DREAM3DConfiguration.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<std::vector<int> >    IntVectorType;
typedef boost::shared_ptr<std::vector<float> > FloatVectorType;
typedef boost::shared_ptr<std::vector<double> > DoubleVectorType;
typedef boost::shared_ptr<std::vector<size_t> > SizeTVectorType;

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
#define DEEP_COPY_SHARED_VECTOR(sharedPtr, obj, VType, type)\
if (NULL != sharedPtr.get())\
{\
  sharedPtr = VType(static_cast<std::vector<type>*>(NULL));\
}\
if (NULL != obj->sharedPtr.get())\
{\
  sharedPtr = VType(new std::vector<type>(*(obj->sharedPtr.get())));\
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace AIM
{




  // These are also used as HDF5 Group/Data/Attribute names so DONT change these.
  namespace Representation {
    const std::string Grain_ID("Grain_ID");
    const std::string SchmidFactor ("SchmidFactor");
    const std::string Neighbor_Grain_ID_List( "Neighbor_Grain_ID_List");
    const std::string KernelAvgDisorientation( "KernelAvgDisorientation");
    const std::string GrainAvgDisorientation ("GrainAvgDisorientation");
    const std::string ImageQuality( "ImageQuality");
    const std::string IPFColor("IPFColor");
    const std::string Phase("Phase");
  }


  namespace HDF5
  {
    const std::string Reconstruction("Reconstruction");
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
    const std::string PhaseFraction("PhaseFraction");
    const std::string CrystalStructure("CrystalStructure");
    const std::string PhaseType("PhaseType");
    const std::string PrecipitateBoundaryFraction("Precipitate Boundary Fraction");
    const std::string Grain_Diameter_Info("Grain_Diameter_Info");
    const std::string Grain_Size_Distribution("GrainSize Distribution");
    const std::string Grain_SizeVBoverA_Distributions("GrainSize Vs B Over A Distributions");
    const std::string Grain_SizeVCoverA_Distributions("GrainSize Vs C Over A Distributions");
    const std::string Grain_SizeVCoverB_Distributions("GrainSize Vs C Over B Distributions");
    const std::string Grain_SizeVNeighbors_Distributions("GrainSize Vs Neighbors Distributions");
    const std::string Grain_SizeVOmega3_Distributions("GrainSize Vs Omega3 Distributions");

    const std::string BinNumber("BinNumber");

    const std::string Average("Average");
    const std::string StandardDeviation("Standard Deviation");

    const std::string Alpha("Alpha");
    const std::string Beta("Beta");
    const std::string Exp_k("Exponent K");

    const std::string DistributionType("Distribution Type");
    const std::string BetaDistribution("Beta Distribution");
    const std::string LogNormalDistribution("Log Normal Distribution");
    const std::string PowerLawDistribution("Power Law Distribution");

    enum ColumnCount
    {
      BetaColumnCount = 2,       //!<
      LogNormalColumnCount = 2,  //!<
      PowerLawColumnCount = 3,   //!<
      UnknownColumCount = 0      //!<
    };
  }


  namespace Reconstruction
  {
    /*    Reconstruction related */
    const std::string AlignmentFile("Alignment.txt");
    const std::string H5VolumeFile("VoxelData.h5");

    const std::string VisualizationVizFile("Visualization.vtk");//11
    const std::string IPFVizFile("IPF_Visualization.vtk");//11
    const std::string DisorientationVizFile("Disorientation_Visualization.vtk");//11
    const std::string ImageQualityVizFile("ImageQuality_Visualization.vtk");//11
    const std::string SchmidFactorVizFile("SchmidFactor_Visualization.vtk");//11
    const std::string DownSampledVizFile("DownSampled_Visualization.vtk");//11
    const std::string HDF5GrainFile("Grains.h5grain");
    const std::string PhFile("Voxels.ph");




    const std::string VoxelDataName("VoxelData");
    const std::string GrainIdScalarName("GrainID");
    const std::string EulerAnglesName("Euler Angles");
    const std::string CrystalStructureName("CrystalStructure");
    const std::string EuclideanScalarName("Euclidean");
    const std::string SchmidFactorScalarName("SchmidFactor");
    const std::string PhaseIdScalarName("PhaseID");
    const std::string KAMScalarName("KAM");
    const std::string GAMScalarName("GAM");
    const std::string LMGScalarName("LMG");
    const std::string ImageQualityScalarName("ImageQuality");
    const std::string SurfaceVoxelScalarName("SurfaceVoxel");


    /**
     * @brief IF YOU CHANGE THE VALUES THERE ARE DEEP RAMIFICATIONS IN THE CODE BUT
     * MOSTLY IN THE HDF5 FILES WHICH ARE WRITTEN USING THE ENUMERATIONS.
     */
    enum CrystalStructure {
        Hexagonal = 0,                   //!< Hexagonal
        Cubic = 1,                       //!< Cubic
        OrthoRhombic = 2,                //!< OrthoRhombic
        AxisOrthoRhombic = 3,            //!< AxisOrthoRhombic
        UnknownCrystalStructure = 999    //!< UnknownCrystalStructure
    };

    enum PhaseType {
      PrimaryPhase = 0,              //!<
      PrecipitatePhase = 1,          //!<
      TransformationPhase = 2,       //!<
      UnknownPhaseType = 999    //!<
    };

    enum AlignmentMethod {
        OuterBoundary = 0,        //!<
        Misorientation = 1,       //!<
        MutualInformation = 2,    //!<
        UnknownAlignmentMethod = 999    //!<
    };

    enum StatisticsType
    {
      Grain_SizeVBoverA = 0,    //!<
      Grain_SizeVCoverA,        //!<
      Grain_SizeVCoverB,        //!<
      Grain_SizeVNeighbors,     //!<
      Grain_SizeVOmega3,        //!<
      UnknownStatisticsGroup = 999    //!<
    };

    enum DistributionType
    {
      Beta = 0,         //!<
      LogNormal = 1,    //!<
      Power = 2,        //!<
      UnknownDistributionType = 999    //!<
    };

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
#endif


// Keep these to remember the names of the texture components in order
//	const int Brassbin = {0.610865,0.785398,0.0};
//	const int Sbin = {0.959931,0.610865,1.134464};
//	const int Copperbin = {1.570796,0.523599,0.785398};
//	const int Shear1bin = {0.0,0.0,0.785398};
//	const int Shear2bin = {0.0,0.959931,0.785398};
//	const int Shear3bin = {0.0,0.610865,0.785398};
//	const int Gossbin = {0.0,0.785398,0.0};
//	const int Cubebin = {0.0,0.0,0.0};
//	const int RC1bin = {0.0,0.349066,0.0};
//	const int RC2bin = {0.0,0.610865,0.0};
//	const int RC3bin = {0.349066,0.0,0.0};
//	const int RC4bin = {0.610865,0.0,0.0};
//	const int Pbin = {1.22173,0.785398,0.0};
//	const int Qbin = {0.959931,0.349066,0.0};
//	const int Rbin = {0.959931,1.308997,0.436332};
  }




  namespace SyntheticBuilder
  {
    /*  Grain Generator Related */

    const std::string CrystallographicErrorFile("CrystallographicError.txt");
    const std::string GrainAnglesFile("GrainAngles.txt");
    const std::string GrainDataFile("GrainData.txt");
    const std::string PackGrainsFile("PackGrains.txt");
    const std::string H5StatisticsFile("Results.h5");

    const std::string VisualizationVizFile("Visualization.vtk");
    const std::string IPFVizFile("IPF_Visualization.vtk");
    const std::string HDF5GrainFile("Grains.h5grain");
    const std::string PhFile("Voxels.ph");

    const std::string GrainIdScalarName("GrainID");



    enum Overlap {
        UnknownOverlap = 0,    //!<
        Rigid = 1,             //!<
        Progressive = 2        //!<
    };

    enum ShapeClass {
        UnknownShapeClass = 0,   //!<
        Ellipsoid = 1,           //!<
        Superellipsoid = 2,      //!<
        CubeOctahedron = 3       //!<
    };

	enum Precipitates {
        UnknownPrecipitates = 0,    //!<
        NoPrecipitates= 1,          //!<
        BoundaryPrecipitates = 2,   //!<
        BulkPrecipitates = 3        //!<
    };

  }

  namespace MicroStats {
    const std::string H5StatisticsFile("Results.h5");
    const std::string GrainDataFile("GrainData.txt");//14
    const std::string DeformationStatsFile("Deformation_Stats.txt");
    const std::string IPFDeformVTKFile("IPF_DeformationData.vtk");

  }




  namespace SurfaceMesh {

    /*   Surface Meshing Related   */
    const std::string VTKExt("vtk");
    const std::string NodesRawFile("nodes_raw.txt");
    const std::string NodesFile("nodes.txt");
    const std::string NodesFileBin("nodes.bin");
    const std::string TrianglesFileBin("triangles.bin");
    const std::string TrianglesFileIndex("triangles");
    const std::string TrianglesFile("triangles.txt");
    const std::string EdgesFileIndex("edges");
    const std::string EdgesFile("edges.txt");
    const std::string VisualizationVizFile("Visualization.vtk");
    const std::string Smooth3DIterationFile("Smooth3D");
    const std::string NodesSmoothedFile("nodes_smoothed.txt");
    const std::string STLFile("Mesh.stl");
  }

  namespace VolumeMeshing {
    /* Volume Meshing Related */
    const std::string MeshFile("volumetric_mesh_v5_1.vtk");
    const std::string MeshFile2("volumetric_mesh_v5_2.vtk");
    const std::string ElementQualityFile("element_quality_measures_v5.txt");
    const std::string VoxelsFile("voxels_v5.txt");
  } // End Namespace Representation
}

#endif /* AIM_CONSTANTS_H_ */
