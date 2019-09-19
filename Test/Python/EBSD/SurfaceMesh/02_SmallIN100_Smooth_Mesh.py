# Pipeline : (02) SmallIN100 Smooth Mesh (from EBSD Surface Meshing)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import surfacemeshingpy as surfacemeshing

import datetime as datetime


def small_in100_smoothmesh():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + "/Data/Output/SurfaceMesh/SmallIN100_Meshed.dream3d")
    
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Laplacian Smoothing
    err = surfacemeshing.laplacian_smoothing(dca,
                                             "TriangleDataContainer",
                                             simpl.DataArrayPath("TriangleDataContainer", "VertexData", "NodeType"),
                                             simpl.DataArrayPath("TriangleDataContainer", "FaceData", "FaceLabels"),
                                             100, 0.25, 0, 0.2, 0.15, 0, 0, False, 0)
    if err < 0:
        print("LaplacianSmoothing ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/SurfaceMesh/SmallIN100_Smoothed.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_smoothmesh()
