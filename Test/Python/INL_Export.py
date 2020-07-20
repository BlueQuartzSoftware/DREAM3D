# Based on INL_Export example
# 
# These are the simpl_py python modules

import simpl
import simplpy
import simpl_helpers as sh
import simpl_test_dirs as sd
import orientationanalysispy
import samplingpy
import processingpy
import reconstructionpy
import genericpy
import statisticspy
import itkimageprocessing
import itkimageprocessingpy

def inl_export():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # ReadAngData
    err = orientationanalysispy.read_ang_data(dca, 'Small IN100 Slice 1', 'Phase Data', 'EBSD Scan Data',
                                              sd.GetDataDirectory() + '/Data/SmallIN100/Slice_1.ang')
    assert err == 0, f'ReadAngData ErrorCondition: {err}'

    # Rotate Sample Reference Frame
    err = simplpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', ''),
                                             simpl.FloatVec3([0.0, 1.0, 0.0]), 180.0, False, sh.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 0)
    assert err == 0, f'RotateSampleRefFrame ErrorCondition: {err}'

    # Rotate Euler Reference Frame
    err = orientationanalysispy.rotate_euler_ref_frame(dca, simpl.FloatVec3([0.0, 0.0, 1.0]), 90.0,
                                                       simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                           'EulerAngles'))
    assert err == 0, f'RotateEulerRefFrame ErrorCondition: {err}'

    # MultiThresholdObjects filter
    err = sh.MultiThresholdObjects(dca, 'Mask',
                                   [('Small IN100 Slice 1', 'EBSD Scan Data', 'Confidence Index', 1, 0.1),
                                    ('Small IN100 Slice 1', 'EBSD Scan Data', 'Image Quality', 1, 120)])
    assert err == 0, f'MultiThresholdObjects ErrorCondition: {err}'

    # Convert Orientation Representation
    err = orientationanalysispy.convert_orientations(dca, 0, 2,
                                                     simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                         'EulerAngles'),
                                                     'Quats')
    assert err == 0, f'ConvertOrientation ErrorCondition: {err}'

    # Isolate Largest Feature (Identify Sample)
    err = processingpy.identify_sample(dca, False,
                                       simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'Mask'))
    assert err == 0, f'IdentifySample ErrorCondition: {err}'

    # Neighbor Orientation Comparison (Bad Data)
    err = orientationanalysispy.bad_data_neighbor_orientation_check(dca, 5, 4,
                                                                    simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                        'EBSD Scan Data',
                                                                                        'Mask'),
                                                                    simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                        'EBSD Scan Data',
                                                                                        'Phases'),
                                                                    simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                        'Phase Data',
                                                                                        'CrystalStructures'),
                                                                    simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                        'EBSD Scan Data',
                                                                                        'Quats'))
    assert err == 0, f'NeighborOrientationComparison ErrorCondition {err}'

    # Neighbor Orientation Correlation
    err = orientationanalysispy.neighbor_orientation_correlation(dca, 5, 0.2, 2,
                                                                 simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                     'EBSD Scan Data',
                                                                                     'Confidence Index'),
                                                                 simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                     'EBSD Scan Data',
                                                                                     'Phases'),
                                                                 simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                     'Phase Data',
                                                                                     'CrystalStructures'),
                                                                 simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                     'EBSD Scan Data',
                                                                                     'Quats'))
    assert err == 0, f'NeighborOrientationCorrelation ErrorCondition {err}'

    # Segment Features (Misorientation)
    err = reconstructionpy.ebsd_segment_features(dca, 'Grain Data', 5, True,
                                                 simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'Mask'),
                                                 simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'Phases'),
                                                 simpl.DataArrayPath('Small IN100 Slice 1', 'Phase Data',
                                                                     'CrystalStructures'),
                                                 simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'Quats'),
                                                 'FeatureIds', 'Active')
    assert err == 0, f'SegmentFeatures ErrorCondition {err}'

    # Find Feature Phases
    err = genericpy.find_feature_phases(dca, simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'FeatureIds'),
                                        simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'Phases'),
                                        simpl.DataArrayPath('Small IN100 Slice 1', 'Grain Data', 'Phases'))
    assert err == 0, f'FindFeaturePhases ErrorCondition {err}'

    # Find Number of Features
    err = statisticspy.find_num_features(dca,
                                         simpl.DataArrayPath('Small IN100 Slice 1', 'Grain Data', 'Phases'),
                                         simpl.DataArrayPath('Small IN100 Slice 1', 'Phase Data', 'NumFeatures'))
    assert err == 0, f'FindNumFeatures ErrorCondition {err}'
    
    # Export INL File
    err = orientationanalysispy.inl_writer(dca, sd.GetBuildDirectory() +
                                           '/Data/Output/INL_Example/Small_IN100.inl',
                                           simpl.DataArrayPath('Small IN100 Slice 1', 'Phase Data', 'MaterialName'),
                                           simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'FeatureIds'),
                                           simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'Phases'),
                                           simpl.DataArrayPath('Small IN100 Slice 1', 'Phase Data',
                                                               'CrystalStructures'),
                                           simpl.DataArrayPath('Small IN100 Slice 1', 'Phase Data', 'NumFeatures'),
                                           simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'EulerAngles'))
    assert err == 0, f'INL Writer ErrorCondition {err}'

    # Generate IPF Colors
    err = orientationanalysispy.generate_ipf_colors(dca, simpl.FloatVec3([0, 0, 1]),
                                                    simpl.DataArrayPath('Small IN100 Slice 1',
                                                                        'EBSD Scan Data', 'Phases'),
                                                    simpl.DataArrayPath('Small IN100 Slice 1',
                                                                        'EBSD Scan Data', 'EulerAngles'),
                                                    simpl.DataArrayPath('Small IN100 Slice 1',
                                                                        'Phase Data', 'CrystalStructures'),
                                                    True,
                                                    simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                        'Mask'),
                                                    'IPFColor')
    assert err == 0, f'GenerateIPFColors ErrorCondition: {err}'

    # ITK Image Writer
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                '/Data/Output/INL_Example/Small_IN100_Slice_1.png',
                                                simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                    'IPFColor'), 0)
    assert err == 0, f'ITKImageWriter ErrorCondition: {err}'

if __name__ == '__main__':
    inl_export()
