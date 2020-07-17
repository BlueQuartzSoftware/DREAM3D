# Based on Image Initial Visualization example
# 
# These are the simpl_py python modules

import simpl
import simpl_helpers as sh
import simpl_test_dirs as sd
import itkimageprocessing
import itkimageprocessingpy

def image_initial_visualization():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Register the ImageIO Factories
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()

    # Import Image Stack [ITK]
    file_list_info = simpl.FileListInfo(2, 0, 11, 174, 1, sd.GetDataDirectory() + '/Data/Image',
                                        'slice_', '', 'tif')
    err = itkimageprocessingpy.itk_import_image_stack(dca, 'RoboMet.3D Image Stack', 'Optical Data',
                                                      simpl.FloatVec3([0, 0, 0]), simpl.FloatVec3([1, 1, 1]),
                                                      file_list_info, 164, 'ImageData')
    assert err == 0, f'ITK Import Image Stack ErrorCondition {err}'

    # Write to DREAM3D file
    err = sh.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/ImagesStack/Images.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    image_initial_visualization()