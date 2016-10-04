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

#include "ConvertHexGridToSquareGridFilterParameter.h"

#include <QtCore/QJsonObject>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertHexGridToSquareGridFilterParameter::ConvertHexGridToSquareGridFilterParameter()
: FilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertHexGridToSquareGridFilterParameter::~ConvertHexGridToSquareGridFilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertHexGridToSquareGridFilterParameter::Pointer ConvertHexGridToSquareGridFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QVariant& defaultValue,
                                                                                                  Category category, ConvertHexGridToSquareGrid* filter, const QString& fileExtension,
                                                                                                  const QString& fileType, int groupIndex)
{
  ConvertHexGridToSquareGridFilterParameter::Pointer ptr = ConvertHexGridToSquareGridFilterParameter::New();
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
QString ConvertHexGridToSquareGridFilterParameter::getWidgetType()
{
  return QString("ConvertHexGridToSquareGridWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridFilterParameter::readJson(const QJsonObject& json)
{
  m_Filter->setFileExtension(json["FileExtension"].toString());
  m_Filter->setFilePrefix(json["FilePrefix"].toString());
  m_Filter->setFileSuffix(json["FileSuffix"].toString());
  m_Filter->setInputPath(json["InputPath"].toString());
  m_Filter->setOutputPath(json["OutputPath"].toString());
  m_Filter->setOutputPrefix(json["OutputPrefix"].toString());
  m_Filter->setPaddingDigits(json["PaddingDigits"].toInt());
  m_Filter->setXResolution(static_cast<float>(json["XResolution"].toDouble()));
  m_Filter->setYResolution(static_cast<float>(json["YResolution"].toDouble()));
  m_Filter->setZEndIndex(json["ZEndIndex"].toString().toLongLong());
  m_Filter->setZStartIndex(json["ZStartIndex"].toString().toLongLong());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGridFilterParameter::writeJson(QJsonObject& json)
{
  json["FileExtension"] = m_Filter->getFileExtension();
  json["FilePrefix"] = m_Filter->getFilePrefix();
  json["FileSuffix"] = m_Filter->getFileSuffix();
  json["InputPath"] = m_Filter->getInputPath();
  json["OutputPath"] = m_Filter->getOutputPath();
  json["OutputPrefix"] = m_Filter->getOutputPrefix();
  json["PaddingDigits"] = m_Filter->getPaddingDigits();
  json["XResolution"] = static_cast<double>(m_Filter->getXResolution());
  json["YResolution"] = static_cast<double>(m_Filter->getYResolution());
  json["ZEndIndex"] = QString::number(m_Filter->getZEndIndex());
  json["ZStartIndex"] = QString::number(m_Filter->getZStartIndex());
}
