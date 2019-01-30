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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
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

#include "Gridify.h"
#include <map>
#include <unordered_map>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Gridify::Gridify()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Gridify::~Gridify() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Gridify::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Gridify::setupFilterParameters()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Gridify::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  std::unordered_map<float, std::vector<QString>> xOrigins;
  std::unordered_map<float, std::vector<QString>> yOrigins;
  std::unordered_map<float, std::vector<QString>> zOrigins;
  for(const auto& eachDC : getDataContainerArray()->getDataContainers())
  {
    if (eachDC->getGeometry()->getGeometryType() != IGeometry::Type::Image)
    {
      continue;
    }

    QString dcName{eachDC->getName()};
    xOrigins[std::get<0>(eachDC->getGeometryAs<ImageGeom>()->getOrigin())].push_back(dcName);
    yOrigins[std::get<1>(eachDC->getGeometryAs<ImageGeom>()->getOrigin())].push_back(dcName);
    zOrigins[std::get<2>(eachDC->getGeometryAs<ImageGeom>()->getOrigin())].push_back(dcName);

    AttributeMatrix::Pointer am{
        AttributeMatrix::New({1}, m_amName, AttributeMatrix::Type::Vertex)
    };
    am->addAttributeArray( m_aaName, UInt16ArrayType::CreateArray(1, std::vector<size_t>{3}, m_aaName) );
    eachDC->addAttributeMatrix(m_amName, am);
  }

  std::map<float, std::vector<QString>> ordered_xOrigins{xOrigins.begin(), xOrigins.end()};
  std::map<float, std::vector<QString>> ordered_yOrigins{yOrigins.begin(), yOrigins.end()};
  std::map<float, std::vector<QString>> ordered_zOrigins{zOrigins.begin(), zOrigins.end()};

  uint16_t index{0};
  for(const auto& eachXOrigin : ordered_xOrigins)
  {
    for(const auto& eachDCName : eachXOrigin.second)
    {
      getDataContainerArray()->getDataContainer(eachDCName)
                             ->getAttributeMatrix(m_amName)
                             ->getAttributeArrayAs<UInt16ArrayType>(m_aaName)
                             ->setValue(0, index);
    }
    index++;
  }

  index = 0;
  for(const auto& eachYOrigin : ordered_yOrigins)
  {
    for(const auto& eachDCName : eachYOrigin.second)
    {
      getDataContainerArray()->getDataContainer(eachDCName)
                             ->getAttributeMatrix(m_amName)
                             ->getAttributeArrayAs<UInt16ArrayType>(m_aaName)
                             ->setValue(1, index);
    }
    index++;
  }

  index = 0;
  for(const auto& eachZOrigin : ordered_zOrigins)
  {
    for(const auto& eachDCName : eachZOrigin.second)
    {
      getDataContainerArray()->getDataContainer(eachDCName)
                             ->getAttributeMatrix(m_amName)
                             ->getAttributeArrayAs<UInt16ArrayType>(m_aaName)
                             ->setValue(2, index);
    }
    index++;
  }

  if(getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Gridify::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Gridify::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }

  if(getWarningCondition() < 0)
  {
    QString ss = QObject::tr("Some warning message");
    setWarningCondition(-88888888);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  if(getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer Gridify::newFilterInstance(bool copyFilterParameters) const
{
  Gridify::Pointer filter = Gridify::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Gridify::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Gridify::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Gridify::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Gridify::getGroupName() const
{
  return SIMPL::FilterGroups::Utilities;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Gridify::getSubGroupName() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString Gridify::getHumanLabel() const
{
  return "Gridify";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid Gridify::getUuid()
{
  return QUuid("{2ffe7d64-3018-5b56-83dd-7aca6fd726c2}");
}
