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
    const std::string StatsFile("stats.txt");//1
    const std::string VolBinFile("volbins.txt"); //2
    const std::string BOverABinsFile("boverabins.txt");//3
    const std::string COverABinsFile("coverabins.txt");//4
    const std::string COverBBinsFile("coverbbins.txt");//5
    const std::string SVNFile("svn.txt");//6
    const std::string SVSFile("svs.txt");//7
    const std::string MisorientationBinsFile("misobins.txt");//8
    const std::string MicroBinsFile("microbins.txt");//9
    const std::string ReconstructedDataFile("reconstructed_data.txt");//10
    const std::string ReconstructedVisualizationFile("reconstructed_visualization.vtk");//11
    const std::string GrainsFile("grains.txt");//12
    const std::string BoundaryCentersFile("boundarycenters.txt");//13
    const std::string AxisOrientationsFile("axisorientations.txt");//14
    const std::string EulerAnglesFile("eulerangles.txt");//15
    const std::string SeNBinsFile("seNBins.txt");

    enum CrystalStructure {
        UnknownCrystalStructure = 0,
        Hexagonal = 1,
        Cubic = 2
    };

    /*  Grain Generator Related */

    const std::string CubeFile("cube.vtk");
    const std::string AnalysisFile("analysis.txt");
    const std::string VolumeFile("volume.txt");

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


    /*   Surface Meshing Related   */

    const std::string VTKExt("vtk");
    const std::string DXExt ("dx");
    const std::string NodesRawFile("nodes_raw.txt");
    const std::string MeshStatFile("mesh_stat.txt");

    const std::string NodesFile("nodes.txt");
    const std::string TrianglesFile("triangles.txt");
    const std::string EdgesFile("edges.txt");

    /* Volume Meshing Related */
    const std::string MeshFile("volumetric_mesh_v5_1.vtk");
    const std::string MeshFile2("volumetric_mesh_v5_2.vtk");
    const std::string ElementQualityFile("element_quality_measures_v5.txt");
    const std::string VoxelsFile("voxels_v5.txt");



  } // End Namespace Representation
}

#endif /* AIM_CONSTANTS_H_ */
