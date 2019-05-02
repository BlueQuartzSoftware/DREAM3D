# Based on INL_Export example
# 
# These are the simpl_py python modules

from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import samplingpy
from dream3d import processingpy
from dream3d import reconstructionpy
from dream3d import genericpy
from dream3d import statisticspy
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy


def inl_export():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # ReadAngData
    err = orientationanalysispy.read_ang_data(dca, "Small IN100 Slice 1", "Phase Data", "EBSD Scan Data",
                                              sd.GetDataDirectory() + "/Data/SmallIN100/Slice_1.ang")
    if err < 0:
        print("ReadAngData ErrorCondition: %d" % err)

    # Rotate Sample Reference Frame
    err = samplingpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", ""),
                                             simpl.FloatVec3Type([0.0, 1.0, 0.0]), 180.0, False)
    if err < 0:
        print("RotateSampleRefFrame ErrorCondition: %d" % err)

    # Rotate Euler Reference Frame
    err = orientationanalysispy.rotate_euler_ref_frame(dca, simpl.FloatVec3Type([0.0, 0.0, 1.0]), 90.0,
                                                       simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                           "EulerAngles"))
    if err < 0:
        print("RotateEulerRefFrame ErrorCondition: %d" % err)

    # MultiThresholdObjects filter
    err = sc.MultiThresholdObjects(dca, "Mask",
                                   [("Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index", 1, 0.1),
                                    ("Small IN100 Slice 1", "EBSD Scan Data", "Image Quality", 1, 120)])
    if err < 0:
        print("MultiThresholdObjects ErrorCondition: %d" % err)

    # Convert Orientation Representation
    err = orientationanalysispy.convert_orientations(dca, 0, 2,
                                                     simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                         "EulerAngles"),
                                                     "Quats")
    if err < 0:
        print("ConvertOrientation ErrorCondition: %d" % err)

    # Isolate Largest Feature (Identify Sample)
    err = processingpy.identify_sample(dca, False,
                                       simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Mask"))
    if err < 0:
        print("IdentifySample ErrorCondition: %d" % err)

    # Neighbor Orientation Comparison (Bad Data)
    err = orientationanalysispy.bad_data_neighbor_orientation_check(dca, 5, 4,
                                                                    simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                        "EBSD Scan Data",
                                                                                        "Mask"),
                                                                    simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                        "EBSD Scan Data",
                                                                                        "Phases"),
                                                                    simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                        "Phase Data",
                                                                                        "CrystalStructures"),
                                                                    simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                        "EBSD Scan Data",
                                                                                        "Quats"))
    if err < 0:
        print("NeighborOrientationComparison ErrorCondition %d" % err)

    # Neighbor Orientation Correlation
    err = orientationanalysispy.neighbor_orientation_correlation(dca, 5, 0.2, 2,
                                                                 simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                     "EBSD Scan Data",
                                                                                     "Confidence Index"),
                                                                 simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                     "EBSD Scan Data",
                                                                                     "Phases"),
                                                                 simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                     "Phase Data",
                                                                                     "CrystalStructures"),
                                                                 simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                     "EBSD Scan Data",
                                                                                     "Quats"))
    if err < 0:
        print("NeighborOrientationCorrelation ErrorCondition %d" % err)

    # Segment Features (Misorientation)
    err = reconstructionpy.ebsd_segment_features(dca, "Grain Data", 5, True,
                                                 simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Mask"),
                                                 simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Phases"),
                                                 simpl.DataArrayPath("Small IN100 Slice 1", "Phase Data",
                                                                     "CrystalStructures"),
                                                 simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Quats"),
                                                 "FeatureIds", "Active")
    if err < 0:
        print("SegmentFeatures ErrorCondition %d" % err)

    # Find Feature Phases
    err = genericpy.find_feature_phases(dca, simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "FeatureIds"),
                                        simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Phases"),
                                        simpl.DataArrayPath("Small IN100 Slice 1", "Grain Data", "Phases"))
    if err < 0:
        print("FindFeaturePhases ErrorCondition %d" % err)

    # Find Number of Features
    err = statisticspy.find_num_features(dca,
                                         simpl.DataArrayPath("Small IN100 Slice 1", "Grain Data", "Phases"),
                                         simpl.DataArrayPath("Small IN100 Slice 1", "Phase Data", "NumFeatures"))
    if err < 0:
        print("FindNumFeatures ErrorCondition %d" % err)
    
    # Export INL File
    err = orientationanalysispy.inl_writer(dca, sd.GetBuildDirectory() +
                                           "/Data/Output/INL_Example/Small_IN100.inl",
                                           simpl.DataArrayPath("Small IN100 Slice 1", "Phase Data", "MaterialName"),
                                           simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "FeatureIds"),
                                           simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Phases"),
                                           simpl.DataArrayPath("Small IN100 Slice 1", "Phase Data",
                                                               "CrystalStructures"),
                                           simpl.DataArrayPath("Small IN100 Slice 1", "Phase Data", "NumFeatures"),
                                           simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "EulerAngles"))
    if err < 0:
        print("INL Writer ErrorCondition %d" % err)

    # Generate IPF Colors
    err = orientationanalysispy.generate_ipf_colors(dca, simpl.FloatVec3Type([0, 0, 1]),
                                                    simpl.DataArrayPath("Small IN100 Slice 1",
                                                                        "EBSD Scan Data", "Phases"),
                                                    simpl.DataArrayPath("Small IN100 Slice 1",
                                                                        "EBSD Scan Data", "EulerAngles"),
                                                    simpl.DataArrayPath("Small IN100 Slice 1",
                                                                        "Phase Data", "CrystalStructures"),
                                                    True,
                                                    simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                        "Mask"),
                                                    "IPFColor")
    if err < 0:
        print("GenerateIPFColors ErrorCondition: %d" % err)

    # ITK Image Writer
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                "/Data/Output/INL_Example/Small_IN100_Slice_1.png",
                                                simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                    "IPFColor"), 0)
    if err < 0:
        print("ITKImageWriter ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    inl_export()
