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

#ifndef _FPCodeGenerator_H_
#define _FPCodeGenerator_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

// stringised version of line number (must be done in two steps)
#define STRINGISE(N) #N
#define EXPAND_THEN_STRINGISE(N) STRINGISE(N)
#define __LINE_STR__ EXPAND_THEN_STRINGISE(__LINE__)

// MSVC-suitable routines for formatting <#pragma message>
#define __LOC__ __FILE__ "(" __LINE_STR__ ")"
#define __OUTPUT_FORMAT__(type) __LOC__ " : " type " : "

// specific message types for <#pragma message>
#define __WARN__ __OUTPUT_FORMAT__("warning")
#define __ERR__ __OUTPUT_FORMAT__("error")
#define __MSG__ __OUTPUT_FORMAT__("programmer's message")
#define __TODO__ __OUTPUT_FORMAT__("to do")

class FPCodeGenerator
{
  public:
    SIMPL_SHARED_POINTERS(FPCodeGenerator)

    static Pointer New(QString humanLabel, QString propertyName, QString category, QString initValue)
    {
      Pointer sharedPtr(new FPCodeGenerator(humanLabel, propertyName, category, initValue));
      return sharedPtr;
    }

    virtual ~FPCodeGenerator();

    virtual QString generateSetupFilterParameters();

    virtual QString generateReadFilterParameters();

    virtual QString generateWriteFilterParameters();

    virtual QString generateDataCheck();

    virtual QString generateFilterParameters();

    virtual QString generateInitializationList();

    virtual QString generateHIncludes();

    virtual QString generateCPPIncludes();

  protected:
    FPCodeGenerator(QString humanLabel, QString propertyName, QString category, QString initValue);

    QString getPropertyName();
    QString getHumanLabel();
    QString getCategory();
    QString getInitValue();

  private:
    QString m_PropertyName;
    QString m_HumanLabel;
    QString m_Category;
    QString m_InitValue;

    FPCodeGenerator(const FPCodeGenerator&); // Copy Constructor Not Implemented
    void operator=(const FPCodeGenerator&); // Operator '=' Not Implemented
};

#endif /* FPCodeGenerator_H_ */