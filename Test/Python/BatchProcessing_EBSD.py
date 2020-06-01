'''
This example shows batch processing of .ang files to generate an IPF Color Image for 
each slice of data. As each filter is instantiated in the loop the filter is executed. 
We are not building up a pipeline and then executing that pipeline.
'''

import numpy as np

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy
import samplingpy
import itkimageprocessing
import itkimageprocessingpy

def batch_process():

    # Each filter is instantiated and executed immediately
    # The programmer is responsible for providing some sort of feedback to the user of the script
    for i in range(1, 10, 1):
        # Create Data Container Array
        dca = simpl.DataContainerArray()

        input_file = f'{sd.GetDataDirectory()}/Data/SmallIN100/Slice_{i}.ang'
        print(f'i:{input_file}')
        err = orientationanalysispy.read_ang_data(dca, 'EBSD Scan', 'Phase Data', 'Scan Data',
                                              input_file)
        if err < 0:
            print(f'ReadAngData ErrorCondition: {err}')

        # Rotate Sample Reference Frame
        err = simplpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath('EBSD Scan', 'Scan Data', ''),
                                                simpl.FloatVec3([0.0, 1.0, 0.0]), 180.0, False, sc.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 0)
        if err < 0:
            print(f'RotateSampleRefFrame ErrorCondition: {err}')

        # Rotate Euler Reference Frame
        err = orientationanalysispy.rotate_euler_ref_frame(dca, simpl.FloatVec3([0.0, 0.0, 1.0]), 90.0,
                                                        simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'EulerAngles'))
        if err < 0:
            print(f'RotateEulerRefFrame ErrorCondition: {err}')

        # Threshold Objects
        err = sc.MultiThresholdObjects(dca, 'Mask', [('EBSD Scan', 'Scan Data', 'Confidence Index', '>', 0.1)])
        if err < 0:
            print(f'MultiThresholdObjects ErrorCondition: {err}')

        # Generate IPF Colors
        err = orientationanalysispy.generate_ipf_colors(dca, simpl.FloatVec3([0, 0, 1]),
                                                        simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'Phases'),
                                                        simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'EulerAngles'),
                                                        simpl.DataArrayPath('EBSD Scan', 'Phase Data', 'CrystalStructures'),
                                                        True,
                                                        simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'Mask'), 'IPFColor')
        if err < 0:
            print(f'GenerateIPFColors ErrorCondition: {err}')

        # ITK Image Writer
        image_file = f'{sd.GetBuildDirectory()}/Data/Output/Examples/Small_IN100_Batch_{i}.png'

        image_writer = itkimageprocessing.ITKImageWriter()
        image_writer.registerImageIOFactories()
        err = itkimageprocessingpy.itk_image_writer(dca, image_file,
                                                    simpl.DataArrayPath('EBSD Scan', 'Scan Data', 'IPFColor'), 0)
        if err < 0:
            print(f'ITKImageWriter ErrorCondition: {err}')

        # Write to DREAM3D file
        output_file = f'{sd.GetBuildDirectory()}/Data/Output/Examples/Small_IN100_Batch_{i}.dream3d'
        err = sc.WriteDREAM3DFile(output_file, dca)
        if err < 0:
            print(f'WriteDREAM3DFile ErrorCondition: {err}')
        #----------------------------------------------------------------------
        # End of Processing Loop
        #----------------------------------------------------------------------

    print('FINISHED')

if __name__ == '__main__':
    batch_process()
