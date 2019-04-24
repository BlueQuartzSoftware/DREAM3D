# Pipeline : (04) SmallIN100 GBCD (from EBSD Surface Meshing)
#
#

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientationanalysis
from dream3d import importexportpy

import datetime as datetime


def small_in100_gbcd():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Read DREAM3D File
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Confidence Index").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "CriticalField").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "EulerAngles").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "FeatureIds").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "FeatureReferenceMisorientations").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Fit").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "GBManhattanDistances").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "IPFColor").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "Image Quality").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("EBSD Scan Data").getDataArrayProxy(
        "KernelAverageMisorientations").Flag =2
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
        "AvgEulerAngles").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AvgQuats").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AxisEulerAngles").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AxisLengths").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Centroids").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "CriticalFields").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "EquivalentDiameters").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "FeatureAvgMisorientations").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "MisorientationList").Flag =2
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
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Poles").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Phases").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Schmids").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Shape Volumes").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SharedSurfaceAreaList").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Size Volumes").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SlipSystems").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Sphericity").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SurfaceAreaVolumeRatio").Flag =2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("NewGrain Data").getDataArrayProxy(
        "Active").Flag =2

    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "FaceAreas").Flag =2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "FaceDihedralAngles").Flag =2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "FaceLabels").Flag =2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "FaceNormals").Flag =2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "SurfaceMeshFaceIPFColors").Flag =2
    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceData").getDataArrayProxy(
        "SurfaceMeshMisorientationColors").Flag =2

    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("FaceFeatureData").Flag =2

    dcap.getDataContainerProxy("TriangleDataContainer").getAttributeMatrixProxy("VertexData").getDataArrayProxy(
        "NodeType").Flag =2

    err = d3d.data_container_reader(dca,
                                    sd.GetBuildDirectory() +
                                    "/Data/Output/SurfaceMesh/SmallIN100_MeshStats.dream3d",
                                    False, dcap)
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Find GBCD
    err = orientationanalysis.find_gbcd(dca, "FaceEnsembleData", 9,
                                        simpl.DataArrayPath("TriangleDataContainer", "FaceData", "FaceLabels"),
                                        simpl.DataArrayPath("TriangleDataContainer", "FaceData", "FaceNormals"),
                                        simpl.DataArrayPath("TriangleDataContainer", "FaceData", "FaceAreas"),
                                        simpl.DataArrayPath("Small IN100", "Grain Data", "AvgEulerAngles"),
                                        simpl.DataArrayPath("Small IN100", "Grain Data", "Phases"),
                                        simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"),
                                        "GBCD")
    if err < 0:
        print("FindGBCD ErrorCondition %d" % err)

    # Export GBCD Pole Figure (GMT 5)
    err = importexportpy.visualize_gbcdgmt(dca, sd.GetBuildDirectory() + "/Data/Output/SmallIN100GBCD/SmallIn100GMT_1.dat",
                                 1, simpl.AxisAngleInput(60, 1, 1, 1),
                                 simpl.DataArrayPath("TriangleDataContainer", "FaceEnsembleData", "GBCD"),
                                 simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"))
    if err < 0:
        print("VisualizeGBCDGMT ErrorCondition %d" % err)

    # Export GBCD Pole Figure (VTK)
    err = importexportpy.visualize_gbcd_pole_figure(dca,
                                          sd.GetBuildDirectory() + "/Data/Output/SmallIN100GBCD/SmallIn100VTK.vtk",
                                          1, simpl.AxisAngleInput(60, 1, 1, 1),
                                          simpl.DataArrayPath("TriangleDataContainer", "FaceEnsembleData", "GBCD"),
                                          simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures"))
    if err < 0:
        print("VisualizeGBCDPoleFigure ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/SurfaceMesh/SmallIN100_GBCD.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_gbcd()
