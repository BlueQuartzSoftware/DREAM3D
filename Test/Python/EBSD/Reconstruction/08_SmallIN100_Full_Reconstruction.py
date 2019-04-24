# Pipeline : (08) SmallIN100 Full Reconstruction (from EBSD Reconstruction)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientation_analysis
from dream3d import reconstructionpy as reconstruction
from dream3d import processingpy as processing
from dream3d import genericpy as generic
from dream3d import statisticspy as statistics


def small_in100_full_reconstruction():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read H5EBSD File
    err = orientation_analysis.read_h5_ebsd(dca, "Small IN100", "Phase Data", "EBSD Scan Data",
                                            sd.GetBuildDirectory() +
                                            "/Data/Output/Reconstruction/SmallIN100.h5ebsd",
                                            1, 117, True, sc.AngleRepresentation.Radians,
                                            simpl.StringSet({"Fit", "Image Quality", "EulerAngles",
                                                             "SEM Signal", "Confidence Index", "Phases"}))
    if err < 0:
        print("ReadH5Ebsd ErrorCondition %d" % err)

    # Threshold Objects
    # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
    selectedThresholds = simpl.ComparisonInputs()
    selectedThresholds.addInput("Small IN100", "EBSD Scan Data", "Confidence Index",
                                simpl.ComparisonOperators.GreaterThan, 0.1)
    selectedThresholds.addInput("Small IN100", "EBSD Scan Data", "Image Quality", simpl.ComparisonOperators.GreaterThan,
                                120)

    err = d3d.multi_threshold_objects(dca, "Mask", selectedThresholds)
    if err < 0:
        print("MultiThresholdObjects ErrorCondition: %d" % err)

    # Convert Orientation Representation
    err = orientation_analysis.convert_orientations(dca, 0, 2,
                                                    simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "EulerAngles"),
                                                    "Quats")
    if err < 0:
        print("ConvertOrientation ErrorCondition: %d" % err)

    # Align Sections (Misorientation)
    err = reconstruction.align_sections_misorientation(dca, 5, True,
                                                       simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Quats"),
                                                       simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                                       simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Mask"),
                                                       simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                           "CrystalStructures"))
    if err < 0:
        print("AlignSectionsMisorientation ErrorCondition: %d" % err)

    # Isolate Largest Feature (Identify Sample)
    err = processing.identify_sample(dca, False, simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Mask"))
    if err < 0:
        print("IsolateLargestFeature ErrorCondition: %d" % err)

    # Align Sections (Feature Centroid)
    err = reconstruction.align_sections_feature_centroid(dca,
                                                         0, True,
                                                         simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Mask"))
    if err < 0:
        print("AlignSectionsFeatureCentroid %d" % err)

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

    # Segment Features (Misorientation)
    err = reconstruction.ebsd_segment_features(dca, "Grain Data", 5, True,
                                               simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Mask"),
                                               simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                               simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"),
                                               simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Quats"),
                                               "FeatureIds", "Active")
    if err < 0:
        print("SegmentFeatures ErrorCondition %d" % err)

    # Find Feature Phases
    err = generic.find_feature_phases(dca, simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                      simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                      simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"))
    if err < 0:
        print("FindFeaturePhases ErrorCondition %d" % err)

    # Find Feature Average Orientations
    err = orientation_analysis.find_avg_orientations(dca,
                                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases"),
                                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Quats"),
                                                     simpl.DataArrayPath("Small IN100", "Phase Data",
                                                                         "CrystalStructures"),
                                                     simpl.DataArrayPath("Small IN100", "Grain Data", "AvgQuats"),
                                                     simpl.DataArrayPath("Small IN100", "Grain Data", "AvgEuler"))
    if err < 0:
        print("FindAvgOrientations ErrorCondition %d" % err)

    # Find Feature Neighbors #1
    err = statistics.find_neighbors(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                    "SharedSurfaceAreaList2", "NeighborList2",
                                    simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                    "", "NumNeighbors2", "", False, False)
    if err < 0:
        print("FindNeighbors #1 ErrorCondition %d" % err)

    # Merge Twins
    err = reconstruction.merge_twins(dca, "NewGrain Data", 3, 2,
                                     simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                     simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                     simpl.DataArrayPath("Small IN100", "Grain Data", "AvgQuats"),
                                     simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"),
                                     "ParentIds", "ParentIds", "Active",
                                     simpl.DataArrayPath("Small IN100", "Grain Data", "NeighborList2"),
                                     simpl.DataArrayPath("", "", ""),
                                     False)
    if err < 0:
        print("MergeTwins ErrorCondition %d" % err)

    # Find Feature Sizes
    err = statistics.find_sizes(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                "Volumes", "EquivalentDiameters", "NumElements", False)
    if err < 0:
        print("FindSizes ErrorCondition %d" % err)

    # Minimum Size
    err = processing.min_size(dca, 16, False, 0,
                              simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                              simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                              simpl.DataArrayPath("Small IN100", "Grain Data", "NumElements"))
    if err < 0:
        print("MinSize ErrorCondition %d" % err)

    # Find Feature Neighbors #2
    err = statistics.find_neighbors(dca, simpl.DataArrayPath("Small IN100", "Grain Data", ""),
                                    "SharedSurfaceAreaList", "NeighborList",
                                    simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                    "", "NumNeighbors", "", False, False)
    if err < 0:
        print("FindNeighbors #2 ErrorCondition %d" % err)

    # Minimum Number of Neighbors
    err = processing.min_neighbors(dca, 2, False, 0, simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds"),
                                   simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                   simpl.DataArrayPath("Small IN100", "Grain Data", "NumNeighbors"))
    if err < 0:
        print("MinNeighbors ErrorCondition %d" % err)

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
    small_in100_full_reconstruction()
