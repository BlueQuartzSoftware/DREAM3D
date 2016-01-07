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

#include "WriteTriangleGeometry.h"


#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "IO/IOConstants.h"


#define WRITE_EDGES_FILE 0
// Include the MOC generated file for this class
#include "moc_WriteTriangleGeometry.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteTriangleGeometry::WriteTriangleGeometry() :
  AbstractFilter(),
  m_DataContainerSelection(""),
  m_OutputNodesFile(""),
  m_OutputTrianglesFile("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteTriangleGeometry::~WriteTriangleGeometry()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteTriangleGeometry::setupFilterParameters()
{
  FilterParameterVector parameters;


  parameters.push_back(OutputFileFilterParameter::New("Output Nodes File", "OutputNodesFile", getOutputNodesFile(), FilterParameter::Parameter));
  parameters.push_back(OutputFileFilterParameter::New("Output Triangles File", "OutputTrianglesFile", getOutputTrianglesFile(), FilterParameter::Parameter));

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(DataContainerSelectionFilterParameter::New("DataContainer", "DataContainerSelection", getDataContainerSelection(), FilterParameter::RequiredArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteTriangleGeometry::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerSelection( reader->readString("DataContainerSelection", getDataContainerSelection()));
  setOutputNodesFile( reader->readString( "OutputNodesFile", getOutputNodesFile() ) );
  setOutputTrianglesFile( reader->readString( "OutputTrianglesFile", getOutputTrianglesFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteTriangleGeometry::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(DataContainerSelection)
  SIMPL_FILTER_WRITE_PARAMETER(OutputNodesFile)
  SIMPL_FILTER_WRITE_PARAMETER(OutputTrianglesFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteTriangleGeometry::dataCheck()
{
  setErrorCondition(0);

  if(true == m_OutputNodesFile.isEmpty())
  {
    setErrorCondition(-380);
    notifyErrorMessage(getHumanLabel(), "The output Nodes file needs to be set", getErrorCondition());
  }

  if(true == m_OutputTrianglesFile.isEmpty())
  {
    setErrorCondition(-382);
    notifyErrorMessage(getHumanLabel(), "The output Triangles file needs to be set", getErrorCondition());
  }

  DataContainer::Pointer dataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getDataContainerSelection());
  if(getErrorCondition() < 0) { return; }

  TriangleGeom::Pointer triangles =  dataContainer->getPrereqGeometry<TriangleGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if (NULL == triangles->getVertices().get())
  {
    setErrorCondition(-386);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Vertices", getErrorCondition());
  }
  // We MUST have Triangles defined also.
  if (NULL == triangles->getTriangles().get())
  {
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), "DataContainer Geometry missing Triangles", getErrorCondition());
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteTriangleGeometry::preflight()
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
void WriteTriangleGeometry::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }
  DataContainer::Pointer dataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getDataContainerSelection());

  TriangleGeom::Pointer triangleGeom = dataContainer->getGeometryAs<TriangleGeom>();
  QString geometryType = triangleGeom->getGeometryTypeAsString();
  float* nodes = triangleGeom->getVertexPointer(0);
  int64_t* triangles = triangleGeom->getTriPointer(0);

  qint64 numNodes = triangleGeom->getNumberOfVertices();
  qint64 maxNodeId = numNodes - 1;
  int64_t numTriangles = triangleGeom->getNumberOfTris();

  // ++++++++++++++ Write the Nodes File +++++++++++++++++++++++++++++++++++++++++++
  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  notifyStatusMessage(getHumanLabel(), "Writing Nodes Text File");
  QFileInfo fi(getOutputNodesFile());
  QDir parentPath = fi.path();

  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  FILE* nodesFile = NULL;
  nodesFile = fopen(getOutputNodesFile().toLatin1().data(), "wb");
  if (NULL == nodesFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), "Error opening Nodes file for writing", -100);
    return;
  }
  fprintf(nodesFile, "# All lines starting with '#' are comments\n");
  fprintf(nodesFile, "# DREAM.3D Nodes file\n");
  fprintf(nodesFile, "# DREAM.3D Version %s\n", SIMPLib::Version::Complete().toLatin1().constData());
  fprintf(nodesFile, "# Node Data is X Y Z space delimited.\n");
  fprintf(nodesFile, "Node Count: %lld\n", numNodes);
  for (int i = 0; i < numNodes; i++)
  {
    fprintf(nodesFile, "%8.5f %8.5f %8.5f\n", nodes[i * 3], nodes[i * 3 + 1], nodes[i * 3 + 2]);
  }
  fclose(nodesFile);

  // ++++++++++++++ Write the Triangles File +++++++++++++++++++++++++++++++++++++++++++
  notifyStatusMessage(getHumanLabel(), "Writing Triangles Text File");
  QFileInfo triFI(getOutputTrianglesFile());
  parentPath = triFI.path();
  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  FILE* triFile = fopen(getOutputTrianglesFile().toLatin1().data(), "wb");
  if (NULL == triFile)
  {
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), "Error opening Triangles file for writing", -100);
    return;
  }

  fprintf(triFile, "# All lines starting with '#' are comments\n");
  fprintf(triFile, "# DREAM.3D Triangle file\n");
  fprintf(triFile, "# DREAM.3D Version %s\n", SIMPLib::Version::Complete().toLatin1().constData());
  fprintf(triFile, "# Each Triangle consists of 3 Node Ids.\n");
  fprintf(triFile, "# NODE IDs START AT 0.\n");
  fprintf(triFile, "Geometry Type: %s\n", geometryType.toLatin1().constData());
  fprintf(triFile, "Node Count: %lld\n", numNodes);
  fprintf(triFile, "Max Node Id: %lld\n", maxNodeId );
  fprintf(triFile, "Triangle Count: %lld\n", (long long int)(numTriangles));

  int n1, n2, n3;
  for (int64_t j = 0; j < numTriangles; ++j)
  {
    n1 = triangles[j * 3];
    n2 = triangles[j * 3 + 1];
    n3 = triangles[j * 3 + 2];

    fprintf(triFile, "%d %d %d\n",  n1, n2, n3);
  }

  fclose(triFile);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteTriangleGeometry::newFilterInstance(bool copyFilterParameters)
{
  WriteTriangleGeometry::Pointer filter = WriteTriangleGeometry::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteTriangleGeometry::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteTriangleGeometry::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteTriangleGeometry::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteTriangleGeometry::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteTriangleGeometry::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteTriangleGeometry::getHumanLabel()
{ return "Write Triangle Geometry"; }

