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

#ifndef _Symmetric6x6FilterParameter_H_
#define _Symmetric6x6FilterParameter_H_

#include "SIMPLib/FilterParameters/FilterParameter.h"

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

Q_DECLARE_METATYPE(FloatVec21_t)

class SIMPLib_EXPORT Symmetric6x6FilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(Symmetric6x6FilterParameter)
    SIMPL_STATIC_NEW_MACRO(Symmetric6x6FilterParameter)
    SIMPL_TYPE_MACRO(Symmetric6x6FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
    const FloatVec21_t& defaultValue, Category category,
     int groupIndex = -1);

    virtual ~Symmetric6x6FilterParameter();

  /**
   * @brief getWidgetType Returns the type of widget that displays and controls
   * this FilterParameter subclass
   * @return
   */
  QString getWidgetType();


protected:
  Symmetric6x6FilterParameter();

private:
  Symmetric6x6FilterParameter(const Symmetric6x6FilterParameter&); // Copy Constructor Not Implemented
  void operator=(const Symmetric6x6FilterParameter&); // Operator '=' Not Implemented
};

#endif /* _Symmetric6x6FilterParameter_H_ */
