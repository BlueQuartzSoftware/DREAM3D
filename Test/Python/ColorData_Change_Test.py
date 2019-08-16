"""

Pipeline example based on Small IN100 Synthetic where IPF Color values are changed
using Numpy and re-added to the attribute matrix

"""

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import samplingpy
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy

# Numpy for some data management assistance
import numpy as np
import datetime as datetime


def color_data_change_test():

    # Create Data Container Array
    dca = simpl.DataContainerArray.New()
    # print("Build Dir: %s" % sd.GetBuildDirectory())
    # ReadAngData
    err = orientationanalysispy.read_ang_data(dca, "EBSD Scan", "Phase Data", "Scan Data",
                                              sd.GetDataDirectory() + "/Data/SmallIN100/Slice_1.ang")
    if err < 0:
        print("ReadAngData ErrorCondition: %d" % err)

    # Rotate Sample Reference Frame
    err = samplingpy.rotate_sample_ref_frame(dca, simpl.DataArrayPath("EBSD Scan", "Scan Data", ""),
                                             simpl.FloatVec3Type([0.0, 1.0, 0.0]), 180.0, False)
    if err < 0:
        print("RotateSampleRefFrame ErrorCondition: %d" % err)

    # Rotate Euler Reference Frame
    err = orientationanalysispy.rotate_euler_ref_frame(dca, simpl.FloatVec3Type([0.0, 0.0, 1.0]), 90.0,
                                                       simpl.DataArrayPath("EBSD Scan", "Scan Data", "EulerAngles"))
    if err < 0:
        print("RotateEulerRefFrame ErrorCondition: %d" % err)

    # Threshold Objects
    err = sc.MultiThresholdObjects(dca, "Mask", [("EBSD Scan", "Scan Data", "Confidence Index", ">", 0.1)])
    if err < 0:
        print("MultiThresholdObjects ErrorCondition: %d" % err)

    # Generate IPF Colors
    err = orientationanalysispy.generate_ipf_colors(dca, simpl.FloatVec3Type([0, 0, 1]),
                                                    simpl.DataArrayPath("EBSD Scan", "Scan Data", "Phases"),
                                                    simpl.DataArrayPath("EBSD Scan", "Scan Data", "EulerAngles"),
                                                    simpl.DataArrayPath("EBSD Scan", "Phase Data", "CrystalStructures"),
                                                    True,
                                                    simpl.DataArrayPath("EBSD Scan", "Scan Data", "Mask"), "IPFColor")
    if err < 0:
        print("GenerateIPFColors ErrorCondition: %d" % err)

    # ITK Image Writer
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                "/Data/Output/Examples/Slice_1_ColorChange_Before.png",
                                                simpl.DataArrayPath("EBSD Scan", "Scan Data", "IPFColor"), 0)
    if err < 0:
        print("ITKImageWriter[1] ErrorCondition: %d" % err)


    # Change the color data
    am = dca.getAttributeMatrix(simpl.DataArrayPath("EBSD Scan", "Scan Data", ""))
    data_array = am.getAttributeArray("IPFColor")
    comp_dims = data_array.getComponentDimensions()
    data = data_array.Data

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
    newdata = simpl.UInt8ArrayType(np_array, "IPFColor", False)
    # Add the DataArray to the AttributeMatrix
    am.addOrReplaceAttributeArray(newdata)

    # ITK Image Writer
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                "/Data/Output/Examples/Slice_1_ColorChange_After.png",
                                                simpl.DataArrayPath("EBSD Scan", "Scan Data", "IPFColor"), 0)
    if err < 0:
        print("ITKImageWriter[2] ErrorCondition: %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/ColorChanged.dream3d", dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)

"""
Main entry point for python script
"""
if __name__ == "__main__":
    color_data_change_test()


