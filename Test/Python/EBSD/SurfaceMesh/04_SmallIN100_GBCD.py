# Pipeline : (04) SmallIN100 GBCD (from EBSD Surface Meshing)

import simpl
import simplpy as d3d
import simpl_helpers as sh
import simpl_test_dirs as sd
import orientationanalysispy as orientationanalysis
import importexportpy

def small_in100_gbcd():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sh.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/SurfaceMesh/SmallIN100_MeshStats.dream3d')
    
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # Find GBCD
    err = orientationanalysis.find_gbcd(dca, 'FaceEnsembleData', 9,
                                        simpl.DataArrayPath('TriangleDataContainer', 'FaceData', 'FaceLabels'),
                                        simpl.DataArrayPath('TriangleDataContainer', 'FaceData', 'FaceNormals'),
                                        simpl.DataArrayPath('TriangleDataContainer', 'FaceData', 'FaceAreas'),
                                        simpl.DataArrayPath('Small IN100', 'Grain Data', 'AvgEulerAngles'),
                                        simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'),
                                        simpl.DataArrayPath('Small IN100', 'Phase Data', 'CrystalStructures'),
                                        'GBCD')
    assert err == 0, f'FindGBCD ErrorCondition {err}'

    # Export GBCD Pole Figure (GMT 5)
    err = importexportpy.visualize_gbcdgmt(dca, sd.GetBuildDirectory() + '/Data/Output/SmallIN100GBCD/SmallIn100GMT_1.dat',
                                 1, simpl.AxisAngleInput(60, 1, 1, 1),
                                 simpl.DataArrayPath('TriangleDataContainer', 'FaceEnsembleData', 'GBCD'),
                                 simpl.DataArrayPath('Small IN100', 'Phase Data', 'CrystalStructures'))
    assert err == 0, f'VisualizeGBCDGMT ErrorCondition {err}'

    # Export GBCD Pole Figure (VTK)
    err = importexportpy.visualize_gbcd_pole_figure(dca,
                                          sd.GetBuildDirectory() + '/Data/Output/SmallIN100GBCD/SmallIn100VTK.vtk',
                                          1, simpl.AxisAngleInput(60, 1, 1, 1),
                                          simpl.DataArrayPath('TriangleDataContainer', 'FaceEnsembleData', 'GBCD'),
                                          simpl.DataArrayPath('Small IN100', 'Phase Data', 'CrystalStructures'))
    assert err == 0, f'VisualizeGBCDPoleFigure ErrorCondition {err}'

    # Write to DREAM3D file
    err = sh.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/SurfaceMesh/SmallIN100_GBCD.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_gbcd()
