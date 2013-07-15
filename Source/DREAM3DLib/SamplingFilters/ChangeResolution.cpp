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
AbstractFilter()
{
  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

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

    option->setHumanLabel("Resolution");
    option->setPropertyName("Resolution");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("Microns");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void ChangeResolution::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(index);
  writer->writeValue("Resolution", getResolution() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeResolution::preflight()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  size_t dims[3];
  m->getDimensions(dims);

  float sizex = (dims[0])*m->getXRes();
  float sizey = (dims[1])*m->getYRes();
  float sizez = (dims[2])*m->getZRes();
  int m_XP = int(sizex / m_Resolution.x);
  int m_YP = int(sizey / m_Resolution.y);
  int m_ZP = int(sizez / m_Resolution.z);

  m->setDimensions(m_XP, m_YP, m_ZP);
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
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

  if(m->getXRes() == m_Resolution.x
      && m->getYRes() == m_Resolution.y
      && m->getZRes() == m_Resolution.z)
  {
    return;
  }

  size_t dims[3];
  m->getDimensions(dims);


  float sizex = (dims[0])*m->getXRes();
  float sizey = (dims[1])*m->getYRes();
  float sizez = (dims[2])*m->getZRes();
  int m_XP = int(sizex / m_Resolution.x);
  int m_YP = int(sizey / m_Resolution.y);
  int m_ZP = int(sizez / m_Resolution.z);
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
        x = (k * m_Resolution.x);
        y = (j * m_Resolution.y);
        z = (i * m_Resolution.z);
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
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setDimensions(m_XP, m_YP, m_ZP);

  notifyStatusMessage("Complete");
}
