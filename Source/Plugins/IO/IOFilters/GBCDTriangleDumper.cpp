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
#include "GBCDTriangleDumper.h"

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/DREAM3DVersion.h"

#include "IO/IOConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GBCDTriangleDumper::GBCDTriangleDumper() :
  AbstractFilter(),
  m_OutputFile(""),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceAreasArrayPath(DREAM3D::Defaults::SurfaceDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_FeatureEulerAnglesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::EulerAngles),
  m_SurfaceMeshFaceAreasArrayName(DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_SurfaceMeshFaceAreas(NULL),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceNormals(NULL),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_FeatureEulerAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GBCDTriangleDumper::~GBCDTriangleDumper()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(FileSystemFilterParameter::New("Output File", "OutputFile", FilterParameterWidgetType::OutputFileWidget, getOutputFile(), false, "", "*.ph", "CMU Feature Growth"));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceNormals", "SurfaceMeshFaceNormalsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceNormalsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceAreas", "SurfaceMeshFaceAreasArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceAreasArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeatureEulerAngles", "FeatureEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureEulerAnglesArrayPath(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath() ) );
  setSurfaceMeshFaceAreasArrayPath(reader->readDataArrayPath("SurfaceMeshFaceAreasArrayPath", getSurfaceMeshFaceAreasArrayPath() ) );
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setOutputFile(reader->readString("OutputFile", getOutputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GBCDTriangleDumper::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceAreasArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceNormalsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputFile)
  writer->closeFilterGroup();
  return index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::dataCheckSurfaceMesh()
{
  setErrorCondition(0);

  if(getOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Output File Set and it was not.").arg(ClassName());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-387);
  }

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }
  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }
  else
  {
    QVector<size_t> dims(1, 2);
    m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 3;
    m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 1;
    m_SurfaceMeshFaceAreasPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceAreasArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshFaceAreasPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshFaceAreas = m_SurfaceMeshFaceAreasPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::dataCheckVoxel()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 3);
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheckSurfaceMesh();
  dataCheckVoxel();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::execute()
{
  int err = 0;

  setErrorCondition(err);

  dataCheckSurfaceMesh();
  dataCheckVoxel();

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  setErrorCondition(0);
  notifyStatusMessage(getHumanLabel(), "Starting");

  FaceArray::Pointer trianglesPtr = sm->getFaces();
  size_t totalFaces = trianglesPtr->getNumberOfTuples();

  FILE* f = fopen(getOutputFile().toLatin1().data(), "wb");
  if (NULL == f)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "Could not open Output file for writing.", getErrorCondition());
    return;
  }

  fprintf(f, "# Triangles Produced from DREAM3D version %s\n", DREAM3DLib::Version::Package().toLatin1().data() );
  fprintf(f, "# Column 1-3:    right hand average orientation (phi1, PHI, phi2 in RADIANS)\n");
  fprintf(f, "# Column 4-6:    left hand average orientation (phi1, PHI, phi2 in RADIANS)\n");
  fprintf(f, "# Column 7-9:    triangle normal\n");
  fprintf(f, "# Column 8:      surface area\n");

  int gid0 = 0; // Feature id 0
  int gid1 = 0; // Feature id 1
  float* euAng0 = NULL;
  float* euAng1 = NULL;
  double* tNorm = NULL;
  for(size_t t = 0; t < totalFaces; ++t)
  {
    // Get the Feature Ids for the triangle
    gid0 = m_SurfaceMeshFaceLabels[t * 2];
    gid1 = m_SurfaceMeshFaceLabels[t * 2 + 1];

    if(gid0 < 0)
    {
      continue;
    }
    if(gid1 < 0)
    {
      continue;
    }

    // Now get the Euler Angles for that feature id, WATCH OUT: This is pointer arithmetic
    euAng0 = m_FeatureEulerAngles + (gid0 * 3);
    euAng1 = m_FeatureEulerAngles + (gid1 * 3);

    // Get the Triangle Normal
    tNorm = m_SurfaceMeshFaceNormals + (t * 3);

    fprintf(f, "%0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f\n", euAng0[0], euAng0[1], euAng0[2],
            euAng1[0], euAng1[1], euAng1[2], tNorm[0], tNorm[1], tNorm[2], m_SurfaceMeshFaceAreas[t]);
  }

  fclose(f);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GBCDTriangleDumper::newFilterInstance(bool copyFilterParameters)
{
  GBCDTriangleDumper::Pointer filter = GBCDTriangleDumper::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GBCDTriangleDumper::getCompiledLibraryName()
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GBCDTriangleDumper::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GBCDTriangleDumper::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GBCDTriangleDumper::getHumanLabel()
{ return "Write GBCD Triangles File"; }

