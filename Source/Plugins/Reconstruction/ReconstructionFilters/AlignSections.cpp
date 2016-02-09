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
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"


#include "Reconstruction/ReconstructionConstants.h"

// Include the MOC generated file for this class
#include "moc_AlignSections.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSections::AlignSections() :
  AbstractFilter(),
  m_DataContainerName(SIMPL::Defaults::ImageDataContainerName),
  m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName),
  m_WriteAlignmentShifts(false),
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
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSections::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAlignmentShiftFileName( reader->readString("AlignmentShiftFileName", getAlignmentShiftFileName()));
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
template<typename T>
void initializeArrayValues(IDataArray::Pointer p, size_t index)
{

    typename DataArray<T>::Pointer ptr = std::dynamic_pointer_cast<DataArray<T>>(p);
    T var = static_cast<T>(0);
    ptr->initializeTuple(index, &var);
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

  size_t dims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  int64_t xspot = 0, yspot = 0;
  int64_t newPosition = 0;
  int64_t currentPosition = 0;

  std::vector<int64_t> xshifts(dims[2], 0);
  std::vector<int64_t> yshifts(dims[2], 0);

  find_shifts(xshifts, yshifts);


  QList<QString> voxelArrayNames = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArrayNames();
  size_t progIncrement = dims[2] / 100;
  size_t prog = 1;
  size_t progressInt = 0;
  size_t slice = 0;

  for (size_t i = 1; i < dims[2]; i++)
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
    for (size_t l = 0; l < dims[1]; l++)
    {
      for (size_t n = 0; n < dims[0]; n++)
      {
        if (yshifts[i] >= 0) { yspot = l; }
        else if (yshifts[i] < 0) { yspot = dims[1] - 1 - l; }
        if (xshifts[i] >= 0) { xspot = n; }
        else if (xshifts[i] < 0) { xspot = dims[0] - 1 - n; }
        newPosition = (slice * dims[0] * dims[1]) + (yspot * dims[0]) + xspot;
        currentPosition = (slice * dims[0] * dims[1]) + ((yspot + yshifts[i]) * dims[0]) + (xspot + xshifts[i]);
        if ((yspot + yshifts[i]) >= 0 && (yspot + yshifts[i]) <= static_cast<int64_t>(dims[1]) - 1 && (xspot + xshifts[i]) >= 0
            && (xspot + xshifts[i]) <= static_cast<int64_t>(dims[0]) - 1)
        {
          for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
            p->copyTuple( static_cast<size_t>(currentPosition), static_cast<size_t>(newPosition));
          }
        }
        if ((yspot + yshifts[i]) < 0 || (yspot + yshifts[i]) > dims[1] - 1 || (xspot + xshifts[i]) < 0
            || (xspot + xshifts[i]) > static_cast<int64_t>(dims[0]) - 1)
        {
          for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            IDataArray::Pointer p = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(*iter);
            EXECUTE_FUNCTION_TEMPLATE(this, initializeArrayValues, p, p, newPosition)
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
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getBrandingString()
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getGroupName()
{ return SIMPL::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getSubGroupName()
{ return SIMPL::FilterSubGroups::AlignmentFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSections::getHumanLabel()
{ return "Align Sections"; }
