/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef _AbstractStructureReader_h_
#define _AbstractStructureReader_h_

class GrainGeneratorFunc;

/**
 * @class AbstractStructureReader AbstractStructureReader.h AIM/GrainGenerator/AbstractStructureReader.h
 * @brief This is the superclass for classes that are written to read a pre-existing
 * microstructure into the Grain Generator Environment.
 * @author Michael A. Jackson (BlueQuartz Software)
 * @date May 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT AbstractStructureReader
{
  public:
    MXA_SHARED_POINTERS( AbstractStructureReader)

    virtual ~AbstractStructureReader() { } ;

    /**
     * @brief Pure virtual method that needs to be implemented by subclasses. This
     * is the main entry point to read the grain structure from the file or other
     * data sink.
     * @param r The GrainGeneratorFunc pointer
     * @return Error. Negative is Error. Zero or Positive is Success.
     */
    virtual int readStructure(GrainGeneratorFunc* r) = 0;

  protected:
    AbstractStructureReader() { } ;

  private:
    AbstractStructureReader(const AbstractStructureReader&); //Not Implemented
    void operator=(const AbstractStructureReader&); //Not Implemented

};

#endif //_AbstractStructureReader_h_
