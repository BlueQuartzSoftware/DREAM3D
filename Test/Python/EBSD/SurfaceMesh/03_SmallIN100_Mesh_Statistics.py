# Pipeline : (03) SmallIN100 Mesh Statistics (from EBSD Surface Meshing)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientationanalysis
from dream3d import surfacemeshingpy as surfacemeshing

import datetime as datetime


def small_in100_mesh_stats():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + "/Data/Output/SurfaceMesh/SmallIN100_Smoothed.dream3d")
    
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Generate Triangle Areas
    err = surfacemeshing.triangle_area_filter(dca,
                                              simpl.DataArrayPath("TriangleDataContainer", "FaceData", "FaceAreas"))
    if err < 0:
        print("TriangleAreaFilter ErrorCondition %d" % err)

    # Generate Triangle Normals
    err = surfacemeshing.triangle_normal_filter(dca,
                                                simpl.DataArrayPath("TriangleDataContainer", "FaceData", "FaceNormals"))
    if err < 0:
        print("TriangleNormalFilter ErrorCondition %d" % err)

    # Find Minimum Triangle Dihedral Angle
    err = surfacemeshing.triangle_dihedral_angle_filter(dca, simpl.DataArrayPath("TriangleDataContainer", "FaceData",
                                                                                 "FaceDihedralAngles"))
    if err < 0:
        print("TriangleDihedralAngleFilter ErrorCondition %d" % err)

    # Generate IPF Colors (Face)
    err = orientationanalysis.generate_face_ipf_coloring(dca,
                                                         simpl.DataArrayPath("TriangleDataContainer", "FaceData",
                                                                             "FaceLabels"),
                                                         simpl.DataArrayPath("TriangleDataContainer", "FaceData",
                                                                             "FaceNormals"),
                                                         simpl.DataArrayPath("Small IN100", "Grain Data",
                                                                             "AvgEulerAngles"),
                                                         simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                                         simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                             "CrystalStructures"),
                                                         "SurfaceMeshFaceIPFColors")
    if err < 0:
        print("GenerateFaceIPFColoring ErrorCondition %d" % err)

    # Generate Misorientation Colors (Face)
    err = orientationanalysis.generate_face_misorientation_coloring(dca,
                                                                    simpl.DataArrayPath("TriangleDataContainer",
                                                                                        "FaceData", "FaceLabels"),
                                                                    simpl.DataArrayPath("Small IN100", "Grain Data",
                                                                                        "AvgQuats"),
                                                                    simpl.DataArrayPath("Small IN100", "Grain Data",
                                                                                        "Phases"),
                                                                    simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                                        "CrystalStructures"),
                                                                    "SurfaceMeshFaceMisorientationColors")
    if err < 0:
        print("GenerateFaceMisorientationColoring ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/SurfaceMesh/SmallIN100_MeshStats.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_mesh_stats()
