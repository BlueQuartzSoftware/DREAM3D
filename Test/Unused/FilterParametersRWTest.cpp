/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */



#include <stdlib.h>
#include <stdio.h>

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "H5Support/QH5Utilities.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/IFilterFactory.hpp"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/IOFilters/DataContainerWriter.h"
#include "SIMPLib/IOFilters/DataContainerReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/QFilterParametersReader.h"
#include "SIMPLib/FilterParameters/QFilterParametersWriter.h"
#include "SIMPLib/TestFilters/GenericExample.h"
#include "SIMPLib/TestFilters/ThresholdExample.h"
#include "SIMPLib/TestFilters/ArraySelectionExample.h"
#include "SIMPLib/TestFilters/TestFilters.h"



#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"
#include "GenerateFeatureIds.h"




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
#define SelectedFeatureArrayNameTestValue "SelectedFeatureArrayNameTestValue"
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
#define FeatureComparisonInputsArrayName1 "FeatureComparisonInputsArrayName1"
#define FeatureComparisonInputsCompOperator1 2
#define FeatureComparisonInputsCompValue1 13.4
#define FeatureComparisonInputsArrayName2 "FeatureComparisonInputsArrayName2"
#define FeatureComparisonInputsCompOperator2 0
#define FeatureComparisonInputsCompValue2 45.6
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
#define SelectedVolumeCellFeatureArraysString1 "SelectedVolumeCellFeatureArraysString1"
#define SelectedVolumeCellFeatureArraysString2 "SelectedVolumeCellFeatureArraysString2"
#define SelectedVolumeCellFeatureArraysString3 "SelectedVolumeCellFeatureArraysString3"
#define SelectedVolumeCellFeatureArraysString4 "SelectedVolumeCellFeatureArraysString4"
#define SelectedVolumeCellEnsembleArraysString1 "SelectedVolumeCellEnsembleArraysString1"
#define SelectedVolumeCellEnsembleArraysString2 "SelectedVolumeCellEnsembleArraysString2"
#define SelectedVolumeCellEnsembleArraysString3 "SelectedVolumeCellEnsembleArraysString3"
#define SelectedVolumeCellEnsembleArraysString4 "SelectedVolumeCellEnsembleArraysString4"
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
#define SelectedSurfaceFaceFeatureArraysString1 "SelectedSurfaceFaceFeatureArraysString1"
#define SelectedSurfaceFaceFeatureArraysString2 "SelectedSurfaceFaceFeatureArraysString2"
#define SelectedSurfaceFaceFeatureArraysString3 "SelectedSurfaceFaceFeatureArraysString3"
#define SelectedSurfaceFaceFeatureArraysString4 "SelectedSurfaceFaceFeatureArraysString4"
#define SelectedSurfaceFaceEnsembleArraysString1 "SelectedSurfaceFaceEnsembleArraysString1"
#define SelectedSurfaceFaceEnsembleArraysString2 "SelectedSurfaceFaceEnsembleArraysString2"
#define SelectedSurfaceFaceEnsembleArraysString3 "SelectedSurfaceFaceEnsembleArraysString3"
#define SelectedSurfaceFaceEnsembleArraysString4 "SelectedSurfaceFaceEnsembleArraysString4"
#define SelectedVertexVertexArraysString1 "SelectedVertexVertexArraysString1"
#define SelectedVertexVertexArraysString2 "SelectedVertexVertexArraysString2"
#define SelectedVertexVertexArraysString3 "SelectedVertexVertexArraysString3"
#define SelectedVertexVertexArraysString4 "SelectedVertexVertexArraysString4"
#define SelectedVertexVertexFeatureArraysString1 "SelectedVertexVertexFeatureArraysString1"
#define SelectedVertexVertexFeatureArraysString2 "SelectedVertexVertexFeatureArraysString2"
#define SelectedVertexVertexFeatureArraysString3 "SelectedVertexVertexFeatureArraysString3"
#define SelectedVertexVertexFeatureArraysString4 "SelectedVertexVertexFeatureArraysString4"
#define SelectedVertexVertexEnsembleArraysString1 "SelectedVertexVertexEnsembleArraysString1"
#define SelectedVertexVertexEnsembleArraysString2 "SelectedVertexVertexEnsembleArraysString2"
#define SelectedVertexVertexEnsembleArraysString3 "SelectedVertexVertexEnsembleArraysString3"
#define SelectedVertexVertexEnsembleArraysString4 "SelectedVertexVertexEnsembleArraysString4"

enum TestCases
{
  Test1 = 0,
  Test2,
  Test3
};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::FilterParametersRWTest::ArraySelectionFile);
  QFile::remove(UnitTest::FilterParametersRWTest::GenericExampleFile);
  QFile::remove(UnitTest::FilterParametersRWTest::ThresholdFile);
  QFile::remove(UnitTest::FilterParametersRWTest::ArraySelectionFileQSettings);
  QFile::remove(UnitTest::FilterParametersRWTest::GenericExampleFileQSettings);
  QFile::remove(UnitTest::FilterParametersRWTest::ThresholdFileQSettings);
#endif
}

// -----------------------------------------------------------------------------
//  Function used to generate test sets
// -----------------------------------------------------------------------------
QSet<QString> getSet(QString name1, QString name2, QString name3, QString name4)
{
  QSet<QString> set1;
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
template<typename T>
T getValue()
{
  return std::numeric_limits<T>::max();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
bool validateValue(T value)
{
  return (value == std::numeric_limits<T>::max() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
QVector<T> getArray()
{
  QVector<T> data(4);
  data[0] = std::numeric_limits<T>::min();
  data[1] = std::numeric_limits<T>::max();
  data[2] = std::numeric_limits<T>::max();
  data[3] = std::numeric_limits<T>::min();
  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void validateArray(QVector<T> data)
{
  DREAM3D_REQUIRE_EQUAL(data.size(), 4)
  DREAM3D_REQUIRE_EQUAL(data[0], std::numeric_limits<T>::min());
  DREAM3D_REQUIRE_EQUAL(data[1], std::numeric_limits<T>::max());
  DREAM3D_REQUIRE_EQUAL(data[2], std::numeric_limits<T>::max());
  DREAM3D_REQUIRE_EQUAL(data[3], std::numeric_limits<T>::min());
}

QVector<float> getArrayFloat()
{
  QVector<float> data(4);
  data[0] = 0.1;
  data[1] = -1.5;
  data[2] = 23.66;
  data[3] = -126.325;
  return data;
}

void validateArray(QVector<float> data)
{
  DREAM3D_REQUIRE_EQUAL(data.size(), 4)
  bool b = AlmostEqualUlpsFinal(data[0], 0.1, 1);
  DREAM3D_REQUIRE_EQUAL(true, b);
  b = AlmostEqualUlpsFinal(data[1], -1.5, 1);
  DREAM3D_REQUIRE_EQUAL(true, b);
  b = AlmostEqualUlpsFinal(data[2], 23.66, 1);
  DREAM3D_REQUIRE_EQUAL(true, b);
  b = AlmostEqualUlpsFinal(data[3], -126.325, 1);
  DREAM3D_REQUIRE_EQUAL(true, b);
}

QVector<double> getArrayDouble()
{
  QVector<double> data(4);
  data[0] = 0.1;
  data[1] = -1.5;
  data[2] = 23.66;
  data[3] = -126.325;
  return data;
}

void validateArray(QVector<double> data)
{
  DREAM3D_REQUIRE_EQUAL(data.size(), 4)
  bool b = AlmostEqualUlpsFinal(data[0], 0.1, 1);
  DREAM3D_REQUIRE_EQUAL(true, b);
  b = AlmostEqualUlpsFinal(data[1], -1.5, 1);
  DREAM3D_REQUIRE_EQUAL(true, b);
  b = AlmostEqualUlpsFinal(data[2], 23.66, 1);
  DREAM3D_REQUIRE_EQUAL(true, b);
  b = AlmostEqualUlpsFinal(data[3], -126.325, 1);
  DREAM3D_REQUIRE_EQUAL(true, b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WritePipelineToQSettings(FilterPipeline::Pointer pipeline, QString filePath)
{
  int err = -1;
  QFilterParametersWriter::Pointer parametersWriter = QFilterParametersWriter::New();
  parametersWriter->openFile(filePath, QSettings::IniFormat);

  // Get the list of filters that are in the pipeline
  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();

  // Now starting with the first filter in the pipeline, start the actual writing
  AbstractFilter::Pointer currentFilter = filters.at(0);
  int index = 0;
  while(NULL != currentFilter.get())
  {
    index = currentFilter->writeFilterParameters(parametersWriter.get(), index);
    currentFilter = currentFilter->getNextFilter();
  }
  parametersWriter->setNumberOfFilters(index);
  parametersWriter->closeFile();
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateArraySelectionExamplePipelineFiles()
{
  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  ArraySelectionExample::Pointer filt = ArraySelectionExample::New();

  QSet<QString> set1 = getSet(SelectedVolumeCellArraysString1, SelectedVolumeCellArraysString2,
                              SelectedVolumeCellArraysString3, SelectedVolumeCellArraysString4);
  filt->setSelectedVolumeCellArrays(set1);

  QSet<QString> set2 = getSet(SelectedVolumeCellFeatureArraysString1, SelectedVolumeCellFeatureArraysString2,
                              SelectedVolumeCellFeatureArraysString3, SelectedVolumeCellFeatureArraysString4);
  filt->setSelectedVolumeCellFeatureArrays(set2);

  QSet<QString> set3 = getSet(SelectedVolumeCellEnsembleArraysString1, SelectedVolumeCellEnsembleArraysString2,
                              SelectedVolumeCellEnsembleArraysString3, SelectedVolumeCellEnsembleArraysString4);
  filt->setSelectedVolumeCellEnsembleArrays(set3);

  QSet<QString> set4 = getSet(SelectedSurfaceVertexArraysString1, SelectedSurfaceVertexArraysString2,
                              SelectedSurfaceVertexArraysString3, SelectedSurfaceVertexArraysString4);
  filt->setSelectedSurfaceVertexArrays(set4);

  QSet<QString> set5 = getSet(SelectedSurfaceFaceArraysString1, SelectedSurfaceFaceArraysString2,
                              SelectedSurfaceFaceArraysString3, SelectedSurfaceFaceArraysString4);
  filt->setSelectedSurfaceFaceArrays(set5);

  QSet<QString> set6 = getSet(SelectedSurfaceEdgeArraysString1, SelectedSurfaceEdgeArraysString2,
                              SelectedSurfaceEdgeArraysString3, SelectedSurfaceEdgeArraysString4);
  filt->setSelectedSurfaceEdgeArrays(set6);

  QSet<QString> set7 = getSet(SelectedSurfaceFaceFeatureArraysString1, SelectedSurfaceFaceFeatureArraysString2,
                              SelectedSurfaceFaceFeatureArraysString3, SelectedSurfaceFaceFeatureArraysString4);
  filt->setSelectedSurfaceFaceFeatureArrays(set7);

  QSet<QString> set8 = getSet(SelectedSurfaceFaceEnsembleArraysString1, SelectedSurfaceFaceEnsembleArraysString2,
                              SelectedSurfaceFaceEnsembleArraysString3, SelectedSurfaceFaceEnsembleArraysString4);
  filt->setSelectedSurfaceFaceEnsembleArrays(set8);

  QSet<QString> set9 = getSet(SelectedVertexVertexArraysString1, SelectedVertexVertexArraysString2,
                              SelectedVertexVertexArraysString3, SelectedVertexVertexArraysString4);
  filt->setSelectedVertexVertexArrays(set9);

  QSet<QString> set10 = getSet(SelectedVertexVertexFeatureArraysString1, SelectedVertexVertexFeatureArraysString2,
                               SelectedVertexVertexFeatureArraysString3, SelectedVertexVertexFeatureArraysString4);
  filt->setSelectedVertexVertexFeatureArrays(set10);

  QSet<QString> set11 = getSet(SelectedVertexVertexEnsembleArraysString1, SelectedVertexVertexEnsembleArraysString2,
                               SelectedVertexVertexEnsembleArraysString3, SelectedVertexVertexEnsembleArraysString4);
  filt->setSelectedVertexVertexEnsembleArrays(set11);

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FilterParametersRWTest::ArraySelectionFile);
  writer->setWriteXdmfFile(false);

  CreateVolumeDataContainer::Pointer cdc = CreateVolumeDataContainer::New();
  pipeline->pushBack(cdc);
  pipeline->pushBack(filt);
  pipeline->pushBack(writer);

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)


  WritePipelineToQSettings(pipeline, UnitTest::FilterParametersRWTest::ArraySelectionFileQSettings);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ValidateArraySelectionExampleFilter(ArraySelectionExample::Pointer filt)
{
  QSet<QString> comp = filt->getSelectedVolumeCellArrays();
  DREAM3D_REQUIRE_EQUAL(comp.size(), 4);
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellArraysString4), == , true)

  comp = filt->getSelectedVolumeCellFeatureArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellFeatureArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellFeatureArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellFeatureArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellFeatureArraysString4), == , true)

  comp = filt->getSelectedVolumeCellEnsembleArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellEnsembleArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellEnsembleArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellEnsembleArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVolumeCellEnsembleArraysString4), == , true)

  comp = filt->getSelectedSurfaceVertexArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceVertexArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceVertexArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceVertexArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceVertexArraysString4), == , true)

  comp = filt->getSelectedSurfaceFaceArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceArraysString4), == , true)

  comp = filt->getSelectedSurfaceEdgeArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceEdgeArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceEdgeArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceEdgeArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceEdgeArraysString4), == , true)

  comp = filt->getSelectedSurfaceFaceFeatureArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceFeatureArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceFeatureArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceFeatureArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceFeatureArraysString4), == , true)

  comp = filt->getSelectedSurfaceFaceEnsembleArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceEnsembleArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceEnsembleArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceEnsembleArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedSurfaceFaceEnsembleArraysString4), == , true)

  comp = filt->getSelectedVertexVertexArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexArraysString4), == , true)

  comp = filt->getSelectedVertexVertexFeatureArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexFeatureArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexFeatureArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexFeatureArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexFeatureArraysString4), == , true)

  comp = filt->getSelectedVertexVertexEnsembleArrays();
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexEnsembleArraysString1), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexEnsembleArraysString2), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexEnsembleArraysString3), == , true)
  DREAM3D_REQUIRED(comp.contains(SelectedVertexVertexEnsembleArraysString4), == , true)

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyArraySelectionExampleQSettingsFilterParameters()
{
  ArraySelectionExample::Pointer filt = ArraySelectionExample::New();
  QFilterParametersReader::Pointer reader = QFilterParametersReader::New();
  reader->openFile(UnitTest::FilterParametersRWTest::ArraySelectionFileQSettings, QSettings::IniFormat);
  int index = 1;
  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get(), index);

  // Compare the filter to what was read from the file
  ValidateArraySelectionExampleFilter(filt);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyArraySelectionExampleHDF5FilterParameters()
{
  // We are done writing a file, now we need to read the file using raw HDF5 codes
  ArraySelectionExample::Pointer filt = ArraySelectionExample::New();
  hid_t fid = -1;
  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::OpenDREAM3DFileForReadingPipeline(UnitTest::FilterParametersRWTest::ArraySelectionFile, fid);
  DREAM3D_REQUIRED(fid, > , 0)
  DREAM3D_REQUIRED(filt.get(), != , NULL)

  // The Generic Filter is the second (index = 1) filter in the pipeline, so set this index value
  int index = 1;

  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get(), index);

  // Compare the filter to what was read from the file
  ValidateArraySelectionExampleFilter(filt);

  // Close up the HDF5 file
  H5Gclose(reader->getPipelineGroupId()); // Closes the "Pipeline" group
  H5Fclose(fid); // Closes the file
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::Pointer GenerateGenericExample()
{
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
  filt->setSelectedVolumeFeatureArrayName(SelectedFeatureArrayNameTestValue);
  filt->setSelectedVolumeEnsembleArrayName(SelectedEnsembleArrayNameTestValue);
  filt->setSelectedSurfaceVertexArrayName(SurfaceMeshVertexArrayNameTestValue);
  filt->setSelectedSurfaceFaceArrayName(SurfaceMeshFaceArrayNameTestValue);
  filt->setSelectedSurfaceEdgeArrayName(SurfaceMeshEdgeArrayNameTestValue);
  filt->setSelectedVertexVertexArrayName(SolidMeshVertexArrayNameTestValue);
  filt->setSelectedVertexFeatureArrayName(SolidMeshFaceArrayNameTestValue);
  filt->setSelectedVertexEnsembleArrayName(SolidMeshEdgeArrayNameTestValue);

  QVector<QString> strVector;
  strVector.push_back(SurfaceMeshVertexArrayNameTestValue);
  strVector.push_back(SurfaceMeshFaceArrayNameTestValue);
  strVector.push_back(SurfaceMeshEdgeArrayNameTestValue);
  strVector.push_back(SolidMeshVertexArrayNameTestValue);
  strVector.push_back(SolidMeshFaceArrayNameTestValue);
  strVector.push_back(SolidMeshEdgeArrayNameTestValue);

  filt->setStrVector(strVector);

  filt->setInt8Array(getArray<int8_t>());
  filt->setInt16Array(getArray<int16_t>());
  filt->setInt32Array(getArray<int32_t>());
  filt->setInt64Array(getArray<int64_t>());
  filt->setUInt8Array(getArray<uint8_t>());
  filt->setUInt16Array(getArray<uint16_t>());
  filt->setUInt32Array(getArray<uint32_t>());
  filt->setUInt64Array(getArray<uint64_t>());
  filt->setFloatArray(getArrayFloat());
  filt->setDoubleArray(getArrayDouble());

  filt->setInt8Value(getValue<int8_t>());
  filt->setInt16Value(getValue<int16_t>());
  filt->setInt32Value(getValue<int32_t>());
  filt->setInt64Value(getValue<int64_t>());
  filt->setUInt8Value(getValue<uint8_t>());
  filt->setUInt16Value(getValue<uint16_t>());
  filt->setUInt32Value(getValue<uint32_t>());
  filt->setUInt64Value(getValue<uint64_t>());
  filt->setFloatValue(-1.521);
  filt->setDoubleValue(2.36547);


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
  filt->setAxisAngleInputs(axisAngleInputsVector);

  return filt;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ValidateGenericExampleFilter(GenericExample::Pointer filt)
{
  // Now one by one, compare each of the filter parameters that you have stored in some constant somewhere to the values that are now in the filt variable.
  // Use DREAM3D_REQUIRED() to make sure each one is what you think it is.
  DREAM3D_REQUIRED( StlFilePrefixTestValue, == , filt->getStlFilePrefix() )
  DREAM3D_REQUIRED( MaxIterationsTestValue, == , filt->getMaxIterations() )
  DREAM3D_REQUIRED( MisorientationToleranceTestValue, == , filt->getMisorientationTolerance() )
  DREAM3D_REQUIRED( InputFileTestValue, == , filt->getInputFile() )
  DREAM3D_REQUIRED( InputPathTestValue, == , filt->getInputPath() )
  DREAM3D_REQUIRED( OutputFileTestValue, == , filt->getOutputFile() )
  DREAM3D_REQUIRED( OutputPathTestValue, == , filt->getOutputPath() )
  DREAM3D_REQUIRED( WriteAlignmentShiftsTestValue, == , filt->getWriteAlignmentShifts() )
  DREAM3D_REQUIRED( ConversionTypeTestValue, == , filt->getConversionType() )
  DREAM3D_REQUIRED( SelectedCellArrayNameTestValue, == , filt->getSelectedVolumeCellArrayName() )
  DREAM3D_REQUIRED( SelectedFeatureArrayNameTestValue, == , filt->getSelectedVolumeFeatureArrayName() )
  DREAM3D_REQUIRED( SelectedEnsembleArrayNameTestValue, == , filt->getSelectedVolumeEnsembleArrayName() )
  DREAM3D_REQUIRED( SurfaceMeshVertexArrayNameTestValue, == , filt->getSelectedSurfaceVertexArrayName() )
  DREAM3D_REQUIRED( SurfaceMeshFaceArrayNameTestValue, == , filt->getSelectedSurfaceFaceArrayName() )
  DREAM3D_REQUIRED( SurfaceMeshEdgeArrayNameTestValue, == , filt->getSelectedSurfaceEdgeArrayName() )
  DREAM3D_REQUIRED( SolidMeshVertexArrayNameTestValue, == , filt->getSelectedVertexVertexArrayName() )
  DREAM3D_REQUIRED( SolidMeshFaceArrayNameTestValue, == , filt->getSelectedVertexFeatureArrayName() )
  DREAM3D_REQUIRED( SolidMeshEdgeArrayNameTestValue, == , filt->getSelectedVertexEnsembleArrayName() )

  // Test the IntVec3Widget
  IntVec3Widget_t intWidgetRead = filt->getDimensions();
  DREAM3D_REQUIRED(IntWidgetXTestValue, == , intWidgetRead.x)
  DREAM3D_REQUIRED(IntWidgetYTestValue, == , intWidgetRead.y)
  DREAM3D_REQUIRED(IntWidgetZTestValue, == , intWidgetRead.z)

  // Test the FloatVec3Widget
  FloatVec3Widget_t floatWidgetRead = filt->getOrigin();
  DREAM3D_REQUIRED(FloatWidgetXTestValue, == , floatWidgetRead.x)
  DREAM3D_REQUIRED(FloatWidgetYTestValue, == , floatWidgetRead.y)
  DREAM3D_REQUIRED(FloatWidgetZTestValue, == , floatWidgetRead.z)

  // Test the AxisAngleInput
  QVector<AxisAngleInput_t> axisAngleVectorRead = filt->getAxisAngleInputs();
  AxisAngleInput_t axisAngles1Read = axisAngleVectorRead[0];
  AxisAngleInput_t axisAngles2Read = axisAngleVectorRead[1];

  DREAM3D_REQUIRED(AxisAngles1AngleTestValue, == , axisAngles1Read.angle)
  DREAM3D_REQUIRED(AxisAngles1HTestValue, == , axisAngles1Read.h)
  DREAM3D_REQUIRED(AxisAngles1KTestValue, == , axisAngles1Read.k)
  DREAM3D_REQUIRED(AxisAngles1LTestValue, == , axisAngles1Read.l)
  DREAM3D_REQUIRED(AxisAngles2AngleTestValue, == , axisAngles2Read.angle)
  DREAM3D_REQUIRED(AxisAngles2HTestValue, == , axisAngles2Read.h)
  DREAM3D_REQUIRED(AxisAngles2KTestValue, == , axisAngles2Read.k)
  DREAM3D_REQUIRED(AxisAngles2LTestValue, == , axisAngles2Read.l)

  // Test the string vector
  QVector<QString> strVectorRead = filt->getStrVector();
  DREAM3D_REQUIRED(SurfaceMeshVertexArrayNameTestValue, == , strVectorRead[0])
  DREAM3D_REQUIRED(SurfaceMeshFaceArrayNameTestValue, == , strVectorRead[1])
  DREAM3D_REQUIRED(SurfaceMeshEdgeArrayNameTestValue, == , strVectorRead[2])
  DREAM3D_REQUIRED(SolidMeshVertexArrayNameTestValue, == , strVectorRead[3])
  DREAM3D_REQUIRED(SolidMeshFaceArrayNameTestValue, == , strVectorRead[4])
  DREAM3D_REQUIRED(SolidMeshEdgeArrayNameTestValue, == , strVectorRead[5])


  validateArray<int8_t>(filt->getInt8Array());
  validateArray<int16_t>(filt->getInt16Array());
  validateArray<int32_t>(filt->getInt32Array());
  validateArray<int64_t>(filt->getInt64Array());
  validateArray<uint8_t>(filt->getUInt8Array());
  validateArray<uint16_t>(filt->getUInt16Array());
  validateArray<uint32_t>(filt->getUInt32Array());
  validateArray<uint64_t>(filt->getUInt64Array());
  validateArray(filt->getFloatArray());
  validateArray(filt->getDoubleArray());

  DREAM3D_REQUIRED(validateValue<int8_t>(filt->getInt8Value()), == , true);
  DREAM3D_REQUIRED(validateValue<int16_t>(filt->getInt16Value()), == , true);
  DREAM3D_REQUIRED(validateValue<int32_t>(filt->getInt32Value()), == , true);
  DREAM3D_REQUIRED(validateValue<int64_t>(filt->getInt64Value()), == , true);
  DREAM3D_REQUIRED(validateValue<uint8_t>(filt->getUInt8Value()), == , true);
  DREAM3D_REQUIRED(validateValue<uint16_t>(filt->getUInt16Value()), == , true);
  DREAM3D_REQUIRED(validateValue<uint32_t>(filt->getUInt32Value()), == , true);
  DREAM3D_REQUIRED(validateValue<uint64_t>(filt->getUInt64Value()), == , true);
  DREAM3D_COMPARE_FLOATS(-1.521, filt->getFloatValue(), 10);
  DREAM3D_REQUIRE_EQUAL(2.36547, filt->getDoubleValue());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyGenericExampleQSettingsFilterParameters()
{
  GenericExample::Pointer filt = GenericExample::New();
  QFilterParametersReader::Pointer reader = QFilterParametersReader::New();
  reader->openFile(UnitTest::FilterParametersRWTest::GenericExampleFileQSettings, QSettings::IniFormat);
  int index = 1;
  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get(), index);

  // Compare the filter to what was read from the file
  ValidateGenericExampleFilter(filt);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyGenericExampleHDF5FilterParameters()
{
  GenericExample::Pointer filt = GenericExample::New();
  hid_t fid = -1;
  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::OpenDREAM3DFileForReadingPipeline(UnitTest::FilterParametersRWTest::GenericExampleFile, fid);
  DREAM3D_REQUIRED(fid, > , 0);
  DREAM3D_REQUIRED(filt.get(), != , NULL);

  // The Generic Filter is the second (index = 1) filter in the pipeline, so set this index value
  int index = 1;

  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get(), index);

  // Compare the filter to what was read from the file
  ValidateGenericExampleFilter(filt);

  // Close up the HDF5 file
  H5Gclose(reader->getPipelineGroupId()); // Closes the "Pipeline" group
  H5Fclose(fid); // Closes the file
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateGenericExamplePipelineFiles()
{
  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  CreateVolumeDataContainer::Pointer cdc = CreateVolumeDataContainer::New();
  pipeline->pushBack(cdc);

  // This creates our GenericExample Filter instance packed with known values
  GenericExample::Pointer filt = GenerateGenericExample();
  pipeline->pushBack(filt);

  // Add a DataContainerWriter filter to write the pipeline
  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(UnitTest::FilterParametersRWTest::GenericExampleFile);
  writer->setWriteXdmfFile(false);
  pipeline->pushBack(writer);

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  WritePipelineToQSettings(pipeline, UnitTest::FilterParametersRWTest::GenericExampleFileQSettings);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ValidateThresholdExampleFilter(ThresholdExample::Pointer filt)
{
  // Test the CellComparisonInputs
  QVector<ComparisonInput_t> cellComparisonInputsVectorRead = filt->getCellComparisonInputs();
  ComparisonInput_t cellComparisonInputs1 = cellComparisonInputsVectorRead[0];
  ComparisonInput_t cellComparisonInputs2 = cellComparisonInputsVectorRead[1];

  QVector<ComparisonInput_t> featureComparisonInputsVectorRead = filt->getFeatureComparisonInputs();
  ComparisonInput_t featureComparisonInputs1 = featureComparisonInputsVectorRead[0];
  ComparisonInput_t featureComparisonInputs2 = featureComparisonInputsVectorRead[1];

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

  DREAM3D_REQUIRED(CellComparisonInputsArrayName1, == , cellComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(CellComparisonInputsCompOperator1, == , cellComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(CellComparisonInputsCompValue1, == , cellComparisonInputs1.compValue)
  DREAM3D_REQUIRED(CellComparisonInputsArrayName2, == , cellComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(CellComparisonInputsCompOperator2, == , cellComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(CellComparisonInputsCompValue2, == , cellComparisonInputs2.compValue)


  //  union {
  //  int64_t i;
  //  double  d;
  //  } comp;
  //  comp.d = FeatureComparisonInputsCompValue1;
  //  printf("%llx\n", comp.i);
  //  comp.d = featureComparisonInputs1.compValue;
  //  printf("%llx\n", comp.i);

  DREAM3D_REQUIRED(FeatureComparisonInputsArrayName1, == , featureComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(FeatureComparisonInputsCompOperator1, == , featureComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(FeatureComparisonInputsCompValue1, == , featureComparisonInputs1.compValue)
  DREAM3D_REQUIRED(FeatureComparisonInputsArrayName2, == , featureComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(FeatureComparisonInputsCompOperator2, == , featureComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(FeatureComparisonInputsCompValue2, == , featureComparisonInputs2.compValue)

  DREAM3D_REQUIRED(EnsembleComparisonInputsArrayName1, == , ensembleComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(EnsembleComparisonInputsCompOperator1, == , ensembleComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(EnsembleComparisonInputsCompValue1, == , ensembleComparisonInputs1.compValue)
  DREAM3D_REQUIRED(EnsembleComparisonInputsArrayName2, == , ensembleComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(EnsembleComparisonInputsCompOperator2, == , ensembleComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(EnsembleComparisonInputsCompValue2, == , ensembleComparisonInputs2.compValue)

  DREAM3D_REQUIRED(PointComparisonInputsArrayName1, == , pointComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(PointComparisonInputsCompOperator1, == , pointComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(PointComparisonInputsCompValue1, == , pointComparisonInputs1.compValue)
  DREAM3D_REQUIRED(PointComparisonInputsArrayName2, == , pointComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(PointComparisonInputsCompOperator2, == , pointComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(PointComparisonInputsCompValue2, == , pointComparisonInputs2.compValue)

  DREAM3D_REQUIRED(FaceComparisonInputsArrayName1, == , faceComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(FaceComparisonInputsCompOperator1, == , faceComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(FaceComparisonInputsCompValue1, == , faceComparisonInputs1.compValue)
  DREAM3D_REQUIRED(FaceComparisonInputsArrayName2, == , faceComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(FaceComparisonInputsCompOperator2, == , faceComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(FaceComparisonInputsCompValue2, == , faceComparisonInputs2.compValue)

  DREAM3D_REQUIRED(EdgeComparisonInputsArrayName1, == , edgeComparisonInputs1.arrayName)
  DREAM3D_REQUIRED(EdgeComparisonInputsCompOperator1, == , edgeComparisonInputs1.compOperator)
  DREAM3D_REQUIRED(EdgeComparisonInputsCompValue1, == , edgeComparisonInputs1.compValue)
  DREAM3D_REQUIRED(EdgeComparisonInputsArrayName2, == , edgeComparisonInputs2.arrayName)
  DREAM3D_REQUIRED(EdgeComparisonInputsCompOperator2, == , edgeComparisonInputs2.compOperator)
  DREAM3D_REQUIRED(EdgeComparisonInputsCompValue2, == , edgeComparisonInputs2.compValue)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyThresholdExampleQSettingsFilterParameters()
{
  ThresholdExample::Pointer filt = ThresholdExample::New();
  QFilterParametersReader::Pointer reader = QFilterParametersReader::New();
  reader->openFile(UnitTest::FilterParametersRWTest::ThresholdFileQSettings, QSettings::IniFormat);
  int index = 1;
  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get(), index);

  // Compare the filter to what was read from the file
  ValidateThresholdExampleFilter(filt);

  FilterPipeline::Pointer pipeline = QFilterParametersReader::ReadPipelineFromFile(UnitTest::FilterParametersRWTest::ThresholdFile, QSettings::IniFormat);
  DREAM3D_REQUIRE(pipeline.get() != NULL)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VerifyThresholdExampleHDF5FilterParameters()
{
  ThresholdExample::Pointer filt = ThresholdExample::New();
  hid_t fid = -1;
  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::OpenDREAM3DFileForReadingPipeline(UnitTest::FilterParametersRWTest::ThresholdFile, fid);
  DREAM3D_REQUIRED(fid, > , 0)
  DREAM3D_REQUIRED(filt.get(), != , NULL)

  // The Generic Filter is the second (index = 1) filter in the pipeline, so set this index value
  int index = 1;

  // This next line should read all the filter parameters into the filter.
  filt->readFilterParameters( reader.get(), index);

  // Compare the filter to what was read from the file
  ValidateThresholdExampleFilter(filt);

  // Close up the HDF5 file
  H5Gclose(reader->getPipelineGroupId()); // Closes the "Pipeline" group
  H5Fclose(fid); // Closes the file

  FilterPipeline::Pointer pipeline = H5FilterParametersReader::ReadPipelineFromFile(UnitTest::FilterParametersRWTest::ThresholdFile);
  DREAM3D_REQUIRE(pipeline.get() != NULL)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateThresholdExamplePipelineFiles()
{
  // Create our Pipeline object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();


  ThresholdExample::Pointer filt = ThresholdExample::New();

  QVector<ComparisonInput_t> cellComparisonInputsVector = getComparisonInputsVector(CellComparisonInputsArrayName1, CellComparisonInputsCompOperator1,
                                                          CellComparisonInputsCompValue1, CellComparisonInputsArrayName2,
                                                          CellComparisonInputsCompOperator2, CellComparisonInputsCompValue2);
  filt->setCellComparisonInputs(cellComparisonInputsVector);

  QVector<ComparisonInput_t> featureComparisonInputsVector = getComparisonInputsVector(FeatureComparisonInputsArrayName1, FeatureComparisonInputsCompOperator1,
                                                             FeatureComparisonInputsCompValue1, FeatureComparisonInputsArrayName2,
                                                             FeatureComparisonInputsCompOperator2, FeatureComparisonInputsCompValue2);
  filt->setFeatureComparisonInputs(featureComparisonInputsVector);

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
  writer->setOutputFile(UnitTest::FilterParametersRWTest::ThresholdFile);
  writer->setWriteXdmfFile(false);

  CreateVolumeDataContainer::Pointer cdc = CreateVolumeDataContainer::New();
  pipeline->pushBack(cdc);
  pipeline->pushBack(filt);
  pipeline->pushBack(writer);

  pipeline->execute();
  int err = pipeline->getErrorCondition();
  DREAM3D_REQUIRED(err, >= , 0)

  WritePipelineToQSettings(pipeline, UnitTest::FilterParametersRWTest::ThresholdFileQSettings);
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer readPipelineFromHDF5File(hid_t fileId)
{
  int err = 0;
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

  // HDF5: Open the "Pipeline" Group
  hid_t pipelineGroupId = H5Gopen(fileId, DREAM3D::StringConstants::PipelineGroupName.toLatin1().data(), H5P_DEFAULT);
  reader->setPipelineGroupId(pipelineGroupId);

  // Use QH5Lite to ask how many "groups" are in the "Pipeline Group"
  QList<QString> groupList;
  err = QH5Utilities::getGroupObjects(pipelineGroupId, H5O_TYPE_GROUP, groupList);

  // Loop over the items getting the "ClassName" attribute from each group
  QString classNameStr = "";
  for (int i = 0; i < groupList.size(); i++)
  {
    QString ss = QString::number(i, 10);
    err = QH5Lite::readStringAttribute(pipelineGroupId, ss, "ClassName", classNameStr);
    if (err < 0)
    {
      return FilterPipeline::NullPointer();
    }

    // Instantiate a new filter using the FilterFactory based on the value of the className attribute
    FilterManager::Pointer fm = FilterManager::Instance();
    IFilterFactory::Pointer ff = fm->getFactoryForFilter(classNameStr);
    AbstractFilter::Pointer filter = ff->create();

    // Read the parameters
    filter->readFilterParameters( reader.get(), i );

    // Add filter to m_PipelineFromFile
    pipeline->pushBack(filter);
  }

  return pipeline;
}
#endif

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

  DREAM3D_REQUIRED(factories.size(), > , 0)

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


  FilterPipeline::Pointer filePipeline = r.get()->getPipelinePointer();

  AbstractFilter::Pointer absFilter = AbstractFilter::New();
  FilterPipeline::FilterContainerType container = filePipeline->getFilterContainer();

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
  DREAM3D_REQUIRED(w1->getNameOfClass(), == , "DataContainerWriter")
  iter++;

  absFilter = *iter;
  DataContainerReader* r1 = DataContainerReader::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(r1->getNameOfClass(), == , "DataContainerReader")
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
  DREAM3D_REQUIRED(w2->getNameOfClass(), == , "DataContainerWriter")
  iter++;

  absFilter = *iter;
  DataContainerReader* r2 = DataContainerReader::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(r2->getNameOfClass(), == , "DataContainerReader")
  iter++;

  absFilter = *iter;
  DataContainerWriter* w3 = DataContainerWriter::SafePointerDownCast(absFilter.get());
  DREAM3D_REQUIRED(w3->getNameOfClass(), == , "DataContainerWriter")
}


// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{

  FilterFactory<Filt0>::Pointer Filt0Factory = FilterFactory<Filt0>::New();
  FilterManager::Instance()->addFilterFactory("Filt0", Filt0Factory);

  FilterFactory<Filt1>::Pointer Filt1Factory = FilterFactory<Filt1>::New();
  FilterManager::Instance()->addFilterFactory("Filt1", Filt1Factory);

  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )


  DREAM3D_REGISTER_TEST( GenerateGenericExamplePipelineFiles() )
  DREAM3D_REGISTER_TEST( VerifyGenericExampleHDF5FilterParameters() )
  DREAM3D_REGISTER_TEST( VerifyGenericExampleQSettingsFilterParameters() )


  DREAM3D_REGISTER_TEST( GenerateThresholdExamplePipelineFiles() )
  DREAM3D_REGISTER_TEST( VerifyThresholdExampleHDF5FilterParameters() )
  DREAM3D_REGISTER_TEST( VerifyThresholdExampleQSettingsFilterParameters() )


  DREAM3D_REGISTER_TEST( GenerateArraySelectionExamplePipelineFiles() )
  DREAM3D_REGISTER_TEST( VerifyArraySelectionExampleHDF5FilterParameters() )
  DREAM3D_REGISTER_TEST( VerifyArraySelectionExampleQSettingsFilterParameters() )


#if REMOVE_TEST_FILES
  //  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif



  PRINT_TEST_SUMMARY();
  return err;
}
