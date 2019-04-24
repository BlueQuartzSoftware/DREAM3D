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
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Confidence Index").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "EulerAngles").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "FeatureIds").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Fit").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "GBManhattanDistances").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "IPFColor").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Image Quality").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Mask").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "ParentIds").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Phases").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "QPManhattanDistances").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Quats").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "SEM Signal").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "TJManhattanDistances").Flag =2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "CrystalStructures").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "LatticeConstants").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Phase Data").getDataArrayProxy(
        "MaterialName").Flag =2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Active").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AspectRatios").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AvgEuler").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AvgQuats").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AxisEulerAngles").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AxisLengths").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Centroids").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "EquivalentDiameters").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NeighborList").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NeighborhoodList").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Neighborhoods").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NumElements").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NumNeighbors").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "NumNeighbors2").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Omega3s").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "ParentIds").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Phases").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Shape Volumes").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SharedSurfaceAreaList").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Size Volumes").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Sphericity").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SurfaceAreaVolumeRatio").Flag =2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("NewGrain Data").getDataArrayProxy(
        "Active").Flag =2

    err = d3d.data_container_reader(dca,
                                    sd.GetBuildDirectory() + "/Data/Output/Statistics/SmallIN100_TransferFieldData.dream3d",
                                    False, dcap)
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
