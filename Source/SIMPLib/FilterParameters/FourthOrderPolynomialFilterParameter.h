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

#ifndef _FourthOrderPolynomialFilterParameter_H_
#define _FourthOrderPolynomialFilterParameter_H_

#include <QtCore/QJsonObject>

#include "SIMPLib/FilterParameters/FilterParameter.h"

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

Q_DECLARE_METATYPE(Float4thOrderPoly_t)

class SIMPLib_EXPORT FourthOrderPolynomialFilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(FourthOrderPolynomialFilterParameter)
    SIMPL_STATIC_NEW_MACRO(FourthOrderPolynomialFilterParameter)
    SIMPL_TYPE_MACRO(FourthOrderPolynomialFilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
    const Float4thOrderPoly_t& defaultValue, Category category,
     int groupIndex = -1);

    virtual ~FourthOrderPolynomialFilterParameter();

  /**
   * @brief getWidgetType Returns the type of widget that displays and controls
   * this FilterParameter subclass
   * @return
   */
  QString getWidgetType();


protected:
  FourthOrderPolynomialFilterParameter();

private:
  FourthOrderPolynomialFilterParameter(const FourthOrderPolynomialFilterParameter&); // Copy Constructor Not Implemented
  void operator=(const FourthOrderPolynomialFilterParameter&); // Operator '=' Not Implemented
};

#endif /* _FourthOrderPolynomialFilterParameter_H_ */
