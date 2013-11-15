#include "FindFeatureNeighborCAxisMisalignments.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceFeatures.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureNeighborCAxisMisalignments::FindFeatureNeighborCAxisMisalignments()  :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_NeighborListArrayName(DREAM3D::FeatureData::NeighborList),
  m_CAxisMisalignmentListArrayName(DREAM3D::FeatureData::CAxisMisalignmentList),
  m_AvgCAxisMisalignmentsArrayName(DREAM3D::FeatureData::AvgCAxisMisalignments),
  m_AvgQuats(NULL),
  m_FeaturePhases(NULL),
  m_NeighborList(NULL),
  m_CAxisMisalignmentList(NULL),
  m_AvgCAxisMisalignments(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureNeighborCAxisMisalignments::~FindFeatureNeighborCAxisMisalignments()
{
}
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int FindFeatureNeighborCAxisMisalignments::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  QString ss;
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 4);
  // Feature Data
  GET_PREREQ_DATA(m, DREAM3D, CellFeatureData, AvgQuats, -301, float, FloatArrayType, features, dims)
  dims[0] = 1;
  GET_PREREQ_DATA(m, DREAM3D, CellFeatureData, FeaturePhases, -303, int32_t, Int32ArrayType, features, dims)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFeatureData, AvgCAxisMisalignments, float, FloatArrayType, 0, features, dims)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, CrystalStructures, -305, unsigned int, XTalStructArrayType, ensembles, dims)

  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  IDataArray::Pointer neighborListPtr = m->getCellFeatureData(m_NeighborListArrayName);
  if (NULL == neighborListPtr.get())
  {
    ss = QObject::tr("NeighborLists are not available and are required for this filter to run. A filter that generates NeighborLists needs to be placed before this filter in the pipeline.");
    setErrorCondition(-305);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>*>(neighborListPtr.get());
  }

  IDataArray::Pointer misalignmentPtr = m->getCellFeatureData(m_CAxisMisalignmentListArrayName);
  if(NULL == misalignmentPtr.get())
  {
    NeighborList<float>::Pointer misalignmentListPtr = NeighborList<float>::New();
    misalignmentListPtr->SetName(m_CAxisMisalignmentListArrayName);
    misalignmentListPtr->Resize(features);
    m->addCellFeatureData(m_CAxisMisalignmentListArrayName, misalignmentListPtr);
    m_CAxisMisalignmentList = misalignmentListPtr.get();
    if (misalignmentListPtr.get() == NULL)
    {
      ss = QObject::tr("MisalignmentLists Array Not Initialized correctly");
      setErrorCondition(-308);
      addErrorMessage(getHumanLabel(), ss, -308);
    }
  }
  else
  {
    m_CAxisMisalignmentList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>(misalignmentPtr.get());
    m_CAxisMisalignmentList->Resize(features);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::preflight()
{
  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureNeighborCAxisMisalignments::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumCellFeatureTuples(), m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *m_NeighborList;

  std::vector<std::vector<float> > misalignmentlists;

  float w;
  float g1[3][3];
  float g2[3][3];
  float g1t[3][3];
  float g2t[3][3];
  float c1[3];
  float c2[3];
  float caxis[3] = {0, 0, 1};
  float neighMisalTot = 0.0f;
  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);


  size_t numfeatures = m->getNumCellFeatureTuples();
  unsigned int phase1, phase2;

  float radToDeg = 180.0 / DREAM3D::Constants::k_Pi;

  size_t nname;
  // float nsa;
  misalignmentlists.resize(numfeatures);
  for (size_t i = 1; i < numfeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    phase1 = m_CrystalStructures[m_FeaturePhases[i]];
    OrientationMath::QuattoMat(q1, g1);
    //transpose the g matrix so when caxis is multiplied by it
    //it will give the sample direction that the caxis is along
    MatrixMath::Transpose3x3(g1, g1t);
    MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
    //normalize so that the dot product can be taken below without
    //dividing by the magnitudes (they would be 1)
    MatrixMath::Normalize3x1(c1);
    misalignmentlists[i].resize(neighborlist[i].size(), -1.0);
    for (size_t j = 0; j < neighborlist[i].size(); j++)
    {
      w = 10000.0;
      nname = neighborlist[i][j];
      phase2 = m_CrystalStructures[m_FeaturePhases[nname]];
      if (phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High) )
      {
        QuaternionMathF::Copy(avgQuats[nname], q2);
        OrientationMath::QuattoMat(q2, g2);
        //transpose the g matrix so when caxis is multiplied by it
        //it will give the sample direction that the caxis is along
        MatrixMath::Transpose3x3(g2, g2t);
        MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);
        //normalize so that the dot product can be taken below without
        //dividing by the magnitudes (they would be 1)
        MatrixMath::Normalize3x1(c2);

        w = GeometryMath::CosThetaBetweenVectors(c1, c2);
        DREAM3DMath::boundF(w, -1, 1);
        w = acosf(w);
        misalignmentlists[i][j] = w * DREAM3D::Constants::k_180OverPi;
      }
      else
      {
        misalignmentlists[i][j] = -100.0f;
      }
      neighMisalTot += misalignmentlists[i][j];
    }
    m_AvgCAxisMisalignments[i] = neighMisalTot / neighborlist[i].size();
    neighMisalTot = 0.0f;
  }

  // We do this to create new set of MisalignmentList objects
  dataCheck(false, m->getNumCellTuples(), m->getNumCellFeatureTuples(), m->getNumCellEnsembleTuples());

  for (size_t i = 1; i < m->getNumCellFeatureTuples(); i++)
  {
    // Set the vector for each list into the NeighborList Object
    NeighborList<float>::SharedVectorType misaL(new std::vector<float>);
    misaL->assign(misalignmentlists[i].begin(), misalignmentlists[i].end());
    m_CAxisMisalignmentList->setList(static_cast<int>(i), misaL);
  }

  notifyStatusMessage("FindFeatureNeighborCAxisMisalignments Completed");
}

