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

#include "EbsdToH5EbsdFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdToH5EbsdFilterParameter::EbsdToH5EbsdFilterParameter()
: FilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdToH5EbsdFilterParameter::~EbsdToH5EbsdFilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdToH5EbsdFilterParameter::Pointer EbsdToH5EbsdFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QVariant& defaultValue, Category category, EbsdToH5Ebsd* filter,
                                                                      const QString& fileExtension, const QString& fileType, int groupIndex)
{
  EbsdToH5EbsdFilterParameter::Pointer ptr = EbsdToH5EbsdFilterParameter::New();
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
QString EbsdToH5EbsdFilterParameter::getWidgetType()
{
  return QString("EbsdToH5EbsdWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdFilterParameter::readJson(const QJsonObject& json)
{
  m_Filter->setOutputFile(json["OutputFile"].toString());
  m_Filter->setZStartIndex(static_cast<int64_t>(std::stoi(json["ZStartIndex"].toString().toStdString())));
  m_Filter->setZEndIndex(static_cast<int64_t>(std::stoi(json["ZEndIndex"].toString().toStdString())));
  m_Filter->setZResolution(static_cast<float>(json["ZResolution"].toDouble()));
  m_Filter->setRefFrameZDir(static_cast<uint32_t>(json["RefFrameZDir"].toInt()));
  m_Filter->setInputPath(json["InputPath"].toString());
  m_Filter->setFilePrefix(json["FilePrefix"].toString());
  m_Filter->setFileSuffix(json["FileSuffix"].toString());
  m_Filter->setFileExtension(json["FileExtension"].toString());
  m_Filter->setPaddingDigits(json["PaddingDigits"].toInt());

  QJsonObject sampleTransObj = json["SampleTransformation"].toObject();
  AxisAngleInput_t sampleTrans;
  sampleTrans.readJson(sampleTransObj);
  m_Filter->setSampleTransformation(sampleTrans);

  QJsonObject eulerObj = json["EulerTransformation"].toObject();
  AxisAngleInput_t euler;
  euler.readJson(eulerObj);
  m_Filter->setEulerTransformation(euler);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdToH5EbsdFilterParameter::writeJson(QJsonObject& json)
{
  json["OutputFile"] = m_Filter->getOutputFile();
  json["ZStartIndex"] = QString::number(m_Filter->getZStartIndex());
  json["ZEndIndex"] = QString::number(m_Filter->getZEndIndex());
  json["ZResolution"] = static_cast<double>(m_Filter->getZResolution());
  json["RefFrameZDir"] = static_cast<int>(m_Filter->getRefFrameZDir());
  json["InputPath"] = m_Filter->getInputPath();
  json["FilePrefix"] = m_Filter->getFilePrefix();
  json["FileSuffix"] = m_Filter->getFileSuffix();
  json["FileExtension"] = m_Filter->getFileExtension();
  json["PaddingDigits"] = m_Filter->getPaddingDigits();

  QJsonObject sampleTransObj;
  AxisAngleInput_t sampleTrans = m_Filter->getSampleTransformation();
  sampleTrans.writeJson(sampleTransObj);
  json["SampleTransformation"] = sampleTransObj;

  QJsonObject eulerObj;
  AxisAngleInput_t euler = m_Filter->getEulerTransformation();
  euler.writeJson(eulerObj);
  json["EulerTransformation"] = eulerObj;
}
