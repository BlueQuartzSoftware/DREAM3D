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
m_XMin(0),
m_YMin(0),
m_ZMin(0),
m_XMax(0),
m_YMax(0),
m_ZMax(0),
m_RenumberGrains(false)
{
  setupFilterOptions();
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
void CropVolume::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("X Min");
    option->setPropertyName("XMin");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Y Min");
    option->setPropertyName("YMin");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Z Min");
    option->setPropertyName("ZMin");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("X Max");
    option->setPropertyName("XMax");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Y Max");
    option->setPropertyName("YMax");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Z Max");
    option->setPropertyName("ZMax");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Renumber Grains");
    option->setPropertyName("RenumberGrains");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  setFilterOptions(options);
}
// -----------------------------------------------------------------------------
void CropVolume::writeFilterOptions(AbstractFilterOptionsWriter* writer)
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
void CropVolume::preflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropVolume::execute()
{
  int err = 0;
  setErrorCondition(err);
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  setErrorCondition(0);

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

  // updateProgressAndMessage(("Operating on Volume"), 50);
  if(dims[0] == (m_XMax - m_XMin) && dims[1] == (m_YMax - m_YMin) && dims[2] == (m_ZMax - m_ZMin))
  {
    return;
  }

  float m_XP = (m_XMax - m_XMin)+1;
  float m_YP = (m_YMax - m_YMin)+1;
  float m_ZP = (m_ZMax - m_ZMin)+1;
  float m_XStart = m_XMin * m->getXRes();
  float m_YStart = m_YMin * m->getYRes();
  float m_ZStart = m_ZMin * m->getZRes();

  float x, y, z;
  int col, row, plane;
  int index;
  int index_old;
  std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
  for (int i = 0; i < m_ZP; i++)
  {
    std::stringstream ss;
    ss << "Cropping Volume - " << ((float)i / m->getZPoints()) * 100 << " Percent Complete";
    notify(ss.str(), 0, Observable::UpdateProgressMessage);
    for (int j = 0; j < m_YP; j++)
    {
      for (int k = 0; k < m_XP; k++)
      {
        x = (k * m->getXRes()) + m_XStart;
        y = (j * m->getYRes()) + m_YStart;
        z = (i * m->getZRes()) + m_ZStart;
        col = int(x / m->getXRes());
        row = int(y / m->getYRes());
        plane = int(z / m->getZRes());
        index_old = (plane * m->getXPoints() * m->getYPoints()) + (row * m->getXPoints()) + col;
        index = (i * m_XP * m_YP) + (j * m_XP) + k;
        for (std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          std::string name = *iter;
          IDataArray::Pointer p = m->getCellData(*iter);
          p->CopyTuple(index_old, index);
        }
      }
    }
  }
  m->setDimensions(m_XP, m_YP, m_ZP);
  int64_t totalPoints = m->getTotalPoints();
  totalPoints = m_XP * m_YP * m_ZP;

  for (std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = m->getCellData(*iter);
    err = p->Resize(totalPoints);
  }

  if (m_RenumberGrains == true)
  {
    std::string m_GrainIdsArrayName(DREAM3D::CellData::GrainIds);
    std::stringstream ss;
    int32_t* m_GrainIds = NULL;
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, totalPoints, 1);

    // Find the unique set of grain ids
    std::set<int32_t> grainIdSet;
    for (int64_t i = 0; i < totalPoints; ++i)
    {
      grainIdSet.insert(m_GrainIds[i]);
    }

    size_t fields = grainIdSet.size();
    bool* m_Active;
    std::string m_ActiveArrayName(DREAM3D::FieldData::Active);
    CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1);
    for (size_t i = 0; i < fields; ++i)
    {
      m_Active[i] = true;
    }

    RenumberGrains::Pointer renum = RenumberGrains::New();
    renum->setDataContainer(m);
    renum->setObservers(getObservers());
    renum->execute();
    setErrorCondition(renum->getErrorCondition());
    setErrorMessage(renum->getErrorMessage());
  }


  notify("Completed", 0, Observable::UpdateProgressMessage);
}

