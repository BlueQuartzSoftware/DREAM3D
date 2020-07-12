# Pipeline : (06) SmallIN100 Postsegmentation Processing (from EBSD Reconstruction)
#
#

import simpl
import simplpy as d3d
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy as orientation_analysis
import reconstructionpy as reconstruction
import processingpy as processing
import genericpy as generic
import statisticspy as statistics

def small_in100_postseg_processing():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Reconstruction/05_SmallIN100_Segmentation.dream3d')
    assert err == 0, f'Read DataContainerArray Structure Failed {err}'

    # Find Feature Phases
    err = generic.find_feature_phases(dca, simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                      simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Phases'),
                                      simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'))
    assert err == 0, f'FindFeaturePhases ErrorCondition {err}'

    # Find Feature Average Orientations
    err = orientation_analysis.find_avg_orientations(dca,
                                                     simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                                     simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Phases'),
                                                     simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Quats'),
                                                     simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                         'CrystalStructures'),
                                                     simpl.DataArrayPath('Small IN100', 'Grain Data', 'AvgQuats'),
                                                     simpl.DataArrayPath('Small IN100', 'Grain Data', 'AvgEuler'))
    assert err == 0, f'FindAvgOrientations ErrorCondition {err}'

    # Find Feature Neighbors #1
    err = statistics.find_neighbors(dca, simpl.DataArrayPath('Small IN100', 'Grain Data', ''),
                                    'SharedSurfaceAreaList2', 'NeighborList2',
                                    simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                    '', 'NumNeighbors2', '', False, False)
    assert err == 0, f'FindNeighbors #1 ErrorCondition {err}'

    # Merge Twins
    err = reconstruction.merge_twins(dca, 'NewGrain Data', 3, 2,
                                     simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                     simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'),
                                     simpl.DataArrayPath('Small IN100', 'Grain Data', 'AvgQuats'),
                                     simpl.DataArrayPath('Small IN100', 'Phase Data', 'CrystalStructures'),
                                     'ParentIds', 'ParentIds', 'Active',
                                     simpl.DataArrayPath('Small IN100', 'Grain Data', 'NeighborList2'),
                                     simpl.DataArrayPath('', '', ''),
                                     False)
    assert err == 0, f'MergeTwins ErrorCondition {err}'

    # Find Feature Sizes
    err = statistics.find_sizes(dca, simpl.DataArrayPath('Small IN100', 'Grain Data', ''),
                                simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                'Volumes', 'EquivalentDiameters', 'NumElements', False)
    assert err == 0, f'FindSizes ErrorCondition {err}'

    # Minimum Size
    err = processing.min_size(dca, 16, False, 0,
                              simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                              simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'),
                              simpl.DataArrayPath('Small IN100', 'Grain Data', 'NumElements'))
    assert err == 0, f'MinSize ErrorCondition {err}'

    # Find Feature Neighbors #12
    err = statistics.find_neighbors(dca, simpl.DataArrayPath('Small IN100', 'Grain Data', ''),
                                    'SharedSurfaceAreaList', 'NeighborList',
                                    simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                    '', 'NumNeighbors', '', False, False)
    assert err == 0, f'FindNeighbors #2 ErrorCondition {err}'

    # Minimum Number of Neighbors
    err = processing.min_neighbors(dca, 2, False, 0, simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                   simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'),
                                   simpl.DataArrayPath('Small IN100', 'Grain Data', 'NumNeighbors'))
    assert err == 0, f'MinNeighbors ErrorCondition {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + '/Data/Output/Reconstruction/06_SmallIN100_Postsegmentation.dream3d',
        dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_postseg_processing()
