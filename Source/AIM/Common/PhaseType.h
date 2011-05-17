/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _PhaseType_h_
#define _PhaseType_h_

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"

namespace AIM {
/**
 * @class PhaseType PhaseType.h PathToHeader/PhaseType.h
 * @brief
 * @author Mike Jackson for BlueQuartz.net
 * @date May 13, 2011
 * @version 1.0
 */
class PhaseType
{
  public:
    const static std::string PrimaryStr;
    const static std::string PrecipitateStr;
    const static std::string TransformationStr;
    const static std::string UnknownPhaseTypeStr;


    virtual ~PhaseType();

    static std::string getPhaseTypeString(AIM::Reconstruction::PhaseType phaseType)
    {
      switch(phaseType)
      {
        case AIM::Reconstruction::PrimaryPhase:
          return PrimaryStr;
        case AIM::Reconstruction::PrecipitatePhase:
          return PrecipitateStr;
        case AIM::Reconstruction::TransformationPhase:
          return TransformationStr;
        case AIM::Reconstruction::UnknownPhaseType:
          return UnknownPhaseTypeStr;
        default:
          break;
      }
      return std::string("Undefined Phase Type (Error)");
    }


    static void getPhaseTypeStringList(std::list<std::string> &list)
    {
      list.push_back(PrimaryStr);
      list.push_back(PrecipitateStr);
      list.push_back(TransformationStr);
      list.push_back(UnknownPhaseTypeStr);
    }

  protected:
    PhaseType();
  private:
    PhaseType(const PhaseType&); //Not Implemented
    void operator=(const PhaseType&); //Not Implemented

};

  const std::string PhaseType::PrimaryStr("Primary");
  const std::string PhaseType::PrecipitateStr("Precipitate");
  const std::string PhaseType::TransformationStr("Transformation");
  const std::string PhaseType::UnknownPhaseTypeStr("Unknown Phase Type");
} // End Namespace 'AIM'

#endif //_PhaseType_h_


