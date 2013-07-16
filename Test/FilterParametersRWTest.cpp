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
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/HDF5/H5FilterParametersReader.h"

#include "H5Support/H5Utilities.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"
#include "TestFilters/GenericFilter.h"
#include "TestFilters/ArraySelectionExample.h"
#include "TestFilters/TestFilters.h"


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
#define CellComparisonInputsArrayName1 "CellComparisonInputsArrayName1"
#define CellComparisonInputsCompOperator1 0
#define CellComparisonInputsCompValue1 15.0
#define CellComparisonInputsArrayName2 "CellComparisonInputsArrayName2"
#define CellComparisonInputsCompOperator2 1
#define CellComparisonInputsCompValue2 21.0
#define FieldComparisonInputsArrayName1 "FieldComparisonInputsArrayName1"
#define FieldComparisonInputsCompOperator1 2
#define FieldComparisonInputsCompValue1 13.4
#define FieldComparisonInputsArrayName2 "FieldComparisonInputsArrayName2"
#define FieldComparisonInputsCompOperator2 0
#define FieldComparisonInputsCompValue2 45.6
#define EnsembleComparisonInputsArrayName1 "EnsembleComparisonInputsArrayName1"
#define EnsembleComparisonInputsCompOperator1 1
#define EnsembleComparisonInputsCompValue1 13.6
#define EnsembleComparisonInputsArrayName2 "EnsembleComparisonInputsArrayName2"
#define EnsembleComparisonInputsCompOperator2 2
#define EnsembleComparisonInputsCompValue2 45.9
#define PointComparisonInputsArrayName1 "PointComparisonInputsArrayName1"
#define PointComparisonInputsCompOperator1 0
#define PointComparisonInputsCompValue1 67.0
#define PointComparisonInputsArrayName2 "PointComparisonInputsArrayName2"
#define PointComparisonInputsCompOperator2 1
#define PointComparisonInputsCompValue2 52.1
#define FaceComparisonInputsArrayName1 "FaceComparisonInputsArrayName1"
#define FaceComparisonInputsCompOperator1 2
#define FaceComparisonInputsCompValue1 12.0
#define FaceComparisonInputsArrayName2 "FaceComparisonInputsArrayName2"
#define FaceComparisonInputsCompOperator2 0
#define FaceComparisonInputsCompValue2 13.1
#define EdgeComparisonInputsArrayName1 "EdgeComparisonInputsArrayName1"
#define EdgeComparisonInputsCompOperator1 1
#define EdgeComparisonInputsCompValue1 13.0
#define EdgeComparisonInputsArrayName2 "EdgeComparisonInputsArrayName2"
#define EdgeComparisonInputsCompOperator2 2
#define EdgeComparisonInputsCompValue2 14.1
#define AxisAngles1AngleTestValue 2.1f
#define AxisAngles1HTestValue 5.67f
#define AxisAngles1KTestValue 7.0f
#define AxisAngles1LTestValue 3.42f
#define AxisAngles2AngleTestValue 4.82f
#define AxisAngles2HTestValue 9.0f
#define AxisAngles2KTestValue 12.234f
#define AxisAngles2LTestValue 16.4f
#define IntWidgetXTestValue 3
#define IntWidgetYTestValue 4
#define IntWidgetZTestValue 5
#define FloatWidgetXTestValue 6.0f
#define FloatWidgetYTestValue 7.234f
#define FloatWidgetZTestValue 12.3f
#define SelectedVoxelCellArraysString1 "SelectedVoxelCellArraysString1"
#define SelectedVoxelCellArraysString2 "SelectedVoxelCellArraysString2"
#define SelectedVoxelCellArraysString3 "SelectedVoxelCellArraysString3"
#define SelectedVoxelCellArraysString4 "SelectedVoxelCellArraysString4"
#define SelectedVoxelFieldArraysString1 "SelectedVoxelFieldArraysString1"
#define SelectedVoxelFieldArraysString2 "SelectedVoxelFieldArraysString2"
#define SelectedVoxelFieldArraysString3 "SelectedVoxelFieldArraysString3"
#define SelectedVoxelFieldArraysString4 "SelectedVoxelFieldArraysString4"
#define SelectedVoxelEnsembleArraysString1 "SelectedVoxelEnsembleArraysString1"
#define SelectedVoxelEnsembleArraysString2 "SelectedVoxelEnsembleArraysString2"
#define SelectedVoxelEnsembleArraysString3 "SelectedVoxelEnsembleArraysString3"
#define SelectedVoxelEnsembleArraysString4 "SelectedVoxelEnsembleArraysString4"
#define SelectedSurfaceMeshVertexArraysString1 "SelectedSurfaceMeshVertexArraysString1"
#define SelectedSurfaceMeshVertexArraysString2 "SelectedSurfaceMeshVertexArraysString2"
#define SelectedSurfaceMeshVertexArraysString3 "SelectedSurfaceMeshVertexArraysString3"
#define SelectedSurfaceMeshVertexArraysString4 "SelectedSurfaceMeshVertexArraysString4"
#define SelectedSurfaceMeshFaceArraysString1 "SelectedSurfaceMeshFaceArraysString1"
#define SelectedSurfaceMeshFaceArraysString2 "SelectedSurfaceMeshFaceArraysString2"
#define SelectedSurfaceMeshFaceArraysString3 "SelectedSurfaceMeshFaceArraysString3"
#define SelectedSurfaceMeshFaceArraysString4 "SelectedSurfaceMeshFaceArraysString4"
#define SelectedSurfaceMeshEdgeArraysString1 "SelectedSurfaceMeshEdgeArraysString1"
#define SelectedSurfaceMeshEdgeArraysString2 "SelectedSurfaceMeshEdgeArraysString2"
#define SelectedSurfaceMeshEdgeArraysString3 "SelectedSurfaceMeshEdgeArraysString3"
#define SelectedSurfaceMeshEdgeArraysString4 "SelectedSurfaceMeshEdgeArraysString4"
#define SelectedSurfaceMeshFieldArraysString1 "SelectedSurfaceMeshFieldArraysString1"
#define SelectedSurfaceMeshFieldArraysString2 "SelectedSurfaceMeshFieldArraysString2"
#define SelectedSurfaceMeshFieldArraysString3 "SelectedSurfaceMeshFieldArraysString3"
#define SelectedSurfaceMeshFieldArraysString4 "SelectedSurfaceMeshFieldArraysString4"
#define SelectedSurfaceMeshEnsembleArraysString1 "SelectedSurfaceMeshEnsembleArraysString1"
#define SelectedSurfaceMeshEnsembleArraysString2 "SelectedSurfaceMeshEnsembleArraysString2"
#define SelectedSurfaceMeshEnsembleArraysString3 "SelectedSurfaceMeshEnsembleArraysString3"
#define SelectedSurfaceMeshEnsembleArraysString4 "SelectedSurfaceMeshEnsembleArraysString4"
#define SelectedSolidMeshVertexArraysString1 "SelectedSolidMeshVertexArraysString1"
#define SelectedSolidMeshVertexArraysString2 "SelectedSolidMeshVertexArraysString2"
#define SelectedSolidMeshVertexArraysString3 "SelectedSolidMeshVertexArraysString3"
#define SelectedSolidMeshVertexArraysString4 "SelectedSolidMeshVertexArraysString4"
#define SelectedSolidMeshFaceArraysString1 "SelectedSolidMeshFaceArraysString1"
#define SelectedSolidMeshFaceArraysString2 "SelectedSolidMeshFaceArraysString2"
#define SelectedSolidMeshFaceArraysString3 "SelectedSolidMeshFaceArraysString3"
#define SelectedSolidMeshFaceArraysString4 "SelectedSolidMeshFaceArraysString4"
#define SelectedSolidMeshEdgeArraysString1 "SelectedSolidMeshEdgeArraysString1"
#define SelectedSolidMeshEdgeArraysString2 "SelectedSolidMeshEdgeArraysString2"
#define SelectedSolidMeshEdgeArraysString3 "SelectedSolidMeshEdgeArraysString3"
#define SelectedSolidMeshEdgeArraysString4 "SelectedSolidMeshEdgeArraysString4"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::FilterParametersRWTest::OutputFile);
#endif
}

// -----------------------------------------------------------------------------
//  Function used to generate test sets
// -----------------------------------------------------------------------------
std::set<std::string> getSet(std::string name1, std::string name2, std::string name3, std::string name4)
{
  std::set<std::string> set1;
  set1.insert(name1);
  set1.insert(name2);
  set1.insert(name3);
  set1.insert(name4);
  return set1;
}

// -----------------------------------------------------------------------------
//  Function used to generate test comparison input vectors
// -----------------------------------------------------------------------------
std::vector<ComparisonInput_t> getComparisonInputsVector(std::string arrayName1, int compOperator1, double compValue1,
                                                         std::string arrayName2, int compOperator2, double compValue2)
{
  ComparisonInput_t comparison1, comparison2;
  comparison1.arrayName = arrayName1;
  comparison1.compOperator = compOperator1;
  comparison1.compValue = compValue1;

  comparison2.arrayName = arrayName2;
  comparison2.compOperator = compOperator2;
  comparison2.compValue = compValue2;

  std::vector<ComparisonInput_t> comparisonVector;
  comparisonVector.push_back(comparison1);
  comparisonVector.push_back(comparison2);

  return comparisonVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExampleTest()
{
  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();


  ArraySelectionExample::Pointer filt = ArraySelectionExample::New();

  std::set<std::string> set1 = getSet(SelectedVoxelCellArraysString1, SelectedVoxelCellArraysString2,
                                      SelectedVoxelCellArraysString3, SelectedVoxelCellArraysString4);
  filt->setSelectedVoxelCellArrays(set1);

  std::set<std::string> set2 = getSet(SelectedVoxelFieldArraysString1, SelectedVoxelFieldArraysString2,
                                      SelectedVoxelFieldArraysString3, SelectedVoxelFieldArraysString4);
  filt->setSelectedVoxelFieldArrays(set2);

  std::set<std::string> set3 = getSet(SelectedVoxelEnsembleArraysString1, SelectedVoxelEnsembleArraysString2,
                                      SelectedVoxelEnsembleArraysString3, SelectedVoxelEnsembleArraysString4);
  filt->setSelectedVoxelEnsembleArrays(set3);

  std::set<std::string> set4 = getSet(SelectedSurfaceMeshVertexArraysString1, SelectedSurfaceMeshVertexArraysString2,
                                      SelectedSurfaceMeshVertexArraysString3, SelectedSurfaceMeshVertexArraysString4);
  filt->setSelectedSurfaceMeshVertexArrays(set4);

  std::set<std::string> set5 = getSet(SelectedSurfaceMeshFaceArraysString1, SelectedSurfaceMeshFaceArraysString2,
                                      SelectedSurfaceMeshFaceArraysString3, SelectedSurfaceMeshFaceArraysString4);
  filt->setSelectedSurfaceMeshFaceArrays(set5);

  std::set<std::string> set6 = getSet(SelectedSurfaceMeshEdgeArraysString1, SelectedSurfaceMeshEdgeArraysString2,
                                      SelectedSurfaceMeshEdgeArraysString3, SelectedSurfaceMeshEdgeArraysString4);
  filt->setSelectedSurfaceMeshEdgeArrays(set6);

  std::set<std::string> set7 = getSet(SelectedSurfaceMeshFieldArraysString1, SelectedSurfaceMeshFieldArraysString2,
                                      SelectedSurfaceMeshFieldArraysString3, SelectedSurfaceMeshFieldArraysString4);
  filt->setSelectedSurfaceMeshFieldArrays(set7);

  std::set<std::string> set8 = getSet(SelectedSurfaceMeshEnsembleArraysString1, SelectedSurfaceMeshEnsembleArraysString2,
                                      SelectedSurfaceMeshEnsembleArraysString3, SelectedSurfaceMeshEnsembleArraysString4);
  filt->setSelectedSurfaceMeshEnsembleArrays(set8);

  std::set<std::string> set9 = getSet(SelectedSolidMeshVertexArraysString1, SelectedSolidMeshVertexArraysString2,
                                      SelectedSolidMeshVertexArraysString3, SelectedSolidMeshVertexArraysString4);
  filt->setSelectedSolidMeshVertexArrays(set9);

  std::set<std::string> set10 = getSet(SelectedSolidMeshFaceArraysString1, SelectedSolidMeshFaceArraysString2,
                                       SelectedSolidMeshFaceArraysString3, SelectedSolidMeshFaceArraysString4);
  filt->setSelectedSolidMeshFaceArrays(set10);

  std::set<std::string> set11 = getSet(SelectedSolidMeshEdgeArraysString1, SelectedSolidMeshEdgeArraysString2,
                                       SelectedSolidMeshEdgeArraysString3, SelectedSolidMeshEdgeArraysString4);
  filt->setSelectedSolidMeshEdgeArrays(set11);

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FilterParametersRWTest::OutputFile);

  pipeline->pushBack(filt);
  pipeline->pushBack(writer);

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)


      // We are done writing a file, now we need to read the file using raw HDF5 codes

      hid_t fid = H5Utilities::openFile(UnitTest::FilterParametersRWTest::OutputFile);
  DREAM3D_REQUIRED(fid, >, 0)

      H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  hid_t pipelineGroupId = H5Gopen(fid, DREAM3D::HDF5::PipelineGroupName.c_str(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);

  err = reader->openOptionsGroup( filt.get() ); // Open the HDF5 Group for this filter
  DREAM3D_REQUIRED(err, >=, 0)

      // This next line should read all the filter parameters into the filter.
      filt->readFilterParameters( reader.get() );

  std::set<std::string> set1Read = filt->getSelectedVoxelCellArrays();
  std::set<std::string>::iterator iter = set1Read.begin();

  DREAM3D_REQUIRED(SelectedVoxelCellArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVoxelCellArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVoxelCellArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVoxelCellArraysString4, ==, *iter)

      std::set<std::string> set2Read = filt->getSelectedVoxelFieldArrays();
  iter = set2Read.begin();

  DREAM3D_REQUIRED(SelectedVoxelFieldArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVoxelFieldArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVoxelFieldArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVoxelFieldArraysString4, ==, *iter)

      std::set<std::string> set3Read = filt->getSelectedVoxelEnsembleArrays();
  iter = set3Read.begin();

  DREAM3D_REQUIRED(SelectedVoxelEnsembleArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVoxelEnsembleArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVoxelEnsembleArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVoxelEnsembleArraysString4, ==, *iter)

      std::set<std::string> set4Read = filt->getSelectedSurfaceMeshVertexArrays();
  iter = set4Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceMeshVertexArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshVertexArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshVertexArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshVertexArraysString4, ==, *iter)

      std::set<std::string> set5Read = filt->getSelectedSurfaceMeshFaceArrays();
  iter = set5Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceMeshFaceArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshFaceArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshFaceArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshFaceArraysString4, ==, *iter)

      std::set<std::string> set6Read = filt->getSelectedSurfaceMeshEdgeArrays();
  iter = set6Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceMeshEdgeArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshEdgeArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshEdgeArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshEdgeArraysString4, ==, *iter)

      std::set<std::string> set7Read = filt->getSelectedSurfaceMeshFieldArrays();
  iter = set7Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceMeshFieldArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshFieldArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshFieldArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshFieldArraysString4, ==, *iter)

      std::set<std::string> set8Read = filt->getSelectedSurfaceMeshEnsembleArrays();
  iter = set8Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceMeshEnsembleArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshEnsembleArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshEnsembleArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceMeshEnsembleArraysString4, ==, *iter)

      std::set<std::string> set9Read = filt->getSelectedSolidMeshVertexArrays();
  iter = set9Read.begin();

  DREAM3D_REQUIRED(SelectedSolidMeshVertexArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSolidMeshVertexArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSolidMeshVertexArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSolidMeshVertexArraysString4, ==, *iter)

      std::set<std::string> set10Read = filt->getSelectedSolidMeshFaceArrays();
  iter = set10Read.begin();

  DREAM3D_REQUIRED(SelectedSolidMeshFaceArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSolidMeshFaceArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSolidMeshFaceArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSolidMeshFaceArraysString4, ==, *iter)

      std::set<std::string> set11Read = filt->getSelectedSolidMeshEdgeArrays();
  iter = set11Read.begin();

  DREAM3D_REQUIRED(SelectedSolidMeshEdgeArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSolidMeshEdgeArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSolidMeshEdgeArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSolidMeshEdgeArraysString4, ==, *iter)

      err = reader->closeOptionsGroup(); // Close the HDF5 group for this filter
  DREAM3D_REQUIRED(err, >=, 0)


      H5Gclose(pipelineGroupId); // Closes the "Pipeline" group
  H5Fclose(fid); // Closes the file
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExampleTest()
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

  IntVec3Widget_t intWidget;
  intWidget.x = IntWidgetXTestValue;
  intWidget.y = IntWidgetYTestValue;
  intWidget.z = IntWidgetZTestValue;
  filt->setDimensions(intWidget);

  FloatVec3Widget_t floatWidget;
  floatWidget.x = FloatWidgetXTestValue;
  floatWidget.y = FloatWidgetYTestValue;
  floatWidget.z = FloatWidgetZTestValue;
  filt->setOrigin(floatWidget);

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
  writer->setOutputFile(UnitTest::FilterParametersRWTest::OutputFile);

  pipeline->pushBack(filt);
  pipeline->pushBack(writer);

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)


  // We are done writing a file, now we need to read the file using raw HDF5 codes

  hid_t fid = H5Utilities::openFile(UnitTest::FilterParametersRWTest::OutputFile);
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
  DREAM3D_REQUIRED( StlFilePrefixTestValue, ==, filt->getStlFilePrefix() )
  DREAM3D_REQUIRED( MaxIterationsTestValue, ==, filt->getMaxIterations() )
  DREAM3D_REQUIRED( MisorientationToleranceTestValue, ==, filt->getMisorientationTolerance() )
  DREAM3D_REQUIRED( InputFileTestValue, ==, filt->getInputFile() )
  DREAM3D_REQUIRED( InputPathTestValue, ==, filt->getInputPath() )
  DREAM3D_REQUIRED( OutputFileTestValue, ==, filt->getOutputFile() )
  DREAM3D_REQUIRED( OutputPathTestValue, ==, filt->getOutputPath() )
  DREAM3D_REQUIRED( WriteAlignmentShiftsTestValue, ==, filt->getWriteAlignmentShifts() )
  DREAM3D_REQUIRED( ConversionTypeTestValue, ==, filt->getConversionType() )
  DREAM3D_REQUIRED( SelectedCellArrayNameTestValue, ==, filt->getSelectedCellArrayName() )
  DREAM3D_REQUIRED( SelectedFieldArrayNameTestValue, ==, filt->getSelectedFieldArrayName() )
  DREAM3D_REQUIRED( SelectedEnsembleArrayNameTestValue, ==, filt->getSelectedEnsembleArrayName() )
  DREAM3D_REQUIRED( SurfaceMeshPointArrayNameTestValue, ==, filt->getSurfaceMeshPointArrayName() )
  DREAM3D_REQUIRED( SurfaceMeshFaceArrayNameTestValue, ==, filt->getSurfaceMeshFaceArrayName() )
  DREAM3D_REQUIRED( SurfaceMeshEdgeArrayNameTestValue, ==, filt->getSurfaceMeshEdgeArrayName() )
  DREAM3D_REQUIRED( SolidMeshPointArrayNameTestValue, ==, filt->getSolidMeshPointArrayName() )
  DREAM3D_REQUIRED( SolidMeshFaceArrayNameTestValue, ==, filt->getSolidMeshFaceArrayName() )
  DREAM3D_REQUIRED( SolidMeshEdgeArrayNameTestValue, ==, filt->getSolidMeshEdgeArrayName() )

  // Test the IntVec3Widget
  IntVec3Widget_t intWidgetRead = filt->getDimensions();
  DREAM3D_REQUIRED(IntWidgetXTestValue, ==, intWidgetRead.x)
  DREAM3D_REQUIRED(IntWidgetYTestValue, ==, intWidgetRead.y)
  DREAM3D_REQUIRED(IntWidgetZTestValue, ==, intWidgetRead.z)

  // Test the FloatVec3Widget
  FloatVec3Widget_t floatWidgetRead = filt->getOrigin();
  DREAM3D_REQUIRED(FloatWidgetXTestValue, ==, floatWidgetRead.x)
  DREAM3D_REQUIRED(FloatWidgetYTestValue, ==, floatWidgetRead.y)
  DREAM3D_REQUIRED(FloatWidgetZTestValue, ==, floatWidgetRead.z)

  // Test the AxisAngleInput
  std::vector<AxisAngleInput_t> axisAngleVectorRead = filt->getAxisAngleRotations();
  AxisAngleInput_t axisAngles1Read = axisAngleVectorRead[0];
  AxisAngleInput_t axisAngles2Read = axisAngleVectorRead[1];

  DREAM3D_REQUIRED(AxisAngles1AngleTestValue, ==, axisAngles1Read.angle)
  DREAM3D_REQUIRED(AxisAngles1HTestValue, ==, axisAngles1Read.h)
  DREAM3D_REQUIRED(AxisAngles1KTestValue, ==, axisAngles1Read.k)
  DREAM3D_REQUIRED(AxisAngles1LTestValue, ==, axisAngles1Read.l)
  DREAM3D_REQUIRED(AxisAngles2AngleTestValue, ==, axisAngles2Read.angle)
  DREAM3D_REQUIRED(AxisAngles2HTestValue, ==, axisAngles2Read.h)
  DREAM3D_REQUIRED(AxisAngles2KTestValue, ==, axisAngles2Read.k)
  DREAM3D_REQUIRED(AxisAngles2LTestValue, ==, axisAngles2Read.l)

  err = reader->closeOptionsGroup(); // Close the HDF5 group for this filter
  DREAM3D_REQUIRED(err, >=, 0)


  H5Gclose(pipelineGroupId); // Closes the "Pipeline" group
  H5Fclose(fid); // Closes the file
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExampleTest()
{
  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();


  GenericFilter::Pointer filt = GenericFilter::New();

  std::vector<ComparisonInput_t> cellComparisonInputsVector = getComparisonInputsVector(CellComparisonInputsArrayName1, CellComparisonInputsCompOperator1,
                                                                                        CellComparisonInputsCompValue1, CellComparisonInputsArrayName2,
                                                                                        CellComparisonInputsCompOperator2, CellComparisonInputsCompValue2);
  filt->setCellComparisonInputs(cellComparisonInputsVector);

  std::vector<ComparisonInput_t> fieldComparisonInputsVector = getComparisonInputsVector(FieldComparisonInputsArrayName1, FieldComparisonInputsCompOperator1,
                                                                                         FieldComparisonInputsCompValue1, FieldComparisonInputsArrayName2,
                                                                                         FieldComparisonInputsCompOperator2, FieldComparisonInputsCompValue2);
  filt->setFieldComparisonInputs(fieldComparisonInputsVector);

  std::vector<ComparisonInput_t> ensembleComparisonInputsVector = getComparisonInputsVector(EnsembleComparisonInputsArrayName1, EnsembleComparisonInputsCompOperator1,
                                                                                            EnsembleComparisonInputsCompValue1, EnsembleComparisonInputsArrayName2,
                                                                                            EnsembleComparisonInputsCompOperator2, EnsembleComparisonInputsCompValue2);
  filt->setEnsembleComparisonInputs(ensembleComparisonInputsVector);

  std::vector<ComparisonInput_t> pointComparisonInputsVector = getComparisonInputsVector(PointComparisonInputsArrayName1, PointComparisonInputsCompOperator1,
                                                                                         PointComparisonInputsCompValue1, PointComparisonInputsArrayName2,
                                                                                         PointComparisonInputsCompOperator2, PointComparisonInputsCompValue2);
  filt->setPointComparisonInputs(pointComparisonInputsVector);

  std::vector<ComparisonInput_t> faceComparisonInputsVector = getComparisonInputsVector(FaceComparisonInputsArrayName1, FaceComparisonInputsCompOperator1,
                                                                                        FaceComparisonInputsCompValue1, FaceComparisonInputsArrayName2,
                                                                                        FaceComparisonInputsCompOperator2, FaceComparisonInputsCompValue2);
  filt->setFaceComparisonInputs(faceComparisonInputsVector);

  std::vector<ComparisonInput_t> edgeComparisonInputsVector = getComparisonInputsVector(EdgeComparisonInputsArrayName1, EdgeComparisonInputsCompOperator1,
                                                                                        EdgeComparisonInputsCompValue1, EdgeComparisonInputsArrayName2,
                                                                                        EdgeComparisonInputsCompOperator2, EdgeComparisonInputsCompValue2);
  filt->setEdgeComparisonInputs(edgeComparisonInputsVector);



  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FilterParametersRWTest::OutputFile);

  pipeline->pushBack(filt);
  pipeline->pushBack(writer);

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)


  // We are done writing a file, now we need to read the file using raw HDF5 codes

  hid_t fid = H5Utilities::openFile(UnitTest::FilterParametersRWTest::OutputFile);
  DREAM3D_REQUIRED(fid, >, 0)

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  hid_t pipelineGroupId = H5Gopen(fid, DREAM3D::HDF5::PipelineGroupName.c_str(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);

  err = reader->openOptionsGroup( filt.get() ); // Open the HDF5 Group for this filter
  DREAM3D_REQUIRED(err, >=, 0)

  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get() );

  // Test the CellComparisonInputs
  std::vector<ComparisonInput_t> cellComparisonInputsVectorRead = filt->getCellComparisonInputs();
  ComparisonInput_t cellComparisonInputs1 = cellComparisonInputsVectorRead[0];
  ComparisonInput_t cellComparisonInputs2 = cellComparisonInputsVectorRead[1];

  std::vector<ComparisonInput_t> fieldComparisonInputsVectorRead = filt->getFieldComparisonInputs();
  ComparisonInput_t fieldComparisonInputs1 = fieldComparisonInputsVectorRead[0];
  ComparisonInput_t fieldComparisonInputs2 = fieldComparisonInputsVectorRead[1];

  std::vector<ComparisonInput_t> ensembleComparisonInputsVectorRead = filt->getEnsembleComparisonInputs();
  ComparisonInput_t ensembleComparisonInputs1 = ensembleComparisonInputsVectorRead[0];
  ComparisonInput_t ensembleComparisonInputs2 = ensembleComparisonInputsVectorRead[1];

  std::vector<ComparisonInput_t> pointComparisonInputsVectorRead = filt->getPointComparisonInputs();
  ComparisonInput_t pointComparisonInputs1 = pointComparisonInputsVectorRead[0];
  ComparisonInput_t pointComparisonInputs2 = pointComparisonInputsVectorRead[1];

  std::vector<ComparisonInput_t> faceComparisonInputsVectorRead = filt->getFaceComparisonInputs();
  ComparisonInput_t faceComparisonInputs1 = faceComparisonInputsVectorRead[0];
  ComparisonInput_t faceComparisonInputs2 = faceComparisonInputsVectorRead[1];

  std::vector<ComparisonInput_t> edgeComparisonInputsVectorRead = filt->getEdgeComparisonInputs();
  ComparisonInput_t edgeComparisonInputs1 = edgeComparisonInputsVectorRead[0];
  ComparisonInput_t edgeComparisonInputs2 = edgeComparisonInputsVectorRead[1];

  DREAM3D_REQUIRED(CellComparisonInputsArrayName1, ==, cellComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(CellComparisonInputsCompOperator1, ==, cellComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(CellComparisonInputsCompValue1, ==, cellComparisonInputs1.compValue)
  DREAM3D_REQUIRED(CellComparisonInputsArrayName2, ==, cellComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(CellComparisonInputsCompOperator2, ==, cellComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(CellComparisonInputsCompValue2, ==, cellComparisonInputs2.compValue)

  DREAM3D_REQUIRED(FieldComparisonInputsArrayName1, ==, fieldComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(FieldComparisonInputsCompOperator1, ==, fieldComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(FieldComparisonInputsCompValue1, ==, fieldComparisonInputs1.compValue)
  DREAM3D_REQUIRED(FieldComparisonInputsArrayName2, ==, fieldComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(FieldComparisonInputsCompOperator2, ==, fieldComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(FieldComparisonInputsCompValue2, ==, fieldComparisonInputs2.compValue)

  DREAM3D_REQUIRED(EnsembleComparisonInputsArrayName1, ==, ensembleComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(EnsembleComparisonInputsCompOperator1, ==, ensembleComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(EnsembleComparisonInputsCompValue1, ==, ensembleComparisonInputs1.compValue)
  DREAM3D_REQUIRED(EnsembleComparisonInputsArrayName2, ==, ensembleComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(EnsembleComparisonInputsCompOperator2, ==, ensembleComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(EnsembleComparisonInputsCompValue2, ==, ensembleComparisonInputs2.compValue)

  DREAM3D_REQUIRED(PointComparisonInputsArrayName1, ==, pointComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(PointComparisonInputsCompOperator1, ==, pointComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(PointComparisonInputsCompValue1, ==, pointComparisonInputs1.compValue)
  DREAM3D_REQUIRED(PointComparisonInputsArrayName2, ==, pointComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(PointComparisonInputsCompOperator2, ==, pointComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(PointComparisonInputsCompValue2, ==, pointComparisonInputs2.compValue)

  DREAM3D_REQUIRED(FaceComparisonInputsArrayName1, ==, faceComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(FaceComparisonInputsCompOperator1, ==, faceComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(FaceComparisonInputsCompValue1, ==, faceComparisonInputs1.compValue)
  DREAM3D_REQUIRED(FaceComparisonInputsArrayName2, ==, faceComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(FaceComparisonInputsCompOperator2, ==, faceComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(FaceComparisonInputsCompValue2, ==, faceComparisonInputs2.compValue)

  DREAM3D_REQUIRED(EdgeComparisonInputsArrayName1, ==, edgeComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(EdgeComparisonInputsCompOperator1, ==, edgeComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(EdgeComparisonInputsCompValue1, ==, edgeComparisonInputs1.compValue)
  DREAM3D_REQUIRED(EdgeComparisonInputsArrayName2, ==, edgeComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(EdgeComparisonInputsCompOperator2, ==, edgeComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(EdgeComparisonInputsCompValue2, ==, edgeComparisonInputs2.compValue)

  err = reader->closeOptionsGroup(); // Close the HDF5 group for this filter
  DREAM3D_REQUIRED(err, >=, 0)


  H5Gclose(pipelineGroupId); // Closes the "Pipeline" group
  H5Fclose(fid); // Closes the file
}



FilterPipeline::Pointer m_PipelineFromFile;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int readPipelineFromFile(hid_t fileId)
{
  int err = 0;
  m_PipelineFromFile->clear();



  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterManagerTest()
{
  FilterManager::Pointer fm = FilterManager::Instance();


  IFilterFactory::Pointer ff = fm->getFactoryForFilter(DataContainerReader::ClassName());
  DREAM3D_REQUIRE_NE(ff.get(), NULL)
  AbstractFilter::Pointer reader = ff->create();
  DREAM3D_REQUIRE_NE(reader.get(), NULL)

  int comp = DataContainerReader::ClassName().compare(reader->getNameOfClass());
  DREAM3D_REQUIRE_EQUAL(comp, 0);



  /* Get all the factories which should be at least 1 */
  FilterManager::Collection factories = fm->getFactories();

  DREAM3D_REQUIRED(factories.size(), >, 0)

  Filt0::Pointer filt0 = Filt0::New();
  Filt1::Pointer filt1 = Filt1::New();

}



// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
#if 0
#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    #endif
#endif



      DREAM3D_REGISTER_TEST( ArraySelectionExampleTest() )
      DREAM3D_REGISTER_TEST( GenericExampleTest() )
      DREAM3D_REGISTER_TEST( ThresholdExampleTest() )
      DREAM3D_REGISTER_TEST( FilterManagerTest() )

#if 0
    #if REMOVE_TEST_FILES
      DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    #endif
#endif


      PRINT_TEST_SUMMARY();
  return err;
}
