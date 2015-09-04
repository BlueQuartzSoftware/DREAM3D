/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
#include "CreateImageGeometry.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateImageGeometry::CreateImageGeometry() :
  AbstractFilter(),
  m_SelectedDataContainer("ImageGeomDataContainer")
{
  m_Dimensions.x = 0;
  m_Dimensions.y = 0;
  m_Dimensions.z = 0;

  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateImageGeometry::~CreateImageGeometry()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(DataContainerSelectionFilterParameter::New("Data Container Destination", "SelectedDataContainer", getSelectedDataContainer(), FilterParameter::Parameter, req));
  }
  parameters.push_back(IntVec3FilterParameter::New("Dimensions", "Dimensions", getDimensions(), FilterParameter::Parameter));
  parameters.push_back(FloatVec3FilterParameter::New("Origin", "Origin", getOrigin(), FilterParameter::Parameter));
  parameters.push_back(FloatVec3FilterParameter::New("Resolution", "Resolution", getResolution(), FilterParameter::Parameter));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDimensions( reader->readIntVec3("Dimensions", getDimensions() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  setSelectedDataContainer(reader->readString("SelectedDataContainer", getSelectedDataContainer()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateImageGeometry::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(Dimensions)
  SIMPL_FILTER_WRITE_PARAMETER(Origin)
  SIMPL_FILTER_WRITE_PARAMETER(Resolution)
  SIMPL_FILTER_WRITE_PARAMETER(SelectedDataContainer)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::dataCheck()
{
  setErrorCondition(0);

  if (m_Dimensions.x == 0 || m_Dimensions.y == 0 || m_Dimensions.z == 0)
  {
    QString ss = QObject::tr("One of the dimensions has a size less than or equal to zero. The minimum size must be postive");
    setErrorCondition(-390);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getSelectedDataContainer().isEmpty())
  {
    QString ss = QObject::tr("The Data Container must have a name");
    setErrorCondition(-391);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSelectedDataContainer());
  if(getErrorCondition() < 0) { return; }


  ImageGeom::Pointer image = ImageGeom::CreateGeometry("ImageGeometry");
  image->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
  image->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  image->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
  m->setGeometry(image);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateImageGeometry::preflight()
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
void CreateImageGeometry::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateImageGeometry::newFilterInstance(bool copyFilterParameters)
{
  CreateImageGeometry::Pointer filter = CreateImageGeometry::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getSubGroupName()
{ return DREAM3D::FilterSubGroups::GenerationFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateImageGeometry::getHumanLabel()
{ return "Create Geometry (Image)"; }
