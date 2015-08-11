/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#ifndef _SHAPETYPE_H_
#define _SHAPETYPE_H_


#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMap>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"



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

    static QString EllipsoidStr() { return QString("Ellipsoid"); }
    static QString SuperEllipsoid() { return QString("Super Ellipsoid"); }
    static QString CubeOctahedronStr() { return QString("Cube Octahedron"); }
    static QString CylinderAStr() { return QString("Cylinder (A)"); }
    static QString CylinderBStr() { return QString("Cylinder (B)"); }
    static QString CylinderCStr() { return QString("Cylinder (C)"); }
    static QString UnknownShapeTypeStr() { return QString("Unknown Shape Type"); }

    static QString getShapeTypeString(unsigned int ShapeType)
    {
      switch(ShapeType)
      {
        case DREAM3D::ShapeType::EllipsoidShape:
          return EllipsoidStr();
        case DREAM3D::ShapeType::SuperEllipsoidShape:
          return SuperEllipsoid();
        case DREAM3D::ShapeType::CubeOctahedronShape:
          return CubeOctahedronStr();
        case DREAM3D::ShapeType::CylinderAShape:
          return CylinderAStr();
        case DREAM3D::ShapeType::CylinderBShape:
          return CylinderBStr();
        case DREAM3D::ShapeType::CylinderCShape:
          return CylinderCStr();
        case DREAM3D::ShapeType::UnknownShapeType:
          return UnknownShapeTypeStr();
        default:
          break;
      }
      return QString("Undefined ShapeType (Error)");
    }

    static unsigned int getShapeType(const char* str)
    {
      if (EllipsoidStr().compare(str) == 0)
      {
        return DREAM3D::ShapeType::EllipsoidShape;
      }
      else if (SuperEllipsoid().compare(str) == 0)
      {
        return DREAM3D::ShapeType::SuperEllipsoidShape;
      }
      else if (CubeOctahedronStr().compare(str) == 0)
      {
        return DREAM3D::ShapeType::CubeOctahedronShape;
      }
      else if (CylinderAStr().compare(str) == 0)
      {
        return DREAM3D::ShapeType::CylinderAShape;
      }
      else if (CylinderBStr().compare(str) == 0)
      {
        return DREAM3D::ShapeType::CylinderBShape;
      }
      else if (CylinderCStr().compare(str) == 0)
      {
        return DREAM3D::ShapeType::CylinderCShape;
      }
      return DREAM3D::ShapeType::UnknownShapeType;
    }


    static void getShapeTypeStrings(QVector<QString>& strings)
    {
      strings.clear();
      strings.push_back(EllipsoidStr());
      strings.push_back(SuperEllipsoid());
      strings.push_back(CubeOctahedronStr());
      strings.push_back(CylinderAStr());
      strings.push_back(CylinderBStr());
      strings.push_back(CylinderCStr());
      strings.push_back(UnknownShapeTypeStr());
    }

    static void getShapeTypeEnums(QVector<unsigned int>& types)
    {
      types.clear();
      types.push_back(DREAM3D::ShapeType::EllipsoidShape);
      types.push_back(DREAM3D::ShapeType::SuperEllipsoidShape);
      types.push_back(DREAM3D::ShapeType::CubeOctahedronShape);
      types.push_back(DREAM3D::ShapeType::CylinderAShape);
      types.push_back(DREAM3D::ShapeType::CylinderBShape);
      types.push_back(DREAM3D::ShapeType::CylinderCShape);
      types.push_back(DREAM3D::ShapeType::UnknownShapeType);
    }

    static void getShapeTypeMap(QMap<int, QString>& map)
    {
      map.clear();
      map[DREAM3D::ShapeType::EllipsoidShape] = EllipsoidStr();
      map[DREAM3D::ShapeType::SuperEllipsoidShape] = SuperEllipsoid();
      map[DREAM3D::ShapeType::CubeOctahedronShape] = CubeOctahedronStr();
      map[DREAM3D::ShapeType::CylinderAShape] = CylinderAStr();
      map[DREAM3D::ShapeType::CylinderBShape] = CylinderBStr();
      map[DREAM3D::ShapeType::CylinderCShape] = CylinderCStr();
      map[DREAM3D::ShapeType::UnknownShapeType] = UnknownShapeTypeStr();
    }
  protected:
    ShapeType();
  private:
    ShapeType(const ShapeType&); //Not Implemented
    void operator=(const ShapeType&); //Not Implemented



};

#endif /* SHAPETYPE_H_ */

