# Pipeline : (07) SmallIN100 Final Processing (from EBSD Reconstruction)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import processingpy as processing

import datetime as datetime


def small_in100_final_processing():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + "/Data/Output/Reconstruction/06_SmallIN100_Postsegmentation.dream3d")
    if err < 0:
            print("Read DataContainerArray Structure Failed %d" % err)

    # Fill Bad Data
    err = processing.fill_bad_data(dca, False, 1000,
                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                   simpl.DataArrayPath("", "", ""))
    if err < 0:
        print("FillBadData ErrorCondition %d" % err)

    # Erode / Dilate Bad Data #1
    err = processing.erode_dilate_bad_data(dca, sc.BadDataOperation.Erode, 2, True, True, True,
                                           simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"))
    if err < 0:
        print("ErodeDilateBadData #1 ErrorCondition %d" % err)

    # Erode / Dilate Bad Data #2
    err = processing.erode_dilate_bad_data(dca, sc.BadDataOperation.Dilate, 2, True, True, True,
                                           simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"))
    if err < 0:
        print("ErodeDilateBadData #1 ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/Reconstruction/SmallIN100_Final.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_final_processing()
