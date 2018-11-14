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

#include "ReadH5EbsdFilterParameter.h"

#include <QtCore/QJsonArray>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5EbsdFilterParameter::ReadH5EbsdFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5EbsdFilterParameter::~ReadH5EbsdFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5EbsdFilterParameter::Pointer ReadH5EbsdFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QVariant& defaultValue, Category category, ReadH5Ebsd* filter,
                                                                  const QString& fileExtension, const QString& fileType, int groupIndex)
{
  ReadH5EbsdFilterParameter::Pointer ptr = ReadH5EbsdFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(defaultValue);
  ptr->setCategory(category);
  ptr->setFileExtension(fileExtension);
  ptr->setFileType(fileType);
  ptr->setGroupIndex(groupIndex);
  ptr->setFilter(filter);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadH5EbsdFilterParameter::getWidgetType() const
{
  return QString("ReadH5EbsdWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdFilterParameter::readJson(const QJsonObject& json)
{
  m_Filter->setInputFile(json["InputFile"].toString());
  m_Filter->setZStartIndex(json["ZStartIndex"].toInt());
  m_Filter->setZEndIndex(json["ZEndIndex"].toInt());
  m_Filter->setUseTransformations(static_cast<bool>(json["UseTransformations"].toInt()));
  m_Filter->setAngleRepresentation(json["AngleRepresentation"].toInt());
  m_Filter->setRefFrameZDir(static_cast<uint32_t>(json["RefFrameZDir"].toInt()));

  QSet<QString> selectedArrayNames;
  QJsonArray selectedArrayNamesObj = json["SelectedArrayNames"].toArray();
  for(int i = 0; i < selectedArrayNamesObj.size(); i++)
  {
    selectedArrayNames.insert(selectedArrayNamesObj[i].toString());
  }
  m_Filter->setSelectedArrayNames(selectedArrayNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5EbsdFilterParameter::writeJson(QJsonObject& json)
{
  json["InputFile"] = m_Filter->getInputFile();
  json["ZStartIndex"] = m_Filter->getZStartIndex();
  json["ZEndIndex"] = m_Filter->getZEndIndex();
  json["UseTransformations"] = static_cast<int>(m_Filter->getUseTransformations());
  json["AngleRepresentation"] = m_Filter->getAngleRepresentation();
  json["RefFrameZDir"] = static_cast<int>(m_Filter->getRefFrameZDir());

  QJsonArray selectedArrayNamesObj;
  QSetIterator<QString> iter(m_Filter->getSelectedArrayNames());
  while(iter.hasNext())
  {
    QString selectedArrayName = iter.next();
    selectedArrayNamesObj.push_back(selectedArrayName);
  }
  json["SelectedArrayNames"] = selectedArrayNamesObj;
}
