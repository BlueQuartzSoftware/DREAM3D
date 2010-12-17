///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
#ifndef AIM_CONSTANTS_H_
#define AIM_CONSTANTS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include "AIM/Common/AIMCommonConfiguration.h"

#include <string>




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
  }

  namespace ANG
  {
    const std::string AngHeader("AngHeader");
    const std::string ZStartIndex("ZStartIndex");
    const std::string ZEndIndex("ZEndIndex");
    const std::string ZResolution("ZResolution");
    const std::string Manufacturer("Manufacturer");
    const std::string TSL("TSL");
    const std::string OriginalFile("OriginalFile");
    const std::string Index("Index");
    const std::string Header("Header");
    const std::string Data("Data");
  }

  namespace HDF5
  {
    const std::string Reconstruction("Reconstruction");
    const std::string AxisOrientation("AxisOrientation");
    const std::string ODF("ODF");
    const std::string MisorientationBins("MisorientationBins");
    const std::string MicroTextureBins("MicroTextureBins");
    const std::string Stats("Stats");
    const std::string Grain_Diameter_Info("Grain_Diameter_Info");
    const std::string Grain_Size_Distribution("GrainSize Distribution");
    const std::string Grain_SizeVBoverA_Distributions("GrainSize Vs B Over A Distributions");
    const std::string Grain_SizeVCoverA_Distributions("GrainSize Vs C Over A Distributions");
    const std::string Grain_SizeVCoverB_Distributions("GrainSize Vs CoverB Distributions");
    const std::string Grain_SizeVNeighbors_Distributions("GrainSize Vs Neighbors Distributions");
    const std::string Grain_SizeVOmega3_Distributions("GrainSize Vs Omega3 Distributions");

    const std::string BinNumber("BinNumber");
    const std::string NumGrains("NumGrains");

    const std::string Average("Average");
    const std::string StandardDeviation("Standard Deviation");

    const std::string Shell_1_Average("Shell 1 Average");
    const std::string Shell_2_Average("Shell 2 Average");
    const std::string Shell_3_Average("Shell 3 Average");
    const std::string Shell_4_Average("Shell 4 Average");

    const std::string Shell_1_StdDev("Shell 1 Standard Deviation");
    const std::string Shell_2_StdDev("Shell 2 Standard Deviation");
    const std::string Shell_3_StdDev("Shell 3 Standard Deviation");
    const std::string Shell_4_StdDev("Shell 4 Standard Deviation");


  }

  namespace Reconstruction
  {
    /*    Reconstruction related */
    const std::string AlignmentFile("Reconstruction_Alignment.txt");
    const std::string AxisOrientationsFile("Reconstruction_AxisODF.txt"); // Convert To HDF5
    const std::string GrainDataFile("Reconstruction_GrainData.txt");//14
    const std::string MicroTextureFile("Reconstruction_MicroTexture.txt"); // Convert To HDF5
    const std::string MisorientationBinsFile("Reconstruction_MoDF.txt"); // Convert To HDF5
    const std::string ODFFile("Reconstruction_ODF.txt"); // Convert To HDF5
    const std::string StatsFile("Reconstruction_Stats.txt"); // Convert To HDF5

    const std::string DisorientationVizFile("Reconstruction_Disorientation_Visualization.vtk");//11
    const std::string ImageQualityVizFile("Reconstruction_ImageQuality_Visualization.vtk");//11
    const std::string IPFVizFile("Reconstruction_IPF_Visualization.vtk");//11
    const std::string SchmidFactorVizFile("Reconstruction_SchmidFactor_Visualization.vtk");//11
    const std::string VisualizationVizFile("Reconstruction_Visualization.vtk");//11
    const std::string DownSampledVizFile("Reconstruction_DownSampled_Visualization.vtk");//11

    const std::string HDF5GrainFile("Reconstruction_Grains.h5grain");
    const std::string HDF5ResultsFile("Reconstruction_Results.h5");

    enum CrystalStructure {
        UnknownCrystalStructure = 0,
        Hexagonal = 1,
        Cubic = 2
    };

    enum AlignmentMethod {
        OuterBoundary = 0,
        Misorientation = 1,
        MutualInformation = 2
    };

  }

  namespace SyntheticBuilder
  {
    /*  Grain Generator Related */
    const std::string CubeFile("Synthetic_Vis.vtk");
    const std::string MoDFFile("Synthetic_MoDF.txt");
    const std::string CrystallographicErrorFile("Synthetic_CrystallographicError.txt");
    const std::string AnalysisFile("Synthetic_Stats.txt");
    const std::string EulerFile("Synthetic_GrainAngles.txt");

    enum Overlap {
        UnknownOverlap = 0,
        Rigid = 1,
        Progressive = 2
    };

    enum ShapeClass {
        UnknownShapeClass = 0,
        Ellipsoid = 1,
        Superellipsoid = 2,
        CubeOctahedron = 3
    };

	enum Precipitates {
        UnknownPrecipitates = 0,
        NoPrecipitates= 1,
        BoundaryPrecipitates = 2,
        BulkPrecipitates = 3
    };

  }

  namespace SurfaceMeshing {

    /*   Surface Meshing Related   */
    const std::string VTKExt("vtk");
    const std::string NodesRawFile("nodes_raw.txt");
    const std::string NodesFile("nodes.txt");
    const std::string TrianglesFileIndex("triangles");
    const std::string TrianglesFile("triangles.txt");
    const std::string EdgesFileIndex("edges");
    const std::string EdgesFile("edges.txt");
    const std::string VisualizationFile("SurfaceMesh_Vis.vtk");
    const std::string Smooth3DIterationFile("Smooth3D");
    const std::string NodesSmoothedFile("nodes_smoothed.txt");
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
