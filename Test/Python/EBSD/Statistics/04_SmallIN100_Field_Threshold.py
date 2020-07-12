# Pipeline : (04) SmallIN100 Field Threshold (from EBSD Statistics)
#
#

import simpl
import simplpy as d3d
import simpl_helpers as sc
import simpl_test_dirs as sd

def small_in100_field_threshold():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Statistics/SmallIN100_TransferFieldData.dream3d')
   
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
    selectedThresholds = simpl.ComparisonInputs()
    selectedThresholds.addInput('Small IN100', 'Grain Data', 'EquivalentDiameters', 1, 5)
    selectedThresholds.addInput('Small IN100', 'Grain Data', 'NumNeighbors', 1, 20)
    selectedThresholds.addInput('Small IN100', 'Grain Data', 'Omega3s', 0, 0.5)

    err = d3d.multi_threshold_objects(dca, 'CriticalFields', selectedThresholds)
    assert err == 0, f'MultiThresholdObjects ErrorCondition: {err}'

    # Copy Feature Array to Element Array
    err = d3d.copy_feature_array_to_element_array(dca,
                                                  simpl.DataArrayPath('Small IN100', 'Grain Data', 'CriticalFields'),
                                                  simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                                  'CriticalField')
    assert err == 0, f'CopyFeatureArrayToElementArray ErrorCondition {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/Statistics/SmallIN100_CopyCritical.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_field_threshold()
