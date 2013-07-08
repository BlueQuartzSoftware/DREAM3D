/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include <stdlib.h>
#include <stdio.h>

#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/HDF5/H5FilterParametersReader.h"

#include "H5Support/H5Utilities.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"
#include "GenericFilter.h"


#define StlFilePrefixTestValue "StlFilePrefixTestValue"
#define MaxIterationsTestValue 20
#define MisorientationToleranceTestValue 43.0
#define InputFileTestValue "InputFileTestValue.html"
#define InputPathTestValue "InputPathTestValue.html"
#define OutputFileTestValue "OutputFileTestValue.html"
#define OutputPathTestValue "OutputPathTestValue.html"
#define WriteAlignmentShiftsTestValue true
#define ConversionTypeTestValue 10
#define SelectedCellArrayNameTestValue "SelectedCellArrayNameTestValue"
#define SelectedFieldArrayNameTestValue "SelectedFieldArrayNameTestValue"
#define SelectedEnsembleArrayNameTestValue "SelectedEnsembleArrayNameTestValue"
#define SurfaceMeshPointArrayNameTestValue "SurfaceMeshPointArrayNameTestValue"
#define SurfaceMeshFaceArrayNameTestValue "SurfaceMeshFaceArrayNameTestValue"
#define SurfaceMeshEdgeArrayNameTestValue "SurfaceMeshEdgeArrayNameTestValue"
#define SolidMeshPointArrayNameTestValue "SolidMeshPointArrayNameTestValue"
#define SolidMeshFaceArrayNameTestValue "SolidMeshFaceArrayNameTestValue"
#define SolidMeshEdgeArrayNameTestValue "SolidMeshEdgeArrayNameTestValue"
#define Comparison1InputArrayNameTestValue "Comparison1InputArrayNameTestValue"
#define Comparison1CompOperatorTestValue 12;
#define Comparison1CompValueTestValue 15.0;
#define Comparison2InputArrayNameTestValue "Comparison2InputArrayNameTestValue"
#define Comparison2CompOperatorTestValue 23;
#define Comparison2CompValueTestValue 21.0;
#define AxisAngles1AngleTestValue 2.1f;
#define AxisAngles1HTestValue 5.67f;
#define AxisAngles1KTestValue 7.0f;
#define AxisAngles1LTestValue 3.42f;
#define AxisAngles2AngleTestValue 4.82f;
#define AxisAngles2HTestValue 9.0f;
#define AxisAngles2KTestValue 12.234f;
#define AxisAngles2LTestValue 16.4f;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::FilterParametersReaderTest::OutputFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParametersRWTest()
{
  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();


  GenericFilter::Pointer filt = GenericFilter::New();

  // Set something for each and every property so you have something to compare against.
  // You may want to make some constants for these values
  filt->setStlFilePrefix(StlFilePrefixTestValue);
  filt->setMaxIterations(MaxIterationsTestValue);
  filt->setMisorientationTolerance(MisorientationToleranceTestValue);
  filt->setInputFile(InputFileTestValue);
  filt->setInputPath(InputPathTestValue);
  filt->setOutputFile(OutputFileTestValue);
  filt->setOutputPath(OutputPathTestValue);
  filt->setWriteAlignmentShifts(WriteAlignmentShiftsTestValue);
  filt->setConversionType(ConversionTypeTestValue);
  filt->setSelectedCellArrayName(SelectedCellArrayNameTestValue);
  filt->setSelectedFieldArrayName(SelectedFieldArrayNameTestValue);
  filt->setSelectedEnsembleArrayName(SelectedEnsembleArrayNameTestValue);
  filt->setSurfaceMeshPointArrayName(SurfaceMeshPointArrayNameTestValue);
  filt->setSurfaceMeshFaceArrayName(SurfaceMeshFaceArrayNameTestValue);
  filt->setSurfaceMeshEdgeArrayName(SurfaceMeshEdgeArrayNameTestValue);
  filt->setSolidMeshPointArrayName(SolidMeshPointArrayNameTestValue);
  filt->setSolidMeshFaceArrayName(SolidMeshFaceArrayNameTestValue);
  filt->setSolidMeshEdgeArrayName(SolidMeshEdgeArrayNameTestValue);

  ComparisonInput_t comparison1;
  comparison1.arrayName = Comparison1InputArrayNameTestValue;
  comparison1.compOperator = Comparison1CompOperatorTestValue;
  comparison1.compValue = Comparison1CompValueTestValue;

  ComparisonInput_t comparison2;
  comparison2.arrayName = Comparison2InputArrayNameTestValue;
  comparison2.compOperator = Comparison2CompOperatorTestValue;
  comparison2.compValue = Comparison2CompValueTestValue;

  std::vector<ComparisonInput_t> comparisonVector;
  comparisonVector.push_back(comparison1);
  comparisonVector.push_back(comparison2);
  filt->setCellComparisonInputs(comparisonVector);

  AxisAngleInput_t axisAngles1;
  axisAngles1.angle = AxisAngles1AngleTestValue;
  axisAngles1.h = AxisAngles1HTestValue;
  axisAngles1.k = AxisAngles1KTestValue;
  axisAngles1.l = AxisAngles1LTestValue;

  AxisAngleInput_t axisAngles2;
  axisAngles2.angle = AxisAngles2AngleTestValue;
  axisAngles2.h = AxisAngles2HTestValue;
  axisAngles2.k = AxisAngles2KTestValue;
  axisAngles2.l = AxisAngles2LTestValue;

  std::vector<AxisAngleInput_t> axisAngleInputsVector;
  axisAngleInputsVector.push_back(axisAngles1);
  axisAngleInputsVector.push_back(axisAngles2);
  filt->setAxisAngleRotations(axisAngleInputsVector);

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FilterParametersReaderTest::OutputFile);

  pipeline->pushBack(filt);
  pipeline->pushBack(writer);

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)


  // We are done writing a file, now we need to read the file using raw HDF5 codes

  hid_t fid = H5Utilities::openFile(UnitTest::FilterParametersReaderTest::OutputFile);
  DREAM3D_REQUIRED(fid, >, 0)

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  hid_t pipelineGroupId = H5Gopen(fid, DREAM3D::HDF5::PipelineGroupName.c_str(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);

  err = reader->openOptionsGroup( filt.get() ); // Open the HDF5 Group for this filter
  DREAM3D_REQUIRED(err, >=, 0)

  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get() );

  // Now one by one, compare each of the filter parameters that you have stored in some constant somewhere to the values that are now in the filt variable.
  // Use DREAM3D_REQUIRED() to make sure each one is what you think it is.
  DREAM3D_REQUIRED(StlFilePrefixTestValue, ==, filt->getStlFilePrefix() )
  DREAM3D_REQUIRED(MaxIterationsTestValue, ==, filt->getMaxIterations() )
  DREAM3D_REQUIRED(MisorientationToleranceTestValue, ==, filt->getMisorientationTolerance() )
  DREAM3D_REQUIRED(InputFileTestValue, ==, filt->getInputFile() )
  DREAM3D_REQUIRED(InputPathTestValue, ==, filt->getInputPath() )
  DREAM3D_REQUIRED(OutputFileTestValue, ==, filt->getOutputFile() )
  DREAM3D_REQUIRED(OutputPathTestValue, ==, filt->getOutputPath() )
  DREAM3D_REQUIRED(WriteAlignmentShiftsTestValue, ==, filt->getWriteAlignmentShifts() )
  DREAM3D_REQUIRED(ConversionTypeTestValue, ==, filt->getConversionType() )
  DREAM3D_REQUIRED(SelectedCellArrayNameTestValue, ==, filt->getSelectedCellArrayName() )
  DREAM3D_REQUIRED(SelectedFieldArrayNameTestValue, ==, filt->getSelectedFieldArrayName() )
  DREAM3D_REQUIRED(SelectedEnsembleArrayNameTestValue, ==, filt->getSelectedEnsembleArrayName() )
  DREAM3D_REQUIRED(SurfaceMeshPointArrayNameTestValue, ==, filt->getSurfaceMeshPointArrayName() )
  DREAM3D_REQUIRED(SurfaceMeshFaceArrayNameTestValue, ==, filt->getSurfaceMeshFaceArrayName() )
  DREAM3D_REQUIRED(SurfaceMeshEdgeArrayNameTestValue, ==, filt->getSurfaceMeshEdgeArrayName() )
  DREAM3D_REQUIRED(SolidMeshPointArrayNameTestValue, ==, filt->getSolidMeshPointArrayName() )
  DREAM3D_REQUIRED(SolidMeshFaceArrayNameTestValue, ==, filt->getSolidMeshFaceArrayName() )
  DREAM3D_REQUIRED(SolidMeshEdgeArrayNameTestValue, ==, filt->getSolidMeshEdgeArrayName() )

  // Test the CellComparisonInputs widget
  std::vector<ComparisonInput_t> comparisonVectorRead = filt->getCellComparisonInputs();
  ComparisonInput_t comparison1Read = comparisonVectorRead[0];
  ComparisonInput_t comparison2Read = comparisonVectorRead[1];

  DREAM3D_REQUIRED(comparison1.arrayName, ==, comparison1Read.arrayName)
  DREAM3D_REQUIRED(comparison1.compOperator, ==, comparison1Read.compOperator)
  DREAM3D_REQUIRED(comparison1.compValue, ==, comparison1Read.compValue)
  DREAM3D_REQUIRED(comparison2.arrayName, ==, comparison2Read.arrayName)
  DREAM3D_REQUIRED(comparison2.compOperator, ==, comparison2Read.compOperator)
  DREAM3D_REQUIRED(comparison2.compValue, ==, comparison2Read.compValue)

  // Test the AxisAngleInput widget
  std::vector<AxisAngleInput_t> axisAngleVectorRead = filt->getAxisAngleRotations();
  AxisAngleInput_t axisAngles1Read = axisAngleVectorRead[0];
  AxisAngleInput_t axisAngles2Read = axisAngleVectorRead[1];

  DREAM3D_REQUIRED(axisAngles1.angle, ==, axisAngles1Read.angle)
  DREAM3D_REQUIRED(axisAngles1.h, ==, axisAngles1Read.h)
  DREAM3D_REQUIRED(axisAngles1.k, ==, axisAngles1Read.k)
  DREAM3D_REQUIRED(axisAngles1.l, ==, axisAngles1Read.l)
  DREAM3D_REQUIRED(axisAngles2.angle, ==, axisAngles2Read.angle)
  DREAM3D_REQUIRED(axisAngles2.h, ==, axisAngles2Read.h)
  DREAM3D_REQUIRED(axisAngles2.k, ==, axisAngles2Read.k)
  DREAM3D_REQUIRED(axisAngles2.l, ==, axisAngles2Read.l)

  err = reader->closeOptionsGroup(); // Close the HDF5 group for this filter
  DREAM3D_REQUIRED(err, >=, 0)


  H5Gclose(pipelineGroupId); // Closes the "Pipeline" group
  H5Fclose(fid); // Closes the file
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( FilterParametersRWTest() )


  PRINT_TEST_SUMMARY();
  return err;
}
