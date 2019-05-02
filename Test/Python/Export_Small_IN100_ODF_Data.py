#
# Example pipeline : Export Small IN100 ODF Data
#

import time

# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientationanalysis
from dream3d import samplingpy as sampling


def export_small_in100_odf_data():
    run_pipeline = False

    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    if run_pipeline:
        pipeline_test(dca)
    else:
        pythonic_test(dca)


def pipeline_test(dca):
    # ReadAngData
    # Read in the SmallIN100 data
    print("# --- ReadAngData ")
    read_ang_data = orientationanalysis.ReadAngData.New()
    read_ang_data.setDataContainerArray(dca)
    read_ang_data.InputFile = sd.GetDataDirectory() + "/Data/SmallIN100/Slice_1.ang"
    read_ang_data.DataContainerName = "Small IN100 Slice 1"
    read_ang_data.CellAttributeMatrixName = "EBSD Scan Data"
    read_ang_data.CellEnsembleAttributeMatrixName = "Phase Data"
    read_ang_data.preflight()
    print("  Preflight Error Code: %s" % read_ang_data.ErrorCondition)
    filter_name = read_ang_data.NameOfClass
    if filter_name != "ReadAngData":
        print("  Error: Filter class name is not correct. %s != ReadAngData" % filter_name)

    # RotateEulerRefFrame
    print("# --- RotateEulerRefFrame ")
    rotate_euler_ref_frame = orientationanalysis.RotateEulerRefFrame.New()
    rotate_euler_ref_frame.setDataContainerArray(dca)
    rotate_euler_ref_frame.RotationAngle = 90.0
    rotate_euler_ref_frame.RotationAxis = simpl.FloatVec3Type([0.0, 0.0, 1.0])
    rotate_euler_ref_frame.CellEulerAnglesArrayPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                       "EulerAngles")
    rotate_euler_ref_frame.preflight()
    print("  Preflight Error Code: %s" % rotate_euler_ref_frame.ErrorCondition)
    filter_name = rotate_euler_ref_frame.NameOfClass
    if filter_name != "RotateEulerRefFrame":
        print("  Error: Filter class name is not correct. %s != RotateEulerRefFrame" % filter_name)

    # RotateSampleRefFrame
    #
    print("# --- RotateSampleRefFrame ")
    rotate_sample_ref_frame = sampling.RotateSampleRefFrame.New()
    rotate_sample_ref_frame.setDataContainerArray(dca)
    rotate_sample_ref_frame.RotationAngle = 180.0
    rotate_sample_ref_frame.RotationAxis = simpl.FloatVec3Type([0.0, 1.0, 0.0])
    rotate_sample_ref_frame.CellAttributeMatrixPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "")
    rotate_sample_ref_frame.preflight()
    print("  Preflight Error Code: %s" % rotate_sample_ref_frame.ErrorCondition)
    filter_name = rotate_sample_ref_frame.NameOfClass
    if filter_name != "RotateSampleRefFrame":
        print("  Error: Filter class name is not correct. %s != RotateSampleRefFrame" % filter_name)

    # MultiThresholdObjects
    #
    print("# --- MultiThresholdObjects ")
    multi_threshold_objects = simpl.MultiThresholdObjects.New()
    multi_threshold_objects.setDataContainerArray(dca)
    multi_threshold_objects.DestinationArrayName = "Mask"
    selected_thresholds = simpl.ComparisonInputs()
    selected_thresholds.addInput("Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index", 1, 0.1)
    selected_thresholds.addInput("Small IN100 Slice 1", "EBSD Scan Data", "Image Quality", 1, 120)
    multi_threshold_objects.SelectedThresholds = selected_thresholds
    multi_threshold_objects.preflight()
    print("  Preflight Error Code: %s" % multi_threshold_objects.ErrorCondition)
    filter_name = multi_threshold_objects.NameOfClass
    if filter_name != "MultiThresholdObjects":
        print("  Error: Filter class name is not correct. %s != MultiThresholdObjects" % filter_name)

    # WriteStatsGenOdfAngleFile
    #
    print("# --- WriteStatsGenOdfAngleFile ")
    write_stats_gen_odf_angle_file = orientationanalysis.WriteStatsGenOdfAngleFile.New()
    write_stats_gen_odf_angle_file.setDataContainerArray(dca)
    write_stats_gen_odf_angle_file.OutputFile = sd.GetBuildDirectory() + "/Data/Output/StatsGenODF.txt"
    write_stats_gen_odf_angle_file.Weight = 1
    write_stats_gen_odf_angle_file.Sigma = 1
    write_stats_gen_odf_angle_file.Delimiter = 2
    write_stats_gen_odf_angle_file.CellPhasesArrayPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                        "Phases")
    write_stats_gen_odf_angle_file.CellEulerAnglesArrayPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data",
                                                                             "EulerAngles")
    write_stats_gen_odf_angle_file.GoodVoxelsArrayPath = simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", "Mask")
    write_stats_gen_odf_angle_file.ConvertToDegrees = True
    write_stats_gen_odf_angle_file.UseGoodVoxels = True
    write_stats_gen_odf_angle_file.preflight()
    print("  Preflight Error Code: %s" % write_stats_gen_odf_angle_file.ErrorCondition)
    filter_name = write_stats_gen_odf_angle_file.NameOfClass
    if filter_name != "WriteStatsGenOdfAngleFile":
        print("  Error: Filter class name is not correct. %s != WriteStatsGenOdfAngleFile" % filter_name)

    # Construct the pipeline
    pipeline = simpl.FilterPipeline.New()
    pipeline.Name = ("New Pipeline")
    print("Pipeline Name: %s " % pipeline.Name)

    pipeline.pushBack(read_ang_data)
    pipeline.pushBack(rotate_euler_ref_frame)
    pipeline.pushBack(rotate_sample_ref_frame)
    pipeline.pushBack(multi_threshold_objects)
    pipeline.pushBack(write_stats_gen_odf_angle_file)

    filter_count = pipeline.size()
    print("Filter Count: %d" % filter_count)

    err = pipeline.preflightPipeline()
    print("Preflight ErrorCondition: %d" % err)

    # Run the pipeline
    data_container = pipeline.run()
    err = pipeline.ErrorCondition
    print("Execute ErrorCondition: %d" % err)

    pipeline.popFront()
    filter_count = pipeline.size()
    print("Filter Count: %d" % filter_count)

    time.sleep(2)


def pythonic_test(dca):
    err = orientationanalysis.read_ang_data(dca, "Small IN100 Slice 1", "Phase Data", "EBSD Scan Data",
                                            sd.GetDataDirectory() + "/Data/SmallIN100/Slice_1.ang")
    if err < 0:
        print("ReadAngData ErrorCondition: %d" % err)

    err = orientationanalysis.rotate_euler_ref_frame(dca, simpl.FloatVec3Type([0.0, 0.0, 1.0]), 90.0,
                                                     simpl.DataArrayPath("Small IN100 Slice 1",
                                                                         "EBSD Scan Data", "EulerAngles"))
    if err < 0:
        print("RotateEulerRefFrame ErrorCondition: %d" % err)

    err = sampling.rotate_sample_ref_frame(dca,
                                           simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", ""),
                                           simpl.FloatVec3Type([0.0, 1.0, 0.0]), 180.0, False)
    if err < 0:
        print("RotateSampleRefFrame ErrorCondition: %d" % err)

    # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
    selected_thresholds = simpl.ComparisonInputs()
    selected_thresholds.addInput("Small IN100 Slice 1", "EBSD Scan Data", "Confidence Index", 1, 0.1)
    selected_thresholds.addInput("Small IN100 Slice 1", "EBSD Scan Data", "Image Quality", 1, 120)

    err = simplpy.multi_threshold_objects(dca, "Mask", selected_thresholds)
    if err < 0:
        print("MultiThresholdObjects ErrorCondition: %d" % err)

    am = dca.getAttributeMatrix(simpl.DataArrayPath("Small IN100 Slice 1", "EBSD Scan Data", ""))
    data_array = am.getAttributeArray("EulerAngles")
    rawdata = data_array.Data
    # print(rawdata)
    err = orientationanalysis.write_stats_gen_odf_angle_file(dca,
                                                             sd.GetBuildDirectory() + "/Data/Output/StatsGenODF.txt",
                                                             1, 1, 2,
                                                             simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                 "EBSD Scan Data", "Phases"),
                                                             simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                 "EBSD Scan Data",
                                                                                 "EulerAngles"),
                                                             simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                 "EBSD Scan Data", "Mask"),
                                                             True, True)
    if err < 0:
        print("WriteStatsGenOdfAngleFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    export_small_in100_odf_data()
    print("Test Complete")
