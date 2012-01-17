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

#include "RenumberGrains.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/IDataArray.h"

RenumberGrains::RenumberGrains()
{
  // TODO Auto-generated constructor stub

}

RenumberGrains::~RenumberGrains()
{
  // TODO Auto-generated destructor stub
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberGrains::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
   std::stringstream ss;
   DataContainer* m = getDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, VoxelData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, -306, bool, BoolArrayType, fields, 1);

   setErrorMessage(ss.str());
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberGrains::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberGrains::execute()
{
  setErrorCondition(0);

  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int64_t totalPoints = m->totalPoints();
  int totalFields = m->getTotalFields();
  dataCheck(false, totalPoints, totalFields, m->crystruct.size());

  size_t goodcount = 1;
  std::vector<size_t> NewNames;
  NewNames.resize(totalFields);

  std::vector<size_t> RemoveList;
  for(size_t i = 1; i < totalFields; i++)
  {
	if(m_Active[i] == false)
	{
		RemoveList.push_back(i);
		NewNames[i] = -1;
	}
	if(m_Active[i] == true)
	{
		NewNames[i] = goodcount;
		goodcount++;
	}
  }

  std::list<std::string> headers = m->getFieldArrayNameList();
  for(std::list<std::string>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
      IDataArray::Pointer p = m->getFieldData(*iter);
	  p->EraseTuples(RemoveList);
  }

  for (int i = 0; i < totalPoints; i++)
  {
    if(m_GrainIds[i] > 0) m_GrainIds[i] = NewNames[m_GrainIds[i]];
  }
    
}


