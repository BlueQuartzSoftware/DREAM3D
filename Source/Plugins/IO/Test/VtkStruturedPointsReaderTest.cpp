/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "IOTestFileLocations.h"

#include "GenerateFeatureIds.h"

#define mxa_bswap(s, d, t)                                                                                                                                                                             \
  t[0] = ptr[s];                                                                                                                                                                                       \
  ptr[s] = ptr[d];                                                                                                                                                                                     \
  ptr[d] = t[0];

class VtkStruturedPointsReaderTest
{
public:
  VtkStruturedPointsReaderTest()
  {
  }
  virtual ~VtkStruturedPointsReaderTest()
  {
  }
  SIMPL_TYPE_MACRO(VtkStruturedPointsReaderTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::VtkStructuredPointsReaderTest::BinaryFile);
    QFile::remove(UnitTest::VtkStructuredPointsReaderTest::AsciiFile);
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void WriteHeader(FILE* f, bool binary, int* dims, float* origin, float* scaling)
  {
    fprintf(f, "# vtk DataFile Version 3.0\n");
    fprintf(f, "vtk output\n");
    if(binary)
    {
      fprintf(f, "BINARY\n");
    }
    else
    {
      fprintf(f, "ASCII\n");
    }
    fprintf(f, "DATASET STRUCTURED_POINTS\n");
    fprintf(f, "DIMENSIONS %d %d %d\n", dims[0], dims[1], dims[2]);
    fprintf(f, "SPACING %f %f %f\n", scaling[0], scaling[1], scaling[2]);
    fprintf(f, "ORIGIN %f %f %f\n", origin[0], origin[1], origin[2]);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T> void WriteScalars(FILE* f, const std::string& type, const std::string& name, int* dims, bool binary)
  {
    fprintf(f, "SCALARS %s %s 1\n", name.c_str(), type.c_str());
    fprintf(f, "LOOKUP_TABLE default\n");

    std::vector<T> data(dims[0]);
    T* dPtr = &(data.front());

    if(binary)
    {
      // Fill with some data
      for(size_t i = 0; i < dims[0]; i++)
      {
        data[i] = static_cast<T>(i);

        char* ptr = (char*)(dPtr + i);
        if(BIGENDIAN == 0)
        {
          if(sizeof(T) == 4)
          {
            mxa_bswap(0, 3, ptr);
            mxa_bswap(1, 2, ptr);
          }
          if(sizeof(T) == 2)
          {
            mxa_bswap(0, 1, ptr);
          }
        }
      }
      // write to file
      for(size_t z = 0; z < dims[2]; z++)
      {
        for(size_t y = 0; y < dims[1]; y++)
        {
          char* ptr = (char*)(dPtr);
          size_t nwrote = fwrite(ptr, sizeof(T), dims[0], f);
          DREAM3D_REQUIRE_EQUAL(nwrote, dims[0])
        }
      }
      fprintf(f, "\n");
    }
    else
    {
      for(size_t z = 0; z < dims[2]; z++)
      {
        for(size_t y = 0; y < dims[1]; y++)
        {
          std::stringstream ss;
          for(size_t i = 0; i < dims[0]; i++)
          {
            ss << i << " ";
            if(i == 20)
            {
              ss << "\n";
            }
          }
          fprintf(f, "%s\n", ss.str().c_str());
        }
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void WriteTestFile(bool binary, const std::string& filePath)
  {
    FILE* f = fopen(filePath.c_str(), "wb");

    int dims[3] = {10, 20, 30};
    float origin[3] = {0.0f, 0.0f, 0.0f};
    float scaling[3] = {0.25f, 0.35f, 0.45f};

    size_t total = dims[0] * dims[1] * dims[2];

    WriteHeader(f, binary, dims, origin, scaling);

    // Start the POINT_DATA Section
    fprintf(f, "POINT_DATA %lu\n", static_cast<unsigned long>(total));
    WriteScalars<quint8>(f, "unsigned_char", "Data_uint8", dims, binary);
    WriteScalars<qint8>(f, "char", "Data_int8", dims, binary);
    WriteScalars<quint16>(f, "unsigned_short", "Data_uint16", dims, binary);
    WriteScalars<qint16>(f, "short", "Data_int16", dims, binary);
    WriteScalars<quint32>(f, "unsigned_int", "Data_uint32", dims, binary);
    WriteScalars<qint32>(f, "int", "Data_int32", dims, binary);
    WriteScalars<quint64>(f, "unsigned_long", "Data_uint64", dims, binary);
    WriteScalars<qint64>(f, "long", "Data_int64", dims, binary);
    WriteScalars<float>(f, "float", "Data_float", dims, binary);
    WriteScalars<double>(f, "double", "Data_double", dims, binary);

    dims[0] -= 1;
    dims[1] -= 1;
    dims[2] -= 1;
    total = dims[0] * dims[1] * dims[2];
    fprintf(f, "\n");
    fprintf(f, "CELL_DATA %lu\n",  static_cast<unsigned long>(total));
    WriteScalars<quint8>(f, "unsigned_char", "Data_uint8", dims, binary);
    WriteScalars<qint8>(f, "char", "Data_int8", dims, binary);
    WriteScalars<quint16>(f, "unsigned_short", "Data_uint16", dims, binary);
    WriteScalars<qint16>(f, "short", "Data_int16", dims, binary);
    WriteScalars<quint32>(f, "unsigned_int", "Data_uint32", dims, binary);
    WriteScalars<qint32>(f, "int", "Data_int32", dims, binary);
    WriteScalars<quint64>(f, "unsigned_long", "Data_uint64", dims, binary);
    WriteScalars<qint64>(f, "long", "Data_int64", dims, binary);
    WriteScalars<float>(f, "float", "Data_float", dims, binary);
    WriteScalars<double>(f, "double", "Data_double", dims, binary);

    fclose(f);
    f = nullptr;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestWritingFiles()
  {
    WriteTestFile(true, UnitTest::VtkStructuredPointsReaderTest::BinaryFile.toStdString());
    WriteTestFile(false, UnitTest::VtkStructuredPointsReaderTest::AsciiFile.toStdString());
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void ReadTestFile(const std::string& filePath)
  {
    DataContainerArray::Pointer dca = DataContainerArray::New();

    // Now instantiate the PhWriter Filter from the FilterManager
    QString filtName = "VtkStructuredPointsReader";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();
      filter->setDataContainerArray(dca);

      QVariant var;
      var.setValue(QString::fromStdString(filePath));
      bool propWasSet = filter->setProperty("InputFile", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->preflight();
      DREAM3D_REQUIRED(filter->getErrorCondition(), >=, 0);

      filter->setDataContainerArray(DataContainerArray::New());

      filter->execute();
      DREAM3D_REQUIRED(filter->getErrorCondition(), >=, 0);
      dca = filter->getDataContainerArray();

      // Need to actually check the data against what _should_ have been written
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestReadingFiles()
  {
    ReadTestFile(UnitTest::VtkStructuredPointsReaderTest::BinaryFile.toStdString());
  }

  /**
* @brief
*/
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestWritingFiles());
    DREAM3D_REGISTER_TEST(TestReadingFiles());
  }

private:
  VtkStruturedPointsReaderTest(const VtkStruturedPointsReaderTest&); // Copy Constructor Not Implemented
  void operator=(const VtkStruturedPointsReaderTest&);               // Operator '=' Not Implemented
};
