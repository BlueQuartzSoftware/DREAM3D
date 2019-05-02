"""
(06) Small IN100 Synthetic - Pipeline example in Python

"""

from dream3d import simplpy as d3d
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientationanalysis
from dream3d import genericpy as generic
from dream3d import statisticspy as statistics
from dream3d import syntheticbuildingpy as syntheticbuilding

# Numpy for some data management assistance
# import numpy as np

import datetime as datetime


def small_in100_test():
    # Data Array Paths used in this script
    feature_ids_array_path = simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "FeatureIds")
    centroids_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "Centroids")
    empty_array_path = simpl.DataArrayPath("", "", "")
    grain_data_attr_matrix = simpl.DataArrayPath("Small IN100", "Grain Data", "")
    equivalent_diameters_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "EquivalentDiameters")
    feature_phases_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "Phases")
    cell_phases_array_path = simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Phases")
    quats_array_path = simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Quats")
    crystal_structures_array_path = simpl.DataArrayPath("Small IN100", "Phase Data", "CrystalStructures")
    avg_quats_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "AvgQuats")
    avg_euler_angles_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "AvgEulerAngles")
    cell_ensemble_attribute_matrix_path = simpl.DataArrayPath("Small IN100", "Phase Data", "")
    neighbor_list_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "NeighborList")
    shared_surface_area_list_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "SharedSurfaceAreaList")
    biased_features_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "BiasedFeatures")
    neighborhoods_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "Neighborhoods")
    aspect_ratios_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "AspectRatios")
    omega3s_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "Omega3s")
    axis_euler_angles_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "AxisEulerAngles")
    surface_features_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "SurfaceFeatures")
    volumes_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "SizeVolumes")
    rdf_array_path = empty_array_path
    max_min_rdf_array_path = empty_array_path
    feature_euler_angles_array_path = simpl.DataArrayPath("Small IN100", "Grain Data", "AvgEulerAngles")

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

    # Several possible input data items are not flagged, the ones commented out
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Active").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "AvgEuler").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "CriticalFields").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "FeatureAvgMisorientations").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "MisorientationList").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "ParentIds").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Phases").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy("Poles").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Schmids").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SlipSystems").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "Sphericity").Flag =2
    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("Grain Data").getDataArrayProxy(
        "SurfaceAreaVolumeRatio").Flag =2

    dcap.getDataContainerProxy("Small IN100").getAttributeMatrixProxy("NewGrain Data").getDataArrayProxy(
        "Active").Flag =2

    dcap_ = sc.CreateDataContainerProxy(dca, [["Small IN100", "EBSD Scan Data", "Confidence Index"],
                                              ["Small IN100", "EBSD Scan Data", "CriticalField"],
                                              ["Small IN100", "EBSD Scan Data", "EulerAngles"],
                                              ["Small IN100", "EBSD Scan Data", "FeatureIds"],
                                              ["Small IN100", "EBSD Scan Data", "FeatureReferenceMisorientations"],
                                              ["Small IN100", "EBSD Scan Data", "Fit"],
                                              ["Small IN100", "EBSD Scan Data", "GBManhattanDistances"],
                                              ["Small IN100", "EBSD Scan Data", "IPFColor"],
                                              ["Small IN100", "EBSD Scan Data", "Image Quality"],
                                              ["Small IN100", "EBSD Scan Data", "KernelAverageMisorientations"],
                                              ["Small IN100", "EBSD Scan Data", "Mask"],
                                              ["Small IN100", "EBSD Scan Data", "ParentIds"],
                                              ["Small IN100", "EBSD Scan Data", "Phases"],
                                              ["Small IN100", "EBSD Scan Data", "QPManhattanDistances"],
                                              ["Small IN100", "EBSD Scan Data", "Quats"],
                                              ["Small IN100", "EBSD Scan Data", "SEM Signal"],
                                              ["Small IN100", "EBSD Scan Data", "TJManhattanDistances"],
                                              ["Small IN100", "Phase Data", "CrystalStructures"],
                                              ["Small IN100", "Phase Data", "LatticeConstants"],
                                              ["Small IN100", "Phase Data", "MaterialName"],
                                              ["Small IN100", "Grain Data", "Active"],
                                              ["Small IN100", "Grain Data", "AvgEuler"],
                                              ["Small IN100", "Grain Data", "CriticalFields"],
                                              ["Small IN100", "Grain Data", "FeatureAvgMisorientations"],
                                              ["Small IN100", "Grain Data", "MisorientationList"],
                                              ["Small IN100", "Grain Data", "ParentIds"],
                                              ["Small IN100", "Grain Data", "Phases"],
                                              ["Small IN100", "Grain Data", "Poles"],
                                              ["Small IN100", "Grain Data", "Schmids"],
                                              ["Small IN100", "Grain Data", "SlipSystems"],
                                              ["Small IN100", "Grain Data", "Sphericity"],
                                              ["Small IN100", "Grain Data", "SurfaceAreaVolumeRatio"]])
    now_time = datetime.datetime.now()
    now_time_seconds = now_time.hour * 3600 + now_time.minute * 60 + now_time.second
    dt = simpl.DateTime(now_time.year, now_time.month, now_time.day, now_time_seconds)
    err = d3d.data_container_reader(dca,
                                    sd.GetBuildDirectory() +
                                    "/Data/Output/Statistics/SmallIN100_CrystalStats.dream3d",
                                    False, dcap_)
    if err < 0:
        print("DataContainerReader ErrorCondition %d" % err)

    # Find Feature Neighbors
    err = statistics.find_neighbors(dca, grain_data_attr_matrix,
                                    "SharedSurfaceAreaList", "NeighborList", feature_ids_array_path,
                                    "", "NumNeighbors", "SurfaceFeatures", False, False)
    if err < 0:
        print("FindNeighbors #1 ErrorCondition: %d" % err)

    # Find Feature Centroids
    err = generic.find_feature_centroids(dca, feature_ids_array_path, centroids_array_path)
    if err < 0:
        print("FindFeatureCentroids ErrorCondition: %d" % err)

    # Find Surface Features
    err = generic.find_surface_features(dca, feature_ids_array_path, surface_features_array_path)
    if err < 0:
        print("FindSurfaceFeatures ErrorCondition: %d" % err)

    # Find Biased Features (Bounding Box)
    err = generic.find_bounding_box_features(dca, False, centroids_array_path, empty_array_path,
                                             surface_features_array_path, "BiasedFeatures")
    if err < 0:
        print("FindBiasedFeatures ErrorCondition: %d" % err)

    # Find Feature Sizes
    err = statistics.find_sizes(dca, grain_data_attr_matrix, feature_ids_array_path, "SizeVolumes",
                                "EquivalentDiameters", "NumElements", False)
    if err < 0:
        print("FindSizes ErrorCondition: %d" % err)

    # Find Feature Shapes
    err = statistics.find_shapes(dca, grain_data_attr_matrix, feature_ids_array_path, centroids_array_path,
                                 "Omega3s", "ShapeVolumes", "AxisLengths", "AxisEulerAngles", "AspectRatios")
    if err < 0:
        print("FindShapes ErrorCondition: %d" % err)

    # Find Feature Neighborhoods
    err = statistics.find_neighborhoods(dca, "NeighborhoodList", 1, equivalent_diameters_array_path,
                                        feature_phases_array_path,
                                        centroids_array_path, "Neighborhoods")
    if err < 0:
        print("FindNeighborhoods ErrorCondition: %d" % err)

    # Find Feature Average Orientations
    err = orientationanalysis.find_avg_orientations(dca, feature_ids_array_path, cell_phases_array_path,
                                                    quats_array_path, crystal_structures_array_path,
                                                    avg_quats_array_path, avg_euler_angles_array_path)
    if err < 0:
        print("FindAvgOrientations ErrorCondition: %d" % err)

    # Generate Ensemble Statistics
    err = statistics.generate_ensemble_statistics(dca, cell_ensemble_attribute_matrix_path,
                                                  "PhaseTypes", [simpl.PhaseType.Primary], [simpl.PhaseType.Primary],
                                                  neighbor_list_array_path, shared_surface_area_list_array_path,
                                                  feature_phases_array_path, biased_features_array_path,
                                                  equivalent_diameters_array_path, neighborhoods_array_path,
                                                  aspect_ratios_array_path, omega3s_array_path,
                                                  axis_euler_angles_array_path, crystal_structures_array_path,
                                                  surface_features_array_path, volumes_array_path,
                                                  rdf_array_path, max_min_rdf_array_path,
                                                  feature_euler_angles_array_path, avg_quats_array_path,
                                                  "Statistics", False,
                                                  True, sc.DistributionFitType.Lognormal,
                                                  True, sc.DistributionFitType.Beta,
                                                  True, sc.DistributionFitType.Beta,
                                                  True, sc.DistributionFitType.Lognormal,
                                                  True, False, False, False, 1)
    if err < 0:
        print("GenerateEnsembleStatistics ErrorCondition: %d" % err)

    # Initialize Synthetic Volume
    err = syntheticbuilding.initialize_synthetic_volume(dca, "SyntheticVolumeDataContainer", "EBSD Scan Data",
                                                        "Phase Data", 6,
                                                        simpl.IntVec3Type([128, 128, 128]),
                                                        simpl.FloatVec3Type([0.25, 0.25, 0.25]),
                                                        simpl.FloatVec3Type([0, 0, 0]),
                                                        simpl.DataArrayPath("Small IN100",
                                                                            "Phase Data", "Statistics"),
                                                        simpl.DataArrayPath("Small IN100",
                                                                            "Phase Data", "PhaseTypes"),
                                                        simpl.DataArrayPath("Small IN100",
                                                                            "Phase Data", "MaterialName"),
                                                        False)
    if err < 0:
        print("InitializeSyntheticVolume ErrorCondition: %d" % err)

    # Establish Shape Types
    err = syntheticbuilding.establish_shape_types(dca,
                                                  simpl.DataArrayPath("Small IN100",
                                                                      "Phase Data", "PhaseTypes"),
                                                  "ShapeTypes", [simpl.ShapeType.Ellipsoid])
    if err < 0:
        print("EstablishShapeTypes ErrorCondition: %d" % err)

    # Pack Primary Phases
    err = syntheticbuilding.pack_primary_phases(dca,
                                                simpl.DataArrayPath("SyntheticVolumeDataContainer", "EBSD Scan Data",
                                                                    ""),
                                                "Grain Data", "Phase Data", "FeatureIds", "Phases",
                                                "Phases", "NumFeatures",
                                                simpl.DataArrayPath("Small IN100",
                                                                    "Phase Data", "Statistics"),
                                                simpl.DataArrayPath("Small IN100",
                                                                    "Phase Data", "PhaseTypes"),
                                                simpl.DataArrayPath("Small IN100",
                                                                    "Phase Data", "MaterialName"),
                                                simpl.DataArrayPath("Small IN100",
                                                                    "Phase Data", "ShapeTypes"),
                                                simpl.DataArrayPath("", "", ""), False,
                                                sc.FeatureGeneration.GenerateFeatures,
                                                "", "", False, False, False,
                                                simpl.DataArrayPath("", "", ""),
                                                simpl.DataArrayPath("", "", ""))
    if err < 0:
        print("PackPrimaryPhases ErrorCondition: %d" % err)

    # Find Feature Neighbors
    err = statistics.find_neighbors(dca, simpl.DataArrayPath("SyntheticVolumeDataContainer", "Grain Data", ""),
                                    "SharedSurfaceAreaList", "NeighborList",
                                    simpl.DataArrayPath("SyntheticVolumeDataContainer", "EBSD Scan Data",
                                                        "FeatureIds"),
                                    "", "NumNeighbors", "SurfaceFeatures", False, True)
    if err < 0:
        print("FindNeighbors #2 ErrorCondition: %d" % err)

    # Match Crystallography
    err = syntheticbuilding.match_crystallography(dca, simpl.DataArrayPath("Small IN100",
                                                                           "Phase Data", "Statistics"),
                                                  simpl.DataArrayPath("Small IN100",
                                                                      "Phase Data", "CrystalStructures"),
                                                  simpl.DataArrayPath("Small IN100",
                                                                      "Phase Data", "PhaseTypes"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "EBSD Scan Data", "FeatureIds"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "Grain Data", "Phases"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "Grain Data", "SurfaceFeatures"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "Grain Data", "NeighborList"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "Grain Data", "SharedSurfaceAreaList"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "Phase Data", "NumFeatures"),
                                                  "EulerAngles", "Volumes", "EulerAngles", "AvgQuats", 100000)
    if err < 0:
        print("MatchCrystallography ErrorCondition: %d" % err)

    # Generate IPF Colors
    err = orientationanalysis.generate_ipf_colors(dca, simpl.FloatVec3Type([0, 0, 1]),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "EBSD Scan Data", "Phases"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "EBSD Scan Data", "EulerAngles"),
                                                  simpl.DataArrayPath("Small IN100",
                                                                      "Phase Data", "CrystalStructures"),
                                                  False,
                                                  simpl.DataArrayPath("", "", ""), "IPFColor")
    if err < 0:
        print("GenerateIPFColors ErrorCondition: %d" % err)

    # Set all the IPFColor values to zero
    # am = dca.getAttributeMatrix(simpl.DataArrayPath("Small IN100", "EBSD Scan Data", ""))
    # data_array = am.getAttributeArray("IPFColor")
    # data = data_array.Data
    # newdata = sc.ConvertToDataArray("IPFColor", np.zeros_like(data).astype('uint8'))
    # am.addAttributeArray("IPFColor", newdata[1])
    # print(type(dataArray.Data))
    # print("Max: %d" % max(dataArray.Data))
    # print("Min: %d" % min(dataArray.Data))

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/Synthetic/06_SmallIN100Synthetic.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    small_in100_test()
