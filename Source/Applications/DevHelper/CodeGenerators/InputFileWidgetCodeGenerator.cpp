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

#include "InputFileWidgetCodeGenerator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputFileWidgetCodeGenerator::InputFileWidgetCodeGenerator(QString humanLabel, QString propertyName, QString category, QString initValue) :
  FPCodeGenerator(humanLabel, propertyName, category, initValue)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputFileWidgetCodeGenerator::~InputFileWidgetCodeGenerator()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InputFileWidgetCodeGenerator::generateSetupFilterParameters()
{
  return "  parameters.push_back(InputFileFilterParameter::New(\"" + getHumanLabel() + "\", \"" + getPropertyName() + "\", get" + getPropertyName() + "(), " + getCategory() + ", \"\", \"*.raw *.bin\"));";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InputFileWidgetCodeGenerator::generateReadFilterParameters()
{
  return "  set" + getPropertyName() + "(reader->readString(\"" + getPropertyName() + "\", get" + getPropertyName() + "()));";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InputFileWidgetCodeGenerator::generateDataCheck()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InputFileWidgetCodeGenerator::generateFilterParameters()
{
  QString contents;
  QTextStream ss(&contents);
  ss << "    SIMPL_FILTER_PARAMETER(QString, " + getPropertyName() + ")\n";
  ss << "    Q_PROPERTY(QString " + getPropertyName() + " READ get" + getPropertyName() + " WRITE set" + getPropertyName() + ")";

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString InputFileWidgetCodeGenerator::generateCPPIncludes()
{
  return "#include \"DREAM3DLib/FilterParameters/InputFileFilterParameter.h\"";
}
