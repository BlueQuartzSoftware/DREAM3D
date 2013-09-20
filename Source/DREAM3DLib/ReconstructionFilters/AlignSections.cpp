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

#include "AlignSections.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "DREAM3DLib/GenericFilters/FindCellQuats.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

using namespace std;




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSections::AlignSections() :
  AbstractFilter(),
  m_WriteAlignmentShifts(true),
  m_AlignmentShiftFileName("")
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSections::~AlignSections()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Alignment Shift File");
    option->setPropertyName("WriteAlignmentShifts");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Alignment File");
    option->setPropertyName("AlignmentShiftFileName");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AlignSections::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("AlignmentShiftFileName", getAlignmentShiftFileName());
  writer->writeValue("WriteAlignmentShifts", getWriteAlignmentShifts());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);


  if(true == m_WriteAlignmentShifts && m_AlignmentShiftFileName.isEmpty() == true)
  {
    QString ss = QObject::tr("The Alignment Shift file name must be set before executing this filter.");
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss, -1);
  }


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::execute()
{
  setErrorCondition(0);
  VolumeDataContainer* m = getVolumeDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);

    QString ss = QObject::tr(" DataContainer was NULL");
    notifyErrorMessage(ss, -1);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

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

  int slice;
  int xspot, yspot;
  DimType newPosition;
  DimType currentPosition;
  //  unsigned int  phase2;

  QVector<int> xshifts(dims[2], 0);
  QVector<int> yshifts(dims[2],0);

  find_shifts(xshifts, yshifts);

  QList<QString> voxelArrayNames = m->getCellArrayNameList();
  DimType progIncrement = dims[2]/100;
  DimType prog = 1;
  int progressInt = 0;


  for (DimType i = 1; i < dims[2]; i++)
  {
    if (i > prog)
    {

      progressInt = ((float)i/dims[2])*100.0;
      QString ss = QObject::tr("Transferring Cell Data - %1% Complete").arg(progressInt);
      notifyStatusMessage(ss);
      prog = prog + progIncrement;
    }
    if (getCancel() == true)
    {
      return;
    }
    slice = static_cast<int>( (dims[2] - 1) - i );
    for (DimType l = 0; l < dims[1]; l++)
    {
      for (DimType n = 0; n < dims[0]; n++)
      {
        if(yshifts[i] >= 0) yspot = static_cast<int>(l);
        else if(yshifts[i] < 0) yspot = static_cast<int>( dims[1] - 1 - l );
        if(xshifts[i] >= 0) xspot = static_cast<int>(n);
        else if(xshifts[i] < 0) xspot = static_cast<int>( dims[0] - 1 - n );
        newPosition = (slice * dims[0] * dims[1]) + (yspot * dims[0]) + xspot;
        currentPosition = (slice * dims[0] * dims[1]) + ((yspot + yshifts[i]) * dims[0]) + (xspot + xshifts[i]);
        if((yspot + yshifts[i]) >= 0 && (yspot + yshifts[i]) <= dims[1] - 1 && (xspot + xshifts[i]) >= 0
           && (xspot + xshifts[i]) <= dims[0] - 1)
        {
          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            QString name = *iter;
            IDataArray::Pointer p = m->getCellData(*iter);
            p->CopyTuple(currentPosition, newPosition);
          }
        }
        if((yspot + yshifts[i]) < 0 || (yspot + yshifts[i]) > dims[1] - 1 || (xspot + xshifts[i]) < 0
           || (xspot + xshifts[i]) > dims[0] - 1)
        {
          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            QString name = *iter;
            IDataArray::Pointer p = m->getCellData(*iter);
            p->InitializeTuple(newPosition, 0.0);          }
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::find_shifts(QVector<int> &xshifts, QVector<int> &yshifts)
{

}

