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

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "GenerateGeometryConnectivity.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateGeometryConnectivity::GenerateGeometryConnectivity() :
  SurfaceMeshFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::DataContainerName),
  m_GenerateVertexTriangleLists(true),
  m_GenerateTriangleNeighbors(true)
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
  parameters.push_back(FilterParameter::New("Generate Per Vertex Cell List", "GenerateVertexTriangleLists", FilterParameterWidgetType::BooleanWidget, getGenerateVertexTriangleLists(), false));
  parameters.push_back(FilterParameter::New("Generate Cell Neighbors List", "GenerateTriangleNeighbors", FilterParameterWidgetType::BooleanWidget, getGenerateTriangleNeighbors(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Data Container", "SurfaceDataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getSurfaceDataContainerName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceDataContainerName(reader->readString("SurfaceDataContainerName", getSurfaceDataContainerName() ) );
  setGenerateVertexTriangleLists( reader->readValue("GenerateVertexTriangleLists", getGenerateVertexTriangleLists()) );
  setGenerateTriangleNeighbors( reader->readValue("GenerateTriangleNeighbors", getGenerateTriangleNeighbors()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateGeometryConnectivity::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(GenerateVertexTriangleLists)
  DREAM3D_FILTER_WRITE_PARAMETER(GenerateTriangleNeighbors)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateGeometryConnectivity::dataCheck()
{
  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == sm.get()) { return; }

  IGeometry::Pointer geom = sm->getGeometry();
  if(NULL == geom.get())
  {
    setErrorCondition(-385);
    QString ss = QObject::tr("DataContainer Geometry is missing.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
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
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());
  IGeometry::Pointer geom = sm->getGeometry();

  if (m_GenerateVertexTriangleLists == true || m_GenerateTriangleNeighbors == true)
  {
    notifyStatusMessage(getHumanLabel(), "Generating Vertex Cell List");
    err = geom->findElementsContainingVert();
    if (err < 0)
    {
      setErrorCondition(-400);
      QString ss = QObject::tr("Error generating vertex cell list for Geometry type %1.").arg(geom->getGeometryTypeAsString());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
  if (m_GenerateTriangleNeighbors == true)
  {
    notifyStatusMessage(getHumanLabel(), "Generating Cell Neighbors List");
    err = geom->findElementNeighbors();
    if (err < 0)
    {
      setErrorCondition(-401);
      QString ss = QObject::tr("Error generating cell neighbor list for Geometry type %1.").arg(geom->getGeometryTypeAsString());
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
{ return SurfaceMeshing::SurfaceMeshingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateGeometryConnectivity::getGroupName()
{ return DREAM3D::FilterGroups::SurfaceMeshingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateGeometryConnectivity::getSubGroupName()
{ return DREAM3D::FilterSubGroups::ConnectivityArrangementFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateGeometryConnectivity::getHumanLabel()
{ return "Generate Surface Mesh Connectivity"; }

