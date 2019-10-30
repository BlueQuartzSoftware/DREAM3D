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

#include "SampleSurfaceMeshSpecifiedPoints.h"

#include <fstream>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMeshSpecifiedPoints::SampleSurfaceMeshSpecifiedPoints()
: m_InputFilePath("")
, m_OutputFilePath("")
, m_NumPoints(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SampleSurfaceMeshSpecifiedPoints::~SampleSurfaceMeshSpecifiedPoints() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::setupFilterParameters()
{
  SampleSurfaceMesh::setupFilterParameters();
  FilterParameterVectorType parameters = getFilterParameters();
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
  clearErrorCode();
  clearWarningCode();
  if(nullptr != m_FeatureIdsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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
  clearErrorCode();
  clearWarningCode();
  initialize();
  DataArrayPath tempPath;

  if(m_InputFilePath.isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-1, ss);
  }

  FileSystemPathHelper::CheckOutputFile(this, "Output File Path", getOutputFilePath(), true);

  DataContainer::Pointer v = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, "SpecifiedPoints", DataContainerID);
  if(getErrorCode() < 0 || nullptr == v.get())
  {
    return;
  }

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(0, SIMPL::Geometry::VertexGeometry, !getInPreflight());
  v->setGeometry(vertices);

  std::vector<size_t> tDims(1, 0);
  v->createNonPrereqAttributeMatrix(this, "SpecifiedPointsData", tDims, AttributeMatrix::Type::Vertex, AttributeMatrixID21);

  std::vector<size_t> cDims(1, 1);
  tempPath.update("SpecifiedPoints", "SpecifiedPointsData", "FeatureIds");
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);              /* Assigns the shared_ptr<>(this, tempPath, -301, dims);  Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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
    setErrorCondition(-1, ss);
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
  std::vector<size_t> tDims(1, m_NumPoints);
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
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


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SampleSurfaceMeshSpecifiedPoints::newFilterInstance(bool copyFilterParameters) const
{
  SampleSurfaceMeshSpecifiedPoints::Pointer filter = SampleSurfaceMeshSpecifiedPoints::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid SampleSurfaceMeshSpecifiedPoints::getUuid() const
{
  return QUuid("{0f44da6f-5272-5d69-8378-9bf0bc4ae4f9}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ResolutionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::getHumanLabel() const
{
  return "Sample Triangle Geometry at Specified Points";
}

// -----------------------------------------------------------------------------
SampleSurfaceMeshSpecifiedPoints::Pointer SampleSurfaceMeshSpecifiedPoints::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<SampleSurfaceMeshSpecifiedPoints> SampleSurfaceMeshSpecifiedPoints::New()
{
  struct make_shared_enabler : public SampleSurfaceMeshSpecifiedPoints
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::getNameOfClass() const
{
  return QString("SampleSurfaceMeshSpecifiedPoints");
}

// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::ClassName()
{
  return QString("SampleSurfaceMeshSpecifiedPoints");
}

// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::setInputFilePath(const QString& value)
{
  m_InputFilePath = value;
}

// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::getInputFilePath() const
{
  return m_InputFilePath;
}

// -----------------------------------------------------------------------------
void SampleSurfaceMeshSpecifiedPoints::setOutputFilePath(const QString& value)
{
  m_OutputFilePath = value;
}

// -----------------------------------------------------------------------------
QString SampleSurfaceMeshSpecifiedPoints::getOutputFilePath() const
{
  return m_OutputFilePath;
}
