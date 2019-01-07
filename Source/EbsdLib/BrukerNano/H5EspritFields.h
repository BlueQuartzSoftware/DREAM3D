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

#include <QtCore/QString>
#include <QtCore/QVector>

#include "EbsdLib/AbstractEbsdFields.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"

#include "EbsdLib/BrukerNano/EspritConstants.h"

/**
 * @class EspritFields EspritFields.h EbsdLib/HKL/EspritFields.h
 * @brief This class simply holds the names of the columns that are present in the
 * HKL .Esprit file.
 *
 * @date Aug 18, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT H5EspritFields : public AbstractEbsdFields
{
public:
  H5EspritFields();
  ~H5EspritFields() override;
  EBSD_TYPE_MACRO_SUPER(H5EspritFields, AbstractEbsdFields)

  QVector<QString> getFieldNames() override;

  template <typename T> T getFilterFeatures()
  {
    T features;
    features.push_back(Ebsd::H5Esprit::DD);
    features.push_back(Ebsd::H5Esprit::MAD);
    features.push_back(Ebsd::H5Esprit::MADPhase);
    features.push_back(Ebsd::H5Esprit::NIndexedBands);
    features.push_back(Ebsd::H5Esprit::PCX);
    features.push_back(Ebsd::H5Esprit::PCY);
    features.push_back(Ebsd::H5Esprit::PHI);
    features.push_back(Ebsd::H5Esprit::Phase);
    features.push_back(Ebsd::H5Esprit::RadonBandCount);
    features.push_back(Ebsd::H5Esprit::RadonQuality);
    features.push_back(Ebsd::H5Esprit::RawPatterns);
    features.push_back(Ebsd::H5Esprit::XBEAM);
    features.push_back(Ebsd::H5Esprit::YBEAM);
    features.push_back(Ebsd::H5Esprit::XSAMPLE);
    features.push_back(Ebsd::H5Esprit::YSAMPLE);
    features.push_back(Ebsd::H5Esprit::phi1);
    features.push_back(Ebsd::H5Esprit::phi2);
    return features;
  }

public:
  H5EspritFields(const H5EspritFields&) = delete;            // Copy Constructor Not Implemented
  H5EspritFields(H5EspritFields&&) = delete;                 // Move Constructor Not Implemented
  H5EspritFields& operator=(const H5EspritFields&) = delete; // Copy Assignment Not Implemented
  H5EspritFields& operator=(H5EspritFields&&) = delete;      // Move Assignment Not Implemented
};
