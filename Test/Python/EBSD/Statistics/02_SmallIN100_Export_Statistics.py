# Pipeline : (02) SmallIN100 Export Statistics (from EBSD Statistics)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_test_dirs as sd
from dream3d import simpl_helpers as sc

import datetime as datetime


def small_in100_export_stats():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + "/Data/Output/Statistics/SmallIN100_Morph.dream3d")
   
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Export Feature Data as CSV File
    err = d3d.feature_data_csv_writer(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                      sd.GetBuildDirectory() +
                                      "/Data/Output/Statistics/SmallIN100_Morph/FeatureStatistics.csv",
                                      False, simpl.DelimiterTypes.Comma, True)
    if err < 0:
        print("FeatureDataCSVWriter ErrorCondition %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_export_stats()
