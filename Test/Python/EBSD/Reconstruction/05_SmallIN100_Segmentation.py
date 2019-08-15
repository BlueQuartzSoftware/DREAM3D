# Pipeline : (05) SmallIN100 Segmentation (from EBSD Reconstruction)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import reconstructionpy as reconstruction

import datetime as datetime


def small_in100_segmentation():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + "/Data/Output/Reconstruction/04_SmallIN100_Presegmentation.dream3d")
    if err < 0:
            print("Read DataContainerArray Structure Failed %d" % err)

    # Segment Features (Misorientation)
    err = reconstruction.ebsd_segment_features(dca, "Grain Data", 5, True,
                                               simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Mask"),
                                               simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                               simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"),
                                               simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Quats"),
                                               "FeatureIds", "Active")
    if err < 0:
        print("SegmentFeatures ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + "/Data/Output/Reconstruction/05_SmallIN100_Segmentation.dream3d",
        dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_segmentation()
