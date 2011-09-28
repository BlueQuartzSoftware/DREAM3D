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
#ifndef _PhaseType_h_
#define _PhaseType_h_

#include <string>
#include <vector>
#include <map>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3D/Common/Constants.h"

namespace DREAM3D {
/**
 * @class PhaseType PhaseType.h PathToHeader/PhaseType.h
 * @brief
 * @author Mike Jackson for BlueQuartz.net
 * @date May 13, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT PhaseType
{
  public:

    virtual ~PhaseType();

    static std::string PrimaryStr() { return std::string("Primary"); }
    static std::string PrecipitateStr() { return std::string("Precipitate"); }
    static std::string TransformationStr() { return std::string("Transformation"); }
    static std::string UnknownPhaseTypeStr() { return std::string("Unknown Phase Type"); }


    static std::string getPhaseTypeString(DREAM3D::Reconstruction::PhaseType phaseType)
    {
      switch(phaseType)
      {
        case DREAM3D::Reconstruction::PrimaryPhase:
          return PrimaryStr();
        case DREAM3D::Reconstruction::PrecipitatePhase:
          return PrecipitateStr();
        case DREAM3D::Reconstruction::TransformationPhase:
          return TransformationStr();
        case DREAM3D::Reconstruction::UnknownPhaseType:
          return UnknownPhaseTypeStr();
        default:
          break;
      }
      return std::string("Undefined Phase Type (Error)");
    }

    static DREAM3D::Reconstruction::PhaseType getPhaseType(const char* str)
    {
      if (PrimaryStr().compare(str) == 0)
      {
        return DREAM3D::Reconstruction::PrimaryPhase;
      }
      else if (PrecipitateStr().compare(str) == 0)
      {
        return DREAM3D::Reconstruction::PrecipitatePhase;
      }
      else if (TransformationStr().compare(str) == 0)
      {
        return DREAM3D::Reconstruction::TransformationPhase;
      }
      return DREAM3D::Reconstruction::UnknownPhaseType;
    }


    static void getPhaseTypeStrings(std::vector<std::string> &strings)
    {
      strings.clear();
      strings.push_back(PrimaryStr());
      strings.push_back(PrecipitateStr());
//      strings.push_back(TransformationStr());
      strings.push_back(UnknownPhaseTypeStr());
    }

    static void getPhaseTypeEnums(std::vector<DREAM3D::Reconstruction::PhaseType> &types)
    {
      types.clear();
      types.push_back(DREAM3D::Reconstruction::PrimaryPhase);
      types.push_back(DREAM3D::Reconstruction::PrecipitatePhase);
//      types.push_back(AIM::Reconstruction::TransformationPhase);
      types.push_back(DREAM3D::Reconstruction::UnknownPhaseType);
    }

    static void getPhaseTypeMap(std::map<int, std::string> &map)
    {
      map.clear();
      map[DREAM3D::Reconstruction::PrimaryPhase] = PrimaryStr();
      map[DREAM3D::Reconstruction::PrecipitatePhase] = PrecipitateStr();
//      map[AIM::Reconstruction::TransformationPhase] = TransformationStr();
      map[DREAM3D::Reconstruction::UnknownPhaseType] = UnknownPhaseTypeStr();
    }

  protected:
    PhaseType();
  private:
    PhaseType(const PhaseType&); //Not Implemented
    void operator=(const PhaseType&); //Not Implemented

};


} // End Namespace 'AIM'

#endif //_PhaseType_h_


