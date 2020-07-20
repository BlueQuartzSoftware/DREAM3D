# Pipeline : (05) SmallIN100 Crystallographic Statistics (from EBSD Statistics)

import simpl
import simplpy as d3d
import simpl_helpers as sh
import simpl_test_dirs as sd
import orientationanalysispy as orientation_analysis

def small_in100_cryst_stats():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    err = sh.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Statistics/SmallIN100_CopyCritical.dream3d')
    
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # We need to remove an Attribute Array that will be recreated in a filter.
    dc = dca.getDataContainer('Small IN100')
    grainDataAM = dc.getAttributeMatrix('Grain Data')
    grainDataAM.removeAttributeArray('AvgQuats')
    
    
    # Find Feature Average Orientations
    err = orientation_analysis.find_avg_orientations(dca,
                                                     simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                                     simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Phases'),
                                                     simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Quats'),
                                                     simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                         'CrystalStructures'),
                                                     simpl.DataArrayPath('Small IN100', 'Grain Data', 'AvgQuats'),
                                                     simpl.DataArrayPath('Small IN100', 'Grain Data', 'AvgEulerAngles'))
    assert err == 0, f'FindAvgOrientations ErrorCondition {err}'

    # Find Feature Neighbor Misorientations
    err = orientation_analysis.find_misorientations(dca,
                                                    simpl.DataArrayPath('Small IN100', 'Grain Data', 'NeighborList'),
                                                    'MisorientationList',
                                                    simpl.DataArrayPath('Small IN100', 'Grain Data', 'AvgQuats'),
                                                    simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'),
                                                    simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                        'CrystalStructures'),
                                                    '', False)
    assert err == 0, f'FindMisorientations ErrorCondition {err}'

    # Find Schmid Factors
    err = orientation_analysis.find_schmids(dca, simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'),
                                            simpl.DataArrayPath('Small IN100', 'Phase Data', 'CrystalStructures'),
                                            simpl.DataArrayPath('Small IN100', 'Grain Data', 'AvgQuats'),
                                            'Schmids', 'SlipSystems', 'Poles', '', '', simpl.FloatVec3([1, 1, 1]),
                                            False, False, simpl.FloatVec3([1, 1, 1]), simpl.FloatVec3([1, 1, 1]))
    assert err == 0, f'FindSchmids ErrorCondition {err}'

    # Find Feature Reference Misorientations
    err = orientation_analysis.find_feature_reference_misorientations(dca,
                                                                      simpl.DataArrayPath('Small IN100',
                                                                                          'EBSD Scan Data',
                                                                                          'FeatureIds'),
                                                                      simpl.DataArrayPath('Small IN100',
                                                                                          'EBSD Scan Data', 'Phases'),
                                                                      simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                                          'CrystalStructures'),
                                                                      simpl.DataArrayPath('Small IN100',
                                                                                          'EBSD Scan Data', 'Quats'),
                                                                      simpl.DataArrayPath('Small IN100', 'Grain Data',
                                                                                          'AvgQuats'),
                                                                      simpl.DataArrayPath('', '', ''),
                                                                      'FeatureAvgMisorientations',
                                                                      'FeatureReferenceMisorientations',
                                                                      sh.ReferenceOrientation.AverageOrientation)
    assert err == 0, f'FindFeatureReferenceMisorientations ErrorCondition {err}'

    # Find Kernel Average Misorientations
    err = orientation_analysis.find_kernel_avg_misorientations(dca,
                                                               simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                   'FeatureIds'),
                                                               simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                   'Phases'),
                                                               simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                                   'CrystalStructures'),
                                                               simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                   'Quats'),
                                                               'KernelAverageMisorientations',
                                                               simpl.IntVec3([1, 1, 1]))
    assert err == 0, f'FindKernelAvgMisorientations ErrorCondition {err}'

    # Write to DREAM3D file
    err = sh.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/Statistics/SmallIN100_CrystalStats.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_cryst_stats()
