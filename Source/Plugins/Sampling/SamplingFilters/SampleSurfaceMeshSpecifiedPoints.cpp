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

#include "SampleSurfaceMeshSpecifiedPoints.h"

#include <fstream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

#include "moc_SampleSurfaceMeshSpecifiedPoints.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMeshSpecifiedPoints::SampleSurfaceMeshSpecifiedPoints()
: SampleSurfaceMesh()
, m_InputFilePath("")
, m_OutputFilePath("")
, m_FeatureIds(nullptr)
, m_NumPoints(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMeshSpecifiedPoints::~SampleSurfaceMeshSpecifiedPoints()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Specified Points File", InputFilePath, FilterParameter::Parameter, SampleSurfaceMeshSpecifiedPoints, "*.raw, *.bin"));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Sampled Values File", OutputFilePath, FilterParameter::Parameter, SampleSurfaceMeshSpecifiedPoints, "*.txt"));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  SampleSurfaceMesh::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setInputFilePath(reader->readString("InputFilePath", getInputFilePath()));
  setOutputFilePath(reader->readString("OutputFilePath", getOutputFilePath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::updateVertexInstancePointers()
{
  setErrorCondition(0);
  if(nullptr != m_FeatureIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::initialize()
{
  m_NumPoints = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::dataCheck()
{
  setErrorCondition(0);
  initialize();
  DataArrayPath tempPath;

  if(true == m_InputFilePath.isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }
  if(true == m_OutputFilePath.isEmpty())
  {
    QString ss = QObject::tr("The output file must be set");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }

  DataContainer::Pointer v = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, "SpecifiedPoints");
  if(getErrorCondition() < 0 || nullptr == v.get())
  {
    return;
  }

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(0, SIMPL::Geometry::VertexGeometry, !getInPreflight());
  v->setGeometry(vertices);

  QVector<size_t> tDims(1, 0);
  v->createNonPrereqAttributeMatrix<AbstractFilter>(this, "SpecifiedPointsData", tDims, SIMPL::AttributeMatrixType::Vertex);

  QVector<size_t> cDims(1, 1);
  tempPath.update("SpecifiedPoints", "SpecifiedPointsData", "FeatureIds");
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);              /* Assigns the shared_ptr<>(this, tempPath, -301, dims);  Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  SampleSurfaceMesh::preflight();
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexGeom::Pointer SampleSurfaceMeshSpecifiedPoints::generate_points()
{
  m_NumPoints = 0;

  std::ifstream inFile;
  inFile.open(m_InputFilePath.toLatin1().data());

  // get the number of points in the specified points file
  inFile >> m_NumPoints;

  if(m_NumPoints <= 0)
  {
    QString ss = QObject::tr("Number of points to sample (%1) must be positive").arg(m_NumPoints);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return VertexGeom::NullPointer();
  }

  DataContainer::Pointer v = getDataContainerArray()->getDataContainer("SpecifiedPoints");

  VertexGeom::Pointer points = VertexGeom::CreateGeometry(m_NumPoints, "Points");

  float coords[3] = {0.0f, 0.0f, 0.0f};

  for(int64_t i = 0; i < m_NumPoints; i++)
  {
    inFile >> coords[0] >> coords[1] >> coords[2];
    points->setCoords(i, coords);
  }

  v->setGeometry(points);

  return points;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::assign_points(Int32ArrayType::Pointer iArray)
{
  QVector<size_t> tDims(1, m_NumPoints);
  getDataContainerArray()->getDataContainer("SpecifiedPoints")->getAttributeMatrix("SpecifiedPointsData")->resizeAttributeArrays(tDims);
  updateVertexInstancePointers();

  int32_t* ids = iArray->getPointer(0);
  for(int64_t i = 0; i < m_NumPoints; i++)
  {
    m_FeatureIds[i] = ids[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  SampleSurfaceMesh::execute();

  std::ofstream outFile;
  outFile.open(m_OutputFilePath.toLatin1().data());
  for(int64_t i = 0; i < m_NumPoints; i++)
  {
    outFile << m_FeatureIds[i] << std::endl;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SampleSurfaceMeshSpecifiedPoints::newFilterInstance(bool copyFilterParameters)
{
  SampleSurfaceMeshSpecifiedPoints::Pointer filter = SampleSurfaceMeshSpecifiedPoints::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getCompiledLibraryName()
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getBrandingString()
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getGroupName()
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getSubGroupName()
{
  return SIMPL::FilterSubGroups::ResolutionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SampleSurfaceMeshSpecifiedPoints::getHumanLabel()
{
  return "Sample Triangle Geometry at Specified Points";
}
