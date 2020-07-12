# Pipeline : (01) SmallIN100 Archive (from EBSD Reconstruction)
#
#

import simpl
import simpl_test_dirs as sd
import orientationanalysispy as orientation_analysis

def small_in100_archive():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    err = orientation_analysis.ebsd_to_h5_ebsd(dca, sd.GetDataDirectory() + '/Data/SmallIN100',
                                               sd.GetBuildDirectory() +
                                               '/Data/Output/Reconstruction/SmallIN100.h5ebsd',
                                               'Slice_', '', 'ang', 1, 1, 117, 0.25, simpl.AxisAngleInput(0, 0, 0, 0),
                                               simpl.AxisAngleInput(0, 0, 0, 0))
    assert err == 0, f'EbsdToH5Ebsd error condition: {err}'

if __name__ == '__main__':
    small_in100_archive()
