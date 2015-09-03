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

#include "Symmetric6x6WidgetCodeGenerator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Symmetric6x6WidgetCodeGenerator::Symmetric6x6WidgetCodeGenerator(QString humanLabel, QString propertyName, QString category, QString initValue) :
  FPCodeGenerator(humanLabel, propertyName, category, initValue)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Symmetric6x6WidgetCodeGenerator::~Symmetric6x6WidgetCodeGenerator()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Symmetric6x6WidgetCodeGenerator::generateSetupFilterParameters()
{
  return "  parameters.push_back(Symmetric6x6FilterParameter::New(\"" + getHumanLabel() + "\", \"" + getPropertyName() + "\", get" + getPropertyName() + "(), " + getCategory() + "));";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Symmetric6x6WidgetCodeGenerator::generateReadFilterParameters()
{
  return "  set" + getPropertyName() + "(reader->readFloatVec21(\"" + getPropertyName() + "\", get" + getPropertyName() + "()));";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Symmetric6x6WidgetCodeGenerator::generateDataCheck()
{
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Symmetric6x6WidgetCodeGenerator::generateFilterParameters()
{
  QString contents;
  QTextStream ss(&contents);
  ss << "    SIMPL_FILTER_PARAMETER(FloatVec21_t, " + getPropertyName() + ")\n";
  ss << "    Q_PROPERTY(FloatVec21_t " + getPropertyName() + " READ get" + getPropertyName() + " WRITE set" + getPropertyName() + ")";

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Symmetric6x6WidgetCodeGenerator::generateHIncludes()
{
  return "#include \"DREAM3DLib/FilterParameters/Symmetric6x6FilterParameter.h\"";
}
