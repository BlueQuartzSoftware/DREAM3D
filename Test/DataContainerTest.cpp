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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/StructArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SIMPLib/DataArrays/StringDataArray.hpp"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/FilterParameters/QFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/H5FilterParametersWriter.h"
#include "SIMPLib/FilterParameters/QFilterParametersReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"


#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "DREAM3DTestFileLocations.h"

#define helper(a, b)\
  a##b


#define MAKE_ARRAY(m_msgType, name)\
  IDataArray::Pointer m_msgType##Ptr = DataArray<m_msgType>::CreateArray(5, name);\
  dataContainer->addCellData(name, m_msgType##Ptr);


#define TEST_DATA_CONTAINER(Type, DCType)\
  {  IDataArray::Pointer t_##Type = Type::CreateArray(5);\
    t_##Type->setName( #Type );\
    m->add##DCType(#Type, t_##Type);\
    IDataArray::Pointer t = m->get##DCType(#Type);\
    DREAM3D_TEST_POINTER(ptr, !=, NULL);\
    t = m->removeCellFeatureData(#Type);\
    DREAM3D_TEST_POINTER(ptr, !=, NULL);\
    t = m->get##DCType(#Type);\
    DREAM3D_REQUIRE_EQUAL(t.get(), NULL);\
  }

namespace DataContainerIOTest
{
  static const size_t Offset = 800;
  static const size_t XSize = 5;
  static const size_t YSize = 4;
  static const size_t ZSize = 3;


  QString TestDir()
  {
    return UnitTest::TestTempDir + QString::fromLatin1("/DataContainerIOTest");
  }

  QString TestFile()
  {
    return TestDir() + QString::fromLatin1("/DataContainerIOTest.h5");
  }

  QString TestFile2()
  {
    return TestDir() + QString::fromLatin1("/DataContainerIOTest_Rewrite.h5");
  }

  QString TestFile3()
  {
    return TestDir() + QString::fromLatin1("/DataContainerIOTest_Subset.h5");
  }

  QString IniFile()
  {
    return TestDir() + QString::fromLatin1("/DataContainerProxyTest.ini");
  }

  QString H5File()
  {
    return TestDir() + QString::fromLatin1("/DataContainerProxyTest.h5");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(DataContainerIOTest::TestFile());
  QFile::remove(DataContainerIOTest::TestFile2());
  QFile::remove(DataContainerIOTest::TestFile3());
  QFile::remove(DataContainerIOTest::IniFile());
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getCellAttributeMatrixName()
{
  return DREAM3D::Defaults::CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getCellFeatureAttributeMatrixName()
{
  return DREAM3D::Defaults::CellFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getCellEnsembleAttributeMatrixName()
{
  return DREAM3D::Defaults::CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void CreateDataArray(AttributeMatrix::Pointer attrMat, QVector<size_t> compDims)
{
  QString name("[");
  for(int i = 0; i < compDims.size(); i++)
  {
    name = name + QString::number(compDims[i]);
    if(i < compDims.size() - 1) { name = name + "x"; }
  }
  name = name + "]";
  typename DataArray<T>::Pointer foo = DataArray<T>::CreateArray(attrMat->getTupleDimensions(), compDims, "RENAME_ME");
  foo->setName(foo->getFullNameOfClass() + name);
  foo->initializeWithValue(static_cast<T>(1) );
  attrMat->addAttributeArray(foo->getName(), foo);

  QString autoName = foo->getName() + "_Auto";
  attrMat->createNonPrereqArray< DataArray<T>, AbstractFilter, T>(NULL, autoName, static_cast<T>(10), compDims);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateStringArray(AttributeMatrix::Pointer attrMat, QVector<size_t> compDims)
{
  QString name("ExampleStringDataArray");
  StringDataArray::Pointer data = StringDataArray::CreateArray(attrMat->getNumTuples(), name);
  for(size_t i = 0; i < attrMat->getNumTuples(); i++)
  {
    QString value = QString("string_%1").arg(i);
    data->setValue(i, value);
  }
  attrMat->addAttributeArray(data->getName(), data);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillAttributeMatrix(AttributeMatrix::Pointer attrMat, QVector<size_t> compDims)
{
  CreateDataArray<int8_t>(attrMat, compDims);
  CreateDataArray<uint8_t>(attrMat, compDims);
  CreateDataArray<int16_t>(attrMat, compDims);
  CreateDataArray<uint16_t>(attrMat, compDims);
  CreateDataArray<int32_t>(attrMat, compDims);
  CreateDataArray<uint32_t>(attrMat, compDims);
  CreateDataArray<int64_t>(attrMat, compDims);
  CreateDataArray<uint64_t>(attrMat, compDims);
  CreateDataArray<float>(attrMat, compDims);
  CreateDataArray<double>(attrMat, compDims);
  CreateStringArray(attrMat, compDims);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PopulateVolumeDataContainer(DataContainer::Pointer dc, QVector<size_t> tupleDims, const QString& name)
{
  // Create the attribute matrix with the dimensions and name
  AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tupleDims, name, DREAM3D::AttributeMatrixType::Cell);
  QVector<size_t> compDims(1, 1); // Create a Single Scalar Component (numComp = 1) Data Array
  FillAttributeMatrix(attrMat, compDims);

  // Add Data Arrays that have a [1x3] component dimensions
  compDims[0] = 3;
  FillAttributeMatrix(attrMat, compDims);

  // Simulate something like a 2D Kikuchi pattern at each data array point, i.e., a 2D image. In this example the size
  // of the image is 10 px high x 20 px Wide. The size of the dimensions go slowest to fastest moving (zyx)
  compDims[0] = 10;
  compDims.push_back(20);
  FillAttributeMatrix(attrMat, compDims);

  // Add the AttributeMatrix to the Data Container
  dc->addAttributeMatrix(attrMat->getName(), attrMat);


  QString autoAddName = name + QString::fromLatin1("_Auto");
  AttributeMatrix::Pointer autoAttrMat = dc->createNonPrereqAttributeMatrix<AbstractFilter>(NULL, autoAddName, tupleDims, DREAM3D::AttributeMatrixType::Cell);
  compDims.resize(0);
  compDims.push_back(1);
  FillAttributeMatrix(autoAttrMat, compDims);

  // Add Data Arrays that have a [1x3] component dimensions
  compDims[0] = 3;
  FillAttributeMatrix(autoAttrMat, compDims);

  // Simulate something like a 2D Kikuchi pattern at each data array point, i.e., a 2D image. In this example the size
  // of the image is 10 px high x 20 px Wide. The size of the dimensions go slowest to fastest moving (zyx)
  compDims[0] = 10;
  compDims.push_back(20);
  FillAttributeMatrix(autoAttrMat, compDims);



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataContainerWriter()
{
  QVector<size_t> tupleDims;
  DataContainerArray::Pointer dca = DataContainerArray::New();
  size_t nx = DataContainerIOTest::XSize;
  size_t ny = DataContainerIOTest::YSize;
  size_t nz = DataContainerIOTest::ZSize;


  DataContainer::Pointer dc = DataContainer::New("DataContainer_TEST");
  ImageGeom::Pointer image = ImageGeom::CreateGeometry("ImageGeom_TEST");
  image->setDimensions(nx, ny, nz);
  dc->setGeometry(image);


  // 1D VolumeDataContainer
  tupleDims.push_back(nx);
  {
    DataContainer::Pointer dc = dca->createNonPrereqDataContainer<AbstractFilter>(NULL, "1D_VolumeDataContainer");
    PopulateVolumeDataContainer(dc, tupleDims, "1D_AttributeMatrix");
  }

  // 2D VolumeDataContainer
  tupleDims.push_back(ny);
  {
    DataContainer::Pointer dc = dca->createNonPrereqDataContainer<AbstractFilter>(NULL, "2D_VolumeDataContainer");
    PopulateVolumeDataContainer(dc, tupleDims, "2D_AttributeMatrix");
  }

  // 3D VolumeDataContainer
  tupleDims.push_back(nz);
  {
    DataContainer::Pointer dc = dca->createNonPrereqDataContainer<AbstractFilter>(NULL, "3D_VolumeDataContainer");
    PopulateVolumeDataContainer(dc, tupleDims, "3D_AttributeMatrix");
  }



  // A DataContainer that mimics some real data
  DataContainer::Pointer m = DataContainer::New(DREAM3D::Defaults::DataContainerName);
  dca->addDataContainer(m);
  m->setGeometry(image);

  AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New(tupleDims, getCellFeatureAttributeMatrixName(), DREAM3D::AttributeMatrixType::CellFeature);
  m->addAttributeMatrix(getCellFeatureAttributeMatrixName(), attrMatrix);

  int size = nx * ny * nz;
  Int32ArrayType::Pointer featureIds = Int32ArrayType::CreateArray(size, DREAM3D::CellData::FeatureIds);
  for (int i = 0; i < size; ++i)
  {
    featureIds->setValue(i, i + DataContainerIOTest::Offset);
  }
  attrMatrix->addAttributeArray(DREAM3D::CellData::FeatureIds, featureIds);

  BoolArrayType::Pointer boolArray = BoolArrayType::CreateArray(size, DREAM3D::CellData::BoundaryCells);
  for (int i = 0; i < size; ++i)
  {
    boolArray->setValue(i, i + DataContainerIOTest::Offset);
  }
  attrMatrix->addAttributeArray(DREAM3D::CellData::BoundaryCells, boolArray);

  QVector<size_t> dims(1, 3);
  FloatArrayType::Pointer avgEuler = FloatArrayType::CreateArray(size, dims, DREAM3D::FeatureData::AxisEulerAngles);
  for(int32_t i = 0; i < size; ++i)
  {
    avgEuler->setComponent(i, 0, i * 0.665f);
    avgEuler->setComponent(i, 1, i * 0.325f);
    avgEuler->setComponent(i, 2, i * 0.165f);
  }
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->addAttributeArray(DREAM3D::FeatureData::AxisEulerAngles, avgEuler);

  tupleDims.resize(1);
  tupleDims[0] = 4;
  AttributeMatrix::Pointer ensemAttrMat = AttributeMatrix::New(tupleDims, getCellEnsembleAttributeMatrixName(), DREAM3D::AttributeMatrixType::CellEnsemble);
  m->addAttributeMatrix(getCellEnsembleAttributeMatrixName(), ensemAttrMat);

  FloatArrayType::Pointer surfArea = FloatArrayType::CreateArray(4, DREAM3D::EnsembleData::TotalSurfaceAreas);
  for (int i = 0; i < 4; ++i)
  {
    surfArea->setValue(i, i + 41.2f);
  }
  m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->addAttributeArray(DREAM3D::EnsembleData::TotalSurfaceAreas, surfArea);

  Observer obs;
  // Send progress messages from PipelineBuilder to this object for display


  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setDataContainerArray(dca);
  writer->setOutputFile(DataContainerIOTest::TestFile());

  // Since we are NOT using the Pipeline Object to execute the filter but instead we are directly executing the filter
  // and we want to know about any error/warning/progress messages we need to connect the filter to our Observer object
  // manually. Normally the Pipeline Object would do this for us. We are NOT using a Pipeline Object because using the
  // Pipeline Object would over write the DataContainer Array that we have created with a blank one thus defeating the
  // entire purpose of the test.
  QObject::connect(writer.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
                   &obs, SLOT(processPipelineMessage(const PipelineMessage&)) );

  writer->execute();
  int err = writer->getErrorCondition();

  DREAM3D_REQUIRE_EQUAL(err, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataContainerReader()
{
//  size_t nx = 0;
//  size_t ny = 0;
//  size_t nz = 0;

  DataContainerArray::Pointer dca = DataContainerArray::New();

  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(DataContainerIOTest::TestFile());
  reader->setDataContainerArray(dca);
  DataContainerArrayProxy dcaProxy = reader->readDataContainerArrayStructure(DataContainerIOTest::TestFile());
  reader->setInputFileDataContainerArrayProxy(dcaProxy);
  reader->execute();
  int err = reader->getErrorCondition();
  DREAM3D_REQUIRE(err >= 0)

  Observer obs;
  // Send progress messages from PipelineBuilder to this object for display


  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setDataContainerArray(dca);
  writer->setOutputFile(DataContainerIOTest::TestFile2());

  // Since we are NOT using the Pipeline Object to execute the filter but instead we are directly executing the filter
  // and we want to know about any error/warning/progress messages we need to connect the filter to our Observer object
  // manually. Normally the Pipeline Object would do this for us. We are NOT using a Pipeline Object because using the
  // Pipeline Object would over write the DataContainer Array that we have created with a blank one thus defeating the
  // entire purpose of the test.
  QObject::connect(writer.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
                   &obs, SLOT(processPipelineMessage(const PipelineMessage&)) );

  writer->execute();
  err = writer->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);

  QMap<QString, DataContainerProxy>& dcsToRead = dcaProxy.dataContainers;
  //uint32_t dcType = DREAM3D::DataContainerType::UnknownDataContainer;
  for (QMap<QString, DataContainerProxy>::iterator iter = dcsToRead.begin(); iter != dcsToRead.end(); ++iter)
  {
    DataContainerProxy& dcProxy = iter.value();
    if (dcProxy.name.compare(DREAM3D::Defaults::DataContainerName) != 0) { dcProxy.flag = Qt::Unchecked; }
    else
    {
      QMap<QString, AttributeMatrixProxy>& attrMatsToRead = dcProxy.attributeMatricies;
      QString amName;
      for(QMap<QString, AttributeMatrixProxy>::iterator iter = attrMatsToRead.begin(); iter != attrMatsToRead.end(); ++iter)
      {
        amName = iter.key();
        if (amName.compare(getCellFeatureAttributeMatrixName()) != 0) { iter.value().flag = Qt::Unchecked; }
        else
        {
          QMap<QString, DataArrayProxy>& dasToRead = iter.value().dataArrays;
          for (QMap<QString, DataArrayProxy>::iterator iter2 = dasToRead.begin(); iter2 != dasToRead.end(); ++iter2)
          {
            if(iter2->name.compare(DREAM3D::CellData::FeatureIds) != 0 && iter2->name.compare(DREAM3D::FeatureData::AxisEulerAngles) != 0) { iter2->flag = DREAM3D::Unchecked; }
          }
        }
      }
    }
  }

  DataContainerArray::Pointer dca2 = DataContainerArray::New();

  DataContainerReader::Pointer reader2 = DataContainerReader::New();
  reader2->setInputFile(DataContainerIOTest::TestFile());
  reader2->setDataContainerArray(dca2);
  reader2->setInputFileDataContainerArrayProxy(dcaProxy);
  reader2->execute();
  err = reader2->getErrorCondition();
  DREAM3D_REQUIRE(err >= 0)

  DataContainerWriter::Pointer writer2 = DataContainerWriter::New();
  writer2->setDataContainerArray(dca2);
  writer2->setOutputFile(DataContainerIOTest::TestFile3());
  QObject::connect(writer.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
                   &obs, SLOT(processPipelineMessage(const PipelineMessage&)) );

  writer2->execute();
  err = writer2->getErrorCondition();
  DREAM3D_REQUIRE_EQUAL(err, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void insertDeleteArray(DataContainer::Pointer m)
{
  // This should fail because there is no attribute matrix
  AttributeMatrix::Pointer attrMatrix = m->getAttributeMatrix(getCellAttributeMatrixName());
  DREAM3D_REQUIRE_NULL_POINTER(attrMatrix.get())

  // Now add an AttributeMatrix to the DataContainer
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer attrMat = m->createAndAddAttributeMatrix(tDims, getCellAttributeMatrixName(), DREAM3D::AttributeMatrixType::Cell);
  DREAM3D_REQUIRE_VALID_POINTER(attrMat.get())

  // Now create an Array and add it to the Attribute Matrix
  typename T::Pointer p = T::CreateArray(5, "Test");


  tDims[0] = 5;
  attrMat->resizeAttributeArrays(tDims);
  int err = attrMat->addAttributeArray("Test", p);
  DREAM3D_REQUIRED(err, >=, 0)

  // Now get it back out as the specific type that we put it in as
  typename T::Pointer t = attrMat->getAttributeArrayAs<T>("Test");
  DREAM3D_REQUIRE_VALID_POINTER(t.get())

  // get the array as an IDataArray
  IDataArray::Pointer ida = attrMat->getAttributeArray("Test");
  DREAM3D_REQUIRE_VALID_POINTER(ida.get());

  QVector<size_t> dims(1, 1);
  t = attrMat->getPrereqArray<T, AbstractFilter>(NULL, "Test", -723, dims);
  DREAM3D_REQUIRE_VALID_POINTER(ida.get());

  // Remove the AttributeArray from the AttributeMatrix
  ida = attrMat->removeAttributeArray( "Test" );
  DREAM3D_REQUIRE_VALID_POINTER(ida.get());

  // Now try and get the array again. we should fail
  t = attrMat->getAttributeArrayAs<T>( "Test" );
  DREAM3D_REQUIRE_NULL_POINTER(t.get());
  // Try to get it as an IDataArray. We should fail
  ida = attrMat->getAttributeArray("Test");
  DREAM3D_REQUIRE_NULL_POINTER(ida.get());

  t = attrMat->getPrereqArray<T, AbstractFilter>(NULL, "Test", -723, dims);
  DREAM3D_REQUIRE_NULL_POINTER(t.get());

  // Remove the AttributeMatrix to setup for the next test.
  m->removeAttributeMatrix(getCellAttributeMatrixName());

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataContainerArrayProxy()
{
  // Create an instance of the DataContainerReader
  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(DataContainerIOTest::TestFile());

  // Read the structure of the Data container from the file marking all elements as readable.
  DataContainerArrayProxy proxy = reader->readDataContainerArrayStructure(DataContainerIOTest::TestFile());

  reader->setInputFileDataContainerArrayProxy(proxy);

  // Now create a QSettings based writer to write the parameters to a .ini file
  QFilterParametersWriter::Pointer qWriter = QFilterParametersWriter::New();
  QString iniFile(DataContainerIOTest::IniFile());
  QFileInfo fi(iniFile);
  if (fi.exists() == true)
  {
    QFile(iniFile).remove();
  }
  qWriter->openFile(DataContainerIOTest::IniFile(), QSettings::IniFormat);
  // Write the Filter Parameters to the file
  reader->writeFilterParameters(qWriter.get(), 0);
  qWriter->closeFile();

  // Read it back into a DataContainerArrayProxy
  QFilterParametersReader::Pointer qReader = QFilterParametersReader::New();
  qReader->openFile(DataContainerIOTest::IniFile(), QSettings::IniFormat);
  reader->readFilterParameters(qReader.get(), 0);
  qReader->closeFile();

  DataContainerArrayProxy dcaProxyFromIni = reader->getInputFileDataContainerArrayProxy();
  //FIXME: This should be validated

  // Now write the proxy to an HDF5/DREAM3D file
  hid_t fid = QH5Utilities::createFile(DataContainerIOTest::H5File() );
  H5FilterParametersWriter::Pointer parametersWriter = H5FilterParametersWriter::New();
  hid_t pipelineGroupId = QH5Utilities::createGroup(fid, DREAM3D::StringConstants::PipelineGroupName);
  parametersWriter->setGroupId(pipelineGroupId);

  int index = reader->writeFilterParameters(parametersWriter.get(), 0);

  int err = QH5Lite::writeScalarAttribute(fid, DREAM3D::StringConstants::PipelineGroupName, DREAM3D::Settings::NumFilters, index);
  DREAM3D_REQUIRE(err >= 0)



  // Lets try to read the Proxy back into memory
  H5FilterParametersReader::Pointer hReader = H5FilterParametersReader::New();
  hReader->setPipelineGroupId(pipelineGroupId);
  reader->readFilterParameters(hReader.get(), 0);

  DataContainerArrayProxy dcaProxy = reader->getInputFileDataContainerArrayProxy();

  int dcaCount = dcaProxy.dataContainers.count();
  DREAM3D_REQUIRE_EQUAL(dcaCount, 4);

  H5Gclose(pipelineGroupId);

  H5Fclose(fid);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestInsertDelete()
{
  DataContainer::Pointer m = DataContainer::New(DREAM3D::Defaults::DataContainerName);

  QList<QString> nameList;

  insertDeleteArray<Int8ArrayType> (m);
  insertDeleteArray<UInt16ArrayType> (m);
  insertDeleteArray<Int16ArrayType> (m);
  insertDeleteArray<UInt16ArrayType>(m);
  insertDeleteArray<Int32ArrayType> (m);
  insertDeleteArray<UInt32ArrayType> (m);
  insertDeleteArray<Int64ArrayType> (m);
  insertDeleteArray<UInt64ArrayType> (m);
  insertDeleteArray<FloatArrayType> (m);
  insertDeleteArray<DoubleArrayType> (m);
  insertDeleteArray<StructArray<uint8_t> > (m);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataArrayPath()
{
  QString dcname = QString::fromLatin1("DataContainer");
  QString amname = QString::fromLatin1("Attribute Matrix");
  QString daname = QString::fromLatin1("Euler Angles");


  DataArrayPath dap0;
  DataArrayPath dap1(dcname, amname, daname);
  DataArrayPath dap2("DataContainer2|AttributeMatrix2|DataArray2");
  DataArrayPath dap3(dap2);

  dap0 = dap3;

  QString serialize = dap1.serialize();
  QStringList strList = dap2.getAsStringList();

  QVector<QString> strVec = dap0.getAsVector();
  DREAM3D_REQUIRE(strVec.size() > 0)

  bool isEmpty = dap1.isEmpty();
  DREAM3D_REQUIRE_EQUAL(isEmpty, false)

  bool isValid = dap3.isValid();
  DREAM3D_REQUIRE_EQUAL(isValid, true)

  QStringList split = dap2.split();

  dap2.update("Foo", "Bar", "Baz");
}

#if 0
template<typename T, typename K>
void _arrayCreation(VolumeDataContainer::Pointer m)
{
  AbstractFilter::Pointer absFilt = AbstractFilter::New();

  QVector<int> dims(1, 2);
  T* ptr = m->createCellData<T, K, AbstractFilter>("Test", 10, dims, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(absFilt->getErrorCondition(), 0);
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  absFilt->setErrorCondition(0);

  // First try getting the array, but pass in a bad array name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellDataSizeCheck<T, K, AbstractFilter>("BAD_ARRAY_NAME", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad size name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellDataSizeCheck<T, K, AbstractFilter>("Test", 10, 1, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad cast type which should produce a null pointer
  // and negative error condition
  bool* bool_ptr =  m->getCellDataSizeCheck<bool, BoolArrayType, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(bool_ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next, pass in all the correct values which should produce a Non NULL pointer value and
  // Zero Error Condition
  ptr = m->getCellDataSizeCheck<T, K, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());

  IDataArray::Pointer t = attrMat->removeAttributeArray( "Test" );
  DREAM3D_TEST_POINTER(ptr, !=, NULL);

  /********************************* Feature Data Tests *********************************************/
  ptr = m->createCellFeatureData<T, K, AbstractFilter>("Test", 10, dims, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(absFilt->getErrorCondition(), 0);
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  absFilt->setErrorCondition(0);

  // First try getting the array, but pass in a bad array name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellFeatureDataSizeCheck<T, K, AbstractFilter>("BAD_ARRAY_NAME", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad size name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellFeatureDataSizeCheck<T, K, AbstractFilter>("Test", 10, 1, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad cast type which should produce a null pointer
  // and negative error condition
  bool_ptr =  m->getCellFeatureDataSizeCheck<bool, BoolArrayType, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(bool_ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next, pass in all the correct values which should produce a Non NULL pointer value and
  // Zero Error Condition
  ptr = m->getCellFeatureDataSizeCheck<T, K, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());

  t = m->removeCellFeatureData( "Test" );
  DREAM3D_TEST_POINTER(ptr, !=, NULL);


  /********************************* Ensemble Data Tests *********************************************/
  ptr = m->createCellEnsembleData<T, K, AbstractFilter>("Test", 10, dims, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(absFilt->getErrorCondition(), 0);
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  absFilt->setErrorCondition(0);

  // First try getting the array, but pass in a bad array name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellEnsembleDataSizeCheck<T, K, AbstractFilter>("BAD_ARRAY_NAME", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad size name which should produce a null pointer
  // and negative error condition
  ptr =  m->getCellEnsembleDataSizeCheck<T, K, AbstractFilter>("Test", 10, 1, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next try getting the array, but pass in a bad cast type which should produce a null pointer
  // and negative error condition
  bool_ptr =  m->getCellEnsembleDataSizeCheck<bool, BoolArrayType, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_REQUIRE_EQUAL(bool_ptr , NULL)
  DREAM3D_REQUIRE_NE(0, absFilt->getErrorCondition());
  absFilt->setErrorCondition(0);

  // Next, pass in all the correct values which should produce a Non NULL pointer value and
  // Zero Error Condition
  ptr = m->getCellEnsembleDataSizeCheck<T, K, AbstractFilter>("Test", 10, 2, absFilt.get());
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
  DREAM3D_REQUIRE_EQUAL(0, absFilt->getErrorCondition());


  t = m->removeCellEnsembleData( "Test" );
  DREAM3D_TEST_POINTER(ptr, !=, NULL);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestArrayCreation()
{
  VolumeDataContainer::Pointer m = VolumeDataContainer::New();
  m->setName(DREAM3D::Defaults::DataContainerName);
  QList<QString> nameList;

  _arrayCreation<int8_t, Int8ArrayType>(m);
  _arrayCreation<uint8_t, UInt8ArrayType>(m);
  _arrayCreation<int16_t, Int16ArrayType>(m);
  _arrayCreation<uint16_t, UInt16ArrayType>(m);
  _arrayCreation<int32_t, Int32ArrayType>(m);
  _arrayCreation<uint32_t, UInt32ArrayType>(m);
  _arrayCreation<int64_t, Int64ArrayType>(m);
  _arrayCreation<uint64_t, UInt64ArrayType>(m);
  _arrayCreation<float, FloatArrayType>(m);
  _arrayCreation<double, DoubleArrayType>(m);


  nameList = m->getCellArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  nameList = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArrayNames();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  nameList = m->getCellEnsembleArrayNameList();
  DREAM3D_REQUIRE_EQUAL(0, nameList.size() );

  // DO NOT TEST THE BoolArrayType - we are using that as a negative test in the _arrayCreation test

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataContainer()
{

  IDataArray::Pointer iDataArray = NeighborList<int>::New();

  NeighborList<int>* neighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >(iDataArray.get());
  assert (neighborList != NULL);


  for (int featureId = 0; featureId < 10; ++featureId)
  {
    for (int neighborId = 0; neighborId < featureId + 5; ++neighborId)
    {
      neighborList->addEntry(featureId, neighborId * 23);
    }
  }
  bool ok = true;

  int value = neighborList->getValue(5, 4, ok);
  BOOST_ASSERT(ok);

  value = neighborList->getValue(12, 4, ok);
  BOOST_ASSERT(!ok);

  std::cout << "Number of Lists: " << neighborList->getNumberOfLists() << std::endl;
  std::cout << "Number of Entries for Feature Id[5]: " << neighborList->getListSize(5) << std::endl;
  std::cout << "Value for [5][3]: " << neighborList->getValue(5, 3, ok) << std::endl;

  VolumeDataContainer::Pointer dataContainer = VolumeDataContainer::New(DREAM3D::Defaults::DataContainerName);
  dataContainer->addCellData("NeighborList", iDataArray);
  {
    MAKE_ARRAY(int8_t, "int8_t_Array" );
    MAKE_ARRAY(int16_t, "int16_t_Array" );
    MAKE_ARRAY(int32_t, "int32_t_Array" );
    MAKE_ARRAY(int64_t, "int64_t_Array" );
    MAKE_ARRAY(uint8_t, "uint8_t_Array" );
    MAKE_ARRAY(uint16_t, "uint16_t_Array" );
    MAKE_ARRAY(uint32_t, "uint32_t_Array" );
    MAKE_ARRAY(uint64_t, "uint64_t_Array" );
    MAKE_ARRAY(float, "float_Array" );
    MAKE_ARRAY(double, "double_Array" );
  }

  {
    // We can get a smaart pointer to the IDataArray Class
    IDataArray::Pointer ptr = dataContainer->getCellData("int32_t_Array");


    double* dPtr = IDataArray::SafeReinterpretCast<IDataArray*, DoubleArrayType*, double*>(ptr.get());
    BOOST_ASSERT(NULL == dPtr);

    int32_t* iPtr = IDataArray::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(ptr.get());
    BOOST_ASSERT(NULL != iPtr);

    // Or we can downcast to the type we know it is (in line)
    Int32ArrayType* intPtr = Int32ArrayType::SafeObjectDownCast<IDataArray*, Int32ArrayType* >(dataContainer->getCellData("int32_t_Array").get());
    if (NULL != intPtr)
    {
      std::cout << "Downcast to intPtr pointer was successful" << std::endl;
      std::cout << "Number of Tuples:" << intPtr->getNumberOfTuples() << std::endl;
      int32_t* p = intPtr->getPointer(0);
      for (int i = 0; i < 5; ++i)
      {
        std::cout << (int)p[i] << std::endl;
      }
      for (int i = 0; i < 5; ++i)
      {
        std::cout << (int)(intPtr->getValue(i)) << std::endl;
      }
    }


    DoubleArrayType* doublePtr = DoubleArrayType::SafeObjectDownCast<IDataArray*, DataArray<double>*>(ptr.get());
    if (NULL != doublePtr)
    {
      std::cout << "Downcast to double pointer was successful" << std::endl;
    }
    else
    {
      std::cout << "Downcast to double pointer FAILED." << std::endl;
    }

  }

  // Test Getting an Array

  // Test removing an Array

  // Test Size of all types of maps

}
#endif



// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  QDir dir(DataContainerIOTest::TestDir());
  dir.mkpath(".");

  std::cout << "Output Directory: " << DataContainerIOTest::TestDir().toStdString() << std::endl;
#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  QMetaObjectUtilities::RegisterMetaTypes();

  DREAM3D_REGISTER_TEST( TestInsertDelete() )

  DREAM3D_REGISTER_TEST( TestDataContainerWriter() )
  DREAM3D_REGISTER_TEST( TestDataContainerArrayProxy() )

  DREAM3D_REGISTER_TEST( TestDataContainerReader() )
  DREAM3D_REGISTER_TEST(TestDataArrayPath() )

#if REMOVE_TEST_FILES
  //DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  PRINT_TEST_SUMMARY();

  return err;
}

