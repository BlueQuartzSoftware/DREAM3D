# Based on Confidence Index Histogram example
# 

# These are the simpl_py python modules
import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy
import statisticspy
import samplingpy

def confidence_index_histogram_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # ReadAngData
    err = orientationanalysispy.read_ang_data(dca,
                                simpl.DataArrayPath('Small IN100 Slice 1'), 
                                'Phase Data', 
                                'EBSD Scan Data', 
                                sd.GetDataDirectory() + '/Data/SmallIN100/Slice_1.ang')
    if err < 0:
        print('ReadAngData ErrorCondition: %d' % err)

    err = simplpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', ''),
                                             simpl.FloatVec3([0.0, 1.0, 0.0]), 180.0, False, sc.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 0)
    if err < 0:
        print('RotateSampleRefFrame ErrorCondition: %d' % err)

    err = orientationanalysispy.rotate_euler_ref_frame(dca, simpl.FloatVec3([0.0, 0.0, 1.0]), 90.0,
                                                       simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                           'EulerAngles'))
    if err < 0:
        print('RotateEulerRefFrame ErrorCondition: %d' % err)

    # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
    selected_thresholds = simpl.ComparisonInputs()
    selected_thresholds.addInput('Small IN100 Slice 1', 'EBSD Scan Data', 'Confidence Index', 0, 0)

    err = sc.MultiThresholdObjects(dca, 'Mask', [('Small IN100 Slice 1', 'EBSD Scan Data', 'Confidence Index', '<', 0)])
    if err < 0:
        print('MultiThresholdObjects ErrorCondition: %d' % err)

    # Replace Value in Array (Conditional)
    err = simplpy.conditional_set_value(dca, simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                 'Confidence Index'),
                                        simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'Mask'), 0)
    if err < 0:
        print('ConditionalSetValue ErrorCondition: %d' % err)

    # Calculate Frequency Histogram
    err = statisticspy.calculate_array_histogram(dca, simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                          'Confidence Index'),
                                                 50, 0, 1, False, 0, 'Histograms', 'CI_Histogram', False, simpl.DataArrayPath(''))
    if err < 0:
        print('CalculatArrayHistogram ErrorCondition: %d' % err)

    # Export ASCII Data
    selected_data_array_paths = [simpl.DataArrayPath('Small IN100 Slice 1', 'Histograms', 'CI_Histogram')]
    err = simplpy.write_ascii_data(dca, selected_data_array_paths,
                                   sd.GetBuildDirectory() + '/Data/Output/Histograms', '', 0, '.csv', 1, 0)
    if err < 0:
        print('WriteAsciiData ErrorCondition: %d' % err)

if __name__ == '__main__':
    confidence_index_histogram_test()
