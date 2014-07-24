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

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

using namespace std;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSections::AlignSections() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_WriteAlignmentShifts(false),
  m_SubtractBackground(false),
  m_AlignmentShiftFileName("")
{
  setupFilterParameters();
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
  FilterParameterVector parameters;

  QStringList linkedProps("AlignmentShiftFileName");
  parameters.push_back(FilterParameter::NewConditional("Write Alignment Shift File", "WriteAlignmentShifts", FilterParameterWidgetType::LinkedBooleanWidget, getWriteAlignmentShifts(), false, linkedProps));
  
  parameters.push_back(FilterParameter::New("Linear Background Subtraction", "SubtractBackground", FilterParameterWidgetType::BooleanWidget, getSubtractBackground(), false));
  parameters.push_back(FileSystemFilterParameter::New("Alignment File", "AlignmentShiftFileName", FilterParameterWidgetType::OutputFileWidget, "QString", false));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAlignmentShiftFileName( reader->readString("AlignmentShiftFileName", getAlignmentShiftFileName()));
  setSubtractBackground( reader->readValue("SubtractBackground", getSubtractBackground()));
  setWriteAlignmentShifts( reader->readValue("WriteAlignmentShifts", getWriteAlignmentShifts()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AlignSections::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(AlignmentShiftFileName)
  DREAM3D_FILTER_WRITE_PARAMETER(SubtractBackground)
  DREAM3D_FILTER_WRITE_PARAMETER(WriteAlignmentShifts)
  writer->closeFilterGroup();

  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::dataCheck()
{
  setErrorCondition(0);

  if(true == m_WriteAlignmentShifts && m_AlignmentShiftFileName.isEmpty() == true)
  {
    QString ss = QObject::tr("The Alignment Shift file name must be set before executing this filter.");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::preflight()
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
void AlignSections::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
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
  QVector<int> yshifts(dims[2], 0);

  find_shifts(xshifts, yshifts);

  if(getSubtractBackground())
  {
    /**fit x and y shifts to lines
     *
     * y = mx + b
     *
     * m = (n*sum(x_i * y_i) - sum(x_i) * sum(y_i)) / (n*sum(x_i^2)-sum(x_i)^2
     *
     * b = (sum(y_i)-m*sum(x_i))/n
     *
     */

    //same for both
    double sumX = 0;//sum(x_i)
    double sumX_2 = 0;//sum(x_i^2)

    //x shift line
    double x_sumY = 0;//sum(y_i)
    double x_sumXY = 0;//sum(x_i * y_i)

    //y shift line
    double y_sumY = 0;//sum(y_i)
    double y_sumXY = 0;//sum(x_i * y_i)

    for (DimType iter = 0; iter < dims[2]; iter++)
    {
      slice = static_cast<int>( (dims[2] - 1) - iter );
      sumX = sumX + iter;
      sumX_2 = sumX_2 + iter*iter;
      x_sumY = x_sumY + xshifts[iter];
      x_sumXY = x_sumXY + iter*xshifts[iter];
      y_sumY = y_sumY + yshifts[iter];
      y_sumXY = y_sumXY + iter*yshifts[iter];
    }

    double mx, my, bx, by;
    mx = (dims[2]*x_sumXY - x_sumXY)/(dims[2]*sumX_2-sumX);
    my = (dims[2]*y_sumXY - y_sumXY)/(dims[2]*sumX_2-sumX);
    bx = (x_sumY-mx*sumX)/dims[2];
    by = (y_sumY-my*sumX)/dims[2];

    ///adjust shifts so that fit line has 0 slope (~ends of the sample are fixed)
    for (DimType iter = 1; iter < dims[2]; iter++)
    {
      slice = static_cast<int>( (dims[2] - 1) - iter );
      xshifts[iter] = xshifts[iter] - iter*mx;
      yshifts[iter] = yshifts[iter] - iter*my;
    }
  }

  QList<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNameList();
  DimType progIncrement = dims[2] / 100;
  DimType prog = 1;
  int progressInt = 0;

  for (DimType i = 1; i < dims[2]; i++)
  {
    if (i > prog)
    {

      progressInt = ((float)i / dims[2]) * 100.0;
      QString ss = QObject::tr("Transferring Cell Data - %1% Complete").arg(progressInt);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
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
        if(yshifts[i] >= 0) { yspot = static_cast<int>(l); }
        else if(yshifts[i] < 0) { yspot = static_cast<int>( dims[1] - 1 - l ); }
        if(xshifts[i] >= 0) { xspot = static_cast<int>(n); }
        else if(xshifts[i] < 0) { xspot = static_cast<int>( dims[0] - 1 - n ); }
        newPosition = (slice * dims[0] * dims[1]) + (yspot * dims[0]) + xspot;
        currentPosition = (slice * dims[0] * dims[1]) + ((yspot + yshifts[i]) * dims[0]) + (xspot + xshifts[i]);
        if((yspot + yshifts[i]) >= 0 && (yspot + yshifts[i]) <= dims[1] - 1 && (xspot + xshifts[i]) >= 0
            && (xspot + xshifts[i]) <= dims[0] - 1)
        {
          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            QString name = *iter;
            IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
            p->copyTuple(currentPosition, newPosition);
          }
        }
        if((yspot + yshifts[i]) < 0 || (yspot + yshifts[i]) > dims[1] - 1 || (xspot + xshifts[i]) < 0
            || (xspot + xshifts[i]) > dims[0] - 1)
        {
          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            QString name = *iter;
            IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
            p->initializeTuple(newPosition, 0.0);
          }
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::find_shifts(QVector<int>& xshifts, QVector<int>& yshifts)
{

}
