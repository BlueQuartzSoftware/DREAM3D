/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SegmentBoundaries.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SegmentBoundaries::SegmentBoundaries() :
  AbstractFilter(),
  m_BoundaryIdsArrayName(DREAM3D::FaceData::BoundaryIds),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_BoundaryIds(NULL),
  m_SurfaceMeshFaceLabels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SegmentBoundaries::~SegmentBoundaries()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentBoundaries::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void SegmentBoundaries::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SegmentBoundaries::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentBoundaries::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-383);
  }
  else
  {
    // We MUST have Triangles
    if(sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -385);
      setErrorCondition(-385);
    }
    else
    {
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceLabels, ss, -386, int32_t, Int32ArrayType, fields, 2)
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, BoundaryIds, ss, int32_t, Int32ArrayType, 0, fields, 1)
    }
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentBoundaries::preflight()
{
  dataCheckSurfaceMesh(true, 1, 1, 1);

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  setErrorCondition(0);
  std::stringstream ss;
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SegmentBoundaries::execute()
{
  int err = 0;
  setErrorCondition(err);

  //get mesh
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  dataCheckSurfaceMesh(false, 0, sm->getNumFaceTuples(), 0);

  DREAM3D::SurfaceMesh::FaceListPointer_t facesPtr = sm->getFaces();
  DREAM3D::SurfaceMesh::Face_t* faces = facesPtr->GetPointer(0);
  size_t numberFaces = facesPtr->GetNumberOfTuples();

  int maxGrainID=0;

  for(int i=0; i<numberFaces; ++i)
  {
    int faceMax=std::max(m_SurfaceMeshFaceLabels[2*i],m_SurfaceMeshFaceLabels[2*i+1]);
    maxGrainID=std::max(maxGrainID, faceMax);
  }

  notifyStatusMessage("Finding boundary pairs");

  std::vector< std::vector <int32_t> > neighborlist (maxGrainID+1);
  for(int i=0; i<numberFaces; ++i)
  {
    int32_t minID=std::min(m_SurfaceMeshFaceLabels[2*i],m_SurfaceMeshFaceLabels[2*i+1]);
    int32_t maxID=std::max(m_SurfaceMeshFaceLabels[2*i],m_SurfaceMeshFaceLabels[2*i+1]);
    if(minID>0)
    {
      neighborlist[minID].push_back(maxID);
    }
  }

  notifyStatusMessage("Calculating boundary IDs");

  int32_t numberIDs=1;//reserve boundary id 0
  std::vector<int32_t> grainPairs;
  for(std::vector< std::vector <int32_t> >::iterator iter=neighborlist.begin(); iter!=neighborlist.end(); ++iter)
  {
    std::sort((*iter).begin(), (*iter).end());
    std::vector<int32_t>::iterator it = std::unique((*iter).begin(), (*iter).end());
    (*iter).resize( std::distance((*iter).begin(),it) );
    grainPairs.push_back(numberIDs);
    numberIDs=numberIDs+(*iter).size();
  }

  notifyStatusMessage("Assigning boundary IDs");

  for(int i=0; i<numberFaces; ++i)
  {
    m_BoundaryIds[i]=0;
    int32_t minID=std::min(m_SurfaceMeshFaceLabels[2*i],m_SurfaceMeshFaceLabels[2*i+1]);
    int32_t maxID=std::max(m_SurfaceMeshFaceLabels[2*i],m_SurfaceMeshFaceLabels[2*i+1]);
    if(minID>0)
    {
      std::vector<int32_t>::iterator it = std::find(neighborlist[minID].begin(), neighborlist[minID].end(), maxID);
      if(it!=neighborlist[minID].end())
      {
        int32_t offset = std::distance(neighborlist[minID].begin(), it);
        m_BoundaryIds[i]=grainPairs[minID]+offset;
      }
    }
  }

  notifyStatusMessage("Completed");
}
