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

#include "ClearData.h"

#include <QtCore/QMap>


#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ClearData::ClearData() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_XMin(0),
  m_YMin(0),
  m_ZMin(0),
  m_XMax(0),
  m_YMax(0),
  m_ZMax(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ClearData::~ClearData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ClearData::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Min");
    option->setPropertyName("XMin");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Column");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Min");
    option->setPropertyName("YMin");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Row");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Min");
    option->setPropertyName("ZMin");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Plane");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Max");
    option->setPropertyName("XMax");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Column");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Max");
    option->setPropertyName("YMax");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Row");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Max");
    option->setPropertyName("ZMax");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Plane");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void ClearData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setXMin( reader->readValue("XMin", getXMin()) );
  setYMin( reader->readValue("YMin", getYMin()) );
  setZMin( reader->readValue("ZMin", getZMin()) );
  setXMax( reader->readValue("XMax", getXMax()) );
  setYMax( reader->readValue("YMax", getYMax()) );
  setZMax( reader->readValue("ZMax", getZMax()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ClearData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("XMin", getXMin() );
  writer->writeValue("YMin", getYMin() );
  writer->writeValue("ZMin", getZMin() );
  writer->writeValue("XMax", getXMax() );
  writer->writeValue("YMax", getYMax() );
  writer->writeValue("ZMax", getZMax() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ClearData::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  QString ss;

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if (getXMax() < getXMin())
  {

    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    addErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getYMax() < getYMin())
  {

    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    addErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getZMax() < getZMin())
  {

    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    addErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getXMin() < 0)
  {

    QString ss = QObject::tr("X Min (%1) less than 0").arg(getXMin());
    addErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getYMin() < 0)
  {

    QString ss = QObject::tr("Y Min (%1) less than 0").arg(getYMin());
    addErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getZMin() < 0)
  {

    QString ss = QObject::tr("Z Min (%1) less than 0").arg(getZMin());
    addErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getXMax() > (static_cast<int64_t>(m->getXPoints())-1))
  {

    QString ss = QObject::tr("The X Max you entered of %1 is greater than your Max X Point of %2").arg(getXMax()).arg(static_cast<int64_t>(m->getXPoints())-1);
    addErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5555);
  }
  if (getYMax() > (static_cast<int64_t>(m->getYPoints())-1))
  {

    QString ss = QObject::tr("The Y Max you entered of %1 is greater than your Max Y Point of %2").arg(getYMax()).arg(static_cast<int64_t>(m->getYPoints())-1);
    addErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5556);
  }
  if (getZMax() > (static_cast<int64_t>(m->getZPoints())-1))
  {

    QString ss = QObject::tr("The Z Max you entered of %1) greater than your Max Z Point of %2").arg(getZMax()).arg(static_cast<int64_t>(m->getZPoints())-1);
    addErrorMessage(getHumanLabel(), ss, -5555);
    setErrorCondition(-5557);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ClearData::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ClearData::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };



  int index;
  QList<QString> voxelArrayNames = m->getCellArrayNameList();
  for (int k = m_ZMin; k < m_ZMax+1; k++)
  {
    for (int j = m_YMin; j < m_YMax+1; j++)
    {
      for (int i = m_XMin; i < m_XMax+1; i++)
      {
        index = (k * dims[0] * dims[1]) + (j * dims[0]) + i;
        for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          QString name = *iter;
          IDataArray::Pointer p = m->getCellData(*iter);
          p->InitializeTuple(index,0);
        }
      }
    }
  }

  notifyStatusMessage("Completed");
}

