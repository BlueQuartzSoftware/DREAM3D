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

#include "ChangeResolution.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

//#include "DREAM3DLib/HDF5/H5VoxelReader.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeResolution::ChangeResolution() :
AbstractFilter(),
m_XRes(1.0f),
m_YRes(1.0f),
m_ZRes(1.0f)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeResolution::~ChangeResolution()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("X Res");
    option->setPropertyName("XRes");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
	option->setUnits("Microns");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Y Res");
    option->setPropertyName("YRes");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
	option->setUnits("Microns");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Z Res");
    option->setPropertyName("ZRes");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
	option->setUnits("Microns");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void ChangeResolution::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("XRes", getXRes() );
  writer->writeValue("YRes", getYRes() );
  writer->writeValue("ZRes", getZRes() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::preflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  VoxelDataContainer* m = getVoxelDataContainer();
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

  if(m->getXRes() == m_XRes
      && m->getYRes() == m_YRes
      && m->getZRes() == m_ZRes)
  {
    return;
  }

  size_t dims[3];
  m->getDimensions(dims);


  float sizex = (dims[0])*m->getXRes();
  float sizey = (dims[1])*m->getYRes();
  float sizez = (dims[2])*m->getZRes();
  int m_XP = int(sizex / m_XRes);
  int m_YP = int(sizey / m_YRes);
  int m_ZP = int(sizez / m_ZRes);
  int64_t totalPoints = m_XP*m_YP*m_ZP;

  float x, y, z;
  int col, row, plane;
  int index;
  int index_old;
  std::vector<size_t> newindicies;
  newindicies.resize(totalPoints);
  for (int i = 0; i < m_ZP; i++)
  {
    std::stringstream ss;
    ss << "Changing Resolution - " << ((float)i/m->getZPoints())*100 << " Percent Complete";
    notifyStatusMessage(ss.str());
    for (int j = 0; j < m_YP; j++)
    {
      for (int k = 0; k < m_XP; k++)
      {
        x = (k * m_XRes);
        y = (j * m_YRes);
        z = (i * m_ZRes);
        col = int(x / m->getXRes());
        row = int(y / m->getYRes());
        plane = int(z / m->getZRes());
        index_old = (plane * m->getXPoints() * m->getYPoints()) + (row * m->getXPoints()) + col;
        index = (i * m_XP * m_YP) + (j * m_XP) + k;
		newindicies[index] = index_old;
      }
    }
  }

  std::list<std::string> voxelArrayNames = m->getCellArrayNameList();
  for (std::list<std::string>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    std::string name = *iter;
    IDataArray::Pointer p = m->getCellData(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name. At least in theory
    IDataArray::Pointer data = p->createNewArray(p->GetNumberOfTuples(), p->GetNumberOfComponents(), p->GetName());
    data->Resize(totalPoints);
    void* source = NULL;
    void* destination = NULL;
    size_t newIndicies_I = 0;
    int nComp = data->GetNumberOfComponents();
    for (size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
    {
      newIndicies_I = newindicies[i];

      source = p->GetVoidPointer((nComp * newIndicies_I));
      destination = data->GetVoidPointer((data->GetNumberOfComponents() * i));
      ::memcpy(destination, source, p->GetTypeSize() * data->GetNumberOfComponents());
    }
    m->addCellData(*iter, data);
  }
  m->setResolution(m_XRes, m_YRes, m_ZRes);
  m->setDimensions(m_XP, m_YP, m_ZP);

  notifyStatusMessage("Complete");
}
