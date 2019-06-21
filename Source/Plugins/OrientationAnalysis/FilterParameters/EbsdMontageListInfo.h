/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"


class OrientationAnalysis_EXPORT EbsdMontageListInfo
{
public:
  EbsdMontageListInfo();
  ~EbsdMontageListInfo();

  qint32 PaddingDigits = 3;
  quint32 Ordering = 0; /* Ordering=0 = RowColumn, Ordering=1 = ColumnRow */
  qint32 RowStart = 0;
  qint32 RowEnd = 2;
  qint32 ColStart = 0;
  qint32 ColEnd = 2;
  qint32 IncrementIndex = 1;
  QString InputPath;
  QString FilePrefix;
  QString FileSuffix;
  QString FileExtension;

  /**
   * @brief writeJson
   * @param json
   */
  void writeJson(QJsonObject& json) const;

  /**
   * @brief readJson
   * @param json
   * @return
   */
  bool readJson(QJsonObject& json);

public:
  EbsdMontageListInfo(const EbsdMontageListInfo&) = default;            // Copy Constructor Not Implemented
  EbsdMontageListInfo(EbsdMontageListInfo&&) = default;                 // Move Constructor Not Implemented
  EbsdMontageListInfo& operator=(const EbsdMontageListInfo&) = default; // Copy Assignment Not Implemented
  EbsdMontageListInfo& operator=(EbsdMontageListInfo&&) = default;      // Move Assignment Not Implemented
};

Q_DECLARE_METATYPE(EbsdMontageListInfo)
