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

#include <memory>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Observable.h"

#include "EMMPMLib/Core/EMMPM_Constants.h"
#include "EMMPMLib/Core/EMMPM_Data.h"


class EMMPMLib_EXPORT MorphFilter : public Observable
{
    Q_OBJECT;

  public:
    using Self = MorphFilter;
    ;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static Pointer New();

    /**
     * @brief Returns the name of the class for MorphFilter;
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for MorphFilter;
     */
    static QString ClassName();

    ~MorphFilter() override;

    unsigned int maxi(int a, int b);

    unsigned int mini(int a, int b);

    /**
     * @brief Performs the Morphological filtering on the input image
     * @param data Non nullptr EMMPM_Data pointer
     * @param curve
     * @param se
     * @param r
     */
    void morphFilt(EMMPM_Data* data, unsigned char* curve, unsigned char* se, int r);

    /**
     * @brief Main entry point for the morphological filter
     * @param data Non nullptr EMMPM_Data pointer
     */
    void multiSE(EMMPM_Data* data);

    /**
     * @brief Setter property for ErrorCode
     */
    void setErrorCode(int value);
    /**
     * @brief Getter property for ErrorCode
     * @return Value of ErrorCode
     */
    int getErrorCode() const;

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual QString getHumanLabel() const;

  protected:
    MorphFilter();

  private:
    int m_ErrorCode = 0;

  public:
    MorphFilter(const MorphFilter&) = delete;            // Copy Constructor Not Implemented
    MorphFilter(MorphFilter&&) = delete;                 // Move Constructor Not Implemented
    MorphFilter& operator=(const MorphFilter&) = delete; // Copy Assignment Not Implemented
    MorphFilter& operator=(MorphFilter&&) = delete;      // Move Assignment Not Implemented
};
