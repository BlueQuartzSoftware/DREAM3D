"""
Test of Two Phase Cubic Hexagonal Particles Equiaxed Pipeline example in Python

"""

from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientationanalysis
from dream3d import genericpy as generic
from dream3d import statisticspy as statistics
from dream3d import syntheticbuildingpy as syntheticbuilding


def two_phase_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Generate Primary StatsData
    err = syntheticbuilding.generate_primary_stats_data(dca, "Primary", 0, 1, 0,
                               0.9, 3, 0.25, 4, 3, 10,
                               True, "StatsGeneratorDataContainer",
                               "CellEnsembleData", False,
                               simpl.DataArrayPath("", "", ""),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ["Euler 1", "Euler 2",
                                                                                              "Euler 3", "Weight",
                                                                                              "Sigma"], ["0"]),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ["Angle(w)", "Axis (h)",
                                                                                              "Axis (k)", "Axis (l)",
                                                                                              "Weight (MRD)"], ["0"]),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ["Euler 1", "Euler 2",
                                                                                              "Euler 3", "Weight",
                                                                                              "Sigma"], ["0"]))

    if err < 0:
        print("Primary StatsData ErrorCondition: %d" % err)

    # Generate Precipitate StatsData
    err = syntheticbuilding.generate_precipitate_stats_data(dca, "Precipitate", 1, 0, 0,
                               0.1, 1.6, 0.2, 4, 3, 2,
                               False, "StatsGeneratorDataContainer",
                               "CellEnsembleData", True,
                               simpl.DataArrayPath("StatsGeneratorDataContainer", "CellEnsembleData", ""),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ["Euler 1", "Euler 2",
                                                                                              "Euler 3", "Weight",
                                                                                              "Sigma"], ["0"]),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ["Angle(w)", "Axis (h)",
                                                                                              "Axis (k)", "Axis (l)",
                                                                                              "Weight (MRD)"], ["0"]),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ["Euler 1", "Euler 2",
                                                                                              "Euler 3", "Weight",
                                                                                              "Sigma"], ["0"]),
                               simpl.FloatVec2Type([10, 80]), 50, simpl.FloatVec3Type([100, 100, 100]))

    if err < 0:
        print("Precipitate StatsData ErrorCondition: %d" % err)

    # Initialize Synthetic Volume
    err = syntheticbuilding.initialize_synthetic_volume(dca, "SyntheticVolumeDataContainer", "CellData",
                                                        "CellEnsembleMatrixName", 6,
                                                        simpl.IntVec3Type([128, 128, 128]),
                                                        simpl.FloatVec3Type([0.5, 0.5, 0.5]),
                                                        simpl.FloatVec3Type([0, 0, 0]),
                                                        simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                            "CellEnsembleData", "Statistics"),
                                                        simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                            "CellEnsembleData", "PhaseTypes"),
                                                        simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                            "CellEnsembleData", "PhaseName"),
                                                        False)
    if err < 0:
        print("InitializeSyntheticVolume ErrorCondition: %d" % err)

    # Establish Shape Types
    err = syntheticbuilding.establish_shape_types(dca,
                                                  simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                      "CellEnsembleData", "PhaseTypes"),
                                                  "ShapeTypes", [simpl.ShapeType.Ellipsoid])
    if err < 0:
        print("EstablishShapeTypes ErrorCondition: %d" % err)

    # Pack Primary Phases
    err = syntheticbuilding.pack_primary_phases(dca,
                                                simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData", ""),
                                                "Grain Data", "CellEnsembleData", "FeatureIds", "Phases",
                                                "Phases", "NumFeatures",
                                                simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                    "CellEnsembleData", "Statistics"),
                                                simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                    "CellEnsembleData", "PhaseTypes"),
                                                simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                    "CellEnsembleData", "PhaseName"),
                                                simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                    "CellEnsembleData", "ShapeTypes"),
                                                simpl.DataArrayPath("", "", ""), False, 0, "", "", False,
                                                False, False,
                                                simpl.DataArrayPath("", "", ""),
                                                simpl.DataArrayPath("", "", ""))
    if err < 0:
        print("PackPrimaryPhases ErrorCondition: %d" % err)

    # Find Boundary Cells (Image)
    err = generic.find_boundary_cells(dca,
                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData", "FeatureIds"),
                                      "BoundaryCells",
                                      True, False)
    if err < 0:
        print("FindBoundaryCells ErrorCondition: %d" % err)

    # Insert Precipitate Phases
    err = syntheticbuilding.insert_precipitate_phases(dca, "", simpl.DataArrayPath("", "", ""), False, 0, "",
                                                      False, False, 0,
                                                      simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                          "CellEnsembleData", "Statistics"),
                                                      simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                          "CellEnsembleData", "PhaseTypes"),
                                                      simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                          "CellEnsembleData", "ShapeTypes"),
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                                          "FeatureIds"),
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                                          "Phases"),
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                                          "BoundaryCells"),
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer", "Grain Data",
                                                                          "Phases"),
                                                      "NumCells", "EquivalentDiameters", "Volumes", "Omega3s",
                                                      "Centroids", "AxisEulerAngles", "AxisLengths",
                                                      simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                          "CellEnsembleData", "NumFeatures"),
                                                      False, simpl.DataArrayPath("", "", ""),
                                                      simpl.DataArrayPath("", "", ""))
    if err < 0:
        print("InsertPrecipitatePhases ErrorCondition: %d" % err)

    # Find Feature Neighbors
    err = statistics.find_neighbors(dca, simpl.DataArrayPath("SyntheticVolumeDataContainer", "Grain Data", ""),
                                    "SharedSurfaceAreaList", "NeighborList",
                                    simpl.DataArrayPath("SyntheticVolumeDataContainer", "CellData",
                                                        "FeatureIds"),
                                    "", "NumNeighbors", "SurfaceFeatures", False, True)
    if err < 0:
        print("FindNeighbors ErrorCondition: %d" % err)

    # Match Crystallography
    err = syntheticbuilding.match_crystallography(dca, simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                           "CellEnsembleData", "Statistics"),
                                                  simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                      "CellEnsembleData", "CrystalStructures"),
                                                  simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                      "CellEnsembleData", "PhaseTypes"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "CellData", "FeatureIds"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "Grain Data", "Phases"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "Grain Data", "SurfaceFeatures"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "Grain Data", "NeighborList"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "Grain Data", "SharedSurfaceAreaList"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "CellEnsembleData", "NumFeatures"),
                                                  "EulerAngles", "Volumes", "EulerAngles", "AvgQuats", 100000)
    if err < 0:
        print("MatchCrystallography ErrorCondition: %d" % err)

    # Generate IPF Colors
    err = orientationanalysis.generate_ipf_colors(dca, simpl.FloatVec3Type([0, 0, 1]),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "CellData", "Phases"),
                                                  simpl.DataArrayPath("SyntheticVolumeDataContainer",
                                                                      "CellData", "EulerAngles"),
                                                  simpl.DataArrayPath("StatsGeneratorDataContainer",
                                                                      "CellEnsembleData", "CrystalStructures"),
                                                  False,
                                                  simpl.DataArrayPath("", "", ""), "IPFColor")
    if err < 0:
        print("GenerateIPFColors ErrorCondition: %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + "/Data/Output/Synthetic/04_TwoPhaseCubicHexagonalEquiaxed.dream3d",
        dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    two_phase_test()
