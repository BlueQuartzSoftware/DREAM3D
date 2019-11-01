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

#include "AlignSectionsList.h"

#include <fstream>

#include <QtCore/QFileInfo>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsList::AlignSectionsList()
: m_InputFile("")
, m_DREAM3DAlignmentFile(false)
{
  // only setting up the child parameters because the parent constructor has already been called
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsList::~AlignSectionsList() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsList::setupFilterParameters()
{
  // getting the current parameters that were set by the parent and adding to it before resetting it
  AlignSections::setupFilterParameters();
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, AlignSectionsList, "*.txt"));
  parameters.push_back(SIMPL_NEW_BOOL_FP("DREAM3D Alignment File Format", DREAM3DAlignmentFile, FilterParameter::Parameter, AlignSectionsList));

  DataContainerSelectionFilterParameter::RequirementType req;
  req.dcGeometryTypes = {IGeometry::Type::Image};
  parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container", DataContainerName, FilterParameter::Parameter, AlignSectionsList, req));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsList::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  //AlignSections::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setInputFile(reader->readString("InputFile", getInputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsList::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsList::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  QString ss;

  AlignSections::dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  QFileInfo fi(m_InputFile);
  if(m_InputFile.isEmpty())
  {
    ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
    setErrorCondition(-15000, ss);
  }
  if(!fi.exists())
  {
    ss = QObject::tr("The input file does not exist: '%1'").arg(getInputFile());
    setErrorCondition(-15001, ss);
  }

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerName(), false);
  ImageGeom::Pointer geom = dc->getGeometryAs<ImageGeom>();
  if(nullptr == geom.get())
  {
    QString ss = QObject::tr("DataContainer '%1' does not have an ImageGeometry").arg(getDataContainerName().getDataContainerName());
    setErrorCondition(-15002, ss);
    return;
  }
  Q_UNUSED(dc)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsList::preflight()
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
void AlignSectionsList::find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  ImageGeom::Pointer geom = m->getGeometryAs<ImageGeom>();

  SizeVec3Type udims = geom->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  int64_t slice = 0;
  int64_t newxshift = 0, newyshift = 0;

  std::ifstream inFile;
  inFile.open(m_InputFile.toLatin1().data());

  if(getDREAM3DAlignmentFile())
  {
    // These are ignored from the input file since DREAM.3D wrote the file
    int64_t slice2 = 0;
    float xShift = 0.0f;
    float yShift = 0.0f;
    for(int64_t iter = 1; iter < dims[2]; iter++)
    {
      inFile >> slice >> slice2 >> newxshift >> newyshift >> xShift >> yShift;
      xshifts[iter] = xshifts[iter - 1] + newxshift;
      yshifts[iter] = yshifts[iter - 1] + newyshift;
    }
  }
  else
  {
    for(int64_t iter = 1; iter < dims[2]; iter++)
    {
      inFile >> slice >> newxshift >> newyshift;
      xshifts[iter] = xshifts[iter - 1] + newxshift;
      yshifts[iter] = yshifts[iter - 1] + newyshift;
    }
  }

  inFile.close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsList::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  AlignSections::execute();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSectionsList::newFilterInstance(bool copyFilterParameters) const
{
  AlignSectionsList::Pointer filter = AlignSectionsList::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsList::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsList::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsList::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsList::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid AlignSectionsList::getUuid() const
{
  return QUuid("{accf8f6c-0551-5da3-9a3d-e4be41c3985c}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsList::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::AlignmentFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsList::getHumanLabel() const
{
  return "Align Sections (List)";
}

// -----------------------------------------------------------------------------
AlignSectionsList::Pointer AlignSectionsList::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<AlignSectionsList> AlignSectionsList::New()
{
  struct make_shared_enabler : public AlignSectionsList
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString AlignSectionsList::getNameOfClass() const
{
  return QString("AlignSectionsList");
}

// -----------------------------------------------------------------------------
QString AlignSectionsList::ClassName()
{
  return QString("AlignSectionsList");
}

// -----------------------------------------------------------------------------
void AlignSectionsList::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString AlignSectionsList::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void AlignSectionsList::setDREAM3DAlignmentFile(bool value)
{
  m_DREAM3DAlignmentFile = value;
}

// -----------------------------------------------------------------------------
bool AlignSectionsList::getDREAM3DAlignmentFile() const
{
  return m_DREAM3DAlignmentFile;
}
