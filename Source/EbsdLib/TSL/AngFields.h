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


#ifndef _angfields_h_
#define _angfields_h_

#include <QtCore/QString>
#include <QtCore/QVector>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/AbstractEbsdFields.h"
#include "EbsdLib/TSL/AngConstants.h"

/**
 * @class AngFields AngFields.h EbsdLib/TSL/AngFields.h
 * @brief This class simply holds the names of the columns that are present in the
 * TSL .ang file.
 *
 * @date Aug 18, 2011
 * @version 1.0
 */
class EbsdLib_EXPORT AngFields : public AbstractEbsdFields
{
  public:
    AngFields();
    virtual ~AngFields();

    virtual QVector<QString> getFieldNames();

    template<typename T>
    T getFilterFeatures()
    {
      T features;

      features.push_back(Ebsd::Ang::ImageQuality);
      features.push_back(Ebsd::Ang::ConfidenceIndex);

      features.push_back(Ebsd::Ang::SEMSignal);
      features.push_back(Ebsd::Ang::Fit);
      features.push_back(Ebsd::Ang::XPosition);
      features.push_back(Ebsd::Ang::YPosition);
      return features;
    }


  private:
    AngFields(const AngFields&); // Copy Constructor Not Implemented
    void operator=(const AngFields&); // Operator '=' Not Implemented
};



#endif /* ANGFIELDS_H_ */

