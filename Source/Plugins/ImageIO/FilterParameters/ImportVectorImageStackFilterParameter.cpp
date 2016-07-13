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

#include "ImportVectorImageStackFilterParameter.h"

#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportVectorImageStackFilterParameter::ImportVectorImageStackFilterParameter() :
FilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportVectorImageStackFilterParameter::~ImportVectorImageStackFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportVectorImageStackFilterParameter::Pointer ImportVectorImageStackFilterParameter::New(const QString& humanLabel, const QString& propertyName,
  const QVariant& defaultValue, Category category, ImportVectorImageStack *filter, int groupIndex)
{

  ImportVectorImageStackFilterParameter::Pointer ptr = ImportVectorImageStackFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(defaultValue);
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setFilter(filter);

  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportVectorImageStackFilterParameter::getWidgetType()
{
  return QString("ImportVectorImageStackWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackFilterParameter::readJson(const QJsonObject &json)
{
  m_Filter->setStartIndex(json["StartIndex"].toString().toLongLong());
  m_Filter->setEndIndex(json["EndIndex"].toString().toLongLong());
  m_Filter->setStartComp(json["StartComp"].toString().toLongLong());
  m_Filter->setEndComp(json["EndComp"].toString().toLongLong());
  m_Filter->setPaddingDigits(json["PaddingDigits"].toInt());
  m_Filter->setRefFrameZDir(static_cast<uint32_t>(json["RefFrameZDir"].toInt()));
  m_Filter->setInputPath(json["InputPath"].toString());
  m_Filter->setFilePrefix(json["FilePrefix"].toString());
  m_Filter->setSeparator(json["Separator"].toString());
  m_Filter->setFileSuffix(json["FileSuffix"].toString());
  m_Filter->setFileExtension(json["FileExtension"].toString());

  QJsonObject originObject = json["Origin"].toObject();
  FloatVec3_t origin;
  origin.readJson(originObject);
  m_Filter->setOrigin(origin);

  QJsonObject resolutionObject = json["Resolution"].toObject();
  FloatVec3_t resolution;
  resolution.readJson(resolutionObject);
  m_Filter->setResolution(resolution);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStackFilterParameter::writeJson(QJsonObject &json)
{
  json["StartIndex"] = QString::number(m_Filter->getStartIndex());
  json["EndIndex"] = QString::number(m_Filter->getEndIndex());
  json["StartComp"] = QString::number(m_Filter->getStartComp());
  json["EndComp"] = QString::number(m_Filter->getEndComp());
  json["PaddingDigits"] = m_Filter->getPaddingDigits();
  json["RefFrameZDir"] = static_cast<int>(m_Filter->getRefFrameZDir());
  json["InputPath"] = m_Filter->getInputPath();
  json["FilePrefix"] = m_Filter->getFilePrefix();
  json["Separator"] = m_Filter->getSeparator();
  json["FileSuffix"] = m_Filter->getFileSuffix();
  json["FileExtension"] = m_Filter->getFileExtension();

  FloatVec3_t origin = m_Filter->getOrigin();
  QJsonObject originObj;
  origin.writeJson(originObj);
  json["Origin"] = originObj;

  FloatVec3_t resolution = m_Filter->getResolution();
  QJsonObject resolutionObj;
  resolution.writeJson(resolutionObj);
  json["Resolution"] = resolutionObj;
}

