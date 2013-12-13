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

#include "FindKernelAvgMisorientations.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindKernelAvgMisorientations::FindKernelAvgMisorientations() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_KernelAverageMisorientationsArrayName(DREAM3D::CellData::KernelAverageMisorientations),
  m_KernelAverageMisorientations(NULL),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  m_KernelSize.x = 1;
  m_KernelSize.y = 1;
  m_KernelSize.z = 1;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindKernelAvgMisorientations::~FindKernelAvgMisorientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Kernel Radius");
    option->setPropertyName("KernelSize");
    option->setWidgetType(FilterParameter::IntVec3Widget);
    option->setValueType("IntVec3Widget_t");
    option->setUnits("Pixels");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setKernelSize( reader->readIntVec3("KernelSize", getKernelSize() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindKernelAvgMisorientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("KernelSize", getKernelSize() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FindKernelAvgMisorientations>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_CellPhasesArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_KernelAverageMisorientationsPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_KernelAverageMisorientationsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_KernelAverageMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_KernelAverageMisorientations = m_KernelAverageMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_CrystalStructuresArrayName, -305, dims)
                           ; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 4;
  m_QuatsPtr = cellAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_QuatsArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindKernelAvgMisorientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QuatF q1;
  QuatF q2;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  int numVoxel; // number of voxels in the feature...
  int good = 0;

  float w, totalmisorientation;
  float n1, n2, n3;
  unsigned int phase1 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  unsigned int phase2 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType xPoints = static_cast<DimType>(udims[0]);
  DimType yPoints = static_cast<DimType>(udims[1]);
  DimType zPoints = static_cast<DimType>(udims[2]);
  DimType point;
  size_t neighbor = 0;
//  int m_KernelSize = 1;
  DimType jStride;
  DimType kStride;
  for (DimType col = 0; col < xPoints; col++)
  {
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType plane = 0; plane < zPoints; plane++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if (m_FeatureIds[point] > 0 && m_CellPhases[point] > 0)
        {
          totalmisorientation = 0.0;
          numVoxel = 0;
          QuaternionMathF::Copy(quats[point], q1);
          phase1 = m_CrystalStructures[m_CellPhases[point]];
          for (int j = -m_KernelSize.z; j < m_KernelSize.z + 1; j++)
          {
            jStride = j * xPoints * yPoints;
            for (int k = -m_KernelSize.y; k < m_KernelSize.y + 1; k++)
            {
              kStride = k * xPoints;
              for (int l = -m_KernelSize.x; l < m_KernelSize.z + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if(plane + j < 0) { good = 0; }
                else if(plane + j > zPoints - 1) { good = 0; }
                else if(row + k < 0) { good = 0; }
                else if(row + k > yPoints - 1) { good = 0; }
                else if(col + l < 0) { good = 0; }
                else if(col + l > xPoints - 1) { good = 0; }
                if(good == 1 && m_FeatureIds[point] == m_FeatureIds[neighbor])
                {
                  w = 10000.0;
                  QuaternionMathF::Copy(quats[neighbor], q2);
                  phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
                  w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
                  w = w * (180.0f / DREAM3D::Constants::k_Pi);
                  totalmisorientation = totalmisorientation + w;
                  numVoxel++;
                }
              }
            }
          }
          m_KernelAverageMisorientations[point] = totalmisorientation / (float)numVoxel;
          if(numVoxel == 0)
          {
            m_KernelAverageMisorientations[point] = 0;
          }
        }
        if (m_FeatureIds[point] == 0 || m_CellPhases[point] == 0)
        {
          m_KernelAverageMisorientations[point] = 0;
        }
      }
    }
  }

  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "FindKernelAvgMisorientations Completed") );
}

