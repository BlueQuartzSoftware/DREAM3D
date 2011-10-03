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
#ifndef DREAM3DFILEREADER_H_
#define DREAM3DFILEREADER_H_



#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/IO/DREAM3DDataFile.h"
#include "DREAM3DLib/IO/PhReader.h"
#include "DREAM3DLib/IO/DxReader.h"

/**
 *
 */
class GrainIdReader : public DREAM3DDataFile
{
  public:

    DREAM3D_SHARED_POINTERS(GrainIdReader)
    DREAM3D_STATIC_NEW_MACRO(GrainIdReader);
    DREAM3D_TYPE_MACRO(GrainIdReader);

    virtual ~GrainIdReader() {};

    DREAM3D_INSTANCE_PROPERTY(AIMArray<int>::Pointer, GrainIds);

    virtual int readGrainIds() { return -1; }

  protected:
    GrainIdReader(){ };

  private:
    GrainIdReader(const GrainIdReader&); // Copy Constructor Not Implemented
    void operator=(const GrainIdReader&); // Operator '=' Not Implemented
};

/**
 *
 */
class PhGrainIdReader : public GrainIdReader
{
  public:
    DREAM3D_SHARED_POINTERS(PhGrainIdReader)
    DREAM3D_STATIC_NEW_MACRO(PhGrainIdReader);
    DREAM3D_TYPE_MACRO(PhGrainIdReader);
    DREAM3D_STATIC_NEW_SUPERCLASS(GrainIdReader, PhGrainIdReader);

    virtual ~PhGrainIdReader() {};

    virtual int readGrainIds()
    {
      PhReader::Pointer reader = PhReader::New();
      reader->setFileName(getFileName());
      int err = reader->readFile();
      int dims[3];
      float resolution[3];
      float origin[3];
      reader->getDimensions(dims);
      reader->getResolution(resolution);
      reader->getOrigin(origin);

      setDimensions(dims);
      setResolution(resolution);
      setOrigin(origin);

      setGrainIds(reader->getData());
      return err;
    }

  protected:
    PhGrainIdReader(){}

  private:
    PhGrainIdReader(const PhGrainIdReader&); // Copy Constructor Not Implemented
    void operator=(const PhGrainIdReader&); // Operator '=' Not Implemented
};

/**
 *
 */
class DxGrainIdReader : public GrainIdReader
{
  public:
    DREAM3D_SHARED_POINTERS(DxGrainIdReader)
    DREAM3D_STATIC_NEW_MACRO(DxGrainIdReader);
    DREAM3D_TYPE_MACRO(DxGrainIdReader);
    DREAM3D_STATIC_NEW_SUPERCLASS(GrainIdReader, DxGrainIdReader);

    virtual ~DxGrainIdReader() {};

    virtual int readGrainIds()
    {
      DxReader::Pointer reader = DxReader::New();
      reader->setFileName(getFileName());
      int err = reader->readFile();
      int dims[3];
      float resolution[3];
      float origin[3];
      reader->getDimensions(dims);
      reader->getResolution(resolution);
      reader->getOrigin(origin);

      setDimensions(dims);
      setResolution(resolution);
      setOrigin(origin);

      setGrainIds(reader->getData());
      return err;
    }

  protected:
    DxGrainIdReader(){}

  private:
    DxGrainIdReader(const DxGrainIdReader&); // Copy Constructor Not Implemented
    void operator=(const DxGrainIdReader&); // Operator '=' Not Implemented
};




#endif /* DREAM3DFILEREADER_H_ */
