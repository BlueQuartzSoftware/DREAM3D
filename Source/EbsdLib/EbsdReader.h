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
#ifndef EBSDREADER_H_
#define EBSDREADER_H_

#include <string>
#include <map>


#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdHeaderEntry.h"

/**
 * @class EbsdReader EbsdReader.h EbsdLib/EbsdReader.h
 * @brief This class is the super class to read an Ebsd data file. This class is
 * meant to be subclassed for each manufacturer so that custom readers and parsers
 * can be written for those data files. The current subclasses are for TSL (.ang)
 * and HKL (.ctf) data files and their HDF5 versions also.
 *
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Aug 24, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT EbsdReader
{
  public:
    EbsdReader();
    EBSD_TYPE_MACRO(EbsdReader)

    virtual ~EbsdReader();

    /**
     * @brief These get filled out if there are errors. Negative values are error codes
     */
    EBSD_INSTANCE_PROPERTY(int, ErrorCode)

    EBSD_INSTANCE_STRING_PROPERTY(ErrorMessage)


    /** @brief Allow the user to set the origin of the scan */
    EBSD_INSTANCE_PROPERTY(Ebsd::RefFrameZDir, UserZDir)
    EBSD_INSTANCE_PROPERTY(float, SampleTransformationAngle)
    EBSD_INSTANCE_PROPERTY(std::vector<float>, SampleTransformationAxis)
    EBSD_INSTANCE_PROPERTY(float, EulerTransformationAngle)
    EBSD_INSTANCE_PROPERTY(std::vector<float>, EulerTransformationAxis)

    /** @brief Sets the file name of the ebsd file to be read */
    EBSD_INSTANCE_STRING_PROPERTY(FileName)
    /** @brief The Number of Columns of Data in the Ebsd Data file */
    EBSD_INSTANCE_PROPERTY(int, NumFields)

    /** @brief The unchanged header from the data file */
    EBSD_INSTANCE_STRING_PROPERTY(OriginalHeader)
    /**
     * @brief Appends text to the current Original Header Text
     * @param more The text to be appended
     */
    void appendOriginalHeader(const std::string &more);


    /* These variables pertain to the memory that this class or subclass will allocate
     * for the various columns of data and how that memory is managed.
     */
    /** @brief Will this class be responsible for deallocating the memory for the data arrays */
    EBSD_INSTANCE_PROPERTY(bool, ManageMemory)
    /** @brief Has the complete header been read */
    EBSD_INSTANCE_PROPERTY(bool, HeaderIsComplete)
    /** @brief The number of elements in a column of data. This should be rows * columns */
    EBSD_INSTANCE_PROPERTY(size_t, NumberOfElements)

    /*
     * Different manufacturers call this value different thingsl. TSL = NumRows | NumCols,
     * HKL=XCells. These methods should be implemented by subclasses to return the proper value.
     */
    /**
     * @brief Returns the X Dimension of the data. This method is pure virtual
     * and should be implemented by subclasses.
     */
    virtual int getXDimension() = 0;
    /**
     * @brief Sets the X Dimension of the data. This method is pure virtual
     * and should be implemented by subclasses.
     */
    virtual void setXDimension(int xdim) = 0;
    /**
     * @brief Returns the Y Dimension of the data. This method is pure virtual
     * and should be implemented by subclasses.
     */
    virtual int getYDimension() = 0;
    /**
     * @brief Sets the Y Dimension of the data. This method is pure virtual
     * and should be implemented by subclasses.
     */
    virtual void setYDimension(int ydim) = 0;

    /**
     * @brief Returns the pointer to the data for a given field
     * @param fieldName The name of the field to return the pointer to.
     */
    virtual void* getPointerByName(const std::string &fieldName) = 0;

    /**
     * @brief Returns an enumeration value that depicts the numerical
     * primitive type that the data is stored as (Int, Float, etc).
     * @param fieldName The name of the field.
     */
    virtual Ebsd::NumType getPointerType(const std::string &fieldName) = 0;

    /**
    * @brief Reads the complete EBSD data file storing all columns of data and the
    * header values in memory
    * @return 0 or positive value on success
    */
    virtual int readFile() = 0;

    /**
    * @brief Reads ONLY the header portion of the EBSD Data file.
    * @return 0 or positive value on success
    */
    virtual int readHeaderOnly() = 0;


    /** @brief Allocates the proper amount of memory (after reading the header portion of the file)
    * and then splats '0' across all the bytes of the memory allocation
    */
    virtual void initPointers(size_t numElements) = 0;

    /** @brief 'free's the allocated memory and sets the pointer to NULL
    */
    virtual void deletePointers() = 0;

    /**
     * @brief Allocats a contiguous chunk of memory to store values from the .ang file
     * @param numberOfElements The number of elements in the Array. This method can
     * also optionally produce SSE aligned memory for use with SSE intrinsics
     * @return Pointer to allocated memory
     */
      template<typename T>
      T* allocateArray(size_t numberOfElements)
      {
  #if defined ( DREAM3D_USE_SSE ) && defined ( __SSE2__ )
        T* m_buffer = static_cast<T*>( _mm_malloc (numberOfElements * sizeof(T), 16) );
  #else
        //T*  m_buffer = new T[numberOfElements];
        T* m_buffer = static_cast<T*>(malloc(sizeof(T) * numberOfElements));
  #endif
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
  #if defined ( DREAM3D_USE_SSE ) && defined ( __SSE2__ )
          _mm_free(ptr );
  #else
          //delete[] ptr;
          free(ptr);
  #endif
          ptr = NULL;
   //       m_NumberOfElements = 0;
        }
      }

  protected:
    std::map<std::string, EbsdHeaderEntry::Pointer> m_HeaderMap;


  private:
    EbsdReader(const EbsdReader&); // Copy Constructor Not Implemented
    void operator=(const EbsdReader&); // Operator '=' Not Implemented

};

#endif /* EBSDREADER_H_ */
