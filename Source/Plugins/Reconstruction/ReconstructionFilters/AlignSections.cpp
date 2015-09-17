/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "AlignSections.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"

#include "Reconstruction/ReconstructionConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSections::AlignSections() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::ImageDataContainerName),
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
  parameters.push_back(LinkedBooleanFilterParameter::New("Write Alignment Shift File", "WriteAlignmentShifts", getWriteAlignmentShifts(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(OutputFileFilterParameter::New("Alignment File", "AlignmentShiftFileName", getAlignmentShiftFileName(), FilterParameter::Parameter, "", "*.txt"));
  parameters.push_back(BooleanFilterParameter::New("Linear Background Subtraction", "SubtractBackground", getSubtractBackground(), FilterParameter::Parameter));
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
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(AlignmentShiftFileName)
  SIMPL_FILTER_WRITE_PARAMETER(SubtractBackground)
  SIMPL_FILTER_WRITE_PARAMETER(WriteAlignmentShifts)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  if (image->getXPoints() <= 1 || image->getYPoints() <= 1 || image->getZPoints() <= 1)
  {
    QString ss = QObject::tr("The Image Geometry is not 3D and cannot be run through this filter. The dimensions are (%1,%2,%3)").arg(image->getXPoints()).arg(image->getYPoints()).arg(image->getZPoints());
    setErrorCondition(-3010);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), "");
  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, tempPath, -301);

  if (true == m_WriteAlignmentShifts && m_AlignmentShiftFileName.isEmpty() == true)
  {
    QString ss = QObject::tr("The alignment shift file name is empty");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
void AlignSections::find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts)
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
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

  DimType slice = 0;
  DimType xspot = 0, yspot = 0;
  DimType newPosition = 0;
  DimType currentPosition = 0;

  std::vector<int64_t> xshifts(dims[2], 0);
  std::vector<int64_t> yshifts(dims[2], 0);

  find_shifts(xshifts, yshifts);

  if (getSubtractBackground())
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

    // same for both
    double sumX = 0.0; // sum(x_i)
    double sumX_2 = 0.0; // sum(x_i^2)

    // x shift line
    double x_sumY = 0.0; // sum(y_i)
    double x_sumXY = 0.0; // sum(x_i * y_i)

    // y shift line
    double y_sumY = 0.0; // sum(y_i)
    double y_sumXY = 0.0; // sum(x_i * y_i)

    for (DimType iter = 0; iter < dims[2]; iter++)
    {
      slice = static_cast<DimType>( (dims[2] - 1) - iter );
      sumX = static_cast<double>(sumX + iter);
      sumX_2 = static_cast<double>(sumX_2 + iter * iter);
      x_sumY = static_cast<double>(x_sumY + xshifts[iter]);
      x_sumXY = static_cast<double>(x_sumXY + iter * xshifts[iter]);
      y_sumY = static_cast<double>(y_sumY + yshifts[iter]);
      y_sumXY = static_cast<double>(y_sumXY + iter * yshifts[iter]);
    }

    double mx = static_cast<double>((dims[2] * x_sumXY - x_sumXY) / (dims[2] * sumX_2 - sumX));
    double my = static_cast<double>((dims[2] * y_sumXY - y_sumXY) / (dims[2] * sumX_2 - sumX));

    // adjust shifts so that fit line has 0 slope (~ends of the sample are fixed)
    for (DimType iter = 1; iter < dims[2]; iter++)
    {
      slice = (dims[2] - 1) - iter;
      xshifts[iter] = static_cast<int64_t>(xshifts[iter] - iter * mx);
      yshifts[iter] = static_cast<int64_t>(yshifts[iter] - iter * my);
    }
  }

  QList<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNames();
  DimType progIncrement = dims[2] / 100;
  DimType prog = 1;
  DimType progressInt = 0;

  for (DimType i = 1; i < dims[2]; i++)
  {
    if (i > prog)
    {

      progressInt = ((float)i / dims[2]) * 100.0f;
      QString ss = QObject::tr("Transferring Cell Data || %1% Complete").arg(progressInt);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      prog = prog + progIncrement;
    }
    if (getCancel() == true)
    {
      return;
    }
    slice = (dims[2] - 1) - i;
    for (DimType l = 0; l < dims[1]; l++)
    {
      for (DimType n = 0; n < dims[0]; n++)
      {
        if (yshifts[i] >= 0) { yspot = l; }
        else if (yshifts[i] < 0) { yspot = dims[1] - 1 - l; }
        if (xshifts[i] >= 0) { xspot = n; }
        else if (xshifts[i] < 0) { xspot = dims[0] - 1 - n; }
        newPosition = (slice * dims[0] * dims[1]) + (yspot * dims[0]) + xspot;
        currentPosition = (slice * dims[0] * dims[1]) + ((yspot + yshifts[i]) * dims[0]) + (xspot + xshifts[i]);
        if ((yspot + yshifts[i]) >= 0 && (yspot + yshifts[i]) <= dims[1] - 1 && (xspot + xshifts[i]) >= 0
            && (xspot + xshifts[i]) <= dims[0] - 1)
        {
          for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
            p->copyTuple(currentPosition, newPosition);
          }
        }
        if ((yspot + yshifts[i]) < 0 || (yspot + yshifts[i]) > dims[1] - 1 || (xspot + xshifts[i]) < 0
            || (xspot + xshifts[i]) > dims[0] - 1)
        {
          for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
            p->initializeTuple(newPosition, 0);
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
AbstractFilter::Pointer AlignSections::newFilterInstance(bool copyFilterParameters)
{
  AlignSections::Pointer filter = AlignSections::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getCompiledLibraryName()
{ return ReconstructionConstants::ReconstructionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getGroupName()
{ return DREAM3D::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getSubGroupName()
{ return DREAM3D::FilterSubGroups::AlignmentFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getHumanLabel()
{ return "Align Sections"; }
