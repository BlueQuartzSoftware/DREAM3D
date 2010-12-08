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

#include <string>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace AIM
{

  namespace Representation {
    const std::string Grain_ID("Grain_ID");
    const std::string SchmidFactor ("SchmidFactor");
    const std::string Neighbor_Grain_ID_List( "Neighbor_Grain_ID_List");
    const std::string KernelAvgDisorientation( "KernelAvgDisorientation");
    const std::string GrainAvgDisorientation ("GrainAvgDisorientation");
    const std::string ImageQuality( "ImageQuality");
  }


  namespace Reconstruction
  {
    /*    Reconstruction related */
    const std::string AlignmentFile("Reconstruction_Alignment.txt");
    const std::string AxisOrientationsFile("Reconstruction_AxisODF.txt");//14
    const std::string GrainDataFile("Reconstruction_GrainData.txt");//14
    const std::string MicroBinsFile("Reconstruction_MicroTexture.txt");//9
    const std::string MisorientationBinsFile("Reconstruction_MoDF.txt");//8
    const std::string EulerAnglesFile("Reconstruction_ODF.txt");//15
    const std::string StatsFile("Reconstruction_Stats.txt");

    const std::string DisorientationVizFile("Reconstruction_Disorientation_Visualization.vtk");//11
    const std::string ImageQualityVizFile("Reconstruction_ImageQuality_Visualization.vtk");//11
    const std::string IPFVizFile("Reconstruction_IPF_Visualization.vtk");//11
    const std::string SchmidFactorVizFile("Reconstruction_SchmidFactor_Visualization.vtk");//11
    const std::string VisualizationVizFile("Reconstruction_Visualization.vtk");//11
    const std::string DownSampledVizFile("Reconstruction_DownSampled_Visualization.vtk");//11

    const std::string HDF5GrainFile("Reconstruction_Grains.h5grain");

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
