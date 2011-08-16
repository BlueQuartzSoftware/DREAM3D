/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef ANGFIELDS_H_
#define ANGFIELDS_H_

#include <QtCore/QStringList>



/*
 *
 */
class AngFilterFields
{
  public:
    AngFilterFields(){};
    virtual ~AngFilterFields(){};

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    QStringList getFieldNames()
    {
      QStringList fields;

      fields.push_back(Ebsd::Ang::ImageQuality.c_str());
      fields.push_back(Ebsd::Ang::ConfidenceIndex.c_str());
      fields.push_back(Ebsd::Ang::PhaseData.c_str());

      fields.push_back(Ebsd::Ang::SEMSignal.c_str());
      fields.push_back(Ebsd::Ang::Fit.c_str());

      return fields;
    }

  private:
    AngFilterFields(const AngFilterFields&); // Copy Constructor Not Implemented
    void operator=(const AngFilterFields&); // Operator '=' Not Implemented
};

#endif /* ANGFIELDS_H_ */
