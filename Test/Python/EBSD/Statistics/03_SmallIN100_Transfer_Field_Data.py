# Pipeline : (03) SmallIN100 Transfer Field Data (from EBSD Statistics)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd

import datetime as datetime


def small_in100_transfer_field_data():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + "/Data/Output/Statistics/SmallIN100_Morph.dream3d")
   
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Copy Feature Array to Element Array
    err = d3d.copy_feature_array_to_element_array(dca,
                                                  simpl.DataArrayPath("Small IN100", "Grain Data",
                                                                      "EquivalentDiameters"),
                                                  simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                                  "EquivalentDiameters")
    if err < 0:
        print("CopyFeatureArrayToElementArray ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + "/Data/Output/Statistics/SmallIN100_TransferFieldData.dream3d",
        dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_transfer_field_data()
