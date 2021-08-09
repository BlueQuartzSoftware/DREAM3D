'''
Pipeline example based on Image_Histogram
'''

import simpl
import simplpy
import simpl_test_dirs as sd
import statstoolboxpy
import itkimageprocessing
import itkimageprocessingpy

def image_histogram():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # ITK Image Reader
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_reader(dca, sd.GetDataDirectory() + '/Data/Image/slice_11.tif',
                                                'ImageDataContainer', 'CellData', 'ImageData')
    assert err == 0, f'ITKImageReader ErrorCondition {err}'

    # Calculate Frequency Histogram
    err = statstoolboxpy.calculate_array_histogram(dca, simpl.DataArrayPath('ImageDataContainer', 'CellData',
                                                                          'ImageData'),
                                                 256, 0, 256, True, False, 'HistogramAttributeMatrix',
                                                 'Image_Histogram', False, '')
    assert err == 0, f'CalculateArrayHistogram ErrorCondition {err}'

    # Export ASCII Data
    # Output Style = 0 Means Multiple Files, Style = 1 Means a Single file
    outStyle = 0 
    selected_data_array_paths = [simpl.DataArrayPath('ImageDataContainer', 'HistogramAttributeMatrix',
                                                     'Image_Histogram')]
    err = simplpy.write_ascii_data(dca, selected_data_array_paths,
                                   sd.GetBuildDirectory() + '/Data/Output/Histograms',
                                   sd.GetBuildDirectory() + '/Data/Output/Histograms/Histogram.csv',
                                   simpl.DelimiterTypes.Comma, '.csv', 10, outStyle)
    assert err == 0, f'WriteAsciiData ErrorCondition: {err}'

if __name__ == '__main__':
    image_histogram()
