/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
* BlueQuartz Software nor the names of its contributors may be used to endorse
* or promote products derived from this software without specific prior written
* permission.
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
*  This code was written under United States Air Force Contract number
*                           FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _SecondOrderPolynomialWidgetCodeGenerator_H_
#define _SecondOrderPolynomialWidgetCodeGenerator_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "DevHelper/CodeGenerators/FPCodeGenerator.h"

class SecondOrderPolynomialWidgetCodeGenerator : public FPCodeGenerator
{
  public:
    SIMPL_SHARED_POINTERS(SecondOrderPolynomialWidgetCodeGenerator)

    static Pointer New(QString humanLabel, QString propertyName, QString category, QString initValue)
    {
      Pointer sharedPtr(new SecondOrderPolynomialWidgetCodeGenerator(humanLabel, propertyName, category, initValue));
      return sharedPtr;
    }

    virtual ~SecondOrderPolynomialWidgetCodeGenerator();

    virtual QString generateSetupFilterParameters();

    virtual QString generateReadFilterParameters();

    virtual QString generateDataCheck();

    virtual QString generateFilterParameters();

    virtual QString generateHIncludes();

  protected:
    SecondOrderPolynomialWidgetCodeGenerator(QString humanLabel, QString propertyName, QString category, QString initValue);

  private:

    SecondOrderPolynomialWidgetCodeGenerator(const SecondOrderPolynomialWidgetCodeGenerator&); // Copy Constructor Not Implemented
    void operator=(const SecondOrderPolynomialWidgetCodeGenerator&); // Operator '=' Not Implemented
};

#endif /* SecondOrderPolynomialWidgetCodeGenerator_H_ */
