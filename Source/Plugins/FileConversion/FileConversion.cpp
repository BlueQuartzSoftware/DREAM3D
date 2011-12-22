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

#include "DREAM3DLib/IO/FileReader.h"
#include "DREAM3DLib/IO/PhReader.h"
#include "DREAM3DLib/IO/DxReader.h"
#include "DREAM3DLib/IO/PhWriter.h"
#include "DREAM3DLib/IO/DxWriter.h"
#include "DREAM3DLib/IO/VtkGrainIdReader.h"
#include "DREAM3DLib/IO/VtkGrainIdWriter.h"
#include "DREAM3DLib/HDF5/H5VoxelGrainIdReader.h"


namespace Detail {

  /**
   * @class H5VoxelGrainIdWriter H5VoxelGrainIdWriter.h DREAM3D/Plugins/FileConversion/H5VoxelGrainIdWriter.h
   * @brief This class is needed for the file conversion of grain ids. Note that
   * we do NOT actually write them to the .h5voxel file as the file would be
   * incomplete without all the other data.
   * @author Michael A. Jackson for BlueQuartz Software
   * @date Dec 21, 2011
   * @version 1.0
   */
  class H5VoxelGrainIdWriter : public DREAM3D::FileWriter
  {
    public:
       DREAM3D_SHARED_POINTERS(H5VoxelGrainIdWriter)
       DREAM3D_STATIC_NEW_MACRO(H5VoxelGrainIdWriter)
       DREAM3D_TYPE_MACRO_SUPER(H5VoxelGrainIdWriter, DREAM3D::FileWriter)

       virtual ~H5VoxelGrainIdWriter() {}


    protected:
       H5VoxelGrainIdWriter() {}

       int writeHeader()
       {
         return 0;
       }
       int writeFile()
       {
         int err = -1;
         setErrorMessage(".h5voxel output files are NOT currently supported");
         /*
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
        */
         setErrorCondition(err);
         return err;
       }

    private:
       H5VoxelGrainIdWriter(const H5VoxelGrainIdWriter&); // Copy Constructor Not Implemented
       void operator=(const H5VoxelGrainIdWriter&); // Operator '=' Not Implemented
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

  std::vector<AbstractFilter::Pointer> pipeline;


  if (MXAFileInfo::exists(m_InputFilePath) && MXAFileInfo::isFile(m_InputFilePath))
  {
    std::string inputExtension = MXAFileInfo::extension(m_InputFilePath);
    if (inputExtension.compare("vtk") == 0)
    {
      AbstractFilter::Pointer filter = VtkGrainIdReader::New();
      DREAM3D::FileReader* reader = DREAM3D::FileReader::SafeObjectDownCast<AbstractFilter*, DREAM3D::FileReader*>(filter.get());
      reader->setFileName(m_InputFilePath);
      pipeline.push_back(filter);
    }
    else if (inputExtension.compare("ph") == 0)
    {
      AbstractFilter::Pointer filter = PhReader::New();
      DREAM3D::FileReader* reader = DREAM3D::FileReader::SafeObjectDownCast<AbstractFilter*, DREAM3D::FileReader*>(filter.get());
      reader->setFileName(m_InputFilePath);
      pipeline.push_back(filter);
    }
    else if (inputExtension.compare("h5voxel") == 0)
    {
     // reader = Detail::H5VoxelGrainIdReader::NewGrainIdReader();
      setErrorCondition(-1);
      updateProgressAndMessage("No suitable File Reader Class could be found", 0);
      return;
    }
    else if (inputExtension.compare("dx") == 0)
    {
      AbstractFilter::Pointer filter = DxReader::New();
      DREAM3D::FileReader* reader = DREAM3D::FileReader::SafeObjectDownCast<AbstractFilter*, DREAM3D::FileReader*>(filter.get());
      reader->setFileName(m_InputFilePath);
      pipeline.push_back(filter);
    }
    else
    {
      setErrorCondition(-1);
      updateProgressAndMessage("No suitable File Reader Class could be found", 0);
      return;
    }
  }



  std::string outputExtension = MXAFileInfo::extension(m_OutputFilePath);
  if (outputExtension.compare("vtk") == 0)
  {
    AbstractFilter::Pointer filter = VtkGrainIdWriter::New();
    DREAM3D::FileReader* reader = DREAM3D::FileReader::SafeObjectDownCast<AbstractFilter*, DREAM3D::FileReader*>(filter.get());
    reader->setFileName(m_OutputFilePath);
    pipeline.push_back(filter);
  }
  else if (outputExtension.compare("ph") == 0)
  {
    AbstractFilter::Pointer filter = PhWriter::New();
    DREAM3D::FileReader* reader = DREAM3D::FileReader::SafeObjectDownCast<AbstractFilter*, DREAM3D::FileReader*>(filter.get());
    reader->setFileName(m_OutputFilePath);
    pipeline.push_back(filter);
  }
  else if (outputExtension.compare("h5voxel") == 0)
  {
    //writer = Detail::H5VoxelGrainIdWriter::NewGrainIdWriter();
    setErrorCondition(-1);
    updateProgressAndMessage("No suitable File Writer Class could be found", 0);
    return;
  }
  else if (outputExtension.compare("dx") == 0)
  {
    AbstractFilter::Pointer filter = DxWriter::New();
    DREAM3D::FileReader* reader = DREAM3D::FileReader::SafeObjectDownCast<AbstractFilter*, DREAM3D::FileReader*>(filter.get());
    reader->setFileName(m_OutputFilePath);
    pipeline.push_back(filter);
  }
  else
  {
    setErrorCondition(-1);
    updateProgressAndMessage("No suitable File Writer Class could be found", 0);
    return;
  }

  DataContainer::Pointer m = DataContainer::New();

  // Start a Benchmark Clock so we can keep track of each filter's execution time
  START_CLOCK()
  // Start looping through the Pipeline
  float progress = 0.0f;
  std::stringstream ss;
  for (std::vector<AbstractFilter::Pointer>::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter)
  {
    progress = progress + 1.0f;
    pipelineProgress(progress / (pipeline.size() + 1) * 100.0f);
    ss.str("");
    ss << "Executing Filter [" << progress << "/" << pipeline.size() << "] - " << (*filter)->getNameOfClass();
    pipelineProgressMessage(ss.str());
    (*filter)->addObserver(static_cast<Observer*>(this));
    (*filter)->setDataContainer(m.get());
    setCurrentFilter(*filter);
    (*filter)->execute();
    (*filter)->removeObserver(static_cast<Observer*>(this));
    err = (*filter)->getErrorCondition();
    if(err < 0)
    {
      setErrorCondition(err);
      pipelineErrorMessage((*filter)->getErrorMessage().c_str());
      pipelineProgress(100);
      pipelineFinished();
      return;
    }
    CHECK_FOR_CANCELED(DataContainer, "FileConversion was canceled", write_fielddata)

    if(DREAM3D_BENCHMARKS)
    {
      std::cout << (*filter)->getNameOfClass() << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;
      millis = MXA::getMilliSeconds();
    }
  }


//  int nx = 0;
//  int ny = 0;
//  int nz = 0;
//
//  reader->setFileName(m_InputFilePath);
//  err = reader->readGrainIds();
//  CHECK_FOR_ERROR(FileConversionFunc, (reader->getErrorMessage()), err);
//  reader->getDimensions(nx, ny, nz);
//  float resolution[3];
//  reader->getResolution(resolution);
//  float origin[3];
//  reader->getOrigin(origin);
//  DataArray<int>::Pointer grainIds = reader->getGrainIds();
//  if (grainIds.get() == NULL)
//  {
//    err = -1;
//    CHECK_FOR_ERROR(FileConversionFunc, "Array pointer was NULL", err);
//  }
//
//  writer->setFileName(m_OutputFilePath);
//  writer->setGrainIds(grainIds);
//  writer->setDimensions(nx, ny, nz);
//  writer->setResolution(resolution);
//  writer->setOrigin(origin);
//
//  err = writer->writeGrainIds();
//
//  CHECK_FOR_ERROR(FileConversionFunc, (writer->getErrorMessage()), err);

  updateProgressAndMessage(("File Conversion Completed"), 100);
}
