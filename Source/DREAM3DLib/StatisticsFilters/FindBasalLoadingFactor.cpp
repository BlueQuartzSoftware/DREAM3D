
#include "FindBasalLoadingFactor.h"

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"
#include "DREAM3DLib/GenericFilters/FindCellQuats.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgCAxes.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBasalLoadingFactor::FindBasalLoadingFactor() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_BasalLoadingFactorArrayName(DREAM3D::FeatureData::BasalLoadingFactor),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_BasalLoadingFactor(NULL),
  m_FeaturePhases(NULL),
  m_AvgQuats(NULL)
{
  m_LoadingDir.x = 1.0f;
  m_LoadingDir.y = 1.0f;
  m_LoadingDir.z = 1.0f;
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBasalLoadingFactor::~FindBasalLoadingFactor()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();

    option->setHumanLabel("Loading Direction");
    option->setPropertyName("LoadingDir");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setLoadingDir( reader->readFloatVec3("LoadingDir", getLoadingDir() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindBasalLoadingFactor::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("LoadingDirection", getLoadingDir() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 4);
  m_AvgQuats = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_AvgQuatsArrayName, -301, features, dims);

  dims[0] = 1;
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFeatureData, BasalLoadingFactor, float, FloatArrayType, 0, features, dims)

  m_FeaturePhases = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_FeaturePhasesArrayName, -302, features, dims);

  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructures = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -305, ensembles, dims);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBasalLoadingFactor::execute()
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

  //int ss = 0;
  QuatF q1;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float sampleLoading[3];
  typedef DataArray<unsigned int> XTalType;

  float w;
  float g1[3][3];
  float g1t[3][3];
  float caxis[3] = {0, 0, 1};
  float c1[3];

  sampleLoading[0] = m_LoadingDir.x;
  sampleLoading[1] = m_LoadingDir.y;
  sampleLoading[2] = m_LoadingDir.z;
  MatrixMath::Normalize3x1(sampleLoading);

  size_t numfeatures = m->getNumCellFeatureTuples();
  for (size_t i = 1; i < numfeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    OrientationMath::QuattoMat(q1, g1);
    //transpose the g matricies so when caxis is multiplied by it
    //it will give the sample direction that the caxis is along
    MatrixMath::Transpose3x3(g1, g1t);
    MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
    //normalize so that the magnitude is 1
    MatrixMath::Normalize3x1(c1);
    if(c1[2] < 0) { MatrixMath::Multiply3x1withConstant(c1, -1); }
    w = GeometryMath::CosThetaBetweenVectors(c1, sampleLoading);
    w = acos(w);
    w *= DREAM3D::Constants::k_180OverPi;
    m_BasalLoadingFactor[i] = w;
  }

  notifyStatusMessage("FindBasalLoadingFactor Completed");
}
