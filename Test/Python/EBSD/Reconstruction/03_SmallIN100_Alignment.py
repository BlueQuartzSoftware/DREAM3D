# Pipeline : (03) SmallIN100 Alignment (from EBSD Reconstruction)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientation_analysis
from dream3d import reconstructionpy as reconstruction
from dream3d import processingpy as processing
from dream3d import orientationanalysis


def small_in100_alignment():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + "/Data/Output/Reconstruction/SmallIN100.dream3d")
    if err < 0:
            print("Read DataContainerArray Structure Failed %d" % err)

    # Threshold Objects
    # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
    selected_thresholds = simpl.ComparisonInputs()
    selected_thresholds.addInput("Small IN100", "EBSD Scan Data", "Confidence Index",
                                simpl.ComparisonOperators.GreaterThan, 0.1)
    selected_thresholds.addInput("Small IN100", "EBSD Scan Data", "Image Quality", simpl.ComparisonOperators.GreaterThan,
                                120)

    err = d3d.multi_threshold_objects(dca, "Mask", selected_thresholds)
    if err < 0:
        print("MultiThresholdObjects ErrorCondition: %d" % err)

    # Convert Orientation Representation
    err = orientation_analysis.convert_orientations(dca, orientationanalysis.OrientationType.Euler, orientationanalysis.OrientationType.Quaternion,
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
        sd.GetBuildDirectory() + "/Data/Output/Reconstruction/03_SmallIN100_Aligned.dream3d",
        dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_alignment()
