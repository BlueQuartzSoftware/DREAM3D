# Pipeline : (04) SmallIN100 Presegmentation Processing (from EBSD Reconstruction)
#
#

import simpl
import simplpy as d3d
import simpl_helpers as sh
import simpl_test_dirs as sd
import orientationanalysispy as orientation_analysis

def small_in100_presegmentation_processing():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sh.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Reconstruction/03_SmallIN100_Aligned.dream3d')
    assert err == 0, f'Read DataContainerArray Structure Failed {err}'

    # Neighbor Orientation Comparison (Bad Data)
    err = orientation_analysis.bad_data_neighbor_orientation_check(dca, 5, 4,
                                                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                       'Mask'),
                                                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                       'Phases'),
                                                                   simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                                       'CrystalStructures'),
                                                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                       'Quats'))
    assert err == 0, f'NeighborOrientationComparison ErrorCondition {err}'

    # Neighbor Orientation Correlation
    err = orientation_analysis.neighbor_orientation_correlation(dca, 5, 0.2, 2,
                                                                simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                    'Confidence Index'),
                                                                simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                    'Phases'),
                                                                simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                                    'CrystalStructures'),
                                                                simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                    'Quats'))
    assert err == 0, f'NeighborOrientationCorrelation ErrorCondition {err}'

    # Delete Data (Remove array helper function)
    sh.RemoveArrays(dca, [('Small IN100', 'EBSD Scan Data', 'IPFColor')])

    # Generate IPF Colors
    err = orientation_analysis.generate_ipf_colors(dca, simpl.FloatVec3([0, 0, 1]),
                                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Phases'),
                                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'EulerAngles'),
                                                   simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                       'CrystalStructures'),
                                                   True,
                                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Mask'),
                                                   'IPFColor')
    assert err == 0, f'GenerateIPFColors ErrorCondition: {err}'

    # Write to DREAM3D file
    err = sh.WriteDREAM3DFile(
        sd.GetBuildDirectory() + '/Data/Output/Reconstruction/04_SmallIN100_Presegmentation.dream3d',
        dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_presegmentation_processing()
