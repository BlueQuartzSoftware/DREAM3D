/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "EstablishMatrixPhase.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/DataContainers/DataContainerMacros.h"

#include "DREAM3DLib/StatsData/MatrixStatsData.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishMatrixPhase::EstablishMatrixPhase() :
AbstractFilter(),
m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_ActiveArrayName(DREAM3D::FieldData::Active),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
m_GrainIds(NULL),
m_CellPhases(NULL),
m_Active(NULL),
m_FieldPhases(NULL),
m_PhaseTypes(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishMatrixPhase::~EstablishMatrixPhase()
{
}
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int EstablishMatrixPhase::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  // Cell Data
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, -301, int32_t, Int32ArrayType, voxels, 1)

  // Field Data
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldPhases,  int32_t, Int32ArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, Active, bool, BoolArrayType, false, fields, 1)

  //Ensemble Data
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, PhaseTypes, -301, unsigned int, PhaseTypeArrayType, ensembles, 1)
  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getCellEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    QString ss = QObject::tr("Stats Array Not Initialized Correctly");
    setErrorCondition(-308);
    addErrorMessage(getHumanLabel(), ss, -308);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishMatrixPhase::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumCellFieldTuples();

  if(totalFields == 0) totalFields = 1;
  dataCheck(false, totalPoints, totalFields, m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  establish_matrix();

  // If there is an error set this to something negative and also set a message
 notifyStatusMessage("EstablishMatrixPhases Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  EstablishMatrixPhase::establish_matrix()
{
 notifyStatusMessage("Establishing Matrix");
  DREAM3D_RANDOMNG_NEW()

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]),
      static_cast<DimType>(udims[1]),
      static_cast<DimType>(udims[2]), };


  sizex = dims[0] * m->getXRes();
  sizey = dims[1] * m->getYRes();
  sizez = dims[2] * m->getZRes();
  totalvol = sizex*sizey*sizez;

  int64_t totalPoints = m->getTotalPoints();
  size_t currentnumgrains = m->getNumCellFieldTuples();
  if(currentnumgrains == 0)
  {
    m->resizeCellFieldDataArrays(1);
    dataCheck(false, totalPoints, 1, m->getNumCellEnsembleTuples());
    currentnumgrains = 1;
  }
  firstMatrixField = currentnumgrains;
  // size_t index;
//  int phase;
  float random;
//  int random2;
//  float xc, yc, zc;
  float totalmatrixfractions = 0.0f;

  size_t numensembles = m->getNumCellEnsembleTuples();

  for (size_t i = 1; i < numensembles; ++i)
  {
  if(m_PhaseTypes[i] == DREAM3D::PhaseType::MatrixPhase)
    {
    MatrixStatsData* mp = MatrixStatsData::SafePointerDownCast(statsDataArray[i].get());
      matrixphases.push_back(i);
      matrixphasefractions.push_back(mp->getPhaseFraction());
      totalmatrixfractions = totalmatrixfractions + mp->getPhaseFraction();
    }
  }
  for (size_t i = 0; i < matrixphases.size(); i++)
  {
    matrixphasefractions[i] = matrixphasefractions[i] / totalmatrixfractions;
    if(i > 0) matrixphasefractions[i] = matrixphasefractions[i] + matrixphasefractions[i - 1];
  }
  size_t j = 0;
  for (size_t i = 0; i < static_cast<size_t>(totalPoints); ++i)
  {
    if(m_GrainIds[i] <= 0)
    {
    random = static_cast<float>( rg.genrand_res53() );
    j = 0;
    while(random > matrixphasefractions[j])
    {
      j++;
    }
    if(m->getNumCellFieldTuples() <= (firstMatrixField+j))
    {
        m->resizeCellFieldDataArrays((firstMatrixField+j) + 1);
        dataCheck(false, totalPoints, (firstMatrixField+j) + 1, m->getNumCellEnsembleTuples());
    }
    m_GrainIds[i] = (firstMatrixField+j);
    m_CellPhases[i] = matrixphases[j];
      m_Active[(firstMatrixField+j)] = true;
      m_FieldPhases[(firstMatrixField+j)] = matrixphases[j];
    }
  }
}

