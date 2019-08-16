"""

Pipeline example based on Edax_IPF_Colors

"""

#import matplotlib.pyplot as plt

try:
    import numpy as np
except ImportError:
    raise RuntimeError("This module depends on the numpy module. Please make\
sure that it is installed properly.")


from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import samplingpy
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy


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
                                                "/Data/Output/Examples/Small_IN100_Slice_1.png",
                                                simpl.DataArrayPath("EBSD Scan", "Scan Data", "IPFColor"), 0)
    if err < 0:
        print("ITKImageWriter ErrorCondition: %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/Examples/Slice_1.dream3d", dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


    # This section shows using MatPlotLib to graph the histogram.
    # dc = dca.getDataContainer("EBSD Scan")
    # am = dc.getAttributeMatrix("Scan Data")
    # iq = am.getAttributeArray("Image Quality")
    # plt.hist(np.asarray(iq), 256)
    # plt.show()


"""
Main entry point for python script
"""
if __name__ == "__main__":
    color_data_change_test()


