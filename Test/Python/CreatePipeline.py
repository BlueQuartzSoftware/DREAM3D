'''
This example shows building up an actual pipeline object with filters, 
preflighting that pipeline and then executing that pipeline.
The important note here is that none of the filters run until the pipeline.run()
is executed.

This example also shows reading in a pipeline from a file and executing that pipeline.
** IMPORTANT: If you are blindly reading a pipeline from a file then you should also be
** import'ing all of the modules.
'''
import numpy as np

import simpl
import simplpy as d3d
import simpl_helpers as sc
import simpl_test_dirs as sd

import orientationanalysis
import orientationanalysispy
import itkimageprocessing



def CreatePipeline():
    print("======== Construct Pipeline")
    #Create an Observer to report errors/progress from the executing pipeline
    pipeline = simpl.FilterPipeline()
    obs = simpl.Observer()
    pipeline.addObserver(obs)

    data_container_name =  'EBSD Scan'
    cell_ensemble_attribute_matrix_name = 'Phase Data'
    cell_attribute_matrix_name = 'Scan Data'
    input_file = f'{sd.GetDataDirectory()}/Data/SmallIN100/Slice_1.ang'

    # Import the EDAX data from an .ang file
    filter = orientationanalysis.ReadAngData()
    filter.DataContainerName = data_container_name
    filter.CellEnsembleAttributeMatrixName = cell_ensemble_attribute_matrix_name
    filter.CellAttributeMatrixName = cell_attribute_matrix_name
    filter.InputFile = input_file
    pipeline.pushBack(filter)

    # Rotate the Sample Reference Frame
    filter = simpl.RotateSampleRefFrame()
    filter.CellAttributeMatrixPath = simpl.DataArrayPath(data_container_name, cell_attribute_matrix_name, '')
    filter.RotationAxis = simpl.FloatVec3([0.0, 1.0, 0.0])
    filter.RotationAngle = 180.0
    filter.SliceBySlice = False
    filter.RotationRepresentationChoice = 0
    pipeline.pushBack(filter)

    # Rotate the Crystal reference frame
    filter = orientationanalysis.RotateEulerRefFrame()
    filter.RotationAxis = simpl.FloatVec3([0.0, 0.0, 1.0])
    filter.RotationAngle = 90.0
    filter.CellEulerAnglesArrayPath = simpl.DataArrayPath(data_container_name, cell_attribute_matrix_name, 'EulerAngles')
    pipeline.pushBack(filter)

    # Threshold the Data by Confidence Index
    filter = simpl.MultiThresholdObjects2()
    thresholds = simpl.ComparisonInputsAdvanced()
    topLevelComparisonSet = simpl.ComparisonSet()
    comparisonValue = simpl.ComparisonValue()
    comparisonValue.AttributeArrayName = 'Confidence Index'
    comparisonValue.CompOperator = 1 # '>'
    comparisonValue.CompValue =  0.1
    topLevelComparisonSet.addComparison(comparisonValue)
    thresholds.addInput(topLevelComparisonSet)
    thresholds.DataContainerName = data_container_name
    thresholds.AttributeMatrixName = cell_attribute_matrix_name
    filter.DestinationArrayName = 'Mask'
    filter.SelectedThresholds = thresholds
    pipeline.pushBack(filter)

    # Generate IPF Colors
    filter = orientationanalysis.GenerateIPFColors()
    filter.ReferenceDir = simpl.FloatVec3([0, 0, 1])
    filter.CellPhasesArrayPath = simpl.DataArrayPath(data_container_name, cell_attribute_matrix_name, 'Phases')
    filter.CellEulerAnglesArrayPath = simpl.DataArrayPath(data_container_name, cell_attribute_matrix_name, 'EulerAngles')
    filter.CrystalStructuresArrayPath = simpl.DataArrayPath(data_container_name, cell_ensemble_attribute_matrix_name, 'CrystalStructures')
    filter.UseGoodVoxels = True
    filter.GoodVoxelsArrayPath = simpl.DataArrayPath(data_container_name, cell_attribute_matrix_name, 'Mask')
    filter.CellIPFColorsArrayName = 'IPFColor'
    pipeline.pushBack(filter)

    # ITK Image Writer
    image_file = "{}/Data/Output/Python_Examples/CreatePipeline/Small_IN100_1.png".format(sd.GetBuildDirectory())
    filter = itkimageprocessing.ITKImageWriter()
    filter.registerImageIOFactories()
    filter.FileName = image_file
    filter.ImageArrayPath = simpl.DataArrayPath(data_container_name, cell_attribute_matrix_name, 'IPFColor')
    filter.Plane = 0
    pipeline.pushBack(filter)

    # Write out a .dream3d file which will also save a copy of the pipeline as JSON
    output_file = f'{sd.GetBuildDirectory()}/Data/Output/Python_Examples/CreatePipeline/Small_IN100_1.dream3d'
    filter = simpl.DataContainerWriter()
    filter.OutputFile = output_file
    pipeline.pushBack(filter)

    # We can preflight the pipeline at this point
    pipeline.preflightPipeline()
    print(f'Preflight error = {pipeline.ErrorCode}')

    # We can now run the built up pipeline
    dca = pipeline.run()
    print(f'Execution error = {pipeline.ErrorCode}')

    pwriter = simpl.JsonFilterParametersWriter()
    err = pwriter.exportPipelineToFile(pipeline, sd.GetTestTempDirectory() + "/out.json", "Name of Pipeline", True, obs)


def ReadPipeline():
    print("======== ReadPipeline")
    obs = simpl.IObserver()
    preader = simpl.JsonFilterParametersReader()
    pipeline = preader.readPipelineFromFile(sd.GetTestTempDirectory() + "/out.json", obs)
    print(f"Filters in Pipeline: {pipeline.size()}")

    # We can preflight the pipeline at this point
    pipeline.preflightPipeline()
    print(f'Preflight error = {pipeline.ErrorCode}')

    # We can now run the built up pipeline
    dca = pipeline.run()
    print(f'Execution error = {pipeline.ErrorCode}')

if __name__ == '__main__':
    print('Starting the Pipeline construction example...')

    CreatePipeline()
    ReadPipeline()

    print('[CreatePipeline] Complete')
