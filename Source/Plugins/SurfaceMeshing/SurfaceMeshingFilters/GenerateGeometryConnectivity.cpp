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

#include "GenerateGeometryConnectivity.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

// Include the MOC generated file for this class
#include "moc_GenerateGeometryConnectivity.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateGeometryConnectivity::GenerateGeometryConnectivity()
: SurfaceMeshFilter()
, m_SurfaceDataContainerName("")
, m_GenerateVertexTriangleLists(true)
, m_GenerateTriangleNeighbors(true)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateGeometryConnectivity::~GenerateGeometryConnectivity()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_BOOL_FP("Generate Per Vertex Element List", GenerateVertexTriangleLists, FilterParameter::Parameter, GenerateGeometryConnectivity));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Generate Element Neighbors List", GenerateTriangleNeighbors, FilterParameter::Parameter, GenerateGeometryConnectivity));
  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Data Container", SurfaceDataContainerName, FilterParameter::RequiredArray, GenerateGeometryConnectivity, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceDataContainerName(reader->readString("SurfaceDataContainerName", getSurfaceDataContainerName()));
  setGenerateVertexTriangleLists(reader->readValue("GenerateVertexTriangleLists", getGenerateVertexTriangleLists()));
  setGenerateTriangleNeighbors(reader->readValue("GenerateTriangleNeighbors", getGenerateTriangleNeighbors()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::dataCheck()
{
  getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometry, AbstractFilter>(this, getSurfaceDataContainerName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::preflight()
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
void GenerateGeometryConnectivity::execute()
{
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());
  IGeometry::Pointer geom = sm->getGeometry();

  if(m_GenerateVertexTriangleLists == true || m_GenerateTriangleNeighbors == true)
  {
    notifyStatusMessage(getHumanLabel(), "Generating Vertex Element List");
    err = geom->findElementsContainingVert();
    if(err < 0)
    {
      setErrorCondition(-400);
      QString ss = QObject::tr("Error generating vertex element list for Geometry type %1").arg(geom->getGeometryTypeAsString());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
  if(m_GenerateTriangleNeighbors == true)
  {
    notifyStatusMessage(getHumanLabel(), "Generating Element Neighbors List");
    err = geom->findElementNeighbors();
    if(err < 0)
    {
      setErrorCondition(-401);
      QString ss = QObject::tr("Error generating element neighbor list for Geometry type %1").arg(geom->getGeometryTypeAsString());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateGeometryConnectivity::newFilterInstance(bool copyFilterParameters)
{
  GenerateGeometryConnectivity::Pointer filter = GenerateGeometryConnectivity::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateGeometryConnectivity::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateGeometryConnectivity::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateGeometryConnectivity::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateGeometryConnectivity::getGroupName()
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateGeometryConnectivity::getSubGroupName()
{
  return SIMPL::FilterSubGroups::ConnectivityArrangementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateGeometryConnectivity::getHumanLabel()
{
  return "Generate Geometry Connectivity";
}
