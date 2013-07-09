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

#include "CropVolume.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVolume::CropVolume() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_XMin(0),
  m_YMin(0),
  m_ZMin(0),
  m_XMax(0),
  m_YMax(0),
  m_ZMax(0),
  m_RenumberGrains(true),
  m_UpdateOrigin(true),
  m_GrainIds(NULL),
  m_Active(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CropVolume::~CropVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Min (Voxels)");
    option->setPropertyName("XMin");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Column");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Min (Voxels)");
    option->setPropertyName("YMin");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Row");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Min (Voxels)");
    option->setPropertyName("ZMin");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Plane");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Max (Voxels)");
    option->setPropertyName("XMax");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Column");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Max (Voxels)");
    option->setPropertyName("YMax");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Row");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Max (Voxels)");
    option->setPropertyName("ZMax");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Plane");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Renumber Grains");
    option->setPropertyName("RenumberGrains");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Update Origin");
    option->setPropertyName("UpdateOrigin");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void CropVolume::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("XMin", getXMin() );
  writer->writeValue("YMin", getYMin() );
  writer->writeValue("ZMin", getZMin() );
  writer->writeValue("XMax", getXMax() );
  writer->writeValue("YMax", getYMax() );
  writer->writeValue("ZMax", getZMax() );
  writer->writeValue("RenumberGrains", getRenumberGrains() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
     addErrorMessage(getHumanLabel(), "The DataContainer Object was NULL", -999);
    return;
  }
  if (m_RenumberGrains == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)
    CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::preflight()
{
  dataCheck(true, 1, 1, 1);

  VoxelDataContainer* m = getVoxelDataContainer();
  setErrorCondition(0);
  std::stringstream ss;
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  if (getXMax() < getXMin())
  {
    ss.str("");
    ss << "X Max (" << getXMax() << ") less than X Min (" << getXMin() << ")";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getYMax() < getYMin())
  {
    ss.str("");
    ss << "Y Max (" << getYMax() << ") less than Y Min (" << getYMin() << ")";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getZMax() < getZMin())
  {
    ss.str("");
    ss << "Z Max (" << getZMax() << ") less than Z Min (" << getZMin() << ")";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getXMin() < 0)
  {
    ss.str("");
    ss << "X Min (" << getXMin() << ") less than 0";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getYMin() < 0)
  {
    ss.str("");
    ss << "Y Min (" << getYMin() << ") less than 0";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getZMin() < 0)
  {
    ss.str("");
    ss <<"Z Min (" << getZMin() << ") less than 0";
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getXMax() > (static_cast<int64_t>(m->getXPoints())-1))
  {
    ss.str("");
    ss << "The X Max you entered of " << getXMax() << " is greater than your Max X Point of " << static_cast<int64_t>(m->getXPoints())-1;
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5555);
  }
  if (getYMax() > (static_cast<int64_t>(m->getYPoints())-1))
  {
    ss.str("");
    ss << "The Y Max you entered of " << getYMax() << " is greater than your Max Y Point of " << static_cast<int64_t>(m->getYPoints())-1;
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5556);
  }
  if (getZMax() > (static_cast<int64_t>(m->getZPoints())-1))
  {
    ss.str("");
    ss << "The Z Max you entered of " << getZMax() << ") greater than your Max Z Point of " << static_cast<int64_t>(m->getZPoints())-1;
    addErrorMessage(getHumanLabel(), ss.str(), -5555);
    setErrorCondition(-5557);
  }

  m->setDimensions((getXMax()-getXMin())-1,(getYMax()-getYMin())-1,(getZMax()-getZMin())-1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
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

  // Check to see if the dims have actually changed.
  if(dims[0] == (m_XMax - m_XMin) && dims[1] == (m_YMax - m_YMin) && dims[2] == (m_ZMax - m_ZMin))
  {
    return;
  }

  std::stringstream ss;
  // Check to make sure the new dimensions are not "out of bounds" and warn the user if they are
  if (dims[0] <= m_XMax)
  {
    ss << "A Maximum value of " << m_XMax << " has been entered for the Max X which is larger than the input volume X Dimension of " << dims[0]
       << " This may lead to junk data being filled into the extra space.";
    notifyWarningMessage(ss.str(), -950);
  }
  if (dims[1] <= m_YMax)
  {
    ss.str("");
    ss << "A Maximum value of " << m_YMax << " has been entered for the Max Y which is larger than the input volume Y Dimension of " << dims[1]
       << " This may lead to junk data being filled into the extra space.";
    notifyWarningMessage(ss.str(), -950);
  }
  if (dims[2] <= m_ZMax)
  {
    ss.str("");
    ss << "A Maximum value of " << m_ZMax << " has been entered for the Max Z which is larger than the input volume Z Dimension of " << dims[2]
       << " This may lead to junk data being filled into the extra space.";
    notifyWarningMessage(ss.str(), -950);
  }

  int m_XP = ( (m_XMax - m_XMin)+1 );
  int m_YP = ( (m_YMax - m_YMin)+1 );
  int m_ZP = ( (m_ZMax - m_ZMin)+1 );

  int col, row, plane;
  int index;
  int index_old;
  std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
  for (int i = 0; i < m_ZP; i++)
  {
    std::stringstream ss;
    ss << "Cropping Volume - Slice " << i << " of " << m_ZP <<  " Complete";
    notifyStatusMessage(ss.str());
    for (int j = 0; j < m_YP; j++)
    {
      for (int k = 0; k < m_XP; k++)
      {
        col = k + m_XMin;
        row = j + m_YMin;
        plane = i + m_ZMin;
        index_old = (plane * m->getXPoints() * m->getYPoints()) + (row * m->getXPoints()) + col;
        index = ( (i * m_XP * m_YP) + (j * m_XP) + k );
        for (std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          std::string name = *iter;
          IDataArray::Pointer p = m->getCellData(*iter);
          p->CopyTuple(index_old, index);
        }
      }
    }
  }
  m->setDimensions(static_cast<size_t>(m_XP), static_cast<size_t>(m_YP), static_cast<size_t>(m_ZP));
  int64_t totalPoints = m->getTotalPoints();
  totalPoints = static_cast<int64_t>( m_XP * m_YP * m_ZP );


  // Resize all the other Voxel Arrays
  for (std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = m->getCellData(*iter);
    err = p->Resize(totalPoints);
  }

  // Grain Ids MUST already be renumbered.
  if (m_RenumberGrains == true)
  {
    totalPoints = m->getTotalPoints();
    size_t totalFields = m->getNumFieldTuples();
    if (0 == totalFields)
    {
      notifyErrorMessage("The number of grains is Zero and should be greater than Zero", -600);
      notifyStatusMessage("Completed");
      return;
    }
    dataCheck(false, totalPoints, totalFields, m->getNumEnsembleTuples());

    std::stringstream ss;

    // Find the unique set of grain ids
    for (size_t i = 1; i < totalFields; ++i)
    {
      m_Active[i] = false;
    }
    for (int64_t i = 0; i < totalPoints; ++i)
    {
      m_Active[m_GrainIds[i]] = true;
    }

    RenumberGrains::Pointer renum = RenumberGrains::New();
    renum->setVoxelDataContainer(m);
    renum->setObservers(getObservers());
    renum->setMessagePrefix(getMessagePrefix());
    renum->execute();
    setErrorCondition(renum->getErrorCondition());
    addErrorMessages(renum->getPipelineMessages());
  }

  if(m_UpdateOrigin == true)
  {
    float resolution[3] = {0.0f, 0.0f, 0.0f};
    m->getResolution(resolution);

    float origin[3] = {0.0f, 0.0f, 0.0f};
    m->getOrigin(origin);
    origin[0] = m_XMin * resolution[0];
    origin[1] = m_YMin * resolution[1];
    origin[2] = m_ZMin * resolution[2];

    m->setOrigin(origin);
  }


  notifyStatusMessage("Completed");
}

