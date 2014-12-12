/* ============================================================================
 * Copyright (c) 2012, BlueQuartz Software
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

#include <QtCore/QFile>
#include <QtCore/QDir>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataContainers/DataContainerArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/DataContainers/AttributeMatrix.h"
#include "DREAM3DLib/DataContainers/DataContainerBundle.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"



#include "Test/TestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  QFile::remove(UnitTest::DataArrayTest::TestFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void AddDataArray(AttributeMatrix::Pointer am, const QString name, QVector<size_t> &tDims, QVector<size_t> &cDims)
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

  AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, "CellAttributeMatrix", DREAM3D::AttributeMatrixType::Cell);
  AddDataArray<uint8_t>(am, "Uint8 Array", tDims, cDims);
  AddDataArray<float>(am, "Float Array", tDims, cDims);
  AddDataArray<int32_t>(am, "Float Array", tDims, cDims);
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
