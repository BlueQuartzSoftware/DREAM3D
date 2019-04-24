# Based on Image Segmentation example
# 
# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy
from dream3d import reconstructionpy
from dream3d import statisticspy
from dream3d import genericpy
from dream3d import orientationanalysispy

"""
Pipeline Annotation:
--------------------
This example pipeline shows how to read in a 3D stack of images, segment them and perform 
some basic size statistics on the segmented features. Since this data set is from an image 
there is no phase data which is created in a filter and phase data is also generated during
the pipeline. Note the technique for accomplishing this as it is handy to do for other pipelines.
"""


def image_segmentation():
      
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Register the ImageIO Factories
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()

    # Common variables
    data_container_name = "RoboMet.3D Image Stack"
    attr_matrix_name = "Optical Data"

    # Import Image Stack [ITK]
    file_list_info = simpl.FileListInfo(2, 0, 11, 174, 1, sd.GetBuildDirectory() + "/Data/Image",
                                        "slice_", "", "tif")
    err = itkimageprocessingpy.itk_import_image_stack(dca, data_container_name, attr_matrix_name,
                                                      simpl.FloatVec3Type([0, 0, 0]), simpl.FloatVec3Type([1, 1, 1]),
                                                      file_list_info, 164, "ImageData")
    if err < 0:
        print("ITK Import Image Stack ErrorCondition %d" % err)

    # MultiThresholdObjects filter
    err = sc.MultiThresholdObjects(dca, "Mask",
                                  [(data_container_name, attr_matrix_name, "ImageData", 2, 0)])
    if err < 0:
        print("MultiThresholdObjects ErrorCondition: %d" % err)

    # Segment Features (Scalar)
    err = reconstructionpy.scalar_segment_features(dca, "Pore Data",
                                                   simpl.DataArrayPath(data_container_name,
                                                                       attr_matrix_name, "ImageData"),
                                                   10, True, simpl.DataArrayPath(data_container_name,
                                                                                 attr_matrix_name, "Mask"),
                                                   "FeatureIds", "Active")
    if err < 0:
        print("ScalarSegmentFeatures ErrorCondition %d" % err)

    # Find Feature Sizes
    err = statisticspy.find_sizes(dca, simpl.DataArrayPath(data_container_name, "Pore Data", ""),
                                  simpl.DataArrayPath(data_container_name, attr_matrix_name, "FeatureIds"),
                                  "Volumes", "EquivalentDiameters", "NumElements", False)
    if err < 0:
        print("FindSizes ErrorCondition %d" % err)

    # Copy Feature Array To Element Array
    err = simplpy.copy_feature_array_to_element_array(dca,
                                                      simpl.DataArrayPath(data_container_name,
                                                                          "Pore Data",
                                                                          "EquivalentDiameters"),
                                                      simpl.DataArrayPath(data_container_name,
                                                                          attr_matrix_name,
                                                                          "FeatureIds"),
                                                      "EquivalentDiameters")
    if err < 0:
        print("CopyFeatureArrayToElementArray ErrorCondition %d" % err)

    # Create Data Array
    err = simplpy.create_data_array(dca, simpl.ScalarTypes.Int32, 1,
                                    simpl.DataArrayPath(data_container_name, attr_matrix_name, "Phases"),
                                    simpl.InitializationType.Manual, "1", (0.0, 151.1))
    if err < 0:
        print("DataArray ErrorCondition: %d" % err)
    
    # Replace Value in Array (Conditional)
    err = simplpy.conditional_set_value(dca, simpl.DataArrayPath(data_container_name,
                                                                 attr_matrix_name,
                                                                 "Phases"),
                                        simpl.DataArrayPath(data_container_name,
                                                            attr_matrix_name,
                                                            "Mask"), 2)
    if err < 0:
        print("ConditionalSetValue ErrorCondition: %d" % err)

    # Find Feature Phases
    err = genericpy.find_feature_phases(dca, simpl.DataArrayPath(data_container_name, attr_matrix_name, "FeatureIds"),
                                        simpl.DataArrayPath(data_container_name, attr_matrix_name, "Phases"),
                                        simpl.DataArrayPath(data_container_name, "Pore Data", "Phases"))
    if err < 0:
        print("FindFeaturePhases ErrorCondition %d" % err)

    # Find Feature Centroids
    err = genericpy.find_feature_centroids(dca, simpl.DataArrayPath(data_container_name, attr_matrix_name,
                                                                    "FeatureIds"),
                                           simpl.DataArrayPath(data_container_name, "Pore Data", "Centroids"))
    if err < 0:
        print("FindFeatureCentroids ErrorCondition %d" % err)
    
    # Create Ensemble Info
    ensemble_info = simpl.EnsembleInfo()
    ensemble_info.addValues(simpl.CrystalStructure.Cubic_High, simpl.PhaseType.Matrix, "Matrix")
    ensemble_info.addValues(simpl.CrystalStructure.Cubic_High, simpl.PhaseType.Precipitate, "Pores")
    err = orientationanalysispy.create_ensemble_info(dca, data_container_name, "EnsembleAttributeMatrix",
                                                     ensemble_info, "CrystalStructures", "PhaseTypes", "PhaseNames")
    if err < 0:
        print("CreateEnsembleInfo ErrorCondition %d" % err)

    # Find Feature Clustering
    err = statisticspy.find_feature_clustering(dca, 10, 2, simpl.DataArrayPath(data_container_name,
                                                                               "EnsembleAttributeMatrix", ""),
                                               False, simpl.DataArrayPath("", "", ""), 
                                               simpl.DataArrayPath(data_container_name, "Pore Data",
                                                                   "EquivalentDiameters"),
                                               simpl.DataArrayPath(data_container_name, "Pore Data", "Phases"),
                                               simpl.DataArrayPath(data_container_name, "Pore Data", "Centroids"),
                                               "ClusteringList", "RDF", "RDFMaxMinDistances")
    if err < 0:
        print("FindFeatureClustering ErrorCondition %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/ImagesStack/Images.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    image_segmentation()
