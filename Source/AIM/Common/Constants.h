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



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace AIM
{

  namespace Representation
  {
    /*    Reconstruction related */
    const std::string StatsFile("Reconstruction_Stats.txt");//1
    const std::string MisorientationBinsFile("Reconstruction_MoDF.txt");//8
    const std::string MicroBinsFile("Reconstruction_MicroTexture.txt");//9
    const std::string ReconstructedVisualizationFile("Reconstruction_Visualization.vtk");//11
    const std::string ReconstructedIPFVisualizationFile("Reconstruction_IPF_Visualization.vtk");//11
    const std::string ReconstructedDisVisualizationFile("Reconstruction_Disorientation_Visualization.vtk");//11
    const std::string ReconstructedIQVisualizationFile("Reconstruction_ImageQuality_Visualization.vtk");//11
    const std::string ReconstructedSFVisualizationFile("Reconstruction_SchmidFactor_Visualization.vtk");//11
    const std::string StructureFile("Reconstruction_Vis.vtk");//11
    const std::string AxisOrientationsFile("Reconstruction_AxisODF.txt");//14
    const std::string graindataFile("Reconstruction_GrainData.txt");//14
    const std::string EulerAnglesFile("Reconstruction_ODF.txt");//15

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
    /*  Grain Generator Related */
    const std::string CubeFile("Synthetic_Vis.vtk");
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

    /* Volume Meshing Related */
    const std::string MeshFile("volumetric_mesh_v5_1.vtk");
    const std::string MeshFile2("volumetric_mesh_v5_2.vtk");
    const std::string ElementQualityFile("element_quality_measures_v5.txt");
    const std::string VoxelsFile("voxels_v5.txt");



  } // End Namespace Representation
}

#endif /* AIM_CONSTANTS_H_ */
