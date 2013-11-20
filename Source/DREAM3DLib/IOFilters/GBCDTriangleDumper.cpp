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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GBCDTriangleDumper::GBCDTriangleDumper() :
  SurfaceMeshFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceAreasArrayName(DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_SurfaceMeshFaceAreas(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormals(NULL),
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

  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.ph");
    option->setFileType("CMU Feature Growth");
    option->setValueType("string");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GBCDTriangleDumper::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("OutputFile", getOutputFile() );
  writer->closeFilterGroup();
  return index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  if(getOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Output File Set and it was not.").arg(ClassName());
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-387);
  }

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
    setErrorCondition(-384);
  }
  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
    setErrorCondition(-384);
  }
  else
  {
    QVector<int> dims(1, 2);
    m_SurfaceMeshFaceLabels = sm->getPrereqArray<int32_t, AbstractFilter>(this, m_FaceAttributeMatrixName,  m_SurfaceMeshFaceLabelsArrayName, -386, features, dims);
    dims[0] = 3;
    m_SurfaceMeshFaceNormals = sm->getPrereqArray<double, AbstractFilter>(this, m_FaceAttributeMatrixName,  m_SurfaceMeshFaceNormalsArrayName, -387, features, dims);
    dims[0] = 1;
    m_SurfaceMeshFaceAreas = sm->getPrereqArray<double, AbstractFilter>(this, m_FaceAttributeMatrixName,  m_SurfaceMeshFaceAreasArrayName, -388, features, dims);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::dataCheckVoxel(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    QString ss = QObject::tr("The Volume Data Container with name '%1'' was not found in the Data Container Array.").arg(getDataContainerName());
    setErrorCondition(-1001);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  QVector<int> dims(1, 3);
  m_FeatureEulerAngles = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeatureEulerAnglesArrayName, -301, features, dims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::preflight()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", -383);
    setErrorCondition(-383);
  }

  dataCheckSurfaceMesh(true, 1, 1, 1);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    addErrorMessage(getHumanLabel(), "VolumeDataContainer is missing", -383);
    setErrorCondition(-383);
  }

  dataCheckVoxel(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::execute()
{
  int err = 0;

  setErrorCondition(err);
  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

  FaceArray::Pointer trianglesPtr = sm->getFaces();
  size_t totalFaces = trianglesPtr->getNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  // Note the use of the voxel datacontainer num ensembles to set the gbcd size
  dataCheckSurfaceMesh(false, 0, totalFaces, m->getNumCellEnsembleTuples());

  size_t totalFeatures = m->getNumCellFeatureTuples();
  size_t totalEnsembles = m->getNumCellEnsembleTuples();

  FILE* f = fopen(getOutputFile().toLatin1().data(), "wb");
  if (NULL == f)
  {
    setErrorCondition(-87000);
    notifyErrorMessage("Could not open Output file for writing.", getErrorCondition());
    return;
  }

  dataCheckVoxel(false, 0, totalFeatures, totalEnsembles);

  float radToDeg = 180.0 / M_PI;

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

    fprintf(f, "%0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f\n", euAng0[0]*radToDeg, euAng0[1]*radToDeg, euAng0[2]*radToDeg,
            euAng1[0]*radToDeg, euAng1[1]*radToDeg, euAng1[2]*radToDeg,
            tNorm[0], tNorm[1], tNorm[2], m_SurfaceMeshFaceAreas[t]);


  }

  fclose(f);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

