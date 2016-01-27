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

#ifndef _FloatVec3FilterParameter_H_
#define _FloatVec3FilterParameter_H_

#include <QtCore/QJsonObject>

#include "SIMPLib/FilterParameters/FilterParameter.h"

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

Q_DECLARE_METATYPE(FloatVec3_t)

class SIMPLib_EXPORT FloatVec3FilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(FloatVec3FilterParameter)
    SIMPL_STATIC_NEW_MACRO(FloatVec3FilterParameter)
    SIMPL_TYPE_MACRO(FloatVec3FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
    const FloatVec3_t& defaultValue, Category category,
     int groupIndex = -1);

    virtual ~FloatVec3FilterParameter();

  /**
   * @brief getWidgetType Returns the type of widget that displays and controls
   * this FilterParameter subclass
   * @return
   */
  QString getWidgetType();


protected:
  FloatVec3FilterParameter();

private:
  FloatVec3FilterParameter(const FloatVec3FilterParameter&); // Copy Constructor Not Implemented
  void operator=(const FloatVec3FilterParameter&); // Operator '=' Not Implemented
};

#endif /* _FloatVec3FilterParameter_H_ */
