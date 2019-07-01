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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/Observable.h"

#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Core/EMMPM_Constants.h"
#include "EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPMLib/Common/StatsDelegate.h"


class EMMPMLib_EXPORT MPMCalculation : public Observable
{
    Q_OBJECT;
  public:
    SIMPL_SHARED_POINTERS(MPMCalculation)
    SIMPL_STATIC_NEW_MACRO(MPMCalculation)
    SIMPL_TYPE_MACRO(MPMCalculation)

    virtual ~MPMCalculation();

    SIMPL_INSTANCE_PROPERTY(EMMPM_Data::Pointer, Data)
    SIMPL_POINTER_PROPERTY(StatsDelegate, StatsDelegate);
    SIMPL_INSTANCE_PROPERTY(int, ErrorCode)

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() const;

    /**
     * @fn void acvmpm(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks);
     * @brief Runs the MPM portion of the EM/MPM algorithm
     * @param data a non nullptr EMMPM_Data pointer
     * @param callbacks a non nullptr EMMPM_CallbackFunctions
     */
    void execute();

  protected:
    MPMCalculation();

  private:

    void calcLoop(int rowStart, int rowEnd,
                  int colStart, int colEnd,
                  real_t* yk);

  public:
    MPMCalculation(const MPMCalculation&) = delete; // Copy Constructor Not Implemented
    MPMCalculation(MPMCalculation&&) = delete;      // Move Constructor Not Implemented
    MPMCalculation& operator=(const MPMCalculation&) = delete; // Copy Assignment Not Implemented
    MPMCalculation& operator=(MPMCalculation&&) = delete;      // Move Assignment Not Implemented
};

