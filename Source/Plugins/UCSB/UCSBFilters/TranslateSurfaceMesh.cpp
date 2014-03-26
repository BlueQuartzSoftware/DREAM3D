/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "TranslateSurfaceMesh.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TranslateSurfaceMesh::TranslateSurfaceMesh() :
  AbstractFilter(),
  m_XShift(0.0f),
  m_YShift(0.0f),
  m_ZShift(0.0f)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TranslateSurfaceMesh::~TranslateSurfaceMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TranslateSurfaceMesh::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Shift");
    option->setPropertyName("XShift");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("microns");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Shift");
    option->setPropertyName("YShift");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("microns");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Shift");
    option->setPropertyName("ZShift");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("microns");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void TranslateSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setXShift( reader->readValue("XShift", getXShift()) );
  setYShift( reader->readValue("YShift", getYShift()) );
  setZShift( reader->readValue("ZShift", getZShift()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TranslateSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("XShift", getXShift() );
  writer->writeValue("YShift", getYShift() );
  writer->writeValue("ZShift", getZShift() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TranslateSurfaceMesh::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TranslateSurfaceMesh::preflight()
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
void TranslateSurfaceMesh::execute()
{
  int err = 0;
  setErrorCondition(err);

  //get mesh
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  dataCheckSurfaceMesh(false, 0, sm->getNumFaceTuples(), 0);

  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = sm->getVertices();
  DREAM3D::SurfaceMesh::Vert_t* nodes = nodesPtr->GetPointer(0);
  size_t numberNodes = nodesPtr->GetNumberOfTuples();

  for(int i=0; i<numberNodes; i++)
  {
    nodes[i].pos[0]=nodes[i].pos[0]+m_XShift;
    nodes[i].pos[1]=nodes[i].pos[1]+m_YShift;
    nodes[i].pos[2]=nodes[i].pos[2]+m_ZShift;
  }

  //update faces + vertexes
  //sm->removeVertexData(DREAM3D::VertexData::SurfaceMeshNodes);
  //sm->setVertices(newNodesPtr);


  notifyStatusMessage("Completed");
}

