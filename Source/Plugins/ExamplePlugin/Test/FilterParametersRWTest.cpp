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
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/HDF5/H5FilterParametersReader.h"

#include "H5Support/H5Utilities.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

#include "ExamplePluginFilters/GenericExample.h"
#include "ExamplePluginFilters/ThresholdExample.h"
#include "ExamplePluginFilters/ArraySelectionExample.h"
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
#define SurfaceMeshVertexArrayNameTestValue "SurfaceMeshVertexArrayNameTestValue"
#define SurfaceMeshFaceArrayNameTestValue "SurfaceMeshFaceArrayNameTestValue"
#define SurfaceMeshEdgeArrayNameTestValue "SurfaceMeshEdgeArrayNameTestValue"
#define SolidMeshVertexArrayNameTestValue "SolidMeshVertexArrayNameTestValue"
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
#define SelectedVolumeCellArraysString1 "SelectedVolumeCellArraysString1"
#define SelectedVolumeCellArraysString2 "SelectedVolumeCellArraysString2"
#define SelectedVolumeCellArraysString3 "SelectedVolumeCellArraysString3"
#define SelectedVolumeCellArraysString4 "SelectedVolumeCellArraysString4"
#define SelectedVolumeFieldArraysString1 "SelectedVolumeFieldArraysString1"
#define SelectedVolumeFieldArraysString2 "SelectedVolumeFieldArraysString2"
#define SelectedVolumeFieldArraysString3 "SelectedVolumeFieldArraysString3"
#define SelectedVolumeFieldArraysString4 "SelectedVolumeFieldArraysString4"
#define SelectedVolumeEnsembleArraysString1 "SelectedVolumeEnsembleArraysString1"
#define SelectedVolumeEnsembleArraysString2 "SelectedVolumeEnsembleArraysString2"
#define SelectedVolumeEnsembleArraysString3 "SelectedVolumeEnsembleArraysString3"
#define SelectedVolumeEnsembleArraysString4 "SelectedVolumeEnsembleArraysString4"
#define SelectedSurfaceVertexArraysString1 "SelectedSurfaceVertexArraysString1"
#define SelectedSurfaceVertexArraysString2 "SelectedSurfaceVertexArraysString2"
#define SelectedSurfaceVertexArraysString3 "SelectedSurfaceVertexArraysString3"
#define SelectedSurfaceVertexArraysString4 "SelectedSurfaceVertexArraysString4"
#define SelectedSurfaceFaceArraysString1 "SelectedSurfaceFaceArraysString1"
#define SelectedSurfaceFaceArraysString2 "SelectedSurfaceFaceArraysString2"
#define SelectedSurfaceFaceArraysString3 "SelectedSurfaceFaceArraysString3"
#define SelectedSurfaceFaceArraysString4 "SelectedSurfaceFaceArraysString4"
#define SelectedSurfaceEdgeArraysString1 "SelectedSurfaceEdgeArraysString1"
#define SelectedSurfaceEdgeArraysString2 "SelectedSurfaceEdgeArraysString2"
#define SelectedSurfaceEdgeArraysString3 "SelectedSurfaceEdgeArraysString3"
#define SelectedSurfaceEdgeArraysString4 "SelectedSurfaceEdgeArraysString4"
#define SelectedSurfaceFieldArraysString1 "SelectedSurfaceFieldArraysString1"
#define SelectedSurfaceFieldArraysString2 "SelectedSurfaceFieldArraysString2"
#define SelectedSurfaceFieldArraysString3 "SelectedSurfaceFieldArraysString3"
#define SelectedSurfaceFieldArraysString4 "SelectedSurfaceFieldArraysString4"
#define SelectedSurfaceEnsembleArraysString1 "SelectedSurfaceEnsembleArraysString1"
#define SelectedSurfaceEnsembleArraysString2 "SelectedSurfaceEnsembleArraysString2"
#define SelectedSurfaceEnsembleArraysString3 "SelectedSurfaceEnsembleArraysString3"
#define SelectedSurfaceEnsembleArraysString4 "SelectedSurfaceEnsembleArraysString4"
#define SelectedVertexVertexArraysString1 "SelectedVertexVertexArraysString1"
#define SelectedVertexVertexArraysString2 "SelectedVertexVertexArraysString2"
#define SelectedVertexVertexArraysString3 "SelectedVertexVertexArraysString3"
#define SelectedVertexVertexArraysString4 "SelectedVertexVertexArraysString4"
#define SelectedVertexFieldArraysString1 "SelectedVertexFieldArraysString1"
#define SelectedVertexFieldArraysString2 "SelectedVertexFieldArraysString2"
#define SelectedVertexFieldArraysString3 "SelectedVertexFieldArraysString3"
#define SelectedVertexFieldArraysString4 "SelectedVertexFieldArraysString4"
#define SelectedVertexEnsembleArraysString1 "SelectedVertexEnsembleArraysString1"
#define SelectedVertexEnsembleArraysString2 "SelectedVertexEnsembleArraysString2"
#define SelectedVertexEnsembleArraysString3 "SelectedVertexEnsembleArraysString3"
#define SelectedVertexEnsembleArraysString4 "SelectedVertexEnsembleArraysString4"

enum TestCases
{
  Test1 = 0,
  Test2,
  Test3
};

FilterPipeline::Pointer m_PipelineFromFile;

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
QSet<std::string> getSet(QString name1, QString name2, QString name3, QString name4)
{
  QSet<std::string> set1;
  set1.insert(name1);
  set1.insert(name2);
  set1.insert(name3);
  set1.insert(name4);
  return set1;
}

// -----------------------------------------------------------------------------
//  Function used to generate test comparison input vectors
// -----------------------------------------------------------------------------
QVector<ComparisonInput_t> getComparisonInputsVector(QString arrayName1, int compOperator1, double compValue1,
                                                         QString arrayName2, int compOperator2, double compValue2)
{
  ComparisonInput_t comparison1, comparison2;
  comparison1.arrayName = arrayName1;
  comparison1.compOperator = compOperator1;
  comparison1.compValue = compValue1;

  comparison2.arrayName = arrayName2;
  comparison2.compOperator = compOperator2;
  comparison2.compValue = compValue2;

  QVector<ComparisonInput_t> comparisonVector;
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

  QSet<std::string> set1 = getSet(SelectedVolumeCellArraysString1, SelectedVolumeCellArraysString2,
                                      SelectedVolumeCellArraysString3, SelectedVolumeCellArraysString4);
  filt->setSelectedVolumeCellArrays(set1);

  QSet<std::string> set2 = getSet(SelectedVolumeFieldArraysString1, SelectedVolumeFieldArraysString2,
                                      SelectedVolumeFieldArraysString3, SelectedVolumeFieldArraysString4);
  filt->setSelectedVolumeFieldArrays(set2);

  QSet<std::string> set3 = getSet(SelectedVolumeEnsembleArraysString1, SelectedVolumeEnsembleArraysString2,
                                      SelectedVolumeEnsembleArraysString3, SelectedVolumeEnsembleArraysString4);
  filt->setSelectedVolumeEnsembleArrays(set3);

  QSet<std::string> set4 = getSet(SelectedSurfaceVertexArraysString1, SelectedSurfaceVertexArraysString2,
                                      SelectedSurfaceVertexArraysString3, SelectedSurfaceVertexArraysString4);
  filt->setSelectedSurfaceVertexArrays(set4);

  QSet<std::string> set5 = getSet(SelectedSurfaceFaceArraysString1, SelectedSurfaceFaceArraysString2,
                                      SelectedSurfaceFaceArraysString3, SelectedSurfaceFaceArraysString4);
  filt->setSelectedSurfaceFaceArrays(set5);

  QSet<std::string> set6 = getSet(SelectedSurfaceEdgeArraysString1, SelectedSurfaceEdgeArraysString2,
                                      SelectedSurfaceEdgeArraysString3, SelectedSurfaceEdgeArraysString4);
  filt->setSelectedSurfaceEdgeArrays(set6);

  QSet<std::string> set7 = getSet(SelectedSurfaceFieldArraysString1, SelectedSurfaceFieldArraysString2,
                                      SelectedSurfaceFieldArraysString3, SelectedSurfaceFieldArraysString4);
  filt->setSelectedSurfaceFieldArrays(set7);

  QSet<std::string> set8 = getSet(SelectedSurfaceEnsembleArraysString1, SelectedSurfaceEnsembleArraysString2,
                                      SelectedSurfaceEnsembleArraysString3, SelectedSurfaceEnsembleArraysString4);
  filt->setSelectedSurfaceEnsembleArrays(set8);

  QSet<std::string> set9 = getSet(SelectedVertexVertexArraysString1, SelectedVertexVertexArraysString2,
                                      SelectedVertexVertexArraysString3, SelectedVertexVertexArraysString4);
  filt->setSelectedVertexVertexArrays(set9);

  QSet<std::string> set10 = getSet(SelectedVertexFieldArraysString1, SelectedVertexFieldArraysString2,
                                       SelectedVertexFieldArraysString3, SelectedVertexFieldArraysString4);
  filt->setSelectedVertexFieldArrays(set10);

  QSet<std::string> set11 = getSet(SelectedVertexEnsembleArraysString1, SelectedVertexEnsembleArraysString2,
                                       SelectedVertexEnsembleArraysString3, SelectedVertexEnsembleArraysString4);
  filt->setSelectedVertexEnsembleArrays(set11);

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FilterParametersRWTest::OutputFile);

  pipeline->pushBack(filt);
  pipeline->pushBack(writer);

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)


  // We are done writing a file, now we need to read the file using raw HDF5 codes
  filt = ArraySelectionExample::New();
  hid_t fid = H5Utilities::openFile(UnitTest::FilterParametersRWTest::OutputFile);
  DREAM3D_REQUIRED(fid, >, 0)

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  hid_t pipelineGroupId = H5Gopen(fid, DREAM3D::HDF5::PipelineGroupName.c_str(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);
  int index = 0;

  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get(), index);

  QSet<std::string> set1Read = filt->getSelectedVolumeCellArrays();
  QSet<std::string>::iterator iter = set1Read.begin();

  DREAM3D_REQUIRED(SelectedVolumeCellArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVolumeCellArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVolumeCellArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVolumeCellArraysString4, ==, *iter)

      QSet<std::string> set2Read = filt->getSelectedVolumeFieldArrays();
  iter = set2Read.begin();

  DREAM3D_REQUIRED(SelectedVolumeFieldArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVolumeFieldArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVolumeFieldArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVolumeFieldArraysString4, ==, *iter)

      QSet<std::string> set3Read = filt->getSelectedVolumeEnsembleArrays();
  iter = set3Read.begin();

  DREAM3D_REQUIRED(SelectedVolumeEnsembleArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVolumeEnsembleArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVolumeEnsembleArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVolumeEnsembleArraysString4, ==, *iter)

      QSet<std::string> set4Read = filt->getSelectedSurfaceVertexArrays();
  iter = set4Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceVertexArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceVertexArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceVertexArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceVertexArraysString4, ==, *iter)

      QSet<std::string> set5Read = filt->getSelectedSurfaceFaceArrays();
  iter = set5Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceFaceArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceFaceArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceFaceArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceFaceArraysString4, ==, *iter)

      QSet<std::string> set6Read = filt->getSelectedSurfaceEdgeArrays();
  iter = set6Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceEdgeArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceEdgeArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceEdgeArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceEdgeArraysString4, ==, *iter)

      QSet<std::string> set7Read = filt->getSelectedSurfaceFieldArrays();
  iter = set7Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceFieldArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceFieldArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceFieldArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceFieldArraysString4, ==, *iter)

      QSet<std::string> set8Read = filt->getSelectedSurfaceEnsembleArrays();
  iter = set8Read.begin();

  DREAM3D_REQUIRED(SelectedSurfaceEnsembleArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceEnsembleArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceEnsembleArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedSurfaceEnsembleArraysString4, ==, *iter)

      QSet<std::string> set9Read = filt->getSelectedVertexVertexArrays();
  iter = set9Read.begin();

  DREAM3D_REQUIRED(SelectedVertexVertexArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVertexVertexArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVertexVertexArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVertexVertexArraysString4, ==, *iter)

      QSet<std::string> set10Read = filt->getSelectedVertexFieldArrays();
  iter = set10Read.begin();

  DREAM3D_REQUIRED(SelectedVertexFieldArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVertexFieldArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVertexFieldArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVertexFieldArraysString4, ==, *iter)

      QSet<std::string> set11Read = filt->getSelectedVertexEnsembleArrays();
  iter = set11Read.begin();

  DREAM3D_REQUIRED(SelectedVertexEnsembleArraysString1, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVertexEnsembleArraysString2, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVertexEnsembleArraysString3, ==, *iter)
      iter++;
  DREAM3D_REQUIRED(SelectedVertexEnsembleArraysString4, ==, *iter)


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


  GenericExample::Pointer filt = GenericExample::New();

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
  filt->setSelectedVolumeCellArrayName(SelectedCellArrayNameTestValue);
  filt->setSelectedVolumeFieldArrayName(SelectedFieldArrayNameTestValue);
  filt->setSelectedVolumeEnsembleArrayName(SelectedEnsembleArrayNameTestValue);
  filt->setSelectedSurfaceVertexArrayName(SurfaceMeshVertexArrayNameTestValue);
  filt->setSelectedSurfaceFaceArrayName(SurfaceMeshFaceArrayNameTestValue);
  filt->setSelectedSurfaceEdgeArrayName(SurfaceMeshEdgeArrayNameTestValue);
  filt->setSelectedVertexVertexArrayName(SolidMeshVertexArrayNameTestValue);
  filt->setSelectedVertexFieldArrayName(SolidMeshFaceArrayNameTestValue);
  filt->setSelectedVertexEnsembleArrayName(SolidMeshEdgeArrayNameTestValue);

  QVector<std::string> strVector;
  strVector.push_back(SurfaceMeshVertexArrayNameTestValue);
  strVector.push_back(SurfaceMeshFaceArrayNameTestValue);
  strVector.push_back(SurfaceMeshEdgeArrayNameTestValue);
  strVector.push_back(SolidMeshVertexArrayNameTestValue);
  strVector.push_back(SolidMeshFaceArrayNameTestValue);
  strVector.push_back(SolidMeshEdgeArrayNameTestValue);

  filt->setStrVector(strVector);

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

  QVector<AxisAngleInput_t> axisAngleInputsVector;
  axisAngleInputsVector.push_back(axisAngles1);
  axisAngleInputsVector.push_back(axisAngles2);
  filt->setCrystalSymmetryRotations(axisAngleInputsVector);

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FilterParametersRWTest::OutputFile);

  pipeline->pushBack(filt);
  pipeline->pushBack(writer);

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)


  // We are done writing a file, now we need to read the file using raw HDF5 codes
  filt = GenericExample::New();
  hid_t fid = H5Utilities::openFile(UnitTest::FilterParametersRWTest::OutputFile);
  DREAM3D_REQUIRED(fid, >, 0)

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  hid_t pipelineGroupId = H5Gopen(fid, DREAM3D::HDF5::PipelineGroupName.c_str(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);
  int index = 0;

  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get(), index);

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
  DREAM3D_REQUIRED( SelectedCellArrayNameTestValue, ==, filt->getSelectedVolumeCellArrayName() )
  DREAM3D_REQUIRED( SelectedFieldArrayNameTestValue, ==, filt->getSelectedVolumeFieldArrayName() )
  DREAM3D_REQUIRED( SelectedEnsembleArrayNameTestValue, ==, filt->getSelectedVolumeEnsembleArrayName() )
  DREAM3D_REQUIRED( SurfaceMeshVertexArrayNameTestValue, ==, filt->getSelectedSurfaceVertexArrayName() )
  DREAM3D_REQUIRED( SurfaceMeshFaceArrayNameTestValue, ==, filt->getSelectedSurfaceFaceArrayName() )
  DREAM3D_REQUIRED( SurfaceMeshEdgeArrayNameTestValue, ==, filt->getSelectedSurfaceEdgeArrayName() )
  DREAM3D_REQUIRED( SolidMeshVertexArrayNameTestValue, ==, filt->getSelectedVertexVertexArrayName() )
  DREAM3D_REQUIRED( SolidMeshFaceArrayNameTestValue, ==, filt->getSelectedVertexFieldArrayName() )
  DREAM3D_REQUIRED( SolidMeshEdgeArrayNameTestValue, ==, filt->getSelectedVertexEnsembleArrayName() )

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
  QVector<AxisAngleInput_t> axisAngleVectorRead = filt->getCrystalSymmetryRotations();
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

  // Test the string vector
  QVector<std::string> strVectorRead = filt->getStrVector();

  DREAM3D_REQUIRED(SurfaceMeshVertexArrayNameTestValue, ==, strVectorRead[0])
  DREAM3D_REQUIRED(SurfaceMeshFaceArrayNameTestValue, ==, strVectorRead[1])
  DREAM3D_REQUIRED(SurfaceMeshEdgeArrayNameTestValue, ==, strVectorRead[2])
  DREAM3D_REQUIRED(SolidMeshVertexArrayNameTestValue, ==, strVectorRead[3])
  DREAM3D_REQUIRED(SolidMeshFaceArrayNameTestValue, ==, strVectorRead[4])
  DREAM3D_REQUIRED(SolidMeshEdgeArrayNameTestValue, ==, strVectorRead[5])


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


  ThresholdExample::Pointer filt = ThresholdExample::New();

  QVector<ComparisonInput_t> cellComparisonInputsVector = getComparisonInputsVector(CellComparisonInputsArrayName1, CellComparisonInputsCompOperator1,
                                                                                        CellComparisonInputsCompValue1, CellComparisonInputsArrayName2,
                                                                                        CellComparisonInputsCompOperator2, CellComparisonInputsCompValue2);
  filt->setCellComparisonInputs(cellComparisonInputsVector);

  QVector<ComparisonInput_t> fieldComparisonInputsVector = getComparisonInputsVector(FieldComparisonInputsArrayName1, FieldComparisonInputsCompOperator1,
                                                                                         FieldComparisonInputsCompValue1, FieldComparisonInputsArrayName2,
                                                                                         FieldComparisonInputsCompOperator2, FieldComparisonInputsCompValue2);
  filt->setFieldComparisonInputs(fieldComparisonInputsVector);

  QVector<ComparisonInput_t> ensembleComparisonInputsVector = getComparisonInputsVector(EnsembleComparisonInputsArrayName1, EnsembleComparisonInputsCompOperator1,
                                                                                            EnsembleComparisonInputsCompValue1, EnsembleComparisonInputsArrayName2,
                                                                                            EnsembleComparisonInputsCompOperator2, EnsembleComparisonInputsCompValue2);
  filt->setEnsembleComparisonInputs(ensembleComparisonInputsVector);

  QVector<ComparisonInput_t> pointComparisonInputsVector = getComparisonInputsVector(PointComparisonInputsArrayName1, PointComparisonInputsCompOperator1,
                                                                                         PointComparisonInputsCompValue1, PointComparisonInputsArrayName2,
                                                                                         PointComparisonInputsCompOperator2, PointComparisonInputsCompValue2);
  filt->setPointComparisonInputs(pointComparisonInputsVector);

  QVector<ComparisonInput_t> faceComparisonInputsVector = getComparisonInputsVector(FaceComparisonInputsArrayName1, FaceComparisonInputsCompOperator1,
                                                                                        FaceComparisonInputsCompValue1, FaceComparisonInputsArrayName2,
                                                                                        FaceComparisonInputsCompOperator2, FaceComparisonInputsCompValue2);
  filt->setFaceComparisonInputs(faceComparisonInputsVector);

  QVector<ComparisonInput_t> edgeComparisonInputsVector = getComparisonInputsVector(EdgeComparisonInputsArrayName1, EdgeComparisonInputsCompOperator1,
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
  filt = ThresholdExample::New();
  hid_t fid = H5Utilities::openFile(UnitTest::FilterParametersRWTest::OutputFile);
  DREAM3D_REQUIRED(fid, >, 0)

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  hid_t pipelineGroupId = H5Gopen(fid, DREAM3D::HDF5::PipelineGroupName.c_str(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);
  int index = 0;

  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get(), index );

  // Test the CellComparisonInputs
  QVector<ComparisonInput_t> cellComparisonInputsVectorRead = filt->getCellComparisonInputs();
  ComparisonInput_t cellComparisonInputs1 = cellComparisonInputsVectorRead[0];
  ComparisonInput_t cellComparisonInputs2 = cellComparisonInputsVectorRead[1];

  QVector<ComparisonInput_t> fieldComparisonInputsVectorRead = filt->getFieldComparisonInputs();
  ComparisonInput_t fieldComparisonInputs1 = fieldComparisonInputsVectorRead[0];
  ComparisonInput_t fieldComparisonInputs2 = fieldComparisonInputsVectorRead[1];

  QVector<ComparisonInput_t> ensembleComparisonInputsVectorRead = filt->getEnsembleComparisonInputs();
  ComparisonInput_t ensembleComparisonInputs1 = ensembleComparisonInputsVectorRead[0];
  ComparisonInput_t ensembleComparisonInputs2 = ensembleComparisonInputsVectorRead[1];

  QVector<ComparisonInput_t> pointComparisonInputsVectorRead = filt->getPointComparisonInputs();
  ComparisonInput_t pointComparisonInputs1 = pointComparisonInputsVectorRead[0];
  ComparisonInput_t pointComparisonInputs2 = pointComparisonInputsVectorRead[1];

  QVector<ComparisonInput_t> faceComparisonInputsVectorRead = filt->getFaceComparisonInputs();
  ComparisonInput_t faceComparisonInputs1 = faceComparisonInputsVectorRead[0];
  ComparisonInput_t faceComparisonInputs2 = faceComparisonInputsVectorRead[1];

  QVector<ComparisonInput_t> edgeComparisonInputsVectorRead = filt->getEdgeComparisonInputs();
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


  H5Gclose(pipelineGroupId); // Closes the "Pipeline" group
  H5Fclose(fid); // Closes the file
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int readPipelineFromFile(hid_t fileId)
{
  int err = 0;
  m_PipelineFromFile->clear();

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  // HDF5: Open the "Pipeline" Group
  hid_t pipelineGroupId = H5Gopen(fileId, DREAM3D::HDF5::PipelineGroupName.c_str(), H5P_DEFAULT);
  reader->setGroupId(pipelineGroupId);

  // Use H5Lite to ask how many "groups" are in the "Pipeline Group"
  std::list<std::string> groupList;
  err = H5Utilities::getGroupObjects(pipelineGroupId, H5O_TYPE_GROUP, groupList);

  // Loop over the items getting the "ClassName" attribute from each group
  QString classNameStr = "";
  for (int i=0; i<groupList.size(); i++)
  {
    std::stringstream ss;
    ss << i;
    err = H5Lite::readStringAttribute(pipelineGroupId, ss.str(), "ClassName", classNameStr);

    // Instantiate a new filter using the FilterFactory based on the value of the className attribute
    FilterManager::Pointer fm = FilterManager::Instance();
    IFilterFactory::Pointer ff = fm->getFactoryForFilter(classNameStr);
    AbstractFilter::Pointer filter = ff->create();

    // Read the parameters
    filter->readFilterParameters( reader.get(), i );

    // Add filter to m_PipelineFromFile
    m_PipelineFromFile->pushBack(filter);
  }

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
//
// -----------------------------------------------------------------------------
void ExistingPipelineTest(QString outputFile, QString inputFile, TestCases test_case)
{
  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  Filt0::Pointer filt0 = Filt0::New();
  Filt1::Pointer filt1 = Filt1::New();
  DataContainerWriter::Pointer w = DataContainerWriter::New();

  DataContainerReader::Pointer r = DataContainerReader::New();
  r->setInputFile(inputFile);


  filt0->setFilt0_Float(13.1f + test_case);
  filt0->setFilt0_Integer(12 + test_case);
  filt1->setFilt1_Float(2.3f + test_case);
  filt1->setFilt1_Integer(4 + test_case);
  w->setOutputFile(outputFile);

  if (test_case == Test1)
  {
    pipeline->pushBack(filt0);
    pipeline->pushBack(filt1);
    pipeline->pushBack(w);
  }
  else if (test_case == Test2)
  {
    pipeline->pushBack(r);
    pipeline->pushBack(filt0);
    pipeline->pushBack(filt1);
    pipeline->pushBack(w);
  }
  else if (test_case == Test3)
  {
    pipeline->pushBack(filt0);
    pipeline->pushBack(filt1);
    pipeline->pushBack(r);
    pipeline->pushBack(w);
  }
  else
  {
    return;
  }

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExistingPipelineCheck(QString fileName)
{
  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  DataContainerReader::Pointer r = DataContainerReader::New();
  r->setInputFile(fileName);

  pipeline->pushBack(r);
  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  m_PipelineFromFile = FilterPipeline::New();
  m_PipelineFromFile = r.get()->getPipelinePointer();

  AbstractFilter::Pointer absFilter = AbstractFilter::New();
  FilterPipeline::FilterContainerType container = m_PipelineFromFile->getFilterContainer();

  DREAM3D_REQUIRED(container.size(), == , 11)

  FilterPipeline::FilterContainerType::iterator iter = container.begin();

  absFilter = *iter;
  Filt0* f0_1 = Filt0::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(f0_1->getFilt0_Float(), == , 15.1f)
  DREAM3D_REQUIRED(f0_1->getFilt0_Integer(), == , 14)
  iter++;

  absFilter = *iter;
  Filt1* f1_1 = Filt1::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(f1_1->getFilt1_Float(), == , 4.3f)
  DREAM3D_REQUIRED(f1_1->getFilt1_Integer(), == , 6)
  iter++;

  absFilter = *iter;
  Filt0* f0_2 = Filt0::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(f0_2->getFilt0_Float(), == , 13.1f)
  DREAM3D_REQUIRED(f0_2->getFilt0_Integer(), == , 12)
  iter++;

  absFilter = *iter;
  Filt1* f1_2 = Filt1::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(f1_2->getFilt1_Float(), == , 2.3f)
  DREAM3D_REQUIRED(f1_2->getFilt1_Integer(), == , 4)
  iter++;

  absFilter = *iter;
  DataContainerWriter* w1 = DataContainerWriter::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(w1->getNameOfClass(), ==, "DataContainerWriter")
  iter++;

  absFilter = *iter;
  DataContainerReader* r1 = DataContainerReader::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(r1->getNameOfClass(), ==, "DataContainerReader")
  iter++;

  absFilter = *iter;
  Filt0* f0_3 = Filt0::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(f0_3->getFilt0_Float(), == , 14.1f)
  DREAM3D_REQUIRED(f0_3->getFilt0_Integer(), == , 13)
  iter++;

  absFilter = *iter;
  Filt1* f1_3 = Filt1::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(f1_3->getFilt1_Float(), == , 3.3f)
  DREAM3D_REQUIRED(f1_3->getFilt1_Integer(), == , 5)
  iter++;

  absFilter = *iter;
  DataContainerWriter* w2 = DataContainerWriter::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(w2->getNameOfClass(), ==, "DataContainerWriter")
  iter++;

  absFilter = *iter;
  DataContainerReader* r2 = DataContainerReader::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(r2->getNameOfClass(), ==, "DataContainerReader")
  iter++;

  absFilter = *iter;
  DataContainerWriter* w3 = DataContainerWriter::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(w3->getNameOfClass(), ==, "DataContainerWriter")
}


// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  FilterFactory<Filt0>::Pointer Filt0Factory = FilterFactory<Filt0>::New();
  FilterManager::Instance()->addFilterFactory("Filt0",Filt0Factory);

  FilterFactory<Filt1>::Pointer Filt1Factory = FilterFactory<Filt1>::New();
  FilterManager::Instance()->addFilterFactory("Filt1",Filt1Factory);

  int err = EXIT_SUCCESS;
#if 1
#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    #endif
#endif



      DREAM3D_REGISTER_TEST( ArraySelectionExampleTest() )
      DREAM3D_REGISTER_TEST( GenericExampleTest() )
      DREAM3D_REGISTER_TEST( ThresholdExampleTest() )
      //DREAM3D_REGISTER_TEST( FilterManagerTest() )

      //DREAM3D_REGISTER_TEST( ExistingPipelineTest(UnitTest::FilterParametersRWTest::TestFile_1, "", Test1) )

      //DREAM3D_REGISTER_TEST( ExistingPipelineTest(UnitTest::FilterParametersRWTest::TestFile_2, UnitTest::FilterParametersRWTest::TestFile_1, Test2) )

      //DREAM3D_REGISTER_TEST( ExistingPipelineTest(UnitTest::FilterParametersRWTest::TestFile_3, UnitTest::FilterParametersRWTest::TestFile_2, Test3) )

      //DREAM3D_REGISTER_TEST( ExistingPipelineCheck(UnitTest::FilterParametersRWTest::TestFile_3) )

#if 1
    #if REMOVE_TEST_FILES
      DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    #endif
#endif


      PRINT_TEST_SUMMARY();
  return err;
}
