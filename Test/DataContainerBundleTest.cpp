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


#include <QtCore/QFile>
#include <QtCore/QDir>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainerBundle.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SIMPLib/Utilities/UnitTestSupport.hpp"



#include "Test/DREAM3DTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::DataContainerBundleTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void AddDataArray(AttributeMatrix::Pointer am, const QString name, QVector<size_t>& tDims, QVector<size_t>& cDims)
{

  typename DataArray<T>::Pointer data = DataArray<T>::CreateArray(tDims, cDims, name);

  am->addAttributeArray(data->getName(), data);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestDataBundleCommonPaths()
{



  QVector<size_t> tDims(3, 0);
  tDims[0] = 10;
  tDims[1] = 20;
  tDims[2] = 30;
  QVector<size_t> cDims(2);
  cDims[0] = 3;
  cDims[1] = 3;

  DataContainer::Pointer dc0 = DataContainer::New("DC 0");
  AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, "CellAttributeMatrix", DREAM3D::AttributeMatrixType::Cell);
  AddDataArray<uint8_t>(am, "Uint8 Array", tDims, cDims);
  AddDataArray<float>(am, "Float Array", tDims, cDims);
  AddDataArray<int32_t>(am, "int32 Array", tDims, cDims);
  dc0->addAttributeMatrix(am->getName(), am);

  QVector<size_t> tupleDims(1, 1);
  AttributeMatrix::Pointer metaAm = AttributeMatrix::New(tupleDims, DataContainerBundle::GetMetaDataName(), DREAM3D::AttributeMatrixType::MetaData);
  dc0->addAttributeMatrix(metaAm->getName(), metaAm);

  DataContainer::Pointer dc1 = dc0->deepCopy();
  dc1->setName("DC 1");

  DataContainer::Pointer dc2 = dc0->deepCopy();
  dc2->setName("DC 2");
  // remove an array so that we have something different
  dc2->getAttributeMatrix("CellAttributeMatrix")->removeAttributeArray("Uint8 Array");

  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->addDataContainer(dc0);
  dca->addDataContainer(dc1);
  dca->addDataContainer(dc2);


  DataContainerBundle::Pointer bundle = DataContainerBundle::New("Bundle 1");
  bundle->addDataContainer(dc0);
  bundle->addDataContainer(dc1);
  bundle->addDataContainer(dc2);

  QVector<DataArrayPath> paths = bundle->findCommonDataArrayPaths();

  int count = paths.count();
  DREAM3D_REQUIRE_EQUAL(count, 2)

}



// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  QDir dir(UnitTest::DataArrayTest::TestDir);
  dir.mkpath(".");

#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  DREAM3D_REGISTER_TEST( TestDataBundleCommonPaths() )

#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif


  PRINT_TEST_SUMMARY();
  return err;
}
