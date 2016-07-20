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

#ifndef _ImportVectorImageStackFilterParameter_H_
#define _ImportVectorImageStackFilterParameter_H_

#include "SIMPLib/FilterParameters/FilterParameter.h"

class ReadEdaxH5DataFilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(ReadEdaxH5DataFilterParameter)
    SIMPL_STATIC_NEW_MACRO(ReadEdaxH5DataFilterParameter)
    SIMPL_TYPE_MACRO(ReadEdaxH5DataFilterParameter)

  typedef std::function<void(QStringList)> SetterCallbackType;
  typedef std::function<QStringList(void)> GetterCallbackType;

  static Pointer New(const QString& humanLabel, const QString& propertyName,
                     const QVariant& defaultValue, const QString &listProperty,
                     Category category, SetterCallbackType setterCallback,
                     GetterCallbackType getterCallback, int groupIndex = -1);

  virtual ~ReadEdaxH5DataFilterParameter();

  QString getWidgetType();

  /**
   * @brief readJson
   * @return
   */
  void readJson(const QJsonObject &json);

  /**
   * @brief writeJson
   * @return
   */
  void writeJson(QJsonObject &json);

  SIMPL_INSTANCE_PROPERTY(SetterCallbackType, SetterCallback)
  SIMPL_INSTANCE_PROPERTY(GetterCallbackType, GetterCallback)

  SIMPL_INSTANCE_PROPERTY(QString, ListProperty)

protected:
  ReadEdaxH5DataFilterParameter();

private:
  ReadEdaxH5DataFilterParameter(const ReadEdaxH5DataFilterParameter&); // Copy Constructor Not Implemented
  void operator=(const ReadEdaxH5DataFilterParameter&); // Operator '=' Not Implemented
};

#endif /* _ImportVectorImageStackFilterParameter_H_ */
