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

#include "RegularizeZSpacing.h"

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

using namespace std;

//#include "DREAM3DLib/HDF5/H5VoxelReader.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegularizeZSpacing::RegularizeZSpacing() :
AbstractFilter(),
m_XRes(1.0f),
m_YRes(1.0f),
m_ZRes(1.0f)
{
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegularizeZSpacing::~RegularizeZSpacing()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Current Z Positions File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterOption::InputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("New Z Res");
    option->setPropertyName("ZRes");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }

  setFilterOptions(options);
}
// -----------------------------------------------------------------------------
void RegularizeZSpacing::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("ZRes", getZRes() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::preflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);


  if (getErrorCondition() < 0)
  {
    return;
  }

  size_t dims[3];
  m->getDimensions(dims);

  ifstream inFile;
  inFile.open(m_InputFile.c_str());

  float zval;
  std::vector<float> zboundvalues;
  zboundvalues.resize(dims[2]+1,0.0);
  for (size_t iter = 0; iter < dims[2]+1; iter++)
  {
	inFile >> zval;
    zboundvalues[iter] = zval;
  }
  inFile.close();

  m_XRes = m->getXRes();
  m_YRes = m->getYRes();
  float sizex = (dims[0])*m_XRes;
  float sizey = (dims[1])*m_YRes;
  float sizez = zboundvalues[dims[2]];
  int m_XP = dims[0];
  int m_YP = dims[1];
  int m_ZP = int(sizez / m_ZRes);
  int64_t totalPoints = m_XP*m_YP*m_ZP;

  int index, oldindex;
  int plane;
  std::vector<size_t> newindicies;
  newindicies.resize(totalPoints);
  for(size_t i = 0; i < m_ZP; i++)
  {
	  plane = 0;
	  for(size_t iter = 1; iter < dims[2]; iter++)
	  {
		if((i*m_ZRes) > zboundvalues[iter]) plane = iter;
	  }
	  for(size_t j = 0; j < m_YP; j++)
	  {
		  for(size_t k = 0; k < m_XP; k++)
		  {
			  oldindex = (plane * dims[0] * dims[1]) + (j * dims[0]) + k;
			  index = (i * dims[0] * dims[1]) + (j * dims[0]) + k;
			  newindicies[index] = oldindex;
		  }
	  }
  }

  std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
  for(std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = m->getCellData(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with 
    // the same name. At least in theory
    IDataArray::Pointer data = p->createNewArray(p->GetNumberOfTuples(), p->GetNumberOfComponents(), p->GetName());
	data->Resize(totalPoints);
	for(size_t i = 0; i < totalPoints; i++)
	{
//		for(size_t j = 0; j < data->GetNumberOfComponents(); j++)
//		{
			void* source = p->GetVoidPointer((data->GetNumberOfComponents()*newindicies[i]));
			void* destination = data->GetVoidPointer((data->GetNumberOfComponents()*i));
			::memcpy(destination, source, p->GetTypeSize()*data->GetNumberOfComponents());
//		}
	}
	m->addCellData(*iter, data);
  }
  m->setResolution(m_XRes, m_YRes, m_ZRes);
  m->setDimensions(m_XP, m_YP, m_ZP);

  notifyStatusMessage("Changing Resolution Complete");
}
