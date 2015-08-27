/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "InitializeData.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/IntFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "Processing/ProcessingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeData::InitializeData() :
  AbstractFilter(),
  m_CellAttributeMatrixPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
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
InitializeData::~InitializeData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::DataStructureRequirements req = AttributeMatrixSelectionFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", getCellAttributeMatrixPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(IntFilterParameter::New("X Min (Column)", "XMin", getXMin(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Y Min (Row)", "YMin", getYMin(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Z Min (Plane)", "ZMin", getZMin(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("X Max (Column)", "XMax", getXMax(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Y Max (Row)", "YMax", getYMax(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Z Max (Plane)", "ZMax", getZMax(), FilterParameter::Parameter));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixPath( reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath() ) );
  setXMin( reader->readValue("XMin", getXMin()) );
  setYMin( reader->readValue("YMin", getYMin()) );
  setZMin( reader->readValue("ZMin", getZMin()) );
  setXMax( reader->readValue("XMax", getXMax()) );
  setYMax( reader->readValue("YMax", getYMax()) );
  setZMax( reader->readValue("ZMax", getZMax()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InitializeData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(XMin)
  DREAM3D_FILTER_WRITE_PARAMETER(YMin)
  DREAM3D_FILTER_WRITE_PARAMETER(ZMin)
  DREAM3D_FILTER_WRITE_PARAMETER(XMax)
  DREAM3D_FILTER_WRITE_PARAMETER(YMax)
  DREAM3D_FILTER_WRITE_PARAMETER(ZMax)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getCellAttributeMatrixPath(), -301);

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName());
  if(NULL == image.get()) { return; }

  if (getXMax() < getXMin())
  {
    QString ss = QObject::tr("X Max (%1) less than X Min (%2)").arg(getXMax()).arg(getXMin());
    setErrorCondition(-5551);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getYMax() < getYMin())
  {
    QString ss = QObject::tr("Y Max (%1) less than Y Min (%2)").arg(getYMax()).arg(getYMin());
    setErrorCondition(-5552);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getZMax() < getZMin())
  {
    QString ss = QObject::tr("Z Max (%1) less than Z Min (%2)").arg(getZMax()).arg(getZMin());
    setErrorCondition(-5553);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getXMin() < 0)
  {
    QString ss = QObject::tr("X Min (%1) less than 0").arg(getXMin());
    setErrorCondition(-5554);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getYMin() < 0)
  {
    QString ss = QObject::tr("Y Min (%1) less than 0").arg(getYMin());
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getZMin() < 0)
  {
    QString ss = QObject::tr("Z Min (%1) less than 0").arg(getZMin());
    setErrorCondition(-5556);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getXMax() > (static_cast<int64_t>(image->getXPoints()) - 1))
  {
    QString ss = QObject::tr("The X Max you entered of %1 is greater than your Max X Point of %2").arg(getXMax()).arg(static_cast<int64_t>(image->getXPoints()) - 1);
    setErrorCondition(-5557);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getYMax() > (static_cast<int64_t>(image->getYPoints()) - 1))
  {
    QString ss = QObject::tr("The Y Max you entered of %1 is greater than your Max Y Point of %2").arg(getYMax()).arg(static_cast<int64_t>(image->getYPoints()) - 1);
    setErrorCondition(-5558);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getZMax() > (static_cast<int64_t>(image->getZPoints()) - 1))
  {
    QString ss = QObject::tr("The Z Max you entered of %1) greater than your Max Z Point of %2").arg(getZMax()).arg(static_cast<int64_t>(image->getZPoints()) - 1);
    setErrorCondition(-5559);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeData::preflight()
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
void InitializeData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_CellAttributeMatrixPath.getDataContainerName());

  size_t udims[3] =
  { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };

  int index;
  QString attrMatName = m_CellAttributeMatrixPath.getAttributeMatrixName();
  QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();
  for (int32_t k = m_ZMin; k < m_ZMax + 1; k++)
  {
    for (int32_t j = m_YMin; j < m_YMax + 1; j++)
    {
      for (int32_t i = m_XMin; i < m_XMax + 1; i++)
      {
        index = (k * dims[0] * dims[1]) + (j * dims[0]) + i;
        for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
          p->initializeTuple(index, 0);
        }
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InitializeData::newFilterInstance(bool copyFilterParameters)
{
  InitializeData::Pointer filter = InitializeData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getCompiledLibraryName()
{ return ProcessingConstants::ProcessingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CleanupFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeData::getHumanLabel()
{ return "Initialize Data"; }
