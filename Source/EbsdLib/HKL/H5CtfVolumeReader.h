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

#pragma once

#include <QtCore/QString>
#include <QtCore/QVector>

//-- EbsdLib Includes
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/HKL/CtfPhase.h"


/**
 * @class H5CtfVolumeReader H5CtfVolumeReader.h Reconstruction/EbsdSupport/H5CtfVolumeReader.h
 * @brief This class loads EBSD data from an HDF5 based file.
 *
 * @date May 23, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT H5CtfVolumeReader : public H5EbsdVolumeReader
{
  public:
    EBSD_SHARED_POINTERS(H5CtfVolumeReader)
    EBSD_STATIC_NEW_SUPERCLASS(H5EbsdVolumeReader, H5CtfVolumeReader)
    EBSD_TYPE_MACRO_SUPER(H5CtfVolumeReader, H5EbsdVolumeReader)

    ~H5CtfVolumeReader() override;

    EBSD_POINTER_PROPERTY(Phase, Phase, int)
    EBSD_POINTER_PROPERTY(X, X, float)
    EBSD_POINTER_PROPERTY(Y, Y, float)
    EBSD_POINTER_PROPERTY(Z, Z, float)
    EBSD_POINTER_PROPERTY(Bands, Bands, int)
    EBSD_POINTER_PROPERTY(Error, Error, int)
    EBSD_POINTER_PROPERTY(Euler1, Euler1, float)
    EBSD_POINTER_PROPERTY(Euler2, Euler2, float)
    EBSD_POINTER_PROPERTY(Euler3, Euler3, float)
    EBSD_POINTER_PROPERTY(MAD, MAD, float)
    EBSD_POINTER_PROPERTY(BC, BC, int)
    EBSD_POINTER_PROPERTY(BS, BS, int)

    /**
     * @brief This method does the actual loading of the OIM data from the data
     * source (files, streams, etc) into the data structures.
     * @param euler1s
     * @param euler2s
     * @param euler3s
     * @param phases
     * @param xpoints
     * @param ypoints
     * @param zpoints
     * @param filters
     * @return
     */
    int loadData(int64_t xpoints, int64_t ypoints, int64_t zpoints, uint32_t ZDir) override;

    /**
     * @brief
     * @return
     */
    QVector<CtfPhase::Pointer> getPhases();



    /**
     * @brief Returns the pointer to the data for a given feature
     * @param featureName The name of the feature to return the pointer to.
     */
    void* getPointerByName(const QString& featureName) override;

    /**
     * @brief Returns an enumeration value that depicts the numerical
     * primitive type that the data is stored as (Int, Float, etc).
     * @param featureName The name of the feature.
     */
    Ebsd::NumType getPointerType(const QString& featureName) override;

    /** @brief Allocates the proper amount of memory (after reading the header portion of the file)
    * and then splats '0' across all the bytes of the memory allocation
    */
    void initPointers(size_t numElements) override;

    /** @brief 'free's the allocated memory and sets the pointer to nullptr
    */
    void deletePointers() override;

  protected:
    H5CtfVolumeReader();

  private:
    QVector<CtfPhase::Pointer> m_Phases;

    /**
     * @brief Allocats a contiguous chunk of memory to store values from the .ang file
     * @param numberOfElements The number of elements in the Array. This method can
     * also optionally produce SSE aligned memory for use with SSE intrinsics
     * @return Pointer to allocated memory
     */
    template<typename T>
    T* allocateArray(size_t numberOfElements)
    {
      T* buffer = nullptr;
      if(numberOfElements == 0)
      {
        return buffer;
      }
      buffer = new T[numberOfElements]();
      return buffer;
    }

    /**
     * @brief Deallocates memory that has been previously allocated. This will set the
     * value of the pointer passed in as the argument to nullptr.
     * @param ptr The pointer to be freed.
     */
    template<typename T>
    void deallocateArrayData(T*& ptr)
    {
      if(ptr != nullptr && getManageMemory())
      {
        delete[](ptr);
        ptr = nullptr;
      }
    }

  public:
    H5CtfVolumeReader(const H5CtfVolumeReader&) = delete;            // Copy Constructor Not Implemented
    H5CtfVolumeReader(H5CtfVolumeReader&&) = delete;                 // Move Constructor Not Implemented
    H5CtfVolumeReader& operator=(const H5CtfVolumeReader&) = delete; // Copy Assignment Not Implemented
    H5CtfVolumeReader& operator=(H5CtfVolumeReader&&) = delete;      // Move Assignment Not Implemented
};


