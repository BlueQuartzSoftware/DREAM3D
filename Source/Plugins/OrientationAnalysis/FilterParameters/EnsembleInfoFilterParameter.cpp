/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "EnsembleInfoFilterParameter.h"

#include <QtCore/QJsonArray>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoFilterParameter::EnsembleInfoFilterParameter()
: m_ShowOperators(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoFilterParameter::~EnsembleInfoFilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleInfoFilterParameter::Pointer EnsembleInfoFilterParameter::New(const QString& humanLabel, const QString& propertyName, EnsembleInfo defaultValue, Category category,
                                                                      SetterCallbackType setterCallback, GetterCallbackType getterCallback, QVector<QString> choices, bool showOperators,
                                                                      int groupIndex)
{
  EnsembleInfoFilterParameter::Pointer ptr = EnsembleInfoFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant var;
  var.setValue(defaultValue);
  ptr->setDefaultValue(var);
  ptr->setCategory(category);
  ptr->setChoices(choices);
  ptr->setShowOperators(showOperators);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnsembleInfoFilterParameter::getWidgetType()
{
  return QString("EnsembleInfoCreationWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonArray jsonArray = jsonValue.toArray();

    EnsembleInfo inputs;
    for(int i = 0; i < jsonArray.size(); i++)
    {
      QJsonObject ensembleObj = jsonArray[i].toObject();

      if(ensembleObj["CrystalStructure"].isDouble() && ensembleObj["PhaseType"].isDouble() && ensembleObj["PhaseName"].isString())
      {
        EnsembleInfo::CrystalStructureType crystalStructure = ensembleObj["CrystalStructure"].toInt();
        PhaseType::EnumType phaseType = ensembleObj["PhaseType"].toInt();
        QString phaseName = ensembleObj["PhaseName"].toString();

        inputs.addValues(static_cast<EnsembleInfo::CrystalStructure>(crystalStructure), static_cast<PhaseType::Type>(phaseType), phaseName);
      }
    }

    m_SetterCallback(inputs);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleInfoFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    QJsonArray inputsArray;

    EnsembleInfo inputs = m_GetterCallback();
    for(int i = 0; i < inputs.size(); i++)
    {
      EnsembleInfo::CrystalStructure crystalStructure;
      PhaseType::Type phaseType;
      QString phaseName;

      inputs.getValues(i, crystalStructure, phaseType, phaseName);

      QJsonObject obj;
      obj["CrystalStructure"] = static_cast<int>(crystalStructure);
      obj["PhaseType"] = static_cast<int>(phaseType);
      obj["PhaseName"] = phaseName;

      inputsArray.push_back(obj);
    }

    json[getPropertyName()] = inputsArray;
  }
}
