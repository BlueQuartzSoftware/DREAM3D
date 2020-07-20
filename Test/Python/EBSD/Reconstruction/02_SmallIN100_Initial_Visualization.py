# Pipeline : (02) SmallIN100 Initial Visualization (from EBSD Reconstruction)
#
#

import simpl
import simpl_helpers
import simpl_test_dirs as sd
import orientationanalysispy as orientation_analysis

def small_in100_initial_visualization():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read H5EBSD File
    err = orientation_analysis.read_h5_ebsd(dca, 'Small IN100', 'Phase Data', 'EBSD Scan Data',
                                            sd.GetBuildDirectory() +
                                            '/Data/Output/Reconstruction/SmallIN100.h5ebsd',
                                            1, 117, True, simpl_helpers.AngleRepresentation.Radians,
                                            simpl.StringSet({'Fit', 'Image Quality', 'EulerAngles',
                                                             'SEM Signal', 'Confidence Index', 'Phases'}))
    assert err == 0, f'ReadH5Ebsd ErrorCondition {err}'

    # Write to DREAM3D file
    err = simpl_helpers.WriteDREAM3DFile(
        sd.GetBuildDirectory() + '/Data/Output/Reconstruction/SmallIN100.dream3d',
        dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_initial_visualization()
