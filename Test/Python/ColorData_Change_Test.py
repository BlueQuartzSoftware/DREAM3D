'''
Pipeline example based on Small IN100 Synthetic where IPF Color values are changed
using Numpy and re-added to the attribute matrix
'''

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy
import samplingpy
import itkimageprocessing
import itkimageprocessingpy

import numpy as np

def color_data_change_test():

    # Create Data Container Array
    dca = simpl.DataContainerArray()
    # print('Build Dir: %s' % sd.GetBuildDirectory())
    # ReadAngData
    err = orientationanalysispy.read_ang_data(dca, 'EBSD Scan', 'Phase Data', 'Scan Data',
                                              sd.GetDataDirectory() + '/Data/SmallIN100/Slice_1.ang')
    assert err == 0, f'ReadAngData ErrorCondition: {err}'

    # Rotate Sample Reference Frame
    err = simplpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath('EBSD Scan', 'Scan Data', ''),
                                             simpl.FloatVec3([0.0, 1.0, 0.0]), 180.0, False, sc.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 0)
    assert err == 0, f'RotateSampleRefFrame ErrorCondition: {err}'

    # Rotate Euler Reference Frame
    err = orientationanalysispy.rotate_euler_ref_frame(dca, simpl.FloatVec3([0.0, 0.0, 1.0]), 90.0,
                                                       simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'EulerAngles'))
    assert err == 0, f'RotateEulerRefFrame ErrorCondition: {err}'

    # Threshold Objects
    err = sc.MultiThresholdObjects(dca, 'Mask', [('EBSD Scan', 'Scan Data', 'Confidence Index', '>', 0.1)])
    assert err == 0, f'MultiThresholdObjects ErrorCondition: {err}'

    # Generate IPF Colors
    err = orientationanalysispy.generate_ipf_colors(dca, simpl.FloatVec3([0, 0, 1]),
                                                    simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'Phases'),
                                                    simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'EulerAngles'),
                                                    simpl.DataArrayPath('EBSD Scan', 'Phase Data', 'CrystalStructures'),
                                                    True,
                                                    simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'Mask'), 'IPFColor')
    assert err == 0, f'GenerateIPFColors ErrorCondition: {err}'

    # ITK Image Writer
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                '/Data/Output/Examples/Slice_1_ColorChange_Before.png',
                                                simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'IPFColor'), 0)
    assert err == 0, f'ITKImageWriter[1] ErrorCondition: {err}'


    # Change the color data
    am = dca.getAttributeMatrix(simpl.DataArrayPath('EBSD Scan', 'Scan Data', ''))
    data_array = am.getAttributeArray('IPFColor')
    comp_dims = data_array.getComponentDimensions()
    data = data_array.npview()

    # Zero array
    # np_array = np.zeros_like(data).astype('uint8').reshape(dataArray.getNumberOfTuples(),
    # dataArray.getNumberOfComponents())

    # Unchanged array
    np_array = np.asanyarray(data, np.uint8).reshape(data_array.getNumberOfTuples(), data_array.getNumberOfComponents())
    for row in np_array:
        if row[0] > 0:
            row[0] = row[0] / 2
        else:
            row[0] = 255
        row[1] = 128
        row[2] = 0
    
    if not np_array.flags.contiguous:
        np_array = np.ascontiguousarray(np_array)
        assert np_array.flags.contiguous, 'Only contiguous arrays are supported.'


    # Create the wrapped DataArray<float> object
    newdata = simpl.UInt8Array(np_array, 'IPFColor', False)
    # Add the DataArray to the AttributeMatrix
    am.addOrReplaceAttributeArray(newdata)

    # ITK Image Writer
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                '/Data/Output/Examples/Slice_1_ColorChange_After.png',
                                                simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'IPFColor'), 0)
    assert err == 0, f'ITKImageWriter[2] ErrorCondition: {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/ColorChanged.dream3d', dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    color_data_change_test()
