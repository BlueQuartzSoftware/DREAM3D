# Pipeline : (07) SmallIN100 Final Processing (from EBSD Reconstruction)
#
#

import simpl
import simplpy as d3d
import simpl_helpers as sc
import simpl_test_dirs as sd
import processingpy as processing

def small_in100_final_processing():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Reconstruction/06_SmallIN100_Postsegmentation.dream3d')
    assert err == 0, f'Read DataContainerArray Structure Failed {err}'

    # Fill Bad Data
    err = processing.fill_bad_data(dca, False, 1000,
                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                   simpl.DataArrayPath('', '', ''))
    assert err == 0, f'FillBadData ErrorCondition {err}'

    # Erode / Dilate Bad Data #1
    err = processing.erode_dilate_bad_data(dca, sc.BadDataOperation.Erode, 2, True, True, True,
                                           simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'))
    assert err == 0, f'ErodeDilateBadData #1 ErrorCondition {err}'

    # Erode / Dilate Bad Data #2
    err = processing.erode_dilate_bad_data(dca, sc.BadDataOperation.Dilate, 2, True, True, True,
                                           simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'))
    assert err == 0, f'ErodeDilateBadData #1 ErrorCondition {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/Reconstruction/SmallIN100_Final.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_final_processing()
