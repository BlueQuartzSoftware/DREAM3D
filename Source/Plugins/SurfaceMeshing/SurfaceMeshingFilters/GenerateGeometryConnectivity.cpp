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

#include "GenerateGeometryConnectivity.h"

#include <QtCore/QTextStream>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"

#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateGeometryConnectivity::GenerateGeometryConnectivity()
: m_SurfaceDataContainerName("")
, m_GenerateVertexTriangleLists(true)
, m_GenerateTriangleNeighbors(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateGeometryConnectivity::~GenerateGeometryConnectivity() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::setupFilterParameters()
{
  SurfaceMeshFilter::setupFilterParameters();
  FilterParameterVectorType parameters;
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
  setSurfaceDataContainerName(reader->readDataArrayPath("SurfaceDataContainerName", getSurfaceDataContainerName()));
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());
  IGeometry::Pointer geom = sm->getGeometry();

  if(m_GenerateVertexTriangleLists || m_GenerateTriangleNeighbors)
  {
    notifyStatusMessage("Generating Vertex Element List");
    int err = geom->findElementsContainingVert();
    if(err < 0)
    {
      QString ss = QObject::tr("Error generating vertex element list for Geometry type %1").arg(geom->getGeometryTypeAsString());
      setErrorCondition(-400, ss);
    }
  }
  if(m_GenerateTriangleNeighbors)
  {
    notifyStatusMessage("Generating Element Neighbors List");
    int err = geom->findElementNeighbors();
    if(err < 0)
    {
      QString ss = QObject::tr("Error generating element neighbor list for Geometry type %1").arg(geom->getGeometryTypeAsString());
      setErrorCondition(-401, ss);
    }
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateGeometryConnectivity::newFilterInstance(bool copyFilterParameters) const
{
  GenerateGeometryConnectivity::Pointer filter = GenerateGeometryConnectivity::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateGeometryConnectivity::getCompiledLibraryName() const
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateGeometryConnectivity::getBrandingString() const
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateGeometryConnectivity::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateGeometryConnectivity::getGroupName() const
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateGeometryConnectivity::getUuid() const
{
  return QUuid("{ea565056-784e-52c5-b705-92f6799714c7}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateGeometryConnectivity::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConnectivityArrangementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateGeometryConnectivity::getHumanLabel() const
{
  return "Generate Geometry Connectivity";
}

// -----------------------------------------------------------------------------
GenerateGeometryConnectivity::Pointer GenerateGeometryConnectivity::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateGeometryConnectivity> GenerateGeometryConnectivity::New()
{
  struct make_shared_enabler : public GenerateGeometryConnectivity
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateGeometryConnectivity::getNameOfClass() const
{
  return QString("GenerateGeometryConnectivity");
}

// -----------------------------------------------------------------------------
QString GenerateGeometryConnectivity::ClassName()
{
  return QString("GenerateGeometryConnectivity");
}

// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::setSurfaceDataContainerName(const DataArrayPath& value)
{
  m_SurfaceDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateGeometryConnectivity::getSurfaceDataContainerName() const
{
  return m_SurfaceDataContainerName;
}

// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::setGenerateVertexTriangleLists(bool value)
{
  m_GenerateVertexTriangleLists = value;
}

// -----------------------------------------------------------------------------
bool GenerateGeometryConnectivity::getGenerateVertexTriangleLists() const
{
  return m_GenerateVertexTriangleLists;
}

// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::setGenerateTriangleNeighbors(bool value)
{
  m_GenerateTriangleNeighbors = value;
}

// -----------------------------------------------------------------------------
bool GenerateGeometryConnectivity::getGenerateTriangleNeighbors() const
{
  return m_GenerateTriangleNeighbors;
}
