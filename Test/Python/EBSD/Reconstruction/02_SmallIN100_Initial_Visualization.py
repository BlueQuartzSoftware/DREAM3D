# Pipeline : (02) SmallIN100 Initial Visualization (from EBSD Reconstruction)
#
#

from dream3d import simpl
from dream3d import simpl_helpers
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientation_analysis


def small_in100_initial_visualization():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read H5EBSD File
    err = orientation_analysis.read_h5_ebsd(dca, "Small IN100", "Phase Data", "EBSD Scan Data",
                                            sd.GetBuildDirectory() +
                                            "/Data/Output/Reconstruction/SmallIN100.h5ebsd",
                                            1, 117, True, simpl_helpers.AngleRepresentation.Radians,
                                            simpl.StringSet({"Fit", "Image Quality", "EulerAngles",
                                                             "SEM Signal", "Confidence Index", "Phases"}))
    if err < 0:
        print("ReadH5Ebsd ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = simpl_helpers.WriteDREAM3DFile(
        sd.GetBuildDirectory() + "/Data/Output/Reconstruction/SmallIN100.dream3d",
        dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_initial_visualization()
