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

#include <QtCore/QVector>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenumberGrains::RenumberGrains() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_GrainIds(NULL),
  m_Active(NULL)
{
  //  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenumberGrains::~RenumberGrains()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberGrains::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Name of Array for Active Grains");
    option->setPropertyName("ActiveArrayName");
    option->setWidgetType(FilterParameter::StringWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberGrains::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  ////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenumberGrains::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberGrains::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, dims)
  GET_PREREQ_DATA(m, DREAM3D, CellFieldData, Active, -306, bool, BoolArrayType, fields, dims)
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberGrains::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  dataCheck(true, 1, 1, 1);

  QList<QString> headers = m->getCellFieldArrayNameList();
  for (QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    IDataArray::Pointer p = m->getCellFieldData(*iter);
    QString type = p->getTypeAsString();
    if(type.compare("NeighborList<T>") == 0) { m->removeCellFieldData(*iter);}
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberGrains::execute()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumCellFieldTuples();
  dataCheck(false, totalPoints, totalFields, m->getNumCellEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  QString ss;
  size_t goodcount = 1;
  std::vector<size_t> NewNames;
  NewNames.resize(totalFields, 0);


  ss = QObject::tr(" - Generating Active Grain List");
  notifyStatusMessage(ss);
  QVector<size_t> RemoveList;
  for(size_t i = 1; i < totalFields; i++)
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
    QList<QString> headers = m->getCellFieldArrayNameList();
    for (QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
    {
      IDataArray::Pointer p = m->getCellFieldData(*iter);

      ss = QObject::tr("Updating Field Array '%1'").arg(*iter);
      notifyStatusMessage(ss);
      QString type = p->getTypeAsString();
      if(type.compare("NeighborList<T>") == 0)
      {
        m->removeCellFieldData(*iter);
      }
      else
      {
        p->EraseTuples(RemoveList);
      }
      //qDebug() << "  Tuples Remain: " << p->getNumberOfTuples() << " NumComp:" << p->GetNumberOfComponents()  ;
    }
    m->setNumCellFieldTuples(m->getNumCellFieldTuples() - RemoveList.size());
    totalFields = m->getNumCellFieldTuples();
    dataCheck(false, totalPoints, totalFields, m->getNumCellEnsembleTuples());

    // Loop over all the points and correct all the grain names
    ss = QObject::tr("Renumbering Cell Region Ids");
    notifyStatusMessage(ss);
    for (int i = 0; i < totalPoints; i++)
    {
      //  notify(ss.str(), 0, Observable::UpdateProgressMessage);
      if(m_GrainIds[i] > 0)
      {
        m_GrainIds[i] = static_cast<int32_t>( NewNames[m_GrainIds[i]] );
      }
    }
  }

  notifyStatusMessage("Complete");
}


