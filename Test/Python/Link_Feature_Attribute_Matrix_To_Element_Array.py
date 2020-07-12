# Pipeline : Link Feature Attribute Matrix to Element Attribute Array Filter Test
# Based on (01) SmallIN100 Morphological Statistics (from EBSD Statistics)

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd
import genericpy as generic
import statisticspy as statistics

def link_feature_map_to_element_array_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Confidence Index').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'EulerAngles').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Fit').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'IPFColor').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Image Quality').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Mask').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Phases').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Quats').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'SEM Signal').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'FeatureIds').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'ParentIds').Flag = 2

    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Phase Data').getDataArrayProxy(
        'CrystalStructures').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Phase Data').getDataArrayProxy(
        'LatticeConstants').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Phase Data').getDataArrayProxy(
        'MaterialName').Flag = 2

    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy('Active').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'AvgEuler').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'AvgQuats').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'NumNeighbors2').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'ParentIds').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy('Phases').Flag = 2

    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('NewGrain Data').getDataArrayProxy(
        'Active').Flag = 2

    err = simplpy.data_container_reader(dca,
                                        sd.GetBuildDirectory() +
                                        '/Data/Output/Reconstruction/SmallIN100_Final.dream3d',
                                        False, dcap)
    assert err == 0, f'DataContainerReader ErrorCondition {err}'

    # Find Feature Centroids
    err = generic.find_feature_centroids(dca, simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                         simpl.DataArrayPath('Small IN100', 'Grain Data', 'Centroids'))
    assert err == 0, f'FindFeatureCentroids ErrorCondition {err}'

    # Find Feature Sizes
    err = statistics.find_sizes(dca, simpl.DataArrayPath('Small IN100', 'Grain Data', ''),
                                simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                'Size Volumes', 'EquivalentDiameters', 'NumElements', False)
    assert err == 0, f'FindSizes ErrorCondition {err}'

    # Find Feature Shapes
    err = statistics.find_shapes(dca, simpl.DataArrayPath('Small IN100', 'Grain Data', ''),
                                 simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                 simpl.DataArrayPath('Small IN100', 'Grain Data', 'Centroids'),
                                 'Omega3s', 'Shape Volumes', 'AxisLengths', 'AxisEulerAngles', 'AspectRatios')
    assert err == 0, f'FindSizes ErrorCondition {err}'

    # Find Feature Neighbors
    err = statistics.find_neighbors(dca, simpl.DataArrayPath('Small IN100', 'Grain Data', ''),
                                    'SharedSurfaceAreaList', 'NeighborList',
                                    simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                    '', 'NumNeighbors', '', False, False)
    assert err == 0, f'FindNeighbors ErrorCondition {err}'

    # Find Feature Neighborhoods
    err = statistics.find_neighborhoods(dca, 'NeighborhoodList', 1,
                                        simpl.DataArrayPath('Small IN100', 'Grain Data', 'EquivalentDiameters'),
                                        simpl.DataArrayPath('Small IN100', 'Grain Data', 'Phases'),
                                        simpl.DataArrayPath('Small IN100', 'Grain Data', 'Centroids'),
                                        'Neighborhoods')
    assert err == 0, f'FindNeighborhoods ErrorCondition {err}'

    # Find Euclidean Distance Map
    err = statistics.find_euclidean_dist_map(dca,
                                             simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                             'GBManhattanDistances', 'TJManhattanDistances', 'QPManhattanDistances',
                                             'NearestNeighbors', True, True, True, False, True)
    assert err == 0, f'FindEuclideanDistanceMap ErrorCondition {err}'

    # Find Surface Area to Volume and Sphericity
    err = statistics.find_surface_area_to_volume(dca,
                                                 simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'FeatureIds'),
                                                 simpl.DataArrayPath('Small IN100', 'Grain Data', 'NumElements'),
                                                 'SurfaceAreaVolumeRatio', 'Sphericity', True)
    assert err == 0, f'FindSurfaceAreaToVolume ErrorCondition {err}'

    # Link Feature Attribute Matrix to Element Attribute Array 
    err = simplpy.link_feature_map_to_element_array(dca, 'FeatureAttributeMatrix',
                                                    simpl.DataArrayPath('Small IN100', 
                                                                        'EBSD Scan Data',
                                                                        'FeatureIds'),
                                                    'Active')
    assert err == 0, f'LinkFeatureMapToElementArray ErrorCondition {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() +
                              '/Data/Output/CoreFilterTests/' + 
                              'LinkFeatureMapToElementArray.dream3d',
                              dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    link_feature_map_to_element_array_test()
