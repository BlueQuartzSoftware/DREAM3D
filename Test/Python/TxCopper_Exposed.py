# Based on TxCopper_Exposed example
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

def txcopper_exposed():
    # Create Data Container Array
    dca = simpl.DataContainerArray()
    data_container_name = 'Cugrid_after 2nd_15kv_2kx_2'

    # Import Oxford Instr. EBSD Data (.ctf)
    err = orientationanalysispy.read_ctf_data(dca,
                                data_container_name,
                                'Phase Data',               # Cell Ensemble Attribute Matrix Name
                                'EBSD Scan Data',           # Cell Attribute Matrix Name
                                sd.GetDataDirectory() + '/Data/Textured_Copper/Cugrid_after 2nd_15kv_2kx_2.ctf',
                                True,      # Convert to Radians
                                True  # Convert Hex EDAX Alignment
                                ) 
    assert err == 0, f'ReadCtfData ErrorCondition: {err}'

    # Rotate Sample Reference Frame
    err = simplpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath(data_container_name, 'EBSD Scan Data', ''),
                                             simpl.FloatVec3([0.0, 1.0, 0.0]), 180.0, False, sc.CreateDynamicTableData([[0.0 for x in range(3)] for y in range(3)]), 0)
    assert err == 0, f'RotateSampleRefFrame ErrorCondition: {err}'

    # Crop Geometry (Image)
    err = samplingpy.crop_image_geometry(dca, '',
                                         simpl.DataArrayPath(data_container_name, 'EBSD Scan Data', ''),
                                         simpl.DataArrayPath(data_container_name, 'Grain Data', ''),
                                         0, 0, 0, 460, 399, 0, False, False, True,
                                         simpl.DataArrayPath(data_container_name, 'EBSD Scan Data', 'FeatureIds'))
    assert err == 0, f'CropImageGeometry ErrorCondition {err}'

    # MultiThresholdObjects filter
    err = sc.MultiThresholdObjects(dca, 'Mask',
                                  [(data_container_name, 'EBSD Scan Data', 'Error', 2, 0)])
    assert err == 0, f'MultiThresholdObjects ErrorCondition: {err}'
    
    # Generate IPF Colors
    err = orientationanalysispy.generate_ipf_colors(dca, simpl.FloatVec3([0, 0, 1]),
                                                    simpl.DataArrayPath(data_container_name,
                                                                        'EBSD Scan Data', 'Phases'),
                                                    simpl.DataArrayPath(data_container_name,
                                                                        'EBSD Scan Data', 'EulerAngles'),
                                                    simpl.DataArrayPath(data_container_name,
                                                                        'Phase Data', 'CrystalStructures'),
                                                    True,
                                                    simpl.DataArrayPath(data_container_name, 'EBSD Scan Data', 'Mask'),
                                                    'IPF_Exposed_001')
    assert err == 0, f'GenerateIPFColors ErrorCondition: {err}'

    # ITK Image Writer
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                '/Data/Output/TexturedCopper/IPF_Exposed.png',
                                                simpl.DataArrayPath(data_container_name, 'EBSD Scan Data',
                                                                    'IPF_Exposed_001'), 0)
    assert err == 0, f'ITKImageWriter ErrorCondition: {err}'

    # Export Pole Figure Images #1
    # Possible enumerations: Image Format, Image Layout, Generation Algorithm / Pole Figure Type
    err = orientationanalysispy.write_pole_figure(dca, 'Exposed_Lambert_',
                                                  sd.GetBuildDirectory() + '/Data/Output/TexturedCopper', 0,
                                                  1000, 32, 32, 0,
                                                  simpl.DataArrayPath(data_container_name, 'EBSD Scan Data',
                                                                      'EulerAngles'),
                                                  simpl.DataArrayPath(data_container_name, 'EBSD Scan Data', 'Phases'),
                                                  simpl.DataArrayPath(data_container_name, 'Phase Data',
                                                                      'CrystalStructures'),
                                                  simpl.DataArrayPath(data_container_name, 'Phase Data',
                                                                      'MaterialName'),
                                                  simpl.DataArrayPath(data_container_name, 'EBSD Scan Data', 'Mask'),
                                                  True, 0)
    assert err == 0, f'WritePoleFigure #1 ErrorCondition: {err}'

    # Export Pole Figure Images #2
    err = orientationanalysispy.write_pole_figure(dca, 'Exposed_Discrete_',
                                                  sd.GetBuildDirectory() + '/Data/Output/TexturedCopper', 0,
                                                  1000, 32, 32, 0,
                                                  simpl.DataArrayPath(data_container_name, 'EBSD Scan Data',
                                                                      'EulerAngles'),
                                                  simpl.DataArrayPath(data_container_name, 'EBSD Scan Data', 'Phases'),
                                                  simpl.DataArrayPath(data_container_name, 'Phase Data',
                                                                      'CrystalStructures'),
                                                  simpl.DataArrayPath(data_container_name, 'Phase Data',
                                                                      'MaterialName'),
                                                  simpl.DataArrayPath(data_container_name, 'EBSD Scan Data', 'Mask'),
                                                  True, 1)
    assert err == 0, f'WritePoleFigure #2 ErrorCondition: {err}'

if __name__ == '__main__':
    txcopper_exposed()
