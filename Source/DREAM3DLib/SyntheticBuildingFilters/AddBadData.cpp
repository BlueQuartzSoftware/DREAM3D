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

#include "AddBadData.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddBadData::AddBadData() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_GBEuclideanDistancesArrayName(DREAM3D::CellData::GBEuclideanDistances),
  m_PoissonNoise(false),
  m_PoissonVolFraction(0.0f),
  m_BoundaryNoise(false),
  m_BoundaryVolFraction(0.0f),
  m_GBEuclideanDistances(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddBadData::~AddBadData()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Add Random Noise");
    option->setPropertyName("PoissonNoise");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Fraction of Random Noise");
    option->setPropertyName("PoissonVolFraction");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Add Boundary Noise");
    option->setPropertyName("BoundaryNoise");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Fraction of Boundary Noise");
    option->setPropertyName("BoundaryVolFraction");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void AddBadData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setPoissonNoise( reader->readValue("PoissonNoise", getPoissonNoise()) );
  setPoissonVolFraction( reader->readValue("PoissonVolFraction", getPoissonVolFraction()) );
  setBoundaryNoise( reader->readValue("BoundaryNoise", getBoundaryNoise()) );
  setBoundaryVolFraction( reader->readValue("BoundaryVolFraction", getBoundaryVolFraction()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AddBadData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("PoissonNoise", getPoissonNoise() );
  writer->writeValue("PoissonVolFraction", getPoissonVolFraction() );
  writer->writeValue("BoundaryNoise", getBoundaryNoise() );
  writer->writeValue("BoundaryVolFraction", getBoundaryVolFraction() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AddBadData>(getDataContainerName(), false, this);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* am = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  // Cell Data
  QVector<int> dims(1, 1);
  m_GBEuclideanDistancesPtr = am->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_GBEuclideanDistancesArrayName, -302, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GBEuclideanDistancesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GBEuclideanDistances = m_GBEuclideanDistancesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::preflight()
{

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(getDataContainerName(), false, this);
  if(getErrorCondition() < 0) { return; }

  int64_t totalPoints = m->getAttributeMatrix(m_CellAttributeMatrixName)->getNumTuples();

  dataCheck(false, totalPoints, 0, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  add_noise();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("AddBadDatas Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  AddBadData::add_noise()
{
  notifyStatusMessage("Adding Noise");
  DREAM3D_RANDOMNG_NEW()

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QList<QString> voxelArrayNames = m->getAttributeMatrix(m_CellAttributeMatrixName)->getAttributeArrayNameList();

  float random = 0.0;
  int64_t totalPoints = m->getTotalPoints();
  for (size_t i = 0; i < static_cast<size_t>(totalPoints); ++i)
  {
    if(m_BoundaryNoise == true && m_GBEuclideanDistances[i] < 1)
    {
      random = static_cast<float>( rg.genrand_res53() );
      if(random < m_BoundaryVolFraction)
      {
        for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          QString name = *iter;
          IDataArray::Pointer p = m->getAttributeMatrix(m_CellAttributeMatrixName)->getAttributeArray(*iter);
          p->InitializeTuple(i, 0);
        }
      }
    }
    if(m_PoissonNoise == true && m_GBEuclideanDistances[i] >= 1)
    {
      random = static_cast<float>( rg.genrand_res53() );
      if(random < m_PoissonVolFraction)
      {
        for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          QString name = *iter;
          IDataArray::Pointer p = m->getAttributeMatrix(m_CellAttributeMatrixName)->getAttributeArray(*iter);
          p->InitializeTuple(i, 0);
        }
      }
    }
  }
}

