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

#include <memory>

#include "WriteIPFStandardTriangle.h"

#include <QtCore/QDir>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/CubicIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/CubicLowIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/HexagonalIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/HexagonalLowIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/MonoclinicIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/OrthorhombicIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TetragonalIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TetragonalLowIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TriclinicIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TrigonalIPFLegendPainter.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/IPFLegendHelpers/TrigonalLowIPFLegendPainter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteIPFStandardTriangle::WriteIPFStandardTriangle()
: m_OutputFile("")
, m_ImageSize(512)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteIPFStandardTriangle::~WriteIPFStandardTriangle() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  QVector<QString> choices = QVector<QString>::fromStdVector(LaueOps::GetLaueNames());
  choices.pop_back(); // Remove the last name because we don't need it.

  parameters.push_back(SIMPL_NEW_CHOICE_FP("Select Laue Class", LaueClass, FilterParameter::Parameter, WriteIPFStandardTriangle, choices, false));

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", OutputFile, FilterParameter::Parameter, WriteIPFStandardTriangle, "*.tif, *.bmp, *.png", "Image"));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Image Size (Square Pixels)", ImageSize, FilterParameter::Parameter, WriteIPFStandardTriangle));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputFile(reader->readString("OutputFile", getOutputFile()));
  setImageSize(reader->readValue("ImageSize", getImageSize()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QString ss;

  FileSystemPathHelper::CheckOutputFile(this, "Output File Path", getOutputFile(), true);

  QFileInfo fi(getOutputFile());
  QString ext = fi.completeSuffix();
  if(ext != "tif" && ext != "bmp" && ext != "png")
  {
    ss = QObject::tr("The output file has an unsupported extension.  Please select a TIF, BMP, or PNG file");
    setErrorCondition(-1004, ss);
    return;
  }

  if(m_ImageSize <= 0)
  {
    setErrorCondition(-1005, "The size of the image must be positive");
    return;
  }

  if(m_LaueClass < 0 || m_LaueClass > 10)
  {
    setErrorCondition(-1006, "The Laue Class value must be in the range [0-10]. See documentation for the complete list of values.");
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  QImage image;

  if(m_LaueClass == 0)
  {
    HexagonalIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 1)
  {
    CubicIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 2)
  {
    HexagonalLowIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 3)
  {
    CubicLowIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 4)
  {
    TriclinicIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 5)
  {
    MonoclinicIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 6)
  {
    OrthorhombicIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 7)
  {
    TetragonalLowIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 8)
  {
    TetragonalIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 9)
  {
    TrigonalLowIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }
  else if(m_LaueClass == 10)
  {
    TrigonalIPFLegendPainter legendPainter;
    image = legendPainter.createLegend(getImageSize(), getImageSize());
  }

  if(!image.isNull())
  {
    writeImage(image);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::writeImage(QImage& image)
{

  QString ss = QObject::tr("Writing Image %1").arg(getOutputFile());
  notifyStatusMessage(ss);

  QFileInfo fi((m_OutputFile));
  QDir parent(fi.absolutePath());
  if(!parent.exists())
  {
    parent.mkpath(fi.absolutePath());
  }

  bool saved = image.save((m_OutputFile));
  if(!saved)
  {
    QString ss = QObject::tr("The Triangle image file '%1' was not saved").arg(getOutputFile());
    setErrorCondition(-90011, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteIPFStandardTriangle::newFilterInstance(bool copyFilterParameters) const
{
  WriteIPFStandardTriangle::Pointer filter = WriteIPFStandardTriangle::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteIPFStandardTriangle::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteIPFStandardTriangle::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteIPFStandardTriangle::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteIPFStandardTriangle::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid WriteIPFStandardTriangle::getUuid() const
{
  return QUuid("{0b72aa9a-6b5a-5677-bf6c-743445e75cc8}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteIPFStandardTriangle::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WriteIPFStandardTriangle::getHumanLabel() const
{
  return "Export IPF Triangle Legend";
}

// -----------------------------------------------------------------------------
WriteIPFStandardTriangle::Pointer WriteIPFStandardTriangle::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<WriteIPFStandardTriangle> WriteIPFStandardTriangle::New()
{
  struct make_shared_enabler : public WriteIPFStandardTriangle
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString WriteIPFStandardTriangle::getNameOfClass() const
{
  return QString("WriteIPFStandardTriangle");
}

// -----------------------------------------------------------------------------
QString WriteIPFStandardTriangle::ClassName()
{
  return QString("WriteIPFStandardTriangle");
}

// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::setLaueClass(int value)
{
  m_LaueClass = value;
}

// -----------------------------------------------------------------------------
int WriteIPFStandardTriangle::getLaueClass() const
{
  return m_LaueClass;
}

// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::setOutputFile(const QString& value)
{
  m_OutputFile = value;
}

// -----------------------------------------------------------------------------
QString WriteIPFStandardTriangle::getOutputFile() const
{
  return m_OutputFile;
}

// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::setImageSize(int value)
{
  m_ImageSize = value;
}

// -----------------------------------------------------------------------------
int WriteIPFStandardTriangle::getImageSize() const
{
  return m_ImageSize;
}
