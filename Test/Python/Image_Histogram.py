"""

Pipeline example based on Image_Histogram

"""

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_test_dirs as sd
from dream3d import statisticspy
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy


def image_histogram():

    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # ITK Image Reader
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_reader(dca, sd.GetDataDirectory() + "/Data/Image/slice_11.tif",
                                                "ImageDataContainer", "CellData", "ImageData")
    if err < 0:
        print("ITKImageReader ErrorCondition %d" % err)

    # Calculate Frequency Histogram
    err = statisticspy.calculate_array_histogram(dca, simpl.DataArrayPath("ImageDataContainer", "CellData",
                                                                          "ImageData"),
                                                 256, 0, 256, True, False, "HistogramAttributeMatrix",
                                                 "Image_Histogram", False, "")
    if err < 0:
        print("CalculateArrayHistogram ErrorCondition %d" % err)

    # Export ASCII Data
    # Output Style = 0 Means Multiple Files, Style = 1 Means a Single file
    outStyle = 0 
    selected_data_array_paths = [simpl.DataArrayPath("ImageDataContainer", "HistogramAttributeMatrix",
                                                     "Image_Histogram")]
    err = simplpy.write_ascii_data(dca, selected_data_array_paths,
                                   sd.GetBuildDirectory() + "/Data/Output/Histograms",
                                   sd.GetBuildDirectory() + "/Data/Output/Histograms/Histogram.csv",
                                   simpl.DelimiterTypes.Comma, ".csv", 10, outStyle)
    if err < 0:
        print("WriteAsciiData ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    image_histogram()


