# Based on Threshold2 MoveData example
# 

# These are the simpl_py python modules

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy as orientationanalysis
import samplingpy as sampling

def threshold2_move_data_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Pipeline Annotation 1
    err = simplpy.pipeline_annotation(dca,
                                      'This pipeline shows using the newer threshold objects filter' \
                                      ' and moving data around in the data structure.')
    assert err == 0, f'PipelineAnnotation 1 ErrorCondition: {err}'

    # ReadAngData
    err = orientationanalysis.read_ang_data(dca, 'Small IN100', 'Phase Data', 'EBSD Scan Data',
                                            sd.GetDataDirectory() + '/Data/SmallIN100/Slice_1.ang')
    assert err == 0, f'ReadAngData ErrorCondition: {err}'

    err = simplpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', ''),
                                           simpl.FloatVec3([0.0, 1.0, 0.0]), 180.0, False, sc.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 0)
    assert err == 0, f'RotateSampleRefFrame ErrorCondition: {err}'

    err = orientationanalysis.rotate_euler_ref_frame(dca, simpl.FloatVec3([0.0, 0.0, 1.0]), 90.0,
                                                     simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                         'EulerAngles'))
    assert err == 0, f'RotateEulerRefFrame ErrorCondition: {err}'

    # MultiThresholdObjects filter
    err = sc.MultiThresholdObjects2(dca, ['Small IN100', 'EBSD Scan Data', ''], 'Mask',
                                    ['AND', ('Confidence Index', 1, 0.1), ('Image Quality', 1, 400),
                                     ['OR', ('Image Quality', 0, 0)]])
    assert err == 0, f'MultiThresholdObjects ErrorCondition: {err}'

    # Pipeline Annotation 2
    err = simplpy.pipeline_annotation(dca, 'These next filters show creating a new DataContainer and AttributeMatrix, \
   then we move some data arrays from the Small IN100/EBSD Scan Data Attribute Matrix to the new attributeMatrix using '
                                      'the  \'Move Multi Data\' filter and the \'Move Data\' filter.')
    assert err == 0, f'PipelineAnnotation 2 ErrorCondition: {err}'

        # Create Data Container
    # Improve helper function for creation of data containers to include a dca
    err = simplpy.create_data_container(dca, 'Extra_DataContainer')
    assert err == 0, f'CreateDataContainer ErrorCondition: {err}'

    # Create Attribute Matrix
    # Using helper function
    dynamic_table_data = sc.CreateDynamicTableData([[189, 201, 1]], ['0', '1', '2'], ['1'])
    err = simplpy.create_attribute_matrix(dca, simpl.DataArrayPath('Extra_DataContainer', 'Positions', ''), 13,
                                          dynamic_table_data)

    assert err == 0, f'CreateAttributeMatrix ErrorCondition: {err}'

    # Move Multiple Data Arrays
    err = sc.MoveMultiData(dca, sc.WhatToMove.AttributeArray, [('Small IN100', 'EBSD Scan Data', 'X Position'),
                                                               simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                                                   'Y Position')],
                           simpl.DataArrayPath('Extra_DataContainer', 'Positions', ''))
    assert err == 0, f'MoveMultiData ErrorCondition: {err}'

    # Move Data Arrays
    err = sc.MoveData(dca, 'Attribute Array', simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'SEM Signal'),
                      simpl.DataArrayPath('Extra_DataContainer', 'Positions', ''))
    assert err == 0, f'MoveData ErrorCondition: {err}'

    # Pipeline Annotation 3
    err = simplpy.pipeline_annotation(dca, 'This next part simply changes (arbitrarily) the Origin \
  and Resolution of the Image Geometry that the data resides on.')
    assert err == 0, f'PipelineAnnotation 3 ErrorCondition: {err}'

        # Set Origin & Resolution (Image)
    err = simplpy.set_origin_resolution_image_geom(dca, 'Small IN100', True, simpl.FloatVec3([50, 25, 10]),
                                                   True, simpl.FloatVec3([1, 1, 25]))
    assert err == 0, f'SetOriginResolutionImageGeom ErrorCondition: {err}'

    # Write to DREAM3D File 1
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/Example/OriginChange.dream3d', dca)
    assert err == 0, f'WriteDREAM3DFile 1 ErrorCondition: {err}'

    # Pipeline Annotation 4
    err = simplpy.pipeline_annotation(dca, 'This next part simply scales the image geometry by some arbitrary amount.')
    assert err == 0, f'PipelineAnnotation 4 ErrorCondition: {err}'

        # Change Scaling of Volume
    err = simplpy.scale_volume(dca, 'Small IN100', '', True, False, simpl.FloatVec3([2, 2, 2]))
    assert err == 0, f'ScaleVolume ErrorCondition: {err}'

        # Write to DREAM3D File 2
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/Example/ScaleVolume.dream3d', dca)
    assert err == 0, f'WriteDREAM3DFile 2 ErrorCondition: {err}'

if __name__ == '__main__':
    threshold2_move_data_test()
