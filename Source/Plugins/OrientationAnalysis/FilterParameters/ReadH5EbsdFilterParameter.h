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

#pragma once

#include <memory>

#include "SIMPLib/FilterParameters/FilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

class ReadH5Ebsd;

/**
 * @brief The ReadH5EbsdFilterParameter class
 */
class OrientationAnalysis_EXPORT ReadH5EbsdFilterParameter : public FilterParameter
{
public:
  using Self = ReadH5EbsdFilterParameter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for ReadH5EbsdFilterParameter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ReadH5EbsdFilterParameter
   */
  static QString ClassName();

  static Pointer New(const QString& humanLabel, const QString& propertyName, const QVariant& defaultValue, Category category, ReadH5Ebsd* filter, const QString& fileExtension = QString(""),
                     const QString& fileType = QString(""), int groupIndex = -1);

  ~ReadH5EbsdFilterParameter() override;

  /**
   * @brief Setter property for FileExtension
   */
  void setFileExtension(const QString& value);
  /**
   * @brief Getter property for FileExtension
   * @return Value of FileExtension
   */
  QString getFileExtension() const;

  /**
   * @brief Setter property for FileType
   */
  void setFileType(const QString& value);
  /**
   * @brief Getter property for FileType
   * @return Value of FileType
   */
  QString getFileType() const;

  /**
   * @brief setFilter
   * @param value
   */
  void setFilter(ReadH5Ebsd* value);

  /**
   * @brief getFilter
   * @return
   */
  ReadH5Ebsd* getFilter() const;

  /**
   * @brief getWidgetType
   * @return
   */
  QString getWidgetType() const override;
  
  /**
 * @brief readJson
 * @return
 */
  void readJson(const QJsonObject& json) override;
  
  /**
 * @brief writeJson
 * @return
 */
  void writeJson(QJsonObject& json) override;

protected:
  ReadH5EbsdFilterParameter();

public:
  ReadH5EbsdFilterParameter(const ReadH5EbsdFilterParameter&) = delete; // Copy Constructor Not Implemented
  ReadH5EbsdFilterParameter(ReadH5EbsdFilterParameter&&) = delete;      // Move Constructor Not Implemented
  ReadH5EbsdFilterParameter& operator=(const ReadH5EbsdFilterParameter&) = delete; // Copy Assignment Not Implemented
  ReadH5EbsdFilterParameter& operator=(ReadH5EbsdFilterParameter&&) = delete;      // Move Assignment Not Implemented

private:
  ReadH5Ebsd* m_Filter = nullptr;

  QString m_FileExtension = {};
  QString m_FileType = {};
};

