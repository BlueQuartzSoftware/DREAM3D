# Pipeline : (02) SmallIN100 Smooth Mesh (from EBSD Surface Meshing)

import simpl
import simplpy as d3d
import simpl_helpers as sh
import simpl_test_dirs as sd
import surfacemeshingpy as surfacemeshing

def small_in100_smoothmesh():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sh.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/SurfaceMesh/SmallIN100_Meshed.dream3d')
    
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # Laplacian Smoothing
    err = surfacemeshing.laplacian_smoothing(dca,
                                             'TriangleDataContainer',
                                             simpl.DataArrayPath('TriangleDataContainer', 'VertexData', 'NodeType'),
                                             simpl.DataArrayPath('TriangleDataContainer', 'FaceData', 'FaceLabels'),
                                             100, 0.25, 0, 0.2, 0.15, 0, 0, False, 0)
    assert err == 0, f'LaplacianSmoothing ErrorCondition {err}'

    # Write to DREAM3D file
    err = sh.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/SurfaceMesh/SmallIN100_Smoothed.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_smoothmesh()
