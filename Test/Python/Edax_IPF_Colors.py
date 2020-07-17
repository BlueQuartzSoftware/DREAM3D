'''
Pipeline example based on Edax_IPF_Colors
'''

import numpy as np

import simpl
import simplpy
import simpl_helpers as sh
import simpl_test_dirs as sd
import orientationanalysispy
import samplingpy
import itkimageprocessing
import itkimageprocessingpy

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
                                             simpl.FloatVec3([0.0, 1.0, 0.0]), 180.0, False, sh.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 0)
    assert err == 0, f'RotateSampleRefFrame ErrorCondition: {err}'

    # Rotate Euler Reference Frame
    err = orientationanalysispy.rotate_euler_ref_frame(dca, simpl.FloatVec3([0.0, 0.0, 1.0]), 90.0,
                                                       simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'EulerAngles'))
    assert err == 0, f'RotateEulerRefFrame ErrorCondition: {err}'

    # Threshold Objects
    err = sh.MultiThresholdObjects(dca, 'Mask', [('EBSD Scan', 'Scan Data', 'Confidence Index', '>', 0.1)])
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
                                                '/Data/Output/Examples/Small_IN100_Slice_1.png',
                                                simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'IPFColor'), 0)
    assert err == 0, f'ITKImageWriter ErrorCondition: {err}'

    # Write to DREAM3D file
    err = sh.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/Examples/Slice_1.dream3d', dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'


    # This section shows using MatPlotLib to graph the histogram.
    # dc = dca.getDataContainer('EBSD Scan')
    # am = dc.getAttributeMatrix('Scan Data')
    # iq = am.getAttributeArray('Image Quality')
    # plt.hist(np.asarray(iq), 256)
    # plt.show()

if __name__ == '__main__':
    color_data_change_test()
