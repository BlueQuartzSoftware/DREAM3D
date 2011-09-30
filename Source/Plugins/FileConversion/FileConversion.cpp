/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "FileConversion.h"

#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXAFileInfo.h"

#include "DREAM3DLib/IO/DREAM3DFileReader.h"
#include "DREAM3DLib/IO/DREAM3DFileWriter.h"

#include "DREAM3DLib/IO/DxReader.h"
#include "DREAM3DLib/IO/DxWriter.hpp"
#include "DREAM3DLib/IO/PhReader.h"
#include "DREAM3DLib/IO/PhWriter.hpp"

namespace Detail {
class FCWrapper
{
  public:
    FCWrapper() {}
    virtual ~FCWrapper() {}


    int convert(const std::string &inFileName,
                DREAM3DFileReader::Pointer reader,
                const std::string &outFileName,
                DREAM3DFileWriter<int>::Pointer writer)
    {
      int nx = 0;
        int ny = 0;
        int nz = 0;
        std::vector<int> grain_indices;
        int err = reader->readFile(inFileName, grain_indices, nx, ny, nz);
        if (err < 0)
        { return err; }

        err = writer->writeFile(outFileName, &(grain_indices.front()), nx, ny, nz);

        return err;
    }



  private:
    FCWrapper(const FCWrapper&); // Copy Constructor Not Implemented
    void operator=(const FCWrapper&); // Operator '=' Not Implemented
};

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileConversion::FileConversion()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileConversion::~FileConversion()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversion::execute()
{
  int err = 0;

  DREAM3DFileReader::Pointer reader = DREAM3DFileReader::NullPointer();

  if (MXAFileInfo::exists(m_InputFilePath) && MXAFileInfo::isFile(m_InputFilePath))
  {
    std::string inputExtension = MXAFileInfo::extension(m_InputFilePath);
    if (inputExtension.compare("vtk") == 0)
    {
    }
    else if (inputExtension.compare("ph") == 0)
    {
      reader = PhReader::NewDREAM3DFileReader();
    }
    else if (inputExtension.compare("h5voxel") == 0)
    {
    }
    else if (inputExtension.compare("dx") == 0)
    {
      reader = DxReader::NewDREAM3DFileReader();
    }
    else
    {
      setErrorCondition(-1);
    }
  }

  DREAM3DFileWriter<int>::Pointer writer = DREAM3DFileWriter<int>::NullPointer();

  std::string outputExtension = MXAFileInfo::extension(m_OutputFilePath);
  if (outputExtension.compare("vtk") == 0)
  {
  }
  else if (outputExtension.compare("ph") == 0)
  {
    writer = PhWriter<int>::NewDREAM3DFileWriter();
  }
  else if (outputExtension.compare("h5voxel") == 0)
  {
  }
  else if (outputExtension.compare("dx") == 0)
  {
    writer = DxWriter<int>::NewDREAM3DFileWriter();
  }
  else
  {
    setErrorCondition(-1);
  }


  if (NULL == writer.get() )
  {
    err = -1;
    CHECK_FOR_ERROR(FileConversionFunc, "No suitable File Writer Class could be found", err);
  }
  if ( NULL == reader.get())
  {
    err = -1;
    CHECK_FOR_ERROR(FileConversionFunc, "No Suitable File Reader Class could be found", err);
  }


  Detail::FCWrapper wrapper;
  err = wrapper.convert(m_InputFilePath, reader, m_OutputFilePath, writer);
  CHECK_FOR_ERROR(FileConversionFunc, "The conversion Failed during the file writing stage.", err);

  updateProgressAndMessage(("File Conversion Completed"), 100);
}
