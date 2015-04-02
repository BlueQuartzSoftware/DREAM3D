/* ============================================================================
* Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
* BlueQuartz Software nor the names of its contributors may be used to endorse
* or promote products derived from this software without specific prior written
* permission.
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
*  This code was written under United States Air Force Contract number
*                           FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QSet>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Plugin/IDREAM3DPlugin.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"
#include "DREAM3DLib/FilterParameters/JsonFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/JsonFilterParametersReader.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DLib/DataContainers/DataContainerArrayProxy.h"
#include "DREAM3DLib/FilterParameters/DynamicTableData.h"

#include "DREAM3DTestFileLocations.h"

namespace StringTest
{
  QString Key = "Test String";

  QString Value = "I am testing the string";
};

namespace StringVectorTest
{
  QString Key = "Test Vector";

  QString Value1 = "VecString1";
  QString Value2 = "VecString2";
  QString Value3 = "VecString3";
  QString Value4 = "VecString4";
};

namespace Int8Test
{
  QString Key = "int8_t test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  int8_t Value = 1;
  int ErrorValueMax = std::numeric_limits<int8_t>().max() + 1;
  int ErrorValueMin = std::numeric_limits<int8_t>().min() - 1;
};

namespace Int16Test
{
  QString Key = "int16_t test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  int16_t Value = 2;
  int ErrorValueMax = std::numeric_limits<int16_t>().max() + 1;
  int ErrorValueMin = std::numeric_limits<int16_t>().min() - 1;
};

namespace Int32Test
{
  QString Key = "int32_t test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  int32_t Value = 3;
  int ErrorValueMax = std::numeric_limits<int32_t>().max() + 1;
  int ErrorValueMin = std::numeric_limits<int32_t>().min() - 1;
};

namespace Int64Test
{
  QString Key = "int64_t test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  int64_t Value = 4;
  int ErrorValueMax = std::numeric_limits<int64_t>().max() + 1;
  int ErrorValueMin = std::numeric_limits<int64_t>().min() - 1;
};

namespace uInt8Test
{
  QString Key = "uint8_t test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  uint8_t Value = 5;
  int ErrorValueMax = std::numeric_limits<uint8_t>().max() + 1;
  int ErrorValueMin = std::numeric_limits<uint8_t>().min() - 1;
};

namespace uInt16Test
{
  QString Key = "uint16_t test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  uint16_t Value = 6;
  int ErrorValueMax = std::numeric_limits<uint16_t>().max() + 1;
  int ErrorValueMin = std::numeric_limits<uint16_t>().min() - 1;
};

namespace uInt32Test
{
  QString Key = "uint32_t test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  uint32_t Value = 7;
  int ErrorValueMax = std::numeric_limits<uint32_t>().max() + 1;
  int ErrorValueMin = std::numeric_limits<uint32_t>().min() - 1;
};

namespace uInt64Test
{
  QString Key = "uint64_t test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  uint64_t Value = 8;
  int ErrorValueMax = std::numeric_limits<uint64_t>().max() + 1;
  int ErrorValueMin = std::numeric_limits<uint64_t>().min() - 1;
};

namespace DoubleTest
{
  QString Key = "double test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  double Value = 9.91235;
  int ErrorValueMax = std::numeric_limits<double>().max() + 1;
  int ErrorValueMin = std::numeric_limits<double>().min() - 1;
};

namespace FloatTest
{
  QString Key = "float test";
  QString ErrorValueMaxKey = "Error Value Max";
  QString ErrorValueMinKey = "Error Value Min";

  float Value = 10.287f;
  int ErrorValueMax = std::numeric_limits<float>().max() + 1;
  int ErrorValueMin = std::numeric_limits<float>().min() - 1;
};

namespace Int8VectorTest
{
  QString Key = "QVector<int8_t> test";

  const int8_t Value1 = 11;
  const int8_t Value2 = 12;
  const int8_t Value3 = 13;
  const int8_t Value4 = 14;
  const int8_t Value5 = 15;
};

namespace Int16VectorTest
{
  QString Key = "QVector<int16_t> test";

  const int16_t Value1 = 11;
  const int16_t Value2 = 12;
  const int16_t Value3 = 13;
  const int16_t Value4 = 14;
  const int16_t Value5 = 15;
};

namespace Int32VectorTest
{
  QString Key = "QVector<int32_t> test";

  const int32_t Value1 = 11;
  const int32_t Value2 = 12;
  const int32_t Value3 = 13;
  const int32_t Value4 = 14;
  const int32_t Value5 = 15;
};

namespace Int64VectorTest
{
  QString Key = "QVector<int64_t> test";

  const int64_t Value1 = 11;
  const int64_t Value2 = 12;
  const int64_t Value3 = 13;
  const int64_t Value4 = 14;
  const int64_t Value5 = 15;
};

namespace uInt8VectorTest
{
  QString Key = "QVector<uint8_t> test";

  const uint8_t Value1 = 11;
  const uint8_t Value2 = 12;
  const uint8_t Value3 = 13;
  const uint8_t Value4 = 14;
  const uint8_t Value5 = 15;
};

namespace uInt16VectorTest
{
  QString Key = "QVector<uint16_t> test";

  const uint16_t Value1 = 11;
  const uint16_t Value2 = 12;
  const uint16_t Value3 = 13;
  const uint16_t Value4 = 14;
  const uint16_t Value5 = 15;
};

namespace uInt32VectorTest
{
  QString Key = "QVector<uint32_t> test";

  const uint32_t Value1 = 11;
  const uint32_t Value2 = 12;
  const uint32_t Value3 = 13;
  const uint32_t Value4 = 14;
  const uint32_t Value5 = 15;
};

namespace uInt64VectorTest
{
  QString Key = "QVector<uint64_t> test";

  const uint64_t Value1 = 11;
  const uint64_t Value2 = 12;
  const uint64_t Value3 = 13;
  const uint64_t Value4 = 14;
  const uint64_t Value5 = 15;
};

namespace DoubleVectorTest
{
  QString Key = "QVector<double> test";

  const double Value1 = 11;
  const double Value2 = 12;
  const double Value3 = 13;
  const double Value4 = 14;
  const double Value5 = 15;
};

namespace FloatVectorTest
{
  QString Key = "QVector<float> test";

  const float Value1 = 11.2f;
  const float Value2 = 12.3f;
  const float Value3 = 13.4f;
  const float Value4 = 14.5f;
  const float Value5 = 15.6f;
};

namespace IntVec3Test
{
  QString Key = "IntVec3_t test";

  int X = 1;
  int Y = 2;
  int Z = 3;
};

namespace FloatVec3Test
{
  QString Key = "FloatVec3_t test";

  float X = 1.2;
  float Y = 2.65;
  float Z = 3.975;
};

namespace FloatVec4Test
{
  QString Key = "FloatVec4_t test";

  float A = 1.2;
  float B = 2.65;
  float C = 3.975;
  float D = 4.6876;
};

namespace FloatVec21Test
{
  QString Key = "FloatVec21_t test";

  float V11 = 1.2;
  float V12 = 2.65;
  float V13 = 3.975;
  float V14 = 4.6876;
  float V15 = 1.2;
  float V16 = 2.65;
  float V22 = 3.975;
  float V23 = 4.6876;
  float V24 = 1.2;
  float V25 = 2.65;
  float V26 = 3.975;
  float V33 = 4.6876;
  float V34 = 1.2;
  float V35 = 2.65;
  float V36 = 3.975;
  float V44 = 4.6876;
  float V45 = 1.2;
  float V46 = 2.65;
  float V55 = 3.975;
  float V56 = 4.6876;
  float V66 = 1.2;
};

namespace Float2ndOrderPolyTest
{
  QString Key = "Float2ndOrderPoly_t test";

  float C20 = 1.2;
  float C02 = 2.65;
  float C11 = 3.975;
  float C10 = 4.6876;
  float C01 = 1.2;
  float C00 = 2.65;
};

namespace Float3rdOrderPolyTest
{
  QString Key = "Float3rdOrderPoly_t test";

  float C30 = 1.2;
  float C03 = 2.65;
  float C21 = 3.975;
  float C12 = 4.6876;
  float C20 = 1.2;
  float C02 = 1.2;
  float C11 = 2.65;
  float C10 = 3.975;
  float C01 = 4.6876;
  float C00 = 1.2;
};

namespace Float4thOrderPolyTest
{
  QString Key = "Float4thOrderPoly_t test";

  float C02 = 1.2;
  float C11 = 2.65;
  float C10 = 3.975;
  float C01 = 4.6876;
  float C00 = 1.2;
  float C40 = 1.2;
  float C04 = 2.65;
  float C31 = 3.975;
  float C13 = 4.6876;
  float C22 = 1.2;
  float C30 = 1.2;
  float C03 = 2.65;
  float C21 = 3.975;
  float C12 = 4.6876;
  float C20 = 1.2;
};

namespace FileListInfoTest
{
  QString Key = "FileListInfo_t test";

  qint32 PaddingDigits = 1.2;
  quint32 Ordering = 2.65;
  qint32 StartIndex = 3.975;
  qint32 EndIndex = 4.6876;
  QString InputPath = "TestString1";
  QString FilePrefix = "TestString2";
  QString FileSuffix = "TestString3";
  QString FileExtension = "TestString4";
};

namespace ComparisonInputTest
{
  QString Key = "ComparisonInput_t test";

  QString DataContainerName = "DataContainer1";
  QString AttributeMatrixName = "AttributeMatrix1";
  QString AttributeArrayName = "AttributeArray1";
  int CompOperator = 3;
  double CompValue = 4.6876;
};

namespace MultipleComparisonInputTest
{
  QString Key = "ComparisonInputs test";

  QString DataContainerName1 = "DataContainer1";
  QString AttributeMatrixName1 = "AttributeMatrix1";
  QString AttributeArrayName1 = "AttributeArray1";
  int CompOperator1 = 3;
  double CompValue1 = 4.6876;

  QString DataContainerName2 = "DataContainer2";
  QString AttributeMatrixName2 = "AttributeMatrix2";
  QString AttributeArrayName2 = "AttributeArray2";
  int CompOperator2 = 4;
  double CompValue2 = 2.935;
};

namespace AxisAngleInputTest
{
  QString Key = "AxisAngleInput_t test";

  float Angle = 0.234;
  float H = 1.23;
  float K = 4.32;
  float L = 3.54;
};

namespace MultipleAxisAngleInputTest
{
  QString Key = "QVector<AxisAngleInput_t> test";

  float Angle1 = 0.234;
  float H1 = 1.23;
  float K1 = 4.32;
  float L1 = 3.54;

  float Angle2 = 6.32;
  float H2 = 9.5677;
  float K2 = 4.8124;
  float L2 = 6.315;
};

namespace StringSetTest
{
  QString Key = "QSet<QString> test";

  QString String1 = "StringSetTest-String1";
  QString String2 = "StringSetTest-String2";
  QString String3 = "StringSetTest-String3";
  QString String4 = "StringSetTest-String4";
};

namespace DataContainerArrayProxyTest
{
  QString Key = "DataContainerArrayProxy test";

  QString DcProxyName = "DataContainer1";
  QString AmProxyName1 = "AttributeMatrix1";
  QString AmProxyName2 = "AttributeMatrix2";
  QString DaProxyName1 = "AttributeArray1";
  QString DaProxyName2 = "AttributeArray2";
  QString DaProxyName3 = "AttributeArray3";
};

namespace DataArrayPathTest
{
  QString Key = "DataArrayPath test";

  QString DCName = "DataContainer";
  QString AMName = "AttributeMatrix";
  QString DAName = "AttributeArray";
};

namespace MultipleDataArrayPathTest
{
  QString Key = "QVector<DataArrayPath> test";

  QString DCName1 = "DataContainer1";
  QString AMName1 = "AttributeMatrix1";
  QString DAName1 = "AttributeArray1";

  QString DCName2 = "DataContainer2";
  QString AMName2 = "AttributeMatrix2";
  QString DAName2 = "AttributeArray2";
};

namespace DynamicTableDataTest
{
  QString Key = "DynamicTableData test";

  int NumRows = 6;
  int NumColumns = 8;
  QString RowHeader1 = "Row 1";
  QString RowHeader2 = "Row 2";
  QString RowHeader3 = "Row 3";
  QString RowHeader4 = "Row 4";
  QString RowHeader5 = "Row 5";
  QString RowHeader6 = "Row 6";
  QString ColumnHeader1 = "Column 1";
  QString ColumnHeader2 = "Column 2";
  QString ColumnHeader3 = "Column 3";
  QString ColumnHeader4 = "Column 4";
  QString ColumnHeader5 = "Column 5";
  QString ColumnHeader6 = "Column 6";
  QString ColumnHeader7 = "Column 7";
  QString ColumnHeader8 = "Column 8";
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::FilterParametersRWTest::OutputFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestJsonWriter()
{
  JsonFilterParametersWriter::Pointer writer = JsonFilterParametersWriter::New();
  writer->setFileName(UnitTest::FilterParametersRWTest::OutputFile);
  writer->setPipelineName("TestPipelineName");

  int err = 0;

  err = writer->openFilterGroup(NULL, 0);
  DREAM3D_REQUIRE_EQUAL(err, 0)

  // QString test
  {
    err = writer->writeValue(StringTest::Key, StringTest::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<QString> test
  {
    QVector<QString> vector;
    vector.push_back(StringVectorTest::Value1);
    vector.push_back(StringVectorTest::Value2);
    vector.push_back(StringVectorTest::Value3);
    vector.push_back(StringVectorTest::Value4);
    err = writer->writeValue(StringVectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // int8_t test
  {
    err = writer->writeValue(Int8Test::Key, Int8Test::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(Int8Test::ErrorValueMaxKey, Int8Test::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(Int8Test::ErrorValueMinKey, Int8Test::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // int16_t test
  {
    err = writer->writeValue(Int16Test::Key, Int16Test::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(Int16Test::ErrorValueMaxKey, Int16Test::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(Int16Test::ErrorValueMinKey, Int16Test::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // int32_t test
  {
    err = writer->writeValue(Int32Test::Key, Int32Test::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(Int32Test::ErrorValueMaxKey, Int32Test::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(Int32Test::ErrorValueMinKey, Int32Test::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // int64_t test
  {
    err = writer->writeValue(Int64Test::Key, Int64Test::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(Int64Test::ErrorValueMaxKey, Int64Test::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(Int64Test::ErrorValueMinKey, Int64Test::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // uint8_t test
  {
    err = writer->writeValue(uInt8Test::Key, uInt8Test::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(uInt8Test::ErrorValueMaxKey, uInt8Test::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(uInt8Test::ErrorValueMinKey, uInt8Test::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // uint16_t test
  {
    err = writer->writeValue(uInt16Test::Key, uInt16Test::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(uInt16Test::ErrorValueMaxKey, uInt16Test::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(uInt16Test::ErrorValueMinKey, uInt16Test::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // uint32_t test
  {
    err = writer->writeValue(uInt32Test::Key, uInt32Test::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(uInt32Test::ErrorValueMaxKey, uInt32Test::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(uInt32Test::ErrorValueMinKey, uInt32Test::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // uint64_t test
  {
    err = writer->writeValue(uInt64Test::Key, uInt64Test::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(uInt64Test::ErrorValueMaxKey, uInt64Test::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(uInt64Test::ErrorValueMinKey, uInt64Test::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // double test
  {
    err = writer->writeValue(DoubleTest::Key, DoubleTest::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(DoubleTest::ErrorValueMaxKey, DoubleTest::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(DoubleTest::ErrorValueMinKey, DoubleTest::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // float test
  {
    err = writer->writeValue(FloatTest::Key, FloatTest::Value);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(FloatTest::ErrorValueMaxKey, FloatTest::ErrorValueMax);
    DREAM3D_REQUIRE_EQUAL(err, 0)

      err = writer->writeValue(FloatTest::ErrorValueMinKey, FloatTest::ErrorValueMin);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<int8_t> test
  {
    QVector<int8_t> vector;
    vector.push_back(Int8VectorTest::Value1);
    vector.push_back(Int8VectorTest::Value2);
    vector.push_back(Int8VectorTest::Value3);
    vector.push_back(Int8VectorTest::Value4);
    vector.push_back(Int8VectorTest::Value5);
    err = writer->writeValue(Int8VectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<int16_t> test
  {
    QVector<int16_t> vector;
    vector.push_back(Int16VectorTest::Value1);
    vector.push_back(Int16VectorTest::Value2);
    vector.push_back(Int16VectorTest::Value3);
    vector.push_back(Int16VectorTest::Value4);
    vector.push_back(Int16VectorTest::Value5);
    err = writer->writeValue(Int16VectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<int32_t> test
  {
    QVector<int32_t> vector;
    vector.push_back(Int32VectorTest::Value1);
    vector.push_back(Int32VectorTest::Value2);
    vector.push_back(Int32VectorTest::Value3);
    vector.push_back(Int32VectorTest::Value4);
    vector.push_back(Int32VectorTest::Value5);
    err = writer->writeValue(Int32VectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<int64_t> test
  {
    QVector<int64_t> vector;
    vector.push_back(Int64VectorTest::Value1);
    vector.push_back(Int64VectorTest::Value2);
    vector.push_back(Int64VectorTest::Value3);
    vector.push_back(Int64VectorTest::Value4);
    vector.push_back(Int64VectorTest::Value5);
    err = writer->writeValue(Int64VectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<uint8_t> test
  {
    QVector<uint8_t> vector;
    vector.push_back(uInt8VectorTest::Value1);
    vector.push_back(uInt8VectorTest::Value2);
    vector.push_back(uInt8VectorTest::Value3);
    vector.push_back(uInt8VectorTest::Value4);
    vector.push_back(uInt8VectorTest::Value5);
    err = writer->writeValue(uInt8VectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<uint16_t> test
  {
    QVector<uint16_t> vector;
    vector.push_back(uInt16VectorTest::Value1);
    vector.push_back(uInt16VectorTest::Value2);
    vector.push_back(uInt16VectorTest::Value3);
    vector.push_back(uInt16VectorTest::Value4);
    vector.push_back(uInt16VectorTest::Value5);
    err = writer->writeValue(uInt16VectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<uint32_t> test
  {
    QVector<uint32_t> vector;
    vector.push_back(uInt32VectorTest::Value1);
    vector.push_back(uInt32VectorTest::Value2);
    vector.push_back(uInt32VectorTest::Value3);
    vector.push_back(uInt32VectorTest::Value4);
    vector.push_back(uInt32VectorTest::Value5);
    err = writer->writeValue(uInt32VectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<uint64_t> test
  {
    QVector<uint64_t> vector;
    vector.push_back(uInt64VectorTest::Value1);
    vector.push_back(uInt64VectorTest::Value2);
    vector.push_back(uInt64VectorTest::Value3);
    vector.push_back(uInt64VectorTest::Value4);
    vector.push_back(uInt64VectorTest::Value5);
    err = writer->writeValue(uInt64VectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<double> test
  {
    QVector<double> vector;
    vector.push_back(DoubleVectorTest::Value1);
    vector.push_back(DoubleVectorTest::Value2);
    vector.push_back(DoubleVectorTest::Value3);
    vector.push_back(DoubleVectorTest::Value4);
    vector.push_back(DoubleVectorTest::Value5);
    err = writer->writeValue(DoubleVectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<float> test
  {
    QVector<float> vector;
    vector.push_back(FloatVectorTest::Value1);
    vector.push_back(FloatVectorTest::Value2);
    vector.push_back(FloatVectorTest::Value3);
    vector.push_back(FloatVectorTest::Value4);
    vector.push_back(FloatVectorTest::Value5);
    err = writer->writeValue(FloatVectorTest::Key, vector);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // IntVec3_t test
  {
    IntVec3_t vec3;
    vec3.x = IntVec3Test::X;
    vec3.y = IntVec3Test::Y;
    vec3.z = IntVec3Test::Z;
    err = writer->writeValue(IntVec3Test::Key, vec3);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // FloatVec3_t test
  {
    FloatVec3_t vec3;
    vec3.x = FloatVec3Test::X;
    vec3.y = FloatVec3Test::Y;
    vec3.z = FloatVec3Test::Z;
    err = writer->writeValue(FloatVec3Test::Key, vec3);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // FloatVec4_t test
  {
    FloatVec4_t vec4;
    vec4.a = FloatVec4Test::A;
    vec4.b = FloatVec4Test::B;
    vec4.c = FloatVec4Test::C;
    vec4.d = FloatVec4Test::D;
    err = writer->writeValue(FloatVec4Test::Key, vec4);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // FloatVec21_t test
  {
    FloatVec21_t vec21;
    vec21.v11 = FloatVec21Test::V11;
    vec21.v12 = FloatVec21Test::V12;
    vec21.v13 = FloatVec21Test::V13;
    vec21.v14 = FloatVec21Test::V14;
    vec21.v15 = FloatVec21Test::V15;
    vec21.v16 = FloatVec21Test::V16;
    vec21.v22 = FloatVec21Test::V22;
    vec21.v23 = FloatVec21Test::V23;
    vec21.v24 = FloatVec21Test::V24;
    vec21.v25 = FloatVec21Test::V25;
    vec21.v26 = FloatVec21Test::V26;
    vec21.v33 = FloatVec21Test::V33;
    vec21.v34 = FloatVec21Test::V34;
    vec21.v35 = FloatVec21Test::V35;
    vec21.v36 = FloatVec21Test::V36;
    vec21.v44 = FloatVec21Test::V44;
    vec21.v45 = FloatVec21Test::V45;
    vec21.v46 = FloatVec21Test::V46;
    vec21.v55 = FloatVec21Test::V55;
    vec21.v56 = FloatVec21Test::V56;
    vec21.v66 = FloatVec21Test::V66;

    err = writer->writeValue(FloatVec21Test::Key, vec21);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // Float2ndOrderPoly_t test
  {
    Float2ndOrderPoly_t poly;
    poly.c20 = Float2ndOrderPolyTest::C20;
    poly.c02 = Float2ndOrderPolyTest::C02;
    poly.c11 = Float2ndOrderPolyTest::C11;
    poly.c10 = Float2ndOrderPolyTest::C10;
    poly.c01 = Float2ndOrderPolyTest::C01;
    poly.c00 = Float2ndOrderPolyTest::C00;

    err = writer->writeValue(Float2ndOrderPolyTest::Key, poly);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // Float3rdOrderPoly_t test
  {
    Float3rdOrderPoly_t poly;
    poly.c30 = Float3rdOrderPolyTest::C30;
    poly.c03 = Float3rdOrderPolyTest::C03;
    poly.c21 = Float3rdOrderPolyTest::C21;
    poly.c12 = Float3rdOrderPolyTest::C12;
    poly.c20 = Float3rdOrderPolyTest::C20;
    poly.c02 = Float3rdOrderPolyTest::C02;
    poly.c11 = Float3rdOrderPolyTest::C11;
    poly.c10 = Float3rdOrderPolyTest::C10;
    poly.c01 = Float3rdOrderPolyTest::C01;
    poly.c00 = Float3rdOrderPolyTest::C00;

    err = writer->writeValue(Float3rdOrderPolyTest::Key, poly);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // Float4thOrderPoly_t test
  {
    Float4thOrderPoly_t poly;
    poly.c02 = Float4thOrderPolyTest::C02;
    poly.c11 = Float4thOrderPolyTest::C11;
    poly.c10 = Float4thOrderPolyTest::C10;
    poly.c01 = Float4thOrderPolyTest::C01;
    poly.c00 = Float4thOrderPolyTest::C00;
    poly.c40 = Float4thOrderPolyTest::C40;
    poly.c04 = Float4thOrderPolyTest::C04;
    poly.c31 = Float4thOrderPolyTest::C31;
    poly.c13 = Float4thOrderPolyTest::C13;
    poly.c22 = Float4thOrderPolyTest::C22;
    poly.c30 = Float4thOrderPolyTest::C30;
    poly.c03 = Float4thOrderPolyTest::C03;
    poly.c21 = Float4thOrderPolyTest::C21;
    poly.c12 = Float4thOrderPolyTest::C12;
    poly.c20 = Float4thOrderPolyTest::C20;

    err = writer->writeValue(Float4thOrderPolyTest::Key, poly);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // FileListInfo_t test
  {
    FileListInfo_t info;
    info.PaddingDigits = FileListInfoTest::PaddingDigits;
    info.Ordering = FileListInfoTest::Ordering;
    info.StartIndex = FileListInfoTest::StartIndex;
    info.EndIndex = FileListInfoTest::EndIndex;
    info.InputPath = FileListInfoTest::InputPath;
    info.FilePrefix = FileListInfoTest::FilePrefix;
    info.FileSuffix = FileListInfoTest::FileSuffix;
    info.FileExtension = FileListInfoTest::FileExtension;

    err = writer->writeValue(FileListInfoTest::Key, info);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // ComparisonInput_t test
  {
    ComparisonInput_t input;
    input.dataContainerName = ComparisonInputTest::DataContainerName;
    input.attributeMatrixName = ComparisonInputTest::AttributeMatrixName;
    input.attributeArrayName = ComparisonInputTest::AttributeArrayName;
    input.compOperator = ComparisonInputTest::CompOperator;
    input.compValue = ComparisonInputTest::CompValue;

    err = writer->writeValue(ComparisonInputTest::Key, input);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // ComparisonInputs test
  {
    ComparisonInputs inputs;

    ComparisonInput_t input1;
    input1.dataContainerName = MultipleComparisonInputTest::DataContainerName1;
    input1.attributeMatrixName = MultipleComparisonInputTest::AttributeMatrixName1;
    input1.attributeArrayName = MultipleComparisonInputTest::AttributeArrayName1;
    input1.compOperator = MultipleComparisonInputTest::CompOperator1;
    input1.compValue = MultipleComparisonInputTest::CompValue1;
    inputs.addInput(input1);

    ComparisonInput_t input2;
    input2.dataContainerName = MultipleComparisonInputTest::DataContainerName2;
    input2.attributeMatrixName = MultipleComparisonInputTest::AttributeMatrixName2;
    input2.attributeArrayName = MultipleComparisonInputTest::AttributeArrayName2;
    input2.compOperator = MultipleComparisonInputTest::CompOperator2;
    input2.compValue = MultipleComparisonInputTest::CompValue2;
    inputs.addInput(input2);

    err = writer->writeValue(MultipleComparisonInputTest::Key, inputs);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // AxisAngleInput_t test
  {
    AxisAngleInput_t input;
    input.angle = AxisAngleInputTest::Angle;
    input.h = AxisAngleInputTest::H;
    input.k = AxisAngleInputTest::K;
    input.l = AxisAngleInputTest::L;

    err = writer->writeValue(AxisAngleInputTest::Key, input);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<AxisAngleInput_t> test
  {
    QVector<AxisAngleInput_t> inputs;

    AxisAngleInput_t input1;
    input1.angle = MultipleAxisAngleInputTest::Angle1;
    input1.h = MultipleAxisAngleInputTest::H1;
    input1.k = MultipleAxisAngleInputTest::K1;
    input1.l = MultipleAxisAngleInputTest::L1;
    inputs.push_back(input1);

    AxisAngleInput_t input2;
    input2.angle = MultipleAxisAngleInputTest::Angle2;
    input2.h = MultipleAxisAngleInputTest::H2;
    input2.k = MultipleAxisAngleInputTest::K2;
    input2.l = MultipleAxisAngleInputTest::L2;
    inputs.push_back(input2);

    err = writer->writeValue(MultipleAxisAngleInputTest::Key, inputs);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QSet<QString> test
  {
    QSet<QString> set;
    set << StringSetTest::String1 << StringSetTest::String2 << StringSetTest::String3 << StringSetTest::String4;

    err = writer->writeArraySelections(StringSetTest::Key, set);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // DataContainerArrayProxy test
  {
    DataContainerArrayProxy proxy;
    DataContainerProxy dcProxy(DataContainerArrayProxyTest::DcProxyName);
    AttributeMatrixProxy amProxy1(DataContainerArrayProxyTest::AmProxyName1);
    AttributeMatrixProxy amProxy2(DataContainerArrayProxyTest::AmProxyName2);
    DataArrayProxy daProxy1(DataContainerArrayProxyTest::DaProxyName1, DataContainerArrayProxyTest::DaProxyName1);
    DataArrayProxy daProxy2(DataContainerArrayProxyTest::DaProxyName2, DataContainerArrayProxyTest::DaProxyName2);
    DataArrayProxy daProxy3(DataContainerArrayProxyTest::DaProxyName3, DataContainerArrayProxyTest::DaProxyName3);
    amProxy1.dataArrays.insert(daProxy1.name, daProxy1);
    amProxy1.dataArrays.insert(daProxy2.name, daProxy2);
    amProxy2.dataArrays.insert(daProxy3.name, daProxy3);
    dcProxy.attributeMatricies.insert(amProxy1.name, amProxy1);
    dcProxy.attributeMatricies.insert(amProxy2.name, amProxy2);
    proxy.dataContainers.insert(dcProxy.name, dcProxy);

    err = writer->writeValue(DataContainerArrayProxyTest::Key, proxy);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // DataArrayPath test
  {
    DataArrayPath path(DataArrayPathTest::DCName, DataArrayPathTest::AMName, DataArrayPathTest::DAName);

    err = writer->writeValue(DataArrayPathTest::Key, path);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // QVector<DataArrayPath> test
  {
    QVector<DataArrayPath> paths;
    paths.push_back(DataArrayPath(MultipleDataArrayPathTest::DCName1, MultipleDataArrayPathTest::AMName1, MultipleDataArrayPathTest::DAName1));
    paths.push_back(DataArrayPath(MultipleDataArrayPathTest::DCName2, MultipleDataArrayPathTest::AMName2, MultipleDataArrayPathTest::DAName2));

    err = writer->writeValue(MultipleDataArrayPathTest::Key, paths);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }

  // DynamicTableData test
  {
    QStringList rHeaders, cHeaders;
    rHeaders << DynamicTableDataTest::RowHeader1 << DynamicTableDataTest::RowHeader2 << DynamicTableDataTest::RowHeader3 << DynamicTableDataTest::RowHeader4;
    rHeaders << DynamicTableDataTest::RowHeader5 << DynamicTableDataTest::RowHeader6;
    cHeaders << DynamicTableDataTest::ColumnHeader1 << DynamicTableDataTest::ColumnHeader2 << DynamicTableDataTest::ColumnHeader3 << DynamicTableDataTest::ColumnHeader4;
    cHeaders << DynamicTableDataTest::ColumnHeader5 << DynamicTableDataTest::ColumnHeader6 << DynamicTableDataTest::ColumnHeader7 << DynamicTableDataTest::ColumnHeader8;
    DynamicTableData data(DynamicTableDataTest::NumRows, DynamicTableDataTest::NumColumns, rHeaders, cHeaders);

    err = writer->writeValue(DynamicTableDataTest::Key, data);
    DREAM3D_REQUIRE_EQUAL(err, 0)
  }


  err = writer->closeFilterGroup();
  DREAM3D_REQUIRE_EQUAL(err, 0)

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestJsonReader()
{
  JsonFilterParametersReader::Pointer reader = JsonFilterParametersReader::New();
  int err = reader->openFile(UnitTest::FilterParametersRWTest::OutputFile);
  DREAM3D_REQUIRE_EQUAL(err, 0)

  err = reader->openFilterGroup(NULL, 0);
  DREAM3D_REQUIRE_EQUAL(err, 0)

  QString str = reader->readString(StringTest::Key, "");
  DREAM3D_REQUIRE_EQUAL(str, StringTest::Value)

  err = reader->closeFilterGroup();
  DREAM3D_REQUIRE_EQUAL(err, 0)

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("FilterParametersRWTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST(loadFilterPlugins());

  DREAM3D_REGISTER_TEST(TestJsonWriter())
  DREAM3D_REGISTER_TEST(TestJsonReader())

  DREAM3D_REGISTER_TEST(RemoveTestFiles())
  PRINT_TEST_SUMMARY();

  return err;
}

