# Pipeline : (04) SmallIN100 Presegmentation Processing (from EBSD Reconstruction)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientation_analysis

import datetime as datetime


def small_in100_presegmentation_processing():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy("Confidence Index").Flag = 2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "EulerAngles").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Fit").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "IPFColor").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Image Quality").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Mask").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Phases").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Quats").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "SEM Signal").Flag =2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "CrystalStructures").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "LatticeConstants").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "MaterialName").Flag =2

    err = d3d.data_container_reader(dca,
                                    sd.GetBuildDirectory() +
                                    "/Data/Output/Reconstruction/03_SmallIN100_Aligned.dream3d",
                                    False, dcap)
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Neighbor Orientation Comparison (Bad Data)
    err = orientation_analysis.bad_data_neighbor_orientation_check(dca, 5, 4,
                                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                       "Mask"),
                                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                       "Phases"),
                                                                   simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                                       "CrystalStructures"),
                                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                       "Quats"))
    if err < 0:
        print("NeighborOrientationComparison ErrorCondition %d" % err)

    # Neighbor Orientation Correlation
    err = orientation_analysis.neighbor_orientation_correlation(dca, 5, 0.2, 2,
                                                                simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                    "Confidence Index"),
                                                                simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                    "Phases"),
                                                                simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                                    "CrystalStructures"),
                                                                simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                    "Quats"))
    if err < 0:
        print("NeighborOrientationCorrelation ErrorCondition %d" % err)

    # Delete Data (Remove array helper function)
    sc.RemoveArrays(dca, [("Small IN100", "EBSD Scan Data", "IPFColor")])

    # Generate IPF Colors
    err = orientation_analysis.generate_ipf_colors(dca, simpl.FloatVec3Type([0, 0, 1]),
                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "EulerAngles"),
                                                   simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                       "CrystalStructures"),
                                                   True,
                                                   simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Mask"),
                                                   "IPFColor")
    if err < 0:
        print("GenerateIPFColors ErrorCondition: %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + "/Data/Output/Reconstruction/04_SmallIN100_Presegmentation.dream3d",
        dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_presegmentation_processing()
