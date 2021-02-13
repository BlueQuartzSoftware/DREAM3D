# Based on Image Segmentation example
# 
# These are the simpl_py python modules

import simpl
import simplpy
import simpl_helpers as sh
import simpl_test_dirs as sd
import itkimageprocessing
import itkimageprocessingpy
import reconstructionpy
import statisticspy
import genericpy
import orientationanalysispy

'''
Pipeline Annotation:
--------------------
This example pipeline shows how to read in a 3D stack of images, segment them and perform 
some basic size statistics on the segmented features. Since this data set is from an image 
there is no phase data which is created in a filter and phase data is also generated during
the pipeline. Note the technique for accomplishing this as it is handy to do for other pipelines.
'''
def image_segmentation():
      
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Register the ImageIO Factories
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()

    # Common variables
    data_container_name = 'RoboMet.3D Image Stack'
    attr_matrix_name = 'Optical Data'

    # Import Image Stack [ITK]
    file_list_info = simpl.StackFileListInfo(2, 0, 11, 174, 1, sd.GetBuildDirectory() + '/Data/Image',
                                        'slice_', '', 'tif')
    err = itkimageprocessingpy.itk_import_image_stack(dca, data_container_name, attr_matrix_name,
                                                      simpl.FloatVec3([0, 0, 0]), simpl.FloatVec3([1, 1, 1]),
                                                      file_list_info, 164, 'ImageData')
    assert err == 0, f'ITK Import Image Stack ErrorCondition {err}'

    # MultiThresholdObjects filter
    err = sh.MultiThresholdObjects(dca, 'Mask',
                                  [(data_container_name, attr_matrix_name, 'ImageData', 2, 0)])
    assert err == 0, f'MultiThresholdObjects ErrorCondition: {err}'

    # Segment Features (Scalar)
    err = reconstructionpy.scalar_segment_features(dca, 'Pore Data',
                                                   simpl.DataArrayPath(data_container_name,
                                                                       attr_matrix_name, 'ImageData'),
                                                   10, True, simpl.DataArrayPath(data_container_name,
                                                                                 attr_matrix_name, 'Mask'),
                                                   'FeatureIds', 'Active')
    assert err == 0, f'ScalarSegmentFeatures ErrorCondition {err}'

    # Find Feature Sizes
    err = statisticspy.find_sizes(dca, simpl.DataArrayPath(data_container_name, 'Pore Data', ''),
                                  simpl.DataArrayPath(data_container_name, attr_matrix_name, 'FeatureIds'),
                                  'Volumes', 'EquivalentDiameters', 'NumElements', False)
    assert err == 0, f'FindSizes ErrorCondition {err}'

    # Copy Feature Array To Element Array
    err = simplpy.copy_feature_array_to_element_array(dca,
                                                      simpl.DataArrayPath(data_container_name,
                                                                          'Pore Data',
                                                                          'EquivalentDiameters'),
                                                      simpl.DataArrayPath(data_container_name,
                                                                          attr_matrix_name,
                                                                          'FeatureIds'),
                                                      'EquivalentDiameters')
    assert err == 0, f'CopyFeatureArrayToElementArray ErrorCondition {err}'

    # Create Data Array
    err = simplpy.create_data_array(dca, simpl.ScalarTypes.Int32, 1,
                                    simpl.DataArrayPath(data_container_name, attr_matrix_name, 'Phases'),
                                    simpl.InitializationType.Manual, '1', (0.0, 151.1))
    assert err == 0, f'DataArray ErrorCondition: {err}'
    
    # Replace Value in Array (Conditional)
    err = simplpy.conditional_set_value(dca, simpl.DataArrayPath(data_container_name,
                                                                 attr_matrix_name,
                                                                 'Phases'),
                                        simpl.DataArrayPath(data_container_name,
                                                            attr_matrix_name,
                                                            'Mask'), 2)
    assert err == 0, f'ConditionalSetValue ErrorCondition: {err}'

    # Find Feature Phases
    err = genericpy.find_feature_phases(dca, simpl.DataArrayPath(data_container_name, attr_matrix_name, 'FeatureIds'),
                                        simpl.DataArrayPath(data_container_name, attr_matrix_name, 'Phases'),
                                        simpl.DataArrayPath(data_container_name, 'Pore Data', 'Phases'))
    assert err == 0, f'FindFeaturePhases ErrorCondition {err}'

    # Find Feature Centroids
    err = genericpy.find_feature_centroids(dca, simpl.DataArrayPath(data_container_name, attr_matrix_name,
                                                                    'FeatureIds'),
                                           simpl.DataArrayPath(data_container_name, 'Pore Data', 'Centroids'))
    assert err == 0, f'FindFeatureCentroids ErrorCondition {err}'
    
    # Create Ensemble Info
    ensemble_info = simpl.EnsembleInfo()
    ensemble_info.addValues(simpl.CrystalStructure.Cubic_High, simpl.PhaseType.Matrix, 'Matrix')
    ensemble_info.addValues(simpl.CrystalStructure.Cubic_High, simpl.PhaseType.Precipitate, 'Pores')
    err = orientationanalysispy.create_ensemble_info(dca, data_container_name, 'EnsembleAttributeMatrix',
                                                     ensemble_info, 'CrystalStructures', 'PhaseTypes', 'PhaseNames')
    assert err == 0, f'CreateEnsembleInfo ErrorCondition {err}'

    # Find Feature Clustering
    err = statisticspy.find_feature_clustering(dca, 10, 2, simpl.DataArrayPath(data_container_name,
                                                                               'EnsembleAttributeMatrix', ''),
                                               False, simpl.DataArrayPath('', '', ''), 
                                               simpl.DataArrayPath(data_container_name, 'Pore Data',
                                                                   'EquivalentDiameters'),
                                               simpl.DataArrayPath(data_container_name, 'Pore Data', 'Phases'),
                                               simpl.DataArrayPath(data_container_name, 'Pore Data', 'Centroids'),
                                               'ClusteringList', 'RDF', 'RDFMaxMinDistances')
    assert err == 0, f'FindFeatureClustering ErrorCondition {err}'

    # Write to DREAM3D file
    err = sh.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/ImagesStack/Images.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    image_segmentation()
