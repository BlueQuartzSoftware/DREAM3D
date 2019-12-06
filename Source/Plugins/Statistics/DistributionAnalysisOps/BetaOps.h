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

#include <vector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "DistributionAnalysisOps/DistributionAnalysisOps.h"
/*
 *
 */
class BetaOps : public DistributionAnalysisOps
{
  public:
    using Self = BetaOps;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    /**
     * @brief Returns the name of the class for BetaOps
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for BetaOps
     */
    static QString ClassName();

    static Pointer New();

    ~BetaOps() override;

    /**
     * @brief calculateParameters
     * @param data
     * @param outputs
     * @return
     */
    int calculateParameters(std::vector<float>& data, FloatArrayType::Pointer outputs) override;

    /**
     * @brief calculateCorrelatedParameters
     * @param data
     * @param outputs
     * @return
     */
    int calculateCorrelatedParameters(std::vector<std::vector<float>>& data, VectorOfFloatArray outputs) override;

  protected:
    BetaOps();

  public:
    BetaOps(const BetaOps&) = delete;        // Copy Constructor Not Implemented
    BetaOps(BetaOps&&) = delete;             // Move Constructor Not Implemented
    BetaOps& operator=(const BetaOps&) = delete; // Copy Assignment Not Implemented
    BetaOps& operator=(BetaOps&&) = delete;      // Move Assignment Not Implemented

  private:
};


