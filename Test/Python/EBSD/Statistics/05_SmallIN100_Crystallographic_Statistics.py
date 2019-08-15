# Pipeline : (05) SmallIN100 Crystallographic Statistics (from EBSD Statistics)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientation_analysis

import datetime as datetime


def small_in100_cryst_stats():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + "/Data/Output/Statistics/SmallIN100_CopyCritical.dream3d")
    
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # We need to remove an Attribute Array that will be recreated in a filter.
    dc = dca.getDataContainer("Small IN100")
    grainDataAM = dc.getAttributeMatrix("Grain Data")
    grainDataAM.removeAttributeArray("AvgQuats")
    
    
    # Find Feature Average Orientations
    err = orientation_analysis.find_avg_orientations(dca,
                                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Quats"),
                                                     simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                         "CrystalStructures"),
                                                     simpl.DataArrayPath("Small IN100", "Grain Data", "AvgQuats"),
                                                     simpl.DataArrayPath("Small IN100", "Grain Data", "AvgEulerAngles"))
    if err < 0:
        print("FindAvgOrientations ErrorCondition %d" % err)

    # Find Feature Neighbor Misorientations
    err = orientation_analysis.find_misorientations(dca,
                                                    simpl.DataArrayPath("Small IN100", "Grain Data", "NeighborList"),
                                                    "MisorientationList",
                                                    simpl.DataArrayPath("Small IN100", "Grain Data", "AvgQuats"),
                                                    simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                                    simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                        "CrystalStructures"),
                                                    "", False)
    if err < 0:
        print("FindMisorientations ErrorCondition %d" % err)

    # Find Schmid Factors
    err = orientation_analysis.find_schmids(dca, simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                            simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"),
                                            simpl.DataArrayPath("Small IN100", "Grain Data", "AvgQuats"),
                                            "Schmids", "SlipSystems", "Poles", "", "", simpl.FloatVec3Type([1, 1, 1]),
                                            False, False, simpl.FloatVec3Type([1, 1, 1]), simpl.FloatVec3Type([1, 1, 1]))
    if err < 0:
        print("FindSchmids ErrorCondition %d" % err)

    # Find Feature Reference Misorientations
    err = orientation_analysis.find_feature_reference_misorientations(dca,
                                                                      simpl.DataArrayPath("Small IN100",
                                                                                          "EBSD Scan Data",
                                                                                          "FeatureIds"),
                                                                      simpl.DataArrayPath("Small IN100",
                                                                                          "EBSD Scan Data", "Phases"),
                                                                      simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                                          "CrystalStructures"),
                                                                      simpl.DataArrayPath("Small IN100",
                                                                                          "EBSD Scan Data", "Quats"),
                                                                      simpl.DataArrayPath("Small IN100", "Grain Data",
                                                                                          "AvgQuats"),
                                                                      simpl.DataArrayPath("", "", ""),
                                                                      "FeatureAvgMisorientations",
                                                                      "FeatureReferenceMisorientations",
                                                                      sc.ReferenceOrientation.AverageOrientation)
    if err < 0:
        print("FindFeatureReferenceMisorientations ErrorCondition %d" % err)

    # Find Kernel Average Misorientations
    err = orientation_analysis.find_kernel_avg_misorientations(dca,
                                                               simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                   "FeatureIds"),
                                                               simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                   "Phases"),
                                                               simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                                   "CrystalStructures"),
                                                               simpl.DataArrayPath("Small IN100", "EBSD Scan Data",
                                                                                   "Quats"),
                                                               "KernelAverageMisorientations",
                                                               simpl.IntVec3Type([1, 1, 1]))
    if err < 0:
        print("FindKernelAvgMisorientations ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/Statistics/SmallIN100_CrystalStats.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_cryst_stats()
