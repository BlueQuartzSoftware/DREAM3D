# Based on Misc_Filters example
# 

# These are the simpl_py python modules
from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import samplingpy
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy


def misc_filters_test():
    """
    Pipeline Annotation
    --------------------
    This pipeline shows some miscellaneous filters that can be used in a pipeline.

    The only thing that this particular "Filter" does is allow you, the user/developer of the pipeline, to do is provide
     your self notes about the purpose of a particular section of pipeline or an entire pipeline.

    For example in this pipeline we are going to read some EBSD data, rename a few things and show other filters being
     used.

    """
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # ReadAngData
    err = orientationanalysispy.read_ang_data(dca, "ImageDataContainer", "CellEnsembleData", "CellData",
                                              sd.GetDataDirectory() + "/Data/SmallIN100/Slice_1.ang")
    if err < 0:
        print("ReadAngData ErrorCondition: %d" % err)

    # Rename Data Container
    err = simplpy.rename_data_container(dca, "ImageDataContainer", "Small IN100 Slice 1")
    if err < 0:
        print("RenameDataContainer ErrorCondition: %d" % err)
  
    # Rename Attribute Matrix
    err = simplpy.rename_attribute_matrix(dca, simpl.DataArrayPath("Small IN100 Slice 1", "CellData", ""),
                                          "EBSD_Data")

    # Rotate Sample Reference Frame
    err = samplingpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath("Small IN100 Slice 1", "EBSD_Data", ""),
                                             simpl.FloatVec3Type([0.0, 1.0, 0.0]), 180.0, False)
    if err < 0:
        print("RotateSampleRefFrame ErrorCondition: %d" % err)

    # Rotate Euler Reference Frame
    err = orientationanalysispy.rotate_euler_ref_frame(dca, simpl.FloatVec3Type([0.0, 0.0, 1.0]), 90.0,
                                                       simpl.DataArrayPath("Small IN100 Slice 1", "EBSD_Data",
                                                                           "EulerAngles"))
    if err < 0:
        print("RotateEulerRefFrame ErrorCondition: %d" % err)

    # Create String Array
    err = simplpy.create_string_array(dca, 1, simpl.DataArrayPath("Small IN100 Slice 1", "CellEnsembleData",
                                                                  "Description"),
                                      "A description of the phase")
    if err < 0:
        print("CreateStringArray ErrorCondition: %d" % err)

    """
    Pipeline Annotation:
    --------------------
    This next section shows applying a color table to the Confidence Index and then saving the image
    """

    # Generate color table #1
    jet_json_array = simpl.JsonArray([
         -1,
         0,
         0,
         0.5625,
         -0.777778,
         0,
         0,
         1,
         -0.269841,
         0,
         1,
         1,
         -0.015873,
         0.5,
         1,
         0.5,
         0.238095,
         1,
         1,
         0,
         0.746032,
         1,
         0,
         0,
         1,
         0.5,
         0,
         0])
    err = simplpy.generate_color_table(dca, "jet", jet_json_array, simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                       "EBSD_Data", "Confidence Index"),
                                       "Confidence_Index_Jet")
    if err < 0:
        print("GenerateColorTable #1 ErrorCondition: %d" % err)

    # ITK Image Writer #1
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                "/Data/Output/Example/Small_IN100_Slice_1_CI_Jet.png",
                                                simpl.DataArrayPath("Small IN100 Slice 1", "EBSD_Data",
                                                                    "Confidence_Index_Jet"), 0)
    if err < 0:
        print("ITKImageWriter #1 ErrorCondition: %d" % err)

    """
    Pipeline Annotation:
    --------------------
    This next section shows extracting the "phi1" Euler angle, applying the "Jet" color table and then saving the image.
    """

    # Extract Component as Attribute Array
    err = simplpy.extract_component_as_array(dca,
                                             simpl.DataArrayPath("Small IN100 Slice 1",
                                                                 "EBSD_Data", "EulerAngles"),
                                             0, "phi1")
    if err < 0:
        print("ExtractComponentAsArray ErrorCondition %d" % err)

    # Generate color table #1
    err = simplpy.generate_color_table(dca, "jet", jet_json_array, simpl.DataArrayPath("Small IN100 Slice 1",
                                                                                       "EBSD_Data", "phi1"),
                                       "phi1_Jet")
    if err < 0:
        print("GenerateColorTable #2 ErrorCondition: %d" % err)

    # ITK Image Writer #2
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                "/Data/Output/Example/Small_IN100_Slice_1_phi1_Jet.png",
                                                simpl.DataArrayPath("Small IN100 Slice 1", "EBSD_Data", "phi1_Jet"), 0)
    if err < 0:
        print("ITKImageWriter #2 ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    misc_filters_test()
