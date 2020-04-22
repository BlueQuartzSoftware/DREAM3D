# Pipeline : (04) SmallIN100 GBCD (from EBSD Surface Meshing)

import simpl
import simplpy as d3d
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy as orientationanalysis
import importexportpy

def small_in100_gbcd():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/SurfaceMesh/SmallIN100_MeshStats.dream3d')
    
    if err < 0:
        print('DataContainerReader ErrorCondition %d' % err)

    # Find GBCD
    err = orientationanalysis.find_gbcd(dca, 'FaceEnsembleData', 9,
                                        simpl.DataArrayPath('TriangleDataContainer', 'FaceData', 'FaceLabels'),
                                        simpl.DataArrayPath('TriangleDataContainer', 'FaceData', 'FaceNormals'),
                                        simpl.DataArrayPath('TriangleDataContainer', 'FaceData', 'FaceAreas'),
                                        simpl.DataArrayPath('Small IN100', 'Grain Data', 'AvgEulerAngles'),
                                        simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'),
                                        simpl.DataArrayPath('Small IN100', 'Phase Data', 'CrystalStructures'),
                                        'GBCD')
    if err < 0:
        print('FindGBCD ErrorCondition %d' % err)

    # Export GBCD Pole Figure (GMT 5)
    err = importexportpy.visualize_gbcdgmt(dca, sd.GetBuildDirectory() + '/Data/Output/SmallIN100GBCD/SmallIn100GMT_1.dat',
                                 1, simpl.AxisAngleInput(60, 1, 1, 1),
                                 simpl.DataArrayPath('TriangleDataContainer', 'FaceEnsembleData', 'GBCD'),
                                 simpl.DataArrayPath('Small IN100', 'Phase Data', 'CrystalStructures'))
    if err < 0:
        print('VisualizeGBCDGMT ErrorCondition %d' % err)

    # Export GBCD Pole Figure (VTK)
    err = importexportpy.visualize_gbcd_pole_figure(dca,
                                          sd.GetBuildDirectory() + '/Data/Output/SmallIN100GBCD/SmallIn100VTK.vtk',
                                          1, simpl.AxisAngleInput(60, 1, 1, 1),
                                          simpl.DataArrayPath('TriangleDataContainer', 'FaceEnsembleData', 'GBCD'),
                                          simpl.DataArrayPath('Small IN100', 'Phase Data', 'CrystalStructures'))
    if err < 0:
        print('VisualizeGBCDPoleFigure ErrorCondition %d' % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/SurfaceMesh/SmallIN100_GBCD.dream3d',
                              dca)
    if err < 0:
        print('WriteDREAM3DFile ErrorCondition: %d' % err)

if __name__ == '__main__':
    small_in100_gbcd()
