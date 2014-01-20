/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _H5MicDATALOADER_H_
#define _H5MicDATALOADER_H_

#include <QtCore/QVector>

//-- Ebsd Lib Includes
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeReader.h"

#include "EbsdLib/HEDM/MicPhase.h"



/**
 * @class H5MicVolumeReader H5MicVolumeReader.h Reconstruction/EbsdSupport/H5MicVolumeReader.h
 * @brief This class loads OIM data from an HDF5 based file.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 23, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT H5MicVolumeReader : public H5EbsdVolumeReader
{
  public:
    EBSD_SHARED_POINTERS(H5MicVolumeReader)
    EBSD_STATIC_NEW_SUPERCLASS(H5EbsdVolumeReader, H5MicVolumeReader)

    virtual ~H5MicVolumeReader();


    EBSD_POINTER_PROPERTY(Euler1, Euler1, float)
    EBSD_POINTER_PROPERTY(Euler2, Euler2, float)
    EBSD_POINTER_PROPERTY(Euler3, Euler3, float)
    EBSD_POINTER_PROPERTY(X, X, float)
    EBSD_POINTER_PROPERTY(Y, Y, float)
    EBSD_POINTER_PROPERTY(Confidence, Conf, float)
    EBSD_POINTER_PROPERTY(Phase, Phase, int)

    /**
     * @brief This method does the actual loading of the OIM data from the data
     * source (files, streams, etc) into the data structures.
     * @param eulerMicles
     * @param phases
     * @param xpoints
     * @param ypoints
     * @param zpoints
     * @param filters
     * @return
     */
    int loadData(int64_t xpoints, int64_t ypoints, int64_t zpoints, Ebsd::RefFrameZDir ZDir);

    /**
     * @brief
     * @return
     */
    QVector<MicPhase::Pointer> getPhases();
    /**
      * @brief Returns the pointer to the data for a given feature
      * @param featureName The name of the feature to return the pointer to.
      */
    void* getPointerByName(const QString& featureName);

    /**
     * @brief Returns an enumeration value that depicts the numerical
     * primitive type that the data is stored as (Int, Float, etc).
     * @param featureName The name of the feature.
     */
    Ebsd::NumType getPointerType(const QString& featureName);

    /** @brief Allocates the proper amount of memory (after reading the header portion of the file)
    * and then splats '0' across all the bytes of the memory allocation
    */
    void initPointers(size_t numElements);

    /** @brief 'free's the allocated memory and sets the pointer to NULL
    */
    void deletePointers();

  protected:
    H5MicVolumeReader();

  private:
    QVector<MicPhase::Pointer> m_Phases;

    H5MicVolumeReader(const H5MicVolumeReader&);    // Copy Constructor Not Implemented
    void operator=(const H5MicVolumeReader&);  // Operator '=' Not Implemented

    /**
     * @brief Allocats a contiguous chunk of memory to store values from the .Mic file
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
      T*  m_buffer = new T[numberOfElements];
#endif
      //      m_NumberOfElements = numberOfElements;
      return m_buffer;
    }

    /**
     * @brief Deallocates memory that has been previously allocated. This will set the
     * value of the pointer passed in as the argument to NULL.
     * @param ptr The pointer to be freed.
     */
    template<typename T>
    void deallocateArrayData(T*& ptr)
    {
      if (ptr != NULL && getManageMemory() == true)
      {
#if defined ( DREAM3D_USE_SSE ) && defined ( __SSE2__ )
        _mm_free(ptr );
#else
        delete[] ptr;
#endif
        ptr = NULL;
        //       m_NumberOfElements = 0;
      }
    }

};

#endif /* _H5MicDATALOADER_H_ */

