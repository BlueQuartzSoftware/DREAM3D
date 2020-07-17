# Pipeline : (01) SmallIN100 Quick Mesh (from EBSD Surface Meshing)

import simpl
import simplpy as d3d
import simpl_helpers as sh
import simpl_test_dirs as sd
import samplingpy as sampling
import surfacemeshingpy as surfacemeshing

def small_in100_quickmesh():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sh.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Statistics/SmallIN100_CrystalStats.dream3d')
    
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # Crop Geometry (Image)
    err = sampling.crop_image_geometry(dca, '',
                                       simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', ''),
                                       simpl.DataArrayPath('Small IN100', 'Grain Data', ''),
                                       41, 41, 0, 140, 140, 99, True, False, True,
                                       simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'))
    assert err == 0, f'CropImageGeometry ErrorCondition {err}'

    # Quick Surface Mesh
    err = surfacemeshing.quick_surface_mesh(dca,
                                            [],
                                            'TriangleDataContainer',
                                            'VertexData',
                                            'FaceData',
                                            simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                            'FaceLabels',
                                            'NodeType',
                                            'FaceFeatureData')
    assert err == 0, f'QuickSurfaceMesh ErrorCondition {err}'

    # Write to DREAM3D file
    err = sh.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/SurfaceMesh/SmallIN100_Meshed.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_quickmesh()
