# Pipeline : (03) SmallIN100 Mesh Statistics (from EBSD Surface Meshing)

import simpl
import simplpy as d3d
import simpl_helpers as sh
import simpl_test_dirs as sd
import orientationanalysispy as orientationanalysis
import surfacemeshingpy as surfacemeshing

def small_in100_mesh_stats():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sh.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/SurfaceMesh/SmallIN100_Smoothed.dream3d')
    
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # Generate Triangle Areas
    err = surfacemeshing.triangle_area_filter(dca,
                                              simpl.DataArrayPath('TriangleDataContainer', 'FaceData', 'FaceAreas'))
    assert err == 0, f'TriangleAreaFilter ErrorCondition {err}'

    # Generate Triangle Normals
    err = surfacemeshing.triangle_normal_filter(dca,
                                                simpl.DataArrayPath('TriangleDataContainer', 'FaceData', 'FaceNormals'))
    assert err == 0, f'TriangleNormalFilter ErrorCondition {err}'

    # Find Minimum Triangle Dihedral Angle
    err = surfacemeshing.triangle_dihedral_angle_filter(dca, simpl.DataArrayPath('TriangleDataContainer', 'FaceData',
                                                                                 'FaceDihedralAngles'))
    assert err == 0, f'TriangleDihedralAngleFilter ErrorCondition {err}'

    # Generate IPF Colors (Face)
    err = orientationanalysis.generate_face_ipf_coloring(dca,
                                                         simpl.DataArrayPath('TriangleDataContainer', 'FaceData',
                                                                             'FaceLabels'),
                                                         simpl.DataArrayPath('TriangleDataContainer', 'FaceData',
                                                                             'FaceNormals'),
                                                         simpl.DataArrayPath('Small IN100', 'Grain Data',
                                                                             'AvgEulerAngles'),
                                                         simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'),
                                                         simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                             'CrystalStructures'),
                                                         'SurfaceMeshFaceIPFColors')
    assert err == 0, f'GenerateFaceIPFColoring ErrorCondition {err}'

    # Generate Misorientation Colors (Face)
    err = orientationanalysis.generate_face_misorientation_coloring(dca,
                                                                    simpl.DataArrayPath('TriangleDataContainer',
                                                                                        'FaceData', 'FaceLabels'),
                                                                    simpl.DataArrayPath('Small IN100', 'Grain Data',
                                                                                        'AvgQuats'),
                                                                    simpl.DataArrayPath('Small IN100', 'Grain Data',
                                                                                        'Phases'),
                                                                    simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                                        'CrystalStructures'),
                                                                    'SurfaceMeshFaceMisorientationColors')
    assert err == 0, f'GenerateFaceMisorientationColoring ErrorCondition {err}'

    # Write to DREAM3D file
    err = sh.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/SurfaceMesh/SmallIN100_MeshStats.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_mesh_stats()
