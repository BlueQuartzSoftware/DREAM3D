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


#ifndef _FilterParameter_H_
#define _FilterParameter_H_

#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include <QtCore/QJsonObject>
#include "EbsdLib/EbsdConstants.h"
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataContainers/DataContainerArrayProxy.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

typedef struct {
  int x; int y; int z;

  void writeJson(QJsonObject &json)
  {
    json["x"] = x;
    json["y"] = y;
    json["z"] = z;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["x"].isDouble() && json["y"].isDouble() && json["z"].isDouble())
    {
      x = json["x"].toInt();
      y = json["y"].toInt();
      z = json["z"].toInt();
      return true;
    }
    return false;
  }
} IntVec3_t;

typedef struct {
  float x; float y; float z;

  void writeJson(QJsonObject &json)
  {
    json["x"] = x;
    json["y"] = y;
    json["z"] = z;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["x"].isDouble() && json["y"].isDouble() && json["z"].isDouble())
    {
      x = static_cast<float>(json["x"].toDouble());
      y = static_cast<float>(json["y"].toDouble());
      z = static_cast<float>(json["z"].toDouble());
      return true;
    }
    return false;
  }
} FloatVec3_t;

typedef struct {
  float a; float b; float c; float d;

  void writeJson(QJsonObject &json)
  {
    json["a"] = a;
    json["b"] = b;
    json["c"] = c;
    json["d"] = d;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["a"].isDouble() && json["b"].isDouble() && json["c"].isDouble() && json["d"].isDouble())
    {
      a = static_cast<float>(json["a"].toDouble());
      b = static_cast<float>(json["b"].toDouble());
      c = static_cast<float>(json["c"].toDouble());
      d = static_cast<float>(json["d"].toDouble());
      return true;
    }
    return false;
  }
} FloatVec4_t;

typedef struct
{
  float v11;
  float v12;
  float v13;
  float v14;
  float v15;
  float v16;
  float v22;
  float v23;
  float v24;
  float v25;
  float v26;
  float v33;
  float v34;
  float v35;
  float v36;
  float v44;
  float v45;
  float v46;
  float v55;
  float v56;
  float v66;

  void writeJson(QJsonObject &json)
  {
    json["v11"] = v11;
    json["v12"] = v12;
    json["v13"] = v13;
    json["v14"] = v14;
    json["v15"] = v15;
    json["v16"] = v16;
    json["v22"] = v22;
    json["v23"] = v23;
    json["v24"] = v24;
    json["v25"] = v25;
    json["v26"] = v26;
    json["v33"] = v33;
    json["v34"] = v34;
    json["v35"] = v35;
    json["v36"] = v36;
    json["v44"] = v44;
    json["v45"] = v45;
    json["v46"] = v46;
    json["v55"] = v55;
    json["v56"] = v56;
    json["v66"] = v66;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["v11"].isDouble() && json["v12"].isDouble() && json["v13"].isDouble() && json["v14"].isDouble()
      && json["v15"].isDouble() && json["v16"].isDouble() && json["v22"].isDouble() && json["v23"].isDouble()
      && json["v24"].isDouble() && json["v25"].isDouble() && json["v26"].isDouble() && json["v33"].isDouble()
      && json["v34"].isDouble() && json["v35"].isDouble() && json["v36"].isDouble() && json["v44"].isDouble()
      && json["v45"].isDouble() && json["v46"].isDouble() && json["v55"].isDouble() && json["v56"].isDouble()
      && json["v66"].isDouble())
    {
      v11 = static_cast<float>(json["v11"].toDouble());
      v12 = static_cast<float>(json["v12"].toDouble());
      v13 = static_cast<float>(json["v13"].toDouble());
      v14 = static_cast<float>(json["v14"].toDouble());
      v15 = static_cast<float>(json["v15"].toDouble());
      v16 = static_cast<float>(json["v16"].toDouble());
      v22 = static_cast<float>(json["v22"].toDouble());
      v23 = static_cast<float>(json["v23"].toDouble());
      v24 = static_cast<float>(json["v24"].toDouble());
      v25 = static_cast<float>(json["v25"].toDouble());
      v26 = static_cast<float>(json["v26"].toDouble());
      v33 = static_cast<float>(json["v33"].toDouble());
      v34 = static_cast<float>(json["v34"].toDouble());
      v35 = static_cast<float>(json["v35"].toDouble());
      v36 = static_cast<float>(json["v36"].toDouble());
      v44 = static_cast<float>(json["v44"].toDouble());
      v45 = static_cast<float>(json["v45"].toDouble());
      v46 = static_cast<float>(json["v46"].toDouble());
      v55 = static_cast<float>(json["v55"].toDouble());
      v56 = static_cast<float>(json["v56"].toDouble());
      v66 = static_cast<float>(json["v66"].toDouble());
      return true;
    }
    return false;
  }
} FloatVec21_t;

typedef struct
{
  float c20; float c02; float c11; float c10; float c01; float c00;

  void writeJson(QJsonObject &json)
  {
    json["c20"] = c20;
    json["c02"] = c02;
    json["c11"] = c11;
    json["c10"] = c10;
    json["c01"] = c01;
    json["c00"] = c00;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["c20"].isDouble() && json["c02"].isDouble() && json["c11"].isDouble() && json["c10"].isDouble()
      && json["c01"].isDouble() && json["c00"].isDouble())
    {
      c20 = static_cast<float>(json["c20"].toDouble());
      c02 = static_cast<float>(json["c02"].toDouble());
      c11 = static_cast<float>(json["c11"].toDouble());
      c10 = static_cast<float>(json["c10"].toDouble());
      c01 = static_cast<float>(json["c01"].toDouble());
      c00 = static_cast<float>(json["c00"].toDouble());
      return true;
    }
    return false;
  }
} Float2ndOrderPoly_t;

typedef struct
{
  float c30; float c03; float c21; float c12; float c20; float c02; float c11; float c10; float c01; float c00;

  void writeJson(QJsonObject &json)
  {
    json["c30"] = c30;
    json["c03"] = c03;
    json["c21"] = c21;
    json["c12"] = c12;
    json["c20"] = c20;
    json["c02"] = c02;
    json["c11"] = c11;
    json["c10"] = c10;
    json["c01"] = c01;
    json["c00"] = c00;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["c30"].isDouble() && json["c03"].isDouble() && json["c21"].isDouble() && json["c12"].isDouble()
      && json["c20"].isDouble() && json["c02"].isDouble() && json["c11"].isDouble() && json["c10"].isDouble()
      && json["c01"].isDouble() && json["c00"].isDouble())
    {
      c30 = static_cast<float>(json["c30"].toDouble());
      c03 = static_cast<float>(json["c03"].toDouble());
      c21 = static_cast<float>(json["c21"].toDouble());
      c12 = static_cast<float>(json["c12"].toDouble());
      c20 = static_cast<float>(json["c20"].toDouble());
      c02 = static_cast<float>(json["c02"].toDouble());
      c11 = static_cast<float>(json["c11"].toDouble());
      c10 = static_cast<float>(json["c10"].toDouble());
      c01 = static_cast<float>(json["c01"].toDouble());
      c00 = static_cast<float>(json["c00"].toDouble());
      return true;
    }
    return false;
  }
} Float3rdOrderPoly_t;

typedef struct
{
  float c40; float c04; float c31; float c13; float c22; float c30; float c03; float c21; float c12; float c20;
  float c02; float c11; float c10; float c01; float c00;

  void writeJson(QJsonObject &json)
  {
    json["c40"] = c40;
    json["c04"] = c04;
    json["c31"] = c31;
    json["c13"] = c13;
    json["c22"] = c22;
    json["c30"] = c30;
    json["c03"] = c03;
    json["c21"] = c21;
    json["c12"] = c12;
    json["c20"] = c20;
    json["c02"] = c02;
    json["c11"] = c11;
    json["c10"] = c10;
    json["c01"] = c01;
    json["c00"] = c00;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["c40"].isDouble() && json["c04"].isDouble() && json["c31"].isDouble() && json["c13"].isDouble()
      && json["c22"].isDouble() && json["c30"].isDouble() && json["c03"].isDouble() && json["c21"].isDouble()
      && json["c12"].isDouble() && json["c20"].isDouble() && json["c02"].isDouble() && json["c11"].isDouble()
      && json["c10"].isDouble() && json["c01"].isDouble() && json["c00"].isDouble())
    {
      c40 = static_cast<float>(json["c40"].toDouble());
      c04 = static_cast<float>(json["c04"].toDouble());
      c31 = static_cast<float>(json["c31"].toDouble());
      c13 = static_cast<float>(json["c13"].toDouble());
      c22 = static_cast<float>(json["c22"].toDouble());
      c30 = static_cast<float>(json["c30"].toDouble());
      c03 = static_cast<float>(json["c03"].toDouble());
      c21 = static_cast<float>(json["c21"].toDouble());
      c12 = static_cast<float>(json["c12"].toDouble());
      c20 = static_cast<float>(json["c20"].toDouble());
      c02 = static_cast<float>(json["c02"].toDouble());
      c11 = static_cast<float>(json["c11"].toDouble());
      c10 = static_cast<float>(json["c10"].toDouble());
      c01 = static_cast<float>(json["c01"].toDouble());
      c00 = static_cast<float>(json["c00"].toDouble());
      return true;
    }
    return false;
  }
} Float4thOrderPoly_t;

typedef struct
{
  qint32 PaddingDigits;
  quint32 Ordering;
  qint32 StartIndex;
  qint32 EndIndex;
  QString InputPath;
  QString FilePrefix;
  QString FileSuffix;
  QString FileExtension;

  void writeJson(QJsonObject &json)
  {
    json["PaddingDigits"] = static_cast<double>(PaddingDigits);
    json["Ordering"] = static_cast<double>(Ordering);
    json["StartIndex"] = static_cast<double>(StartIndex);
    json["EndIndex"] = static_cast<double>(EndIndex);
    json["InputPath"] = InputPath;
    json["FilePrefix"] = FilePrefix;
    json["FileSuffix"] = FileSuffix;
    json["FileExtension"] = FileExtension;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["PaddingDigits"].isDouble() && json["Ordering"].isDouble() && json["StartIndex"].isDouble() && json["EndIndex"].isDouble()
      && json["InputPath"].isString() && json["FilePrefix"].isString() && json["FileSuffix"].isString() && json["FileExtension"].isString())
    {
      PaddingDigits = static_cast<qint32>(json["PaddingDigits"].toDouble());
      Ordering = static_cast<quint32>(json["Ordering"].toDouble());
      StartIndex = static_cast<qint32>(json["StartIndex"].toDouble());
      EndIndex = static_cast<qint32>(json["EndIndex"].toDouble());
      InputPath = json["InputPath"].toString();
      FilePrefix = json["FilePrefix"].toString();
      FileSuffix = json["FileSuffix"].toString();
      FileExtension = json["FileExtension"].toString();
      return true;
    }
    return false;
  }
}
FileListInfo_t;


//typedef struct { float angle; float h; float k; float l; } AxisAngleInput_t;

Q_DECLARE_METATYPE(IntVec3_t)
Q_DECLARE_METATYPE(FloatVec3_t)
Q_DECLARE_METATYPE(FloatVec4_t)
Q_DECLARE_METATYPE(FloatVec21_t)
Q_DECLARE_METATYPE(Float2ndOrderPoly_t)
Q_DECLARE_METATYPE(Float3rdOrderPoly_t)
Q_DECLARE_METATYPE(Float4thOrderPoly_t)
Q_DECLARE_METATYPE(AxisAngleInput_t)
Q_DECLARE_METATYPE(FileListInfo_t)


/**
 * @class FilterParameter FilterParameter.h DREAM3DLib/FilterParameters/FilterParameter.h
 * @brief This class holds the various properties that an input parameter to a
 * filter needs to describe itself.
 *
 * @date Jan 17, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(FilterParameter)
    DREAM3D_TYPE_MACRO(FilterParameter)

    enum Category
    {
      Parameter = 0,
      RequiredArray = 1,
      CreatedArray = 2,
      Uncategorized = 3
    };

    virtual ~FilterParameter();

    DREAM3D_INSTANCE_STRING_PROPERTY(HumanLabel)
    DREAM3D_INSTANCE_STRING_PROPERTY(PropertyName)

    /**
     * @brief getWidgetType This is a pure virtual function. All subclasses need
     * to implement this function.
     * @return
     */
    virtual QString getWidgetType()  = 0;

    DREAM3D_VIRTUAL_INSTANCE_PROPERTY(QVariant, DefaultValue)
    DREAM3D_INSTANCE_PROPERTY(Category, Category)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadOnly)
    DREAM3D_INSTANCE_PROPERTY(int, GroupIndex)

  protected:
    FilterParameter();

  private:
    FilterParameter(const FilterParameter&); // Copy Constructor Not Implemented
    void operator=(const FilterParameter&); // Operator '=' Not Implemented
};

typedef QVector<FilterParameter::Pointer> FilterParameterVector;

#endif /* _FilterParameter_H_ */

