#
# Example pipeline : Export Small IN100 ODF Data
#

# These are the simpl_py python modules

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy as orientationanalysis
import samplingpy as sampling

def export_small_in100_odf_data():
    run_pipeline = False

    # Create Data Container Array
    dca = simpl.DataContainerArray()

    if run_pipeline:
        pipeline_test(dca)
    else:
        pythonic_test(dca)

def pipeline_test(dca):
    # ReadAngData
    # Read in the SmallIN100 data
    print('# --- ReadAngData ')
    read_ang_data = orientationanalysis.ReadAngData.New()
    read_ang_data.setDataContainerArray(dca)
    read_ang_data.InputFile = sd.GetDataDirectory() + '/Data/SmallIN100/Slice_1.ang'
    read_ang_data.DataContainerName = 'Small IN100 Slice 1'
    read_ang_data.CellAttributeMatrixName = 'EBSD Scan Data'
    read_ang_data.CellEnsembleAttributeMatrixName = 'Phase Data'
    read_ang_data.preflight()
    err = read_ang_data.ErrorCondition
    assert err == 0, f'  {read_ang_data.NameOfClass} Preflight Error Code: {err}'

    # RotateEulerRefFrame
    print('# --- RotateEulerRefFrame ')
    rotate_euler_ref_frame = orientationanalysis.RotateEulerRefFrame.New()
    rotate_euler_ref_frame.setDataContainerArray(dca)
    rotate_euler_ref_frame.RotationAngle = 90.0
    rotate_euler_ref_frame.RotationAxis = simpl.FloatVec3([0.0, 0.0, 1.0])
    rotate_euler_ref_frame.CellEulerAnglesArrayPath = simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                       'EulerAngles')
    rotate_euler_ref_frame.preflight()
    err = rotate_euler_ref_frame.ErrorCondition
    assert err == 0, f'  {rotate_euler_ref_frame.NameOfClass} Preflight Error Code: {err}'

    # RotateSampleRefFrame
    #
    print('# --- RotateSampleRefFrame ')
    rotate_sample_ref_frame = simpl.RotateSampleRefFrame.New()
    rotate_sample_ref_frame.setDataContainerArray(dca)
    rotate_sample_ref_frame.RotationAngle = 180.0
    rotate_sample_ref_frame.RotationAxis = simpl.FloatVec3([0.0, 1.0, 0.0])
    rotate_sample_ref_frame.CellAttributeMatrixPath = simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', '')
    rotate_sample_ref_frame.preflight()
    err = rotate_sample_ref_frame.ErrorCondition
    assert err == 0, f'  {rotate_sample_ref_frame.NameOfClass} Preflight Error Code: {err}'

    # MultiThresholdObjects
    #
    print('# --- MultiThresholdObjects ')
    multi_threshold_objects = simpl.MultiThresholdObjects.New()
    multi_threshold_objects.setDataContainerArray(dca)
    multi_threshold_objects.DestinationArrayName = 'Mask'
    selected_thresholds = simpl.ComparisonInputs()
    selected_thresholds.addInput('Small IN100 Slice 1', 'EBSD Scan Data', 'Confidence Index', 1, 0.1)
    selected_thresholds.addInput('Small IN100 Slice 1', 'EBSD Scan Data', 'Image Quality', 1, 120)
    multi_threshold_objects.SelectedThresholds = selected_thresholds
    multi_threshold_objects.preflight()
    err = multi_threshold_objects.ErrorCondition
    assert err == 0, f'  {multi_threshold_objects.NameOfClass} Preflight Error Code: {err}'

    # WriteStatsGenOdfAngleFile
    #
    print('# --- WriteStatsGenOdfAngleFile ')
    write_stats_gen_odf_angle_file = orientationanalysis.WriteStatsGenOdfAngleFile.New()
    write_stats_gen_odf_angle_file.setDataContainerArray(dca)
    write_stats_gen_odf_angle_file.OutputFile = sd.GetBuildDirectory() + '/Data/Output/StatsGenODF.txt'
    write_stats_gen_odf_angle_file.Weight = 1
    write_stats_gen_odf_angle_file.Sigma = 1
    write_stats_gen_odf_angle_file.Delimiter = 2
    write_stats_gen_odf_angle_file.CellPhasesArrayPath = simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                        'Phases')
    write_stats_gen_odf_angle_file.CellEulerAnglesArrayPath = simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data',
                                                                             'EulerAngles')
    write_stats_gen_odf_angle_file.GoodVoxelsArrayPath = simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', 'Mask')
    write_stats_gen_odf_angle_file.ConvertToDegrees = True
    write_stats_gen_odf_angle_file.UseGoodVoxels = True
    write_stats_gen_odf_angle_file.preflight()
    err = write_stats_gen_odf_angle_file.ErrorCondition
    assert err == 0, f'  {write_stats_gen_odf_angle_file.NameOfClass} Preflight Error Code: {err}'

    # Construct the pipeline
    pipeline = simpl.FilterPipeline.New()
    pipeline.Name = ('New Pipeline')
    print('Pipeline Name: %s ' % pipeline.Name)

    pipeline.pushBack(read_ang_data)
    pipeline.pushBack(rotate_euler_ref_frame)
    pipeline.pushBack(rotate_sample_ref_frame)
    pipeline.pushBack(multi_threshold_objects)
    pipeline.pushBack(write_stats_gen_odf_angle_file)

    filter_count = pipeline.size()
    print('Filter Count: %d' % filter_count)

    err = pipeline.preflightPipeline()
    assert err == 0, f'  Pipeline Preflight Error Code: {err}'

    # Run the pipeline
    data_container = pipeline.run()
    err = pipeline.ErrorCondition
    assert err == 0, f'Execute Error Code: {err}'

    pipeline.popFront()
    filter_count = pipeline.size()
    print('Filter Count: %d' % filter_count)

def pythonic_test(dca):
    err = orientationanalysis.read_ang_data(dca, 'Small IN100 Slice 1', 'Phase Data', 'EBSD Scan Data',
                                            sd.GetDataDirectory() + '/Data/SmallIN100/Slice_1.ang')
    assert err == 0, f'ReadAngData ErrorCondition: {err}'

    err = orientationanalysis.rotate_euler_ref_frame(dca, simpl.FloatVec3([0.0, 0.0, 1.0]), 90.0,
                                                     simpl.DataArrayPath('Small IN100 Slice 1',
                                                                         'EBSD Scan Data', 'EulerAngles'))
    assert err == 0, f'RotateEulerRefFrame ErrorCondition: {err}'

    err = simplpy.rotate_sample_ref_frame(dca,
                                           simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', ''),
                                           simpl.FloatVec3([0.0, 1.0, 0.0]), 180.0, False, sc.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 0)
    assert err == 0, f'RotateSampleRefFrame ErrorCondition: {err}'

    # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
    selected_thresholds = simpl.ComparisonInputs()
    selected_thresholds.addInput('Small IN100 Slice 1', 'EBSD Scan Data', 'Confidence Index', 1, 0.1)
    selected_thresholds.addInput('Small IN100 Slice 1', 'EBSD Scan Data', 'Image Quality', 1, 120)

    err = simplpy.multi_threshold_objects(dca, 'Mask', selected_thresholds)
    assert err == 0, f'MultiThresholdObjects ErrorCondition: {err}'

    am = dca.getAttributeMatrix(simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD Scan Data', ''))
    data_array = am.getAttributeArray('EulerAngles')
    rawdata = data_array.npview()
    # print(rawdata)
    err = orientationanalysis.write_stats_gen_odf_angle_file(dca,
                                                             sd.GetBuildDirectory() + '/Data/Output/StatsGenODF.txt',
                                                             1, 1, 2,
                                                             simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                 'EBSD Scan Data', 'Phases'),
                                                             simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                 'EBSD Scan Data',
                                                                                 'EulerAngles'),
                                                             simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                 'EBSD Scan Data', 'Mask'),
                                                             True, True)
    assert err == 0, f'WriteStatsGenOdfAngleFile ErrorCondition: {err}'

if __name__ == '__main__':
    export_small_in100_odf_data()
    print('Test Complete')
