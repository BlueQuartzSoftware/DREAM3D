# Based on Misc_Filters example
# 

# These are the simpl_py python modules
import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy
import samplingpy
import itkimageprocessing
import itkimageprocessingpy

def misc_filters_test():
    '''
    Pipeline Annotation
    --------------------
    This pipeline shows some miscellaneous filters that can be used in a pipeline.

    The only thing that this particular 'Filter' does is allow you, the user/developer of the pipeline, to do is provide
     your self notes about the purpose of a particular section of pipeline or an entire pipeline.

    For example in this pipeline we are going to read some EBSD data, rename a few things and show other filters being
     used.

    '''
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # ReadAngData
    err = orientationanalysispy.read_ang_data(dca, 'ImageDataContainer', 'CellEnsembleData', 'CellData',
                                              sd.GetDataDirectory() + '/Data/SmallIN100/Slice_1.ang')
    assert err == 0, f'ReadAngData ErrorCondition: {err}'

    # Rename Data Container
    err = simplpy.rename_data_container(dca, 'ImageDataContainer', 'Small IN100 Slice 1')
    assert err == 0, f'RenameDataContainer ErrorCondition: {err}'
  
    # Rename Attribute Matrix
    err = simplpy.rename_attribute_matrix(dca, simpl.DataArrayPath('Small IN100 Slice 1', 'CellData', ''),
                                          'EBSD_Data')

    # Rotate Sample Reference Frame
    err = simplpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD_Data', ''),
                                             simpl.FloatVec3([0.0, 1.0, 0.0]), 180.0, False, sc.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 0)
    assert err == 0, f'RotateSampleRefFrame ErrorCondition: {err}'

    # Rotate Euler Reference Frame
    err = orientationanalysispy.rotate_euler_ref_frame(dca, simpl.FloatVec3([0.0, 0.0, 1.0]), 90.0,
                                                       simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD_Data',
                                                                           'EulerAngles'))
    assert err == 0, f'RotateEulerRefFrame ErrorCondition: {err}'

    # Create String Array
    err = simplpy.create_string_array(dca, 1, simpl.DataArrayPath('Small IN100 Slice 1', 'CellEnsembleData',
                                                                  'Description'),
                                      'A description of the phase')
    assert err == 0, f'CreateStringArray ErrorCondition: {err}'

    '''
    Pipeline Annotation:
    --------------------
    This next section shows applying a color table to the Confidence Index and then saving the image
    '''

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
    err = simplpy.generate_color_table(dca, 'jet', jet_json_array, simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                       'EBSD_Data', 'Confidence Index'),
                                       'Confidence_Index_Jet')
    assert err == 0, f'GenerateColorTable #1 ErrorCondition: {err}'

    # ITK Image Writer #1
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                '/Data/Output/Example/Small_IN100_Slice_1_CI_Jet.png',
                                                simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD_Data',
                                                                    'Confidence_Index_Jet'), 0)
    assert err == 0, f'ITKImageWriter #1 ErrorCondition: {err}'

    '''
    Pipeline Annotation:
    --------------------
    This next section shows extracting the 'phi1' Euler angle, applying the 'Jet' color table and then saving the image.
    '''

    # Extract Component as Attribute Array
    err = simplpy.extract_component_as_array(dca,
                                             simpl.DataArrayPath('Small IN100 Slice 1',
                                                                 'EBSD_Data', 'EulerAngles'),
                                             0, 'phi1')
    assert err == 0, f'ExtractComponentAsArray ErrorCondition {err}'

    # Generate color table #1
    err = simplpy.generate_color_table(dca, 'jet', jet_json_array, simpl.DataArrayPath('Small IN100 Slice 1',
                                                                                       'EBSD_Data', 'phi1'),
                                       'phi1_Jet')
    assert err == 0, f'GenerateColorTable #2 ErrorCondition: {err}'

    # ITK Image Writer #2
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                '/Data/Output/Example/Small_IN100_Slice_1_phi1_Jet.png',
                                                simpl.DataArrayPath('Small IN100 Slice 1', 'EBSD_Data', 'phi1_Jet'), 0)
    assert err == 0, f'ITKImageWriter #2 ErrorCondition: {err}'

if __name__ == '__main__':
    misc_filters_test()
