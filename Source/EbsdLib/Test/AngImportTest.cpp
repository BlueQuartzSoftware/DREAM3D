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



#include <string.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QtDebug>

#include "EbsdLib/EbsdLib.h"


#if EbsdLib_HDF5_SUPPORT
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#endif

#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/TSL/H5AngImporter.h"

#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "EbsdLib/Test/EbsdLibTestFileLocations.h"

class AngImportTest
{
  public:
    AngImportTest(){}
    virtual ~AngImportTest() {}


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void RemoveTestFiles()
    {
#if REMOVE_TEST_FILES
      QFile::remove(UnitTest::AngImportTest::H5EbsdOutputFile);
#endif
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestMissingHeaders()
    {

      AngReader reader;
      reader.setFileName(UnitTest::AngImportTest::MissingHeader1);
      int err = reader.readHeaderOnly();
      // It should read through this header just fine
      DREAM3D_REQUIRE(err > 0)

          int value = reader.getNumEvenCols();
      DREAM3D_REQUIRE_EQUAL(value, -1)

          value = reader.getNumOddCols();
      DREAM3D_REQUIRE_EQUAL(value, -1)

          value = reader.getNumRows();
      DREAM3D_REQUIRE_EQUAL(value, -1)

          float step = reader.getXStep();
      DREAM3D_REQUIRE_EQUAL(step, 0.0f)

          step = reader.getYStep();
      DREAM3D_REQUIRE_EQUAL(step, 0.0f)

          err = reader.readFile();
      qDebug() << reader.getErrorMessage();
      DREAM3D_REQUIRED(err, == , -110)
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestMissingGrid()
    {

      AngReader reader;
      reader.setFileName(UnitTest::AngImportTest::GridMissing);
      int err = reader.readHeaderOnly();
      // It should read through this header just fine
      DREAM3D_REQUIRE(err > 0)



          err = reader.readFile();
      qDebug() << reader.getErrorMessage();
      DREAM3D_REQUIRED(err, == , -300)
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestHexGrid()
    {

      AngReader reader;
      reader.setFileName(UnitTest::AngImportTest::HexHeader);
      int err = reader.readHeaderOnly();
      // It should read through this header just fine
      DREAM3D_REQUIRE(err > 0)



          err = reader.readFile();
      qDebug() << reader.getErrorMessage();
      DREAM3D_REQUIRED(err, == , -400)
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestShortFile()
    {
      AngReader reader;
      reader.setFileName(UnitTest::AngImportTest::ShortFile);
      int err = reader.readFile();
      // It should read through this header just fine but die when reading the file because there is not enough data being read
      qDebug() << reader.getErrorMessage();
      DREAM3D_REQUIRED(err, < , 0)
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void TestNormalFile()
    {
      // This is just a normal Ang file, well formed and should read without error
      AngReader reader;
      reader.setFileName(UnitTest::AngImportTest::TestFile1);
      int err = reader.readFile();
      qDebug() << reader.getErrorMessage();
      DREAM3D_REQUIRED(err, == , 0)
    }


    void operator()()
    {
      int err = EXIT_SUCCESS;

      DREAM3D_REGISTER_TEST( TestMissingHeaders() )
          DREAM3D_REGISTER_TEST( TestHexGrid() )
          DREAM3D_REGISTER_TEST( TestMissingGrid() )
          DREAM3D_REGISTER_TEST( TestShortFile() )
          DREAM3D_REGISTER_TEST( TestNormalFile() )
          DREAM3D_REGISTER_TEST( RemoveTestFiles() )
    }


};

