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
#ifndef _SHAPETYPE_H_
#define _SHAPETYPE_H_


#include <string>
#include <vector>
#include <map>

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"

namespace AIM
{

  /**
   * @class ShapeType ShapeType.h PathToHeader/ShapeType.h
   * @brief
   * @author Mike Jackson for BlueQuartz.net
   * @date June 28, 2011
   * @version 1.0
   */
  class DREAM3DLib_EXPORT ShapeType
  {
    public:

      virtual ~ShapeType();

      static std::string EllipsoidStr() { return std::string("Ellipsoid"); }
      static std::string SuperEllipsoid() { return std::string("Super Ellipsoid"); }
      static std::string CubeOctahedronStr() { return std::string("Cube Octahedron"); }
      static std::string CylinderStr() { return std::string("Cylinder"); }
      static std::string UnknownShapeTypeStr() { return std::string("Unknown Shape Type"); }

      static std::string getShapeTypeString(AIM::SyntheticBuilder::ShapeType ShapeType)
      {
        switch(ShapeType)
        {
          case AIM::SyntheticBuilder::EllipsoidShape:
            return EllipsoidStr();
          case AIM::SyntheticBuilder::SuperEllipsoidShape:
            return SuperEllipsoid();
          case AIM::SyntheticBuilder::CubeOctahedronShape:
            return CubeOctahedronStr();
          case AIM::SyntheticBuilder::CylinderShape:
            return CylinderStr();
          case AIM::SyntheticBuilder::UnknownShapeType:
            return UnknownShapeTypeStr();
          default:
            break;
        }
        return std::string("Undefined ShapeType (Error)");
      }

      static AIM::SyntheticBuilder::ShapeType getShapeType(const char* str)
      {
        if (EllipsoidStr().compare(str) == 0)
        {
          return AIM::SyntheticBuilder::EllipsoidShape;
        }
        else if (SuperEllipsoid().compare(str) == 0)
        {
          return AIM::SyntheticBuilder::SuperEllipsoidShape;
        }
        else if (CubeOctahedronStr().compare(str) == 0)
        {
          return AIM::SyntheticBuilder::CubeOctahedronShape;
        }
        else if (CylinderStr().compare(str) == 0)
        {
          return AIM::SyntheticBuilder::CylinderShape;
        }
        return AIM::SyntheticBuilder::UnknownShapeType;
      }


      static void getShapeTypeStrings(std::vector<std::string> &strings)
      {
        strings.clear();
        strings.push_back(EllipsoidStr());
        strings.push_back(SuperEllipsoid());
        strings.push_back(CubeOctahedronStr());
        strings.push_back(CylinderStr());
        strings.push_back(UnknownShapeTypeStr());
      }

      static void getShapeTypeEnums(std::vector<AIM::SyntheticBuilder::ShapeType> &types)
      {
        types.clear();
        types.push_back(AIM::SyntheticBuilder::EllipsoidShape);
        types.push_back(AIM::SyntheticBuilder::SuperEllipsoidShape);
        types.push_back(AIM::SyntheticBuilder::CubeOctahedronShape);
        types.push_back(AIM::SyntheticBuilder::CylinderShape);
        types.push_back(AIM::SyntheticBuilder::UnknownShapeType);
      }

      static void getShapeTypeMap(std::map<int, std::string> &map)
      {
        map.clear();
        map[AIM::SyntheticBuilder::EllipsoidShape] = EllipsoidStr();
        map[AIM::SyntheticBuilder::SuperEllipsoidShape] = SuperEllipsoid();
        map[AIM::SyntheticBuilder::CubeOctahedronShape] = CubeOctahedronStr();
        map[AIM::SyntheticBuilder::CylinderShape] = CylinderStr();
        map[AIM::SyntheticBuilder::UnknownShapeType] = UnknownShapeTypeStr();
      }
    protected:
      ShapeType();
    private:
      ShapeType(const ShapeType&); //Not Implemented
      void operator=(const ShapeType&); //Not Implemented



  };

} /* namespace AIM */
#endif /* SHAPETYPE_H_ */
