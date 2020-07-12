'''
Test of Single Cubic Phase Equiaxed Pipeline example in Python
'''

import simpl
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy as orientationanalysis
import statisticspy as statistics
import syntheticbuildingpy as syntheticbuilding

def single_cubic_phase_rolled():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Stats Generator Filter
    err = syntheticbuilding.generate_primary_stats_data(dca, 'Primary', 0, 1, 1,
                               1, 2.3, 0.4, 4, 3, 10.5,
                               True, 'StatsGeneratorDataContainer',
                               'CellEnsembleData', False,
                               simpl.DataArrayPath('', '', ''),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ['Euler 1', 'Euler 2',
                                                                                              'Euler 3', 'Weight',
                                                                                              'Sigma'], ['0']),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ['Angle(w)', 'Axis (h)',
                                                                                              'Axis (k)', 'Axis (l)',
                                                                                              'Weight (MRD)'], ['0']),
                               simpl.DynamicTableData([simpl.VectorDouble([0, 0, 0, 0, 0])], ['Euler 1', 'Euler 2',
                                                                                              'Euler 3', 'Weight',
                                                                                              'Sigma'], ['0']))

    assert err == 0, f'StatsGeneratorFilter ErrorCondition: {err}'

    # Initialize Synthetic Volume
    err = syntheticbuilding.initialize_synthetic_volume(dca, 'SyntheticVolumeDataContainer', 'CellData',
                                                        'CellEnsembleMatrixName', 6,
                                                        simpl.IntVec3([128, 128, 128]),
                                                        simpl.FloatVec3([0.5, 0.5, 0.5]),
                                                        simpl.FloatVec3([0, 0, 0]),
                                                        simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                            'CellEnsembleData', 'Statistics'),
                                                        simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                            'CellEnsembleData', 'PhaseTypes'),
                                                        simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                            'CellEnsembleData', 'PhaseName'),
                                                        False, 0, 'NOT NEEDED')
    assert err == 0, f'InitializeSyntheticVolume ErrorCondition: {err}'

    # Establish Shape Types
    err = syntheticbuilding.establish_shape_types(dca,
                                                  simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                      'CellEnsembleData', 'PhaseTypes'),
                                                  'ShapeTypes', [simpl.ShapeType.Ellipsoid])
    assert err == 0, f'EstablishShapeTypes ErrorCondition: {err}'

    # Pack Primary Phases
    err = syntheticbuilding.pack_primary_phases(dca,
                                                simpl.DataArrayPath('SyntheticVolumeDataContainer', 'CellData',
                                                                    ''),
                                                'Grain Data', 'CellEnsembleData', 'FeatureIds', 'Phases',
                                                'Phases', 'NumFeatures',
                                                simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                    'CellEnsembleData', 'Statistics'),
                                                simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                    'CellEnsembleData', 'PhaseTypes'),
                                                simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                    'CellEnsembleData', 'PhaseName'),
                                                simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                    'CellEnsembleData', 'ShapeTypes'),
                                                simpl.DataArrayPath('', '', ''), False, 0, '', '', False,
                                                False, False,
                                                simpl.DataArrayPath('', '', ''),
                                                simpl.DataArrayPath('', '', ''))
    assert err == 0, f'PackPrimaryPhases ErrorCondition: {err}'

    # Find Feature Neighbors
    err = statistics.find_neighbors(dca, simpl.DataArrayPath('SyntheticVolumeDataContainer', 'Grain Data', ''),
                                    'SharedSurfaceAreaList', 'NeighborList',
                                    simpl.DataArrayPath('SyntheticVolumeDataContainer', 'CellData',
                                                        'FeatureIds'),
                                    '', 'NumNeighbors', 'SurfaceFeatures', False, True)
    assert err == 0, f'FindNeighbors ErrorCondition: {err}'

    # Match Crystallography
    err = syntheticbuilding.match_crystallography(dca, simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                           'CellEnsembleData', 'Statistics'),
                                                  simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                      'CellEnsembleData', 'CrystalStructures'),
                                                  simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                      'CellEnsembleData', 'PhaseTypes'),
                                                  simpl.DataArrayPath('SyntheticVolumeDataContainer',
                                                                      'CellData', 'FeatureIds'),
                                                  simpl.DataArrayPath('SyntheticVolumeDataContainer',
                                                                      'Grain Data', 'Phases'),
                                                  simpl.DataArrayPath('SyntheticVolumeDataContainer',
                                                                      'Grain Data', 'SurfaceFeatures'),
                                                  simpl.DataArrayPath('SyntheticVolumeDataContainer',
                                                                      'Grain Data', 'NeighborList'),
                                                  simpl.DataArrayPath('SyntheticVolumeDataContainer',
                                                                      'Grain Data', 'SharedSurfaceAreaList'),
                                                  simpl.DataArrayPath('SyntheticVolumeDataContainer',
                                                                      'CellEnsembleData', 'NumFeatures'),
                                                  'EulerAngles', 'Volumes', 'EulerAngles', 'AvgQuats', 100000)
    assert err == 0, f'MatchCrystallography ErrorCondition: {err}'

    # Generate IPF Colors
    err = orientationanalysis.generate_ipf_colors(dca, simpl.FloatVec3([0, 0, 1]),
                                                  simpl.DataArrayPath('SyntheticVolumeDataContainer',
                                                                      'CellData', 'Phases'),
                                                  simpl.DataArrayPath('SyntheticVolumeDataContainer',
                                                                      'CellData', 'EulerAngles'),
                                                  simpl.DataArrayPath('StatsGeneratorDataContainer',
                                                                      'CellEnsembleData', 'CrystalStructures'),
                                                  False,
                                                  simpl.DataArrayPath('', '', ''), 'IPFColor')
    assert err == 0, f'GenerateIPFColors ErrorCondition: {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/Synthetic/03_CubicSingleRolledOut.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    single_cubic_phase_rolled()
