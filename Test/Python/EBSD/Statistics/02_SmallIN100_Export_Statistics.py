# Pipeline : (02) SmallIN100 Export Statistics (from EBSD Statistics)

import simpl
import simplpy as d3d
import simpl_test_dirs as sd
import simpl_helpers as sh

def small_in100_export_stats():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sh.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Statistics/SmallIN100_Morph.dream3d')
   
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # Export Feature Data as CSV File
    err = d3d.feature_data_csv_writer(dca, simpl.DataArrayPath('Small IN100', 'Grain Data', ''),
                                      sd.GetBuildDirectory() +
                                      '/Data/Output/Statistics/SmallIN100_Morph/FeatureStatistics.csv',
                                      False, simpl.DelimiterTypes.Comma, True)
    assert err == 0, f'FeatureDataCSVWriter ErrorCondition {err}'

if __name__ == '__main__':
    small_in100_export_stats()
