# Pipeline : (03) SmallIN100 Transfer Field Data (from EBSD Statistics)
#
#

import simpl
import simplpy as d3d
import simpl_helpers as sc
import simpl_test_dirs as sd

def small_in100_transfer_field_data():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Statistics/SmallIN100_Morph.dream3d')
   
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # Copy Feature Array to Element Array
    err = d3d.copy_feature_array_to_element_array(dca,
                                                  simpl.DataArrayPath('Small IN100', 'Grain Data',
                                                                      'EquivalentDiameters'),
                                                  simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                                  'EquivalentDiameters')
    assert err == 0, f'CopyFeatureArrayToElementArray ErrorCondition {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + '/Data/Output/Statistics/SmallIN100_TransferFieldData.dream3d',
        dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_transfer_field_data()
