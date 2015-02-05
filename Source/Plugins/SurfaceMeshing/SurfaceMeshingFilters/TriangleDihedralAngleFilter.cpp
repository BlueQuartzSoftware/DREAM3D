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
#include "TriangleDihedralAngleFilter.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Math/DREAM3DMath.h"



/**
 * @brief The CalculateDihedralAnglesImpl class
 */
class CalculateDihedralAnglesImpl
{
    SharedVertexList::Pointer m_Nodes;
    SharedTriList::Pointer m_Triangles;
    double* m_DihedralAngles;

  public:
    CalculateDihedralAnglesImpl(SharedVertexList::Pointer nodes,
                                SharedTriList::Pointer triangles,
                                double* DihedralAngles) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_DihedralAngles(DihedralAngles)
    {}
    virtual ~CalculateDihedralAnglesImpl() {}

    /**
     * @brief generate Generates the dihedral angle for the triangles
     * @param start The starting Index
     * @param end The ending Index
     */
    void generate(size_t start, size_t end) const
    {
      float* nodes = m_Nodes->getPointer(0);
      int64_t* triangles = m_Triangles->getPointer(0);

      float radToDeg = 180.0 / DREAM3D::Constants::k_Pi;

      float ABx, ABy, ABz, ACx, ACy, ACz, BCx, BCy, BCz;
      float magAB, magAC, magBC;
      float dihedralAngle1, dihedralAngle2, dihedralAngle3, minDihedralAngle;
      for (size_t i = start; i < end; i++)
      {
        minDihedralAngle = 180.0;

        ABx = nodes[triangles[i*3]*3+0] - nodes[triangles[i*3+1]*3+0];
        ABy = nodes[triangles[i*3]*3+1] - nodes[triangles[i*3+1]*3+1];
        ABz = nodes[triangles[i*3]*3+2] - nodes[triangles[i*3+1]*3+2];
        magAB = sqrt(ABx * ABx + ABy * ABy + ABz * ABz);

        ACx = nodes[triangles[i*3]*3+0] - nodes[triangles[i*3+2]*3+0];
        ACy = nodes[triangles[i*3]*3+1] - nodes[triangles[i*3+2]*3+1];
        ACz = nodes[triangles[i*3]*3+2] - nodes[triangles[i*3+2]*3+2];
        magAC = sqrt(ACx * ACx + ACy * ACy + ACz * ACz);

        BCx = nodes[triangles[i*3+1]*3+0] - nodes[triangles[i*3+2]*3+0];
        BCy = nodes[triangles[i*3+1]*3+1] - nodes[triangles[i*3+2]*3+1];
        BCz = nodes[triangles[i*3+1]*3+2] - nodes[triangles[i*3+2]*3+2];
        magBC = sqrt(BCx * BCx + BCy * BCy + BCz * BCz);

        dihedralAngle1 = radToDeg * acos(((ABx * ACx) + (ABy * ACy) + (ABz * ACz)) / (magAB * magAC));
        // 180 - angle because AB points out of vertex and BC points into vertex, so angle is actually angle outside of triangle
        dihedralAngle2 = 180.0 - (radToDeg * acos(((ABx * BCx) + (ABy * BCy) + (ABz * BCz)) / (magAB * magBC)));
        dihedralAngle3 = radToDeg * acos(((BCx * ACx) + (BCy * ACy) + (BCz * ACz)) / (magBC * magAC));
        minDihedralAngle = dihedralAngle1;
        if(dihedralAngle2 < minDihedralAngle) { minDihedralAngle = dihedralAngle2; }
        if(dihedralAngle3 < minDihedralAngle) { minDihedralAngle = dihedralAngle3; }
        m_DihedralAngles[i]  = minDihedralAngle;

      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleDihedralAngleFilter::TriangleDihedralAngleFilter() :
  SurfaceMeshFilter(),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, ""),
  m_SurfaceMeshTriangleDihedralAnglesArrayName(DREAM3D::FaceData::SurfaceMeshFaceDihedralAngles),
  m_SurfaceMeshTriangleDihedralAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleDihedralAngleFilter::~TriangleDihedralAngleFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Face Attribute Matrix Name", "FaceAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getFaceAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshTriangleDihedralAngles", "SurfaceMeshTriangleDihedralAnglesArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshTriangleDihedralAnglesArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFaceAttributeMatrixName(reader->readDataArrayPath("FaceAttributeMatrixName", getFaceAttributeMatrixName() ) );
  setSurfaceMeshTriangleDihedralAnglesArrayName(reader->readString("SurfaceMeshTriangleDihedralAnglesArrayName", getSurfaceMeshTriangleDihedralAnglesArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleDihedralAngleFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FaceAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshTriangleDihedralAnglesArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::dataCheck()
{
  DataArrayPath tempPath;
  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getFaceAttributeMatrixName().getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  IGeometry::Pointer geom = sm->getGeometry();
  if(NULL == geom.get())
  {
    setErrorCondition(-385);
    QString ss = QObject::tr("DataContainer Geometry is missing.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  TriangleGeom::Pointer triangles = sm->getGeometryAs<TriangleGeom>();
  if(NULL == triangles.get())
  {
    setErrorCondition(-384);
    QString ss = QObject::tr("DataContainer Geometry is not compatible. The Geometry type is %1").arg(geom->getGeometryTypeAsString());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

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
  else
  {
    QVector<size_t> dims(1, 1);
    tempPath.update(getFaceAttributeMatrixName().getDataContainerName(), getFaceAttributeMatrixName().getAttributeMatrixName(), getSurfaceMeshTriangleDihedralAnglesArrayName() );
    m_SurfaceMeshTriangleDihedralAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshTriangleDihedralAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshTriangleDihedralAngles = m_SurfaceMeshTriangleDihedralAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::preflight()
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
void TriangleDihedralAngleFilter::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getFaceAttributeMatrixName().getDataContainerName());
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  // No check because datacheck() made sure we can do the next line.
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, triangleGeom->getNumberOfTris()),
                      CalculateDihedralAnglesImpl(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleDihedralAngles), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateDihedralAnglesImpl serial(triangleGeom->getVertices(), triangleGeom->getTriangles(), m_SurfaceMeshTriangleDihedralAngles);
    serial.generate(0, triangleGeom->getNumberOfTris());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer TriangleDihedralAngleFilter::newFilterInstance(bool copyFilterParameters)
{
  TriangleDihedralAngleFilter::Pointer filter = TriangleDihedralAngleFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleDihedralAngleFilter::getCompiledLibraryName()
{ return SurfaceMeshing::SurfaceMeshingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleDihedralAngleFilter::getGroupName()
{ return DREAM3D::FilterGroups::SurfaceMeshingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleDihedralAngleFilter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MiscFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TriangleDihedralAngleFilter::getHumanLabel()
{ return "Find Minimum Triangle Dihedral Angle"; }

