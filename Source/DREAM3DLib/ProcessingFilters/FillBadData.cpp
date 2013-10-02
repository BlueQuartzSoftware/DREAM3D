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

#include "FillBadData.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"



#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FillBadData::FillBadData() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_MinAllowedDefectSize(1),
m_AlreadyChecked(NULL),
m_Neighbors(NULL),
m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FillBadData::~FillBadData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Minimum Allowed Defect Size");
    option->setPropertyName("MinAllowedDefectSize");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
	option->setUnits("Pixels");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FillBadData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMinAllowedDefectSize( reader->readValue("MinAllowedDefectSize", getMinAllowedDefectSize()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FillBadData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MinAllowedDefectSize", getMinAllowedDefectSize() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FillBadData::execute()
{
  setErrorCondition(0);
 // int err = 0;
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }


  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0 && getErrorCondition() != -305)
  {
    return;
  }
  setErrorCondition(0);

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->GetPointer(0);
  neighborsPtr->initializeWithValues(-1);

  BoolArrayType::Pointer alreadCheckedPtr = BoolArrayType::CreateArray(totalPoints, "AlreadyChecked");
  m_AlreadyChecked = alreadCheckedPtr->GetPointer(0);
  alreadCheckedPtr->initializeWithZeros();

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  std::vector<int > neighs;
  std::vector<int > remove;
  size_t count = 1;
  int good = 1;
  int neighbor;
  int index = 0;
  float x, y, z;
  DimType column, row, plane;
  int neighpoint;
  size_t numgrains = m->getNumFieldTuples();

  int neighpoints[6];
  neighpoints[0] = static_cast<int>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<int>(-dims[0]);
  neighpoints[2] = static_cast<int>(-1);
  neighpoints[3] = static_cast<int>(1);
  neighpoints[4] = static_cast<int>(dims[0]);
  neighpoints[5] = static_cast<int>(dims[0] * dims[1]);
  std::vector<int> currentvlist;

  for (int64_t iter = 0; iter < totalPoints; iter++)
  {
    m_AlreadyChecked[iter] = false;
	if (m_GrainIds[iter] != 0) m_AlreadyChecked[iter] = true;
  }
  for (int64_t i = 0; i < totalPoints; i++)
  {
		if(m_AlreadyChecked[i] == false && m_GrainIds[i] == 0)
		{
			currentvlist.push_back( static_cast<int>(i) );
			count = 0;
			while(count < currentvlist.size())
			{
				index = currentvlist[count];
				column = index % dims[0];
				row = (index / dims[0]) % dims[1];
				plane = index / (dims[0] * dims[1]);
				for (DimType j = 0; j < 6; j++)
				{
					good = 1;
					neighbor = index + neighpoints[j];
					if (j == 0 && plane == 0) good = 0;
					if (j == 5 && plane == (dims[2] - 1)) good = 0;
					if (j == 1 && row == 0) good = 0;
					if (j == 4 && row == (dims[1] - 1)) good = 0;
					if (j == 2 && column == 0) good = 0;
					if (j == 3 && column == (dims[0] - 1)) good = 0;
					if (good == 1 && m_GrainIds[neighbor] == 0 && m_AlreadyChecked[neighbor] == false)
					{
						currentvlist.push_back(neighbor);
						m_AlreadyChecked[neighbor] = true;
					}
				}
				count++;
			}
			if((int)currentvlist.size() >= m_MinAllowedDefectSize)
			{
				for (size_t k = 0; k < currentvlist.size(); k++)
				{
					m_GrainIds[currentvlist[k]] = 0;
				}
			}
			if((int)currentvlist.size() < m_MinAllowedDefectSize)
			{
				for (size_t k = 0; k < currentvlist.size(); k++)
				{
					m_GrainIds[currentvlist[k]] = -1;
				}
			}
			currentvlist.clear();
		}
  }

  int grainname, grain;
  int current = 0;
  int most = 0;

  std::vector<int > n(numgrains + 1,0);
  while (count != 0)
  {
    count = 0;
    for (int i = 0; i < totalPoints; i++)
    {
      grainname = m_GrainIds[i];
      if (grainname < 0)
      {
        count++;
		current = 0;
		most = 0;
		x = static_cast<float>(i % dims[0]);
        y = static_cast<float>((i / dims[0]) % dims[1]);
        z = static_cast<float>(i / (dims[0] * dims[1]));
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = i + neighpoints[j];
          if (j == 0 && z == 0) good = 0;
          if (j == 5 && z == (dims[2] - 1)) good = 0;
          if (j == 1 && y == 0) good = 0;
          if (j == 4 && y == (dims[1] - 1)) good = 0;
          if (j == 2 && x == 0) good = 0;
          if (j == 3 && x == (dims[0] - 1)) good = 0;
          if (good == 1)
          {
            grain = m_GrainIds[neighpoint];
			if (grain > 0)
            {
				n[grain]++;
				current = n[grain];
				if (current > most)
				{
					most = current;
					m_Neighbors[i] = neighpoint;
				}
			}
		  }
		}
		for (int l = 0; l < 6; l++)
		{
		  good = 1;
          neighpoint = i + neighpoints[l];
		  if (l == 0 && z == 0) good = 0;
		  if (l == 5 && z == (dims[2] - 1)) good = 0;
		  if (l == 1 && y == 0) good = 0;
		  if (l == 4 && y == (dims[1] - 1)) good = 0;
		  if (l == 2 && x == 0) good = 0;
		  if (l == 3 && x == (dims[0] - 1)) good = 0;
		  if (good == 1)
		  {
			grain = m_GrainIds[neighpoint];
			if(grain > 0) n[grain] = 0;
		  }
		}
      }
    }
    std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
    for (size_t j = 0; j < totalPoints; j++)
    {
      grainname = m_GrainIds[j];
      neighbor = m_Neighbors[j];
      if (grainname < 0 && neighbor != -1 && m_GrainIds[neighbor] > 0)
      {
          for(std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            std::string name = *iter;
            IDataArray::Pointer p = m->getCellData(*iter);
            p->CopyTuple(neighbor, j);
          }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
 notifyStatusMessage("Filling Bad Data Complete");
}
