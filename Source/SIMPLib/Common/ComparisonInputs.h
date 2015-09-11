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

#ifndef _ComparisonInputs_H_
#define _ComparisonInputs_H_

#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include <QtCore/QJsonObject>

#include "SIMPLib/SIMPLib.h"

typedef struct
{
  QString dataContainerName;
  QString attributeMatrixName;
  QString attributeArrayName;
  int compOperator;
  double compValue;

  void writeJson(QJsonObject& json)
  {
    json["Data Container Name"] = dataContainerName;
    json["Attribute Matrix Name"] = attributeMatrixName;
    json["Attribute Array Name"] = attributeArrayName;
    json["Comparison Operator"] = compOperator;
    json["Comparison Value"] = compValue;
  }

  bool readJson(QJsonObject& json)
  {
    if (json["Data Container Name"].isString() && json["Attribute Matrix Name"].isString() && json["Attribute Array Name"].isString()
        && json["Comparison Operator"].isDouble() && json["Comparison Value"].isDouble())
    {
      dataContainerName = json["Data Container Name"].toString();
      attributeMatrixName = json["Attribute Matrix Name"].toString();
      attributeArrayName = json["Attribute Array Name"].toString();
      compOperator = json["Comparison Operator"].toInt();
      compValue = json["Comparison Value"].toDouble();
      return true;
    }
    return false;
  }

} ComparisonInput_t;

/**
 * @class ComparisonInputs ComparisonInputs.h DREAM3DLib/Common/ComparisonInputs.h
 * @brief  This class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Feb 18, 2014
 * @version 1.0
 */
class SIMPLib_EXPORT ComparisonInputs : public QObject
{
    Q_OBJECT

  public:
    ComparisonInputs();
    ComparisonInputs(const ComparisonInputs& rhs);
    //explicit ComparisonInputs(ComparisonInputs& rhs);

    virtual ~ComparisonInputs();

    int size();

    void addInput(const QString dataContainerName,
                  const QString attributeMatrixName,
                  const QString arrayName,
                  int compOperator,
                  double compValue);
    void addInput(const ComparisonInput_t& input);

    ComparisonInput_t& getInput(int index);
    QVector<ComparisonInput_t>& getInputs();

    void operator=(const ComparisonInputs&);

    ComparisonInput_t& operator[](int index);

  private:

    QVector<ComparisonInput_t> m_Inputs;


};


Q_DECLARE_METATYPE(ComparisonInput_t)
Q_DECLARE_METATYPE(ComparisonInputs)


#endif /* _ComparisonInputs_H_ */
