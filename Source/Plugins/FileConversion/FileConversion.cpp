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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/HDF5/H5VoxelReader.h"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"
#include "GrainIdReader.h"
#include "GrainIdWriter.h"
#include "VtkGrainIdReader.h"




namespace Detail {

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  class _H5VoxelReader : public GrainIdReader
  {
    public:
      DREAM3D_SHARED_POINTERS(_H5VoxelReader);
      DREAM3D_STATIC_NEW_MACRO(_H5VoxelReader);
      DREAM3D_TYPE_MACRO(_H5VoxelReader);
      DREAM3D_STATIC_NEW_SUPERCLASS(GrainIdReader, _H5VoxelReader);


      virtual ~_H5VoxelReader() {}

      int readFile(std::string filePath, std::vector<int> &data, int &nx, int &ny, int &nz)
      {
        int err = 0;
        H5VoxelReader::Pointer reader = H5VoxelReader::New();
        reader->setFileName(filePath);
        int volDims[3];
        float spacing[3];
        err = reader->getSizeAndResolution(volDims, spacing);
        int totalpoints = volDims[0] * volDims[1] * volDims[2];
        data.resize(totalpoints);

        err = reader->readScalarData<int>(DREAM3D::VTK::GrainIdScalarName, &(data.front()));
        return err;

      }

    protected:
      _H5VoxelReader() {}

    private:
      _H5VoxelReader(const _H5VoxelReader&); // Copy Constructor Not Implemented
      void operator=(const _H5VoxelReader&); // Operator '=' Not Implemented

  };



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  class _H5VoxelWriter : public GrainIdWriter
  {
    public:
       DREAM3D_SHARED_POINTERS(_H5VoxelWriter)
       DREAM3D_STATIC_NEW_MACRO(_H5VoxelWriter)
       DREAM3D_TYPE_MACRO(_H5VoxelWriter)
       static GrainIdWriter::Pointer NewDREAM3DFileWriter()
       {
         _H5VoxelWriter* ptr = new _H5VoxelWriter();
         GrainIdWriter::Pointer shared_ptr(dynamic_cast<GrainIdWriter*>(ptr));
         return shared_ptr;
       }
       _H5VoxelWriter() {}
       virtual ~_H5VoxelWriter() {}

       int writeGrainIds()
       {/*
         int err = 0;

         int totalpoints = xpoints * ypoints * zpoints;

         H5VoxelWriter::Pointer writer = H5VoxelWriter::New();
         writer->setFileName(getFileName());
         int volDims = {xpoints, ypoints, zpoints};
         float resolution[3];
         getResolution(resolution);
         float origin[3];
         getOrigin(origin);
         err = writer->writeStructuredPoints(volDims, resolution, origin, false);
         if (err < 0)
         {
           return err;
         }

         err = writer->writeGrainIds(grain_index, totalpoints, true);


         return err;
       */}
    private:
       _H5VoxelWriter(const _H5VoxelWriter&); // Copy Constructor Not Implemented
       void operator=(const _H5VoxelWriter&); // Operator '=' Not Implemented
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

  GrainIdReader::Pointer reader = GrainIdReader::NullPointer();

  if (MXAFileInfo::exists(m_InputFilePath) && MXAFileInfo::isFile(m_InputFilePath))
  {
    std::string inputExtension = MXAFileInfo::extension(m_InputFilePath);
    if (inputExtension.compare("vtk") == 0)
    {
      reader = VtkGrainIdReader::NewGrainIdReader();
    }
    else if (inputExtension.compare("ph") == 0)
    {
      reader = PhGrainIdReader::NewGrainIdReader();
    }
    else if (inputExtension.compare("h5voxel") == 0)
    {
      //reader = Detail::_H5VoxelReader::NewDREAM3DFileReader();
    }
    else if (inputExtension.compare("dx") == 0)
    {
      reader = DxGrainIdReader::NewGrainIdReader();
    }
    else
    {
      setErrorCondition(-1);
    }
  }

  GrainIdWriter::Pointer writer = GrainIdWriter::NullPointer();

  std::string outputExtension = MXAFileInfo::extension(m_OutputFilePath);
  if (outputExtension.compare("vtk") == 0)
  {
    writer = VtkGrainIdWriter::NewGrainIdWriter();
  }
  else if (outputExtension.compare("ph") == 0)
  {
    writer = PhGrainIdWriter::NewGrainIdWriter();
  }
  else if (outputExtension.compare("h5voxel") == 0)
  {
    //writer = Detail::_H5VoxelWriter::NewGrainIdWriter();
  }
  else if (outputExtension.compare("dx") == 0)
  {
    writer = DxGrainIdWriter::NewGrainIdWriter();
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


  int nx = 0;
  int ny = 0;
  int nz = 0;

  reader->setFileName(m_InputFilePath);
  err = reader->readGrainIds();
  CHECK_FOR_ERROR(FileConversionFunc, "Reading the input file failed.", err);
  reader->getDimensions(nx, ny, nz);
  float resolution[3];
  reader->getResolution(resolution);
  float origin[3];
  reader->getOrigin(origin);
  AIMArray<int>::Pointer grainIds = reader->getGrainIds();

  writer->setFileName(m_OutputFilePath);
  writer->setGrainIds(grainIds);
  writer->setDimensions(nx, ny, nz);
  writer->setResolution(resolution);
  writer->setOrigin(origin);

  err = writer->writeGrainIds();

  CHECK_FOR_ERROR(FileConversionFunc, "The conversion Failed during the file writing stage.", err);

  updateProgressAndMessage(("File Conversion Completed"), 100);
}
