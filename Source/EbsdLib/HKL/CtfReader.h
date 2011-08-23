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




#ifndef _CTFREADER_H_
#define _CTFREADER_H_

#include <string>
#include <map>
#include <vector>

#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLibConfiguration.h"
#include "EbsdLib/EbsdConstants.h"
#include "CtfConstants.h"
#include "CtfHeaderEntry.h"
#include "CtfPhase.h"



/**
* @class CtfReader CtfReader.h EbsdLib/HKL/CtfReader.h
* @brief This class is a self contained HKL .ctf file reader and will read a
* single .ctf file and store all the data in column centric pointers.
* @author Michael A. Jackson for BlueQuartz Software
* @date Aug 1, 2011
* @version 1.0
*/
class EbsdLib_EXPORT CtfReader
{
  public:
    CtfReader();
    virtual ~CtfReader();

    /** @brief Allow the user to set the origin of the scan */
    EBSD_INSTANCE_PROPERTY(Ebsd::RefFrameOrigin, UserOrigin)
    EBSD_INSTANCE_PROPERTY(Ebsd::RefFrameZDir, UserZDir)
    EBSD_INSTANCE_PROPERTY(bool, AxesFlipped)

    /** @brief Sets the file name of the ang file to be read */
    EBSD_INSTANCE_STRING_PROPERTY( FileName )
    EBSD_INSTANCE_PROPERTY(int, NumFields);

    EBSD_INSTANCE_STRING_PROPERTY(CompleteHeader);

    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, std::string, Channel, Ebsd::Ctf::ChannelTextFile)
    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, std::string, Prj, Ebsd::Ctf::Prj)
    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, std::string, Author, Ebsd::Ctf::Author)
    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, std::string, JobMode, Ebsd::Ctf::JobMode)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, XCells, Ebsd::Ctf::XCells)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, YCells, Ebsd::Ctf::YCells)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, XStep, Ebsd::Ctf::XStep)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, YStep, Ebsd::Ctf::YStep)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, AcqE1, Ebsd::Ctf::AcqE1)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, AcqE2, Ebsd::Ctf::AcqE2)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, AcqE3, Ebsd::Ctf::AcqE3)
    EbsdHeader_INSTANCE_PROPERTY(CtfStringHeaderEntry, std::string, Euler, Ebsd::Ctf::Euler)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, Mag, Ebsd::Ctf::Mag)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, Coverage, Ebsd::Ctf::Coverage)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, Device, Ebsd::Ctf::Device)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, KV, Ebsd::Ctf::KV)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, TiltAngle, Ebsd::Ctf::TiltAngle)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<float>, float, TiltAxis, Ebsd::Ctf::TiltAxis)
    EbsdHeader_INSTANCE_PROPERTY(CtfHeaderEntry<int>, int, NumPhases, Ebsd::Ctf::NumPhases)

    EBSD_INSTANCE_PROPERTY(std::vector<CtfPhase::Pointer>, PhaseVector)


    EBSD_POINTER_PROPERTY(Phase, Phase, int)
    EBSD_POINTER_PROPERTY(X, X, float)
    EBSD_POINTER_PROPERTY(Y, Y, float)
    EBSD_POINTER_PROPERTY(BandCount, BandCount, int)
    EBSD_POINTER_PROPERTY(Error, Error, int)
    EBSD_POINTER_PROPERTY(Euler1, Euler1, float)
    EBSD_POINTER_PROPERTY(Euler2, Euler2, float)
    EBSD_POINTER_PROPERTY(Euler3, Euler3, float)
    EBSD_POINTER_PROPERTY(MeanAngularDeviation, MAD, float)
    EBSD_POINTER_PROPERTY(BandContrast, BC, int)
    EBSD_POINTER_PROPERTY(BandSlope, BS, int)

    /**
     * @brief Returns the pointer to the data for a given field
     * @param fieldName The name of the field to return the pointer to.
     */
    void* getPointerByName(const std::string &fieldName);

    /**
     * @brief Returns an enumeration value that depicts the numerical
     * primitive type that the data is stored as (Int, Float, etc).
     * @param fieldName The name of the field.
     */
    Ebsd::NumType getPointerType(const std::string &fieldName);

    /**
    * @brief Reads the complete HKL .ctf file.
    * @return 1 on success
    */
    virtual int readFile();

    /**
    * @brief Reads ONLY the header portion of the HKL .ctf file
    * @return 1 on success
    */
    virtual int readHeaderOnly();

    virtual void printHeader(std::ostream &out);

  protected:
      // Needed by subclasses
      std::map<std::string, EbsdHeaderEntry::Pointer> m_Headermap;

      /** @brief Allocates the proper amount of memory (after reading the header portion of the file)
      * and then splats '0' across all the bytes of the memory allocation
      */
      void initPointers(size_t numElements);

      /** @brief 'free's the allocated memory and sets the pointer to NULL
      */
      void deletePointers();

      /**
       * @brief Allocats a contiguous chunk of memory to store values from the .ang file
       * @param numberOfElements The number of elements in the Array. This method can
       * also optionally produce SSE aligned memory for use with SSE intrinsics
       * @return Pointer to allocated memory
       */
        template<typename T>
        T* allocateArray(size_t numberOfElements)
        {
    #if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
          T* m_buffer = static_cast<T*>( _mm_malloc (numberOfElements * sizeof(T), 16) );
    #else
          T*  m_buffer = new T[numberOfElements];
    #endif
          m_NumberOfElements = numberOfElements;
          return m_buffer;
        }

      /**
       * @brief Deallocates memory that has been previously allocated. This will set the
       * value of the pointer passed in as the argument to NULL.
       * @param ptr The pointer to be freed.
       */
        template<typename T>
        void deallocateArrayData(T* &ptr)
        {
          if (ptr != NULL && this->m_ManageMemory == true)
          {
    #if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
            _mm_free(ptr );
    #else
            delete[] ptr;
    #endif
            ptr = NULL;
            m_NumberOfElements = 0;
          }
        }

  private:
     bool m_ManageMemory;  // We are going to forcibly manage the memory. There is currently NO option otherwise.
     bool m_headerComplete;
     size_t m_NumberOfElements;

     std::vector<std::string> tokenize(char* buf, char delimiter);

     int getHeaderLines(std::ifstream &reader, std::vector<std::vector<std::string> > &headerLines);

     /**
      * Checks that the line is the header of the columns for the data.
      *
      * @param columns
      *            line values
      * @return <code>true</code> if the line is the columns header line,
      *         <code>false</code> otherwise
      */
     bool isDataHeaderLine(std::vector<std::string> &columns);

     /**
      *
      */
     int parseHeaderLines(std::vector<std::vector<std::string> > &headerLines);

     /**
      * @brief Reads a line of Data from the ASCII based file
      * @param line The current line of data
      * @param row Current Row of Data
      * @param i The current index into a flat array
      * @param xCells Number of X Data Points
      * @param yCells Number of Y Data Points
      * @param col The current Column of Data
      */
     void readData(const std::string &line, int row, int col, size_t i, int xCells, int yCells );

    CtfReader(const CtfReader&); // Copy Constructor Not Implemented
    void operator=(const CtfReader&); // Operator '=' Not Implemented
};


#endif /* _CTFREADER_H_ */
