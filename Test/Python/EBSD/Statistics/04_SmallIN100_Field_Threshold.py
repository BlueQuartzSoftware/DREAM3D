# Pipeline : (04) SmallIN100 Field Threshold (from EBSD Statistics)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd

import datetime as datetime


def small_in100_field_threshold():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + "/Data/Output/Statistics/SmallIN100_TransferFieldData.dream3d")
   
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
    selectedThresholds = simpl.ComparisonInputs()
    selectedThresholds.addInput("Small IN100", "Grain Data", "EquivalentDiameters", 1, 5)
    selectedThresholds.addInput("Small IN100", "Grain Data", "NumNeighbors", 1, 20)
    selectedThresholds.addInput("Small IN100", "Grain Data", "Omega3s", 0, 0.5)

    err = d3d.multi_threshold_objects(dca, "CriticalFields", selectedThresholds)
    if err < 0:
        print("MultiThresholdObjects ErrorCondition: %d" % err)

    # Copy Feature Array to Element Array
    err = d3d.copy_feature_array_to_element_array(dca,
                                                  simpl.DataArrayPath("Small IN100", "Grain Data", "CriticalFields"),
                                                  simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                                  "CriticalField")
    if err < 0:
        print("CopyFeatureArrayToElementArray ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/Statistics/SmallIN100_CopyCritical.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_field_threshold()
