"""

Pipeline example based on Import_ASCII

"""

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy


def import_ascii():

    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Pipeline Annotation:
    """
    This pipeline shows how to import column oriented data into DREAM.3D.

    # Create DataContainer #
    We need somewhere to store the data

    # Create Geometry #
    Create the proper geometry that the data is stored on

    # Import ASCII Data #
    This imports all the data from the .ang file into DREAM.3D. **Note that there is a filter specific for this and you should use that filter. We are just using that file because it is a convenient file that is distributed with DREAM.3D**

    # Combine Attribute Arrrays #
    This will take the phi1, PHI, phi2 arrays and combine them into a single 3 Component AttributeArray
    """

    # Create the Data Container
    err = simplpy.create_data_container(dca, "DataContainer")
    if err < 0:
        print("DataContainer ErrorCondition: %d" % err)

    # Create Geometry
    err = sc.CreateGeometry(dca, sc.ArrayHandling.CopyArrays, simpl.IGeometry.Type.Image, "DataContainer", False, 
                            dimensions=simpl.IntVec3Type([189, 201, 1]), origin=simpl.FloatVec3Type([0, 0, 0]),
                            spacing=simpl.FloatVec3Type([0.25, 0.25, 1]), cell_attribute_matrix_name="CellData")
    if err < 0:
        print("Create Geometry -  ErrorCondition: %d" % err)

    # Import ASCII Data- Vertex Coordinates (Using helper function)
    importFile = sd.GetDataDirectory() + "/Data/SmallIN100/Slice_1.ang"

    err = simplpy.read_ascii_data(dca, sc.CreateAsciiWizardData(importFile, 33, 38021, [' '], True, True, 
                                  simpl.DataArrayPath("DataContainer", "CellData", ""),
                                  ["phi1", "Phi", "phi2", "x", "y", "Image Quality",
                                   "Confidence Index", "Phase", "SEM Signal", "Fit"],
                                  3, [37989], ["float", "float", "float", "float", "float",
                                  "float", "float", "int32_t", "float", "float"]))
    if err < 0:
        print("Import ASCII Data -  ErrorCondition: %d" % err)
    
    # Combine Attribute Arrays:
    selected_data_array_paths = [simpl.DataArrayPath("DataContainer", "CellData", "phi1"),
                                 simpl.DataArrayPath("DataContainer", "CellData", "Phi"),
                                 simpl.DataArrayPath("DataContainer", "CellData", "phi1")]
    err = simplpy.combine_attribute_arrays(dca, selected_data_array_paths, "Eulers", False)
    if err < 0:
        print("Combined Attribute Arrays -  ErrorCondition: %d" % err)

    # Delete Data
    # Remove array helper function:
    err = sc.RemoveArrays(dca, [("DataContainer", "CellData", "phi1"), ("DataContainer", "CellData", "phi2"),
                                ("DataContainer", "CellData", "Phi")])
    if err < 0:
        print("Remove Arrays -  ErrorCondition: %d" % err)

    # Create Ensemble Info
    ensemble_info = simpl.EnsembleInfo()
    ensemble_info.addValues(simpl.CrystalStructure.Cubic_High, simpl.PhaseType.Primary, "Nickel")
    err = orientationanalysispy.create_ensemble_info(dca, "DataContainer", "EnsembleAttributeMatrix",
                                                     ensemble_info, "CrystalStructures", "PhaseTypes", "PhaseNames")
    if err < 0:
        print("CreateEnsembleInfo ErrorCondition %d" % err)

    # Replace Value in Array
    err = simplpy.replace_value_in_array(dca, simpl.DataArrayPath("DataContainer", "CellData", "Phase"),
                                         0, 1)
    if err < 0:
        print("ReplaceValueInArray ErrorCondition %d" % err)

    # Generate IPF Colors
    err = orientationanalysispy.generate_ipf_colors(dca, simpl.FloatVec3Type([0, 0, 1]),
                                                    simpl.DataArrayPath("DataContainer", "CellData", "Phase"),
                                                    simpl.DataArrayPath("DataContainer", "CellData", "Eulers"),
                                                    simpl.DataArrayPath("DataContainer",
                                                                        "EnsembleAttributeMatrix",
                                                                        "CrystalStructures"),
                                                    False,
                                                    simpl.DataArrayPath("", "", ""), "IPFColor")
    if err < 0:
        print("GenerateIPFColors ErrorCondition: %d" % err)

    # ITK Image Writer
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()
    err = itkimageprocessingpy.itk_image_writer(dca, sd.GetBuildDirectory() +
                                                "/Data/Output/Example/Import_ASCII_IPF.png",
                                                simpl.DataArrayPath("DataContainer", "CellData", "IPFColor"), 0)
    if err < 0:
        print("ITKImageWriter ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    import_ascii()


