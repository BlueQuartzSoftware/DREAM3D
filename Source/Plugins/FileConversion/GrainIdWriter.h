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

#ifndef _DREAM3DFileWriter_h_
#define _DREAM3DFileWriter_h_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/IO/DREAM3DDataFile.h"
#include "DREAM3DLib/IO/PhWriter.hpp"
#include "DREAM3DLib/IO/DxWriter.hpp"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"

/**
 * @class DREAM3DFileWriter DREAM3DFileWriter.h DREAM3DLib/IO/DREAM3DFileWriter.h
 * @brief
 * @author mjackson
 * @date Sep 29, 2011
 * @version $Revision$
 */
class GrainIdWriter : public DREAM3DDataFile
{
  public:
    DREAM3D_SHARED_POINTERS(GrainIdWriter);
    DREAM3D_STATIC_NEW_MACRO(GrainIdWriter);
    DREAM3D_TYPE_MACRO(GrainIdWriter);
    virtual ~GrainIdWriter() { }

    DREAM3D_INSTANCE_PROPERTY(DataArray<int>::Pointer, GrainIds);
    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorMessage);

    virtual int writeGrainIds() { return -1; }

  protected:
    GrainIdWriter() {}

  private:
    GrainIdWriter(const GrainIdWriter&); //Not Implemented
    void operator=(const GrainIdWriter&); //Not Implemented

};

/**
 *
 */
class PhGrainIdWriter : public GrainIdWriter
{
  public:
    DREAM3D_SHARED_POINTERS(PhGrainIdWriter)
    DREAM3D_STATIC_NEW_MACRO(PhGrainIdWriter);
    DREAM3D_TYPE_MACRO(PhGrainIdWriter);
    DREAM3D_STATIC_NEW_SUPERCLASS(GrainIdWriter, PhGrainIdWriter);

    virtual ~PhGrainIdWriter() {};

    virtual int writeGrainIds()
    {
      PhWriter::Pointer writer = PhWriter::New();
      writer->setFileName(getFileName());
      writer->setData(getGrainIds());
      int dims[3];
      float resolution[3];
      float origin[3];
      getDimensions(dims);
      getResolution(resolution);
      getOrigin(origin);
      writer->setDimensions(dims);
      writer->setResolution(resolution);
      writer->setOrigin(origin);
      int err = writer->writeFile();
      if (err < 0)
      {
        setErrorMessage("Error Writing Ph File");
      }
      return err;
    }

  protected:
    PhGrainIdWriter(){}

  private:
    PhGrainIdWriter(const PhGrainIdWriter&); // Copy Constructor Not Implemented
    void operator=(const PhGrainIdWriter&); // Operator '=' Not Implemented
};

/**
 *
 */
class DxGrainIdWriter : public GrainIdWriter
{
  public:
    DREAM3D_SHARED_POINTERS(DxGrainIdWriter)
    DREAM3D_STATIC_NEW_MACRO(DxGrainIdWriter);
    DREAM3D_TYPE_MACRO(DxGrainIdWriter);
    DREAM3D_STATIC_NEW_SUPERCLASS(GrainIdWriter, DxGrainIdWriter);

    virtual ~DxGrainIdWriter() {};

    virtual int writeGrainIds()
    {
      DxWriter::Pointer writer = DxWriter::New();
      writer->setFileName(getFileName());
      writer->setData(getGrainIds());
      int dims[3];
      float resolution[3];
      float origin[3];
      getDimensions(dims);
      getResolution(resolution);
      getOrigin(origin);
      writer->setDimensions(dims);
      writer->setResolution(resolution);
      writer->setOrigin(origin);
      int err = writer->writeFile();
      if (err < 0)
      {
        setErrorMessage("Error Writing Dx File");
      }
      return err;
    }

  protected:
    DxGrainIdWriter(){}

  private:
    DxGrainIdWriter(const DxGrainIdWriter&); // Copy Constructor Not Implemented
    void operator=(const DxGrainIdWriter&); // Operator '=' Not Implemented
};

/**
 * @brief This class is here solely to help the GrainIdVtkRectilinearGridWriter
 * class write data. It is here to implement the bare minimum of public variables
 * in order to use the templated classes above.
 */
namespace VtkFileWriter
{
  class VtkGrainIdHelper
  {
    public:
      DREAM3D_SHARED_POINTERS(VtkGrainIdHelper);
      DREAM3D_STATIC_NEW_MACRO(VtkGrainIdHelper);
      virtual ~VtkGrainIdHelper() {};

      float resx;
      float resy;
      float resz;

      int xpoints;
      int ypoints;
      int zpoints;
      int totalpoints;
      int* grain_indicies;

    protected:
      VtkGrainIdHelper() {
        grain_indicies = NULL;
      }
    private:
      VtkGrainIdHelper(const VtkGrainIdHelper&); // Copy Constructor Not Implemented
      void operator=(const VtkGrainIdHelper&); // Operator '=' Not Implemented
  };
}



/**
 * @brief This class exists solely to help convert files from ph and dx format
 * into vtk format. It will ONLY write out GrainId values to a vtkRectilinearGrid
 * file of binary type.
 */

class VtkGrainIdWriter : public GrainIdWriter
{
  public:
    DREAM3D_SHARED_POINTERS(VtkGrainIdWriter);
    DREAM3D_STATIC_NEW_MACRO(VtkGrainIdWriter);
    DREAM3D_TYPE_MACRO(VtkGrainIdWriter);
    DREAM3D_STATIC_NEW_SUPERCLASS(GrainIdWriter, VtkGrainIdWriter);

    virtual ~VtkGrainIdWriter() { }

    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFiles)

    int writeGrainIds()
    {

      // Copy all the variables into the helper class from above
      VtkFileWriter::VtkGrainIdHelper::Pointer r = VtkFileWriter::VtkGrainIdHelper::New();
      getDimensions(r->xpoints, r->ypoints, r->zpoints);
      getResolution(r->resx, r->resy, r->resz);
      r->grain_indicies = getGrainIds()->GetPointer(0);

      VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<VtkFileWriter::VtkGrainIdHelper>(r.get()));
      std::vector<VtkScalarWriter*> scalarsToWrite;
      w0->m_WriteBinaryFiles = m_WriteBinaryFiles;
      scalarsToWrite.push_back(w0);
      VTKRectilinearGridFileWriter writer;
      writer.setWriteBinaryFiles(m_WriteBinaryFiles);
      int err = writer.write<VtkFileWriter::VtkGrainIdHelper>(getFileName(), r.get(), scalarsToWrite);
      if (err < 0)
      {
        setErrorMessage("Error Writing vtk File");
      }
      return err;
    }

  protected:
    VtkGrainIdWriter() :
      GrainIdWriter()
    {
      m_WriteBinaryFiles = true;
    }

  private:
    VtkGrainIdWriter(const VtkGrainIdWriter&); // Copy Constructor Not Implemented
    void operator=(const VtkGrainIdWriter&); // Operator '=' Not Implemented
};



#endif //_DREAM3DFileWriter_h_

