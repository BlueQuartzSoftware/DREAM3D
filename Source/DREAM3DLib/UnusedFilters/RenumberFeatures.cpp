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

#include "RenumberFeatures.h"

#include <sstream>

#include <QtCore/QVector>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenumberFeatures::RenumberFeatures() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_Active(NULL)
{
  //  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenumberFeatures::~RenumberFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Name of Array for Active Features");
    parameter->setPropertyName("ActiveArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  ////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenumberFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, RenumberFeatures>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);} /* Now assign the raw pointer to data from the DataArray<T> object */
  m_ActivePtr = cellFeatureAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_ActiveArrayName, -306, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {m_Active = m_ActivePtr.lock()->getPointer(0);} /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::preflight()
{
  dataCheck();

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QList<QString> headers = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArrayNameList();
  for (QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    IDataArray::Pointer p = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(*iter);
    QString type = p->getTypeAsString();
    if(type.compare("NeighborList<T>") == 0) { m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->removeAttributeArray(*iter);}
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  QString ss;
  size_t goodcount = 1;
  QVector<size_t> NewNames(totalFeatures, 0);

  ss = QObject::tr(" - Generating Active Feature List");
  notifyStatusMessage(getHumanLabel(), ss);
  QVector<size_t> RemoveList;
  for(size_t i = 1; i < totalFeatures; i++)
  {
    if(m_Active[i] == false)
    {
      RemoveList.push_back(i);
      NewNames[i] = 0;
    }
    else
    {
      NewNames[i] = goodcount;
      goodcount++;
    }
  }

  if(RemoveList.size() > 0)
  {
    QList<QString> headers = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArrayNameList();
    for (QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
    {
      IDataArray::Pointer p = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getAttributeArray(*iter);

      ss = QObject::tr("Updating Feature Array '%1'").arg(*iter);
      notifyStatusMessage(getHumanLabel(), ss);
      QString type = p->getTypeAsString();
      if(type.compare("NeighborList<T>") == 0)
      {
        m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->removeAttributeArray(*iter);
      }
      else
      {
        p->eraseTuples(RemoveList);
      }
    }
    QVector<size_t> tDims(1, (totalFeatures - RemoveList.size()));
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->setTupleDimensions(tDims);
    dataCheck();

    // Loop over all the points and correct all the feature names
    ss = QObject::tr("Renumbering Cell Region Ids");
    notifyStatusMessage(getHumanLabel(), ss);
    for (int i = 0; i < totalPoints; i++)
    {
      if(m_FeatureIds[i] > 0)
      {
        m_FeatureIds[i] = static_cast<int32_t>( NewNames[m_FeatureIds[i]] );
      }
    }
  }
  notifyStatusMessage(getHumanLabel(), "Complete");
}



