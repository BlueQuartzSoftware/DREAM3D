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

#include <QtCore/QMap>
#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

using namespace std;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegularizeZSpacing::RegularizeZSpacing() :
  AbstractFilter(),
  m_CellAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_InputFile(""),
  m_NewZRes(1.0f)
{
  setupFilterParameters();
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
void RegularizeZSpacing::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("Current Z Positions File", "InputFile", FilterParameterWidgetType::InputFileWidget, getInputFile(), false));
  parameters.push_back(FilterParameter::New("New Z Res", "NewZRes", FilterParameterWidgetType::DoubleWidget, getNewZRes(), false, "Microns"));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellAttributeMatrixPath( reader->readDataArrayPath("CellAttributeMatrixPath", getCellAttributeMatrixPath() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setNewZRes( reader->readValue("NewZRes", getNewZRes()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RegularizeZSpacing::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("CellAttributeMatrixPath", getCellAttributeMatrixPath() );
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("NewZRes", getNewZRes() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::dataCheck(bool preflight)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getCellAttributeMatrixPath().getDataContainerName(), false);
  if (getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixPath().getAttributeMatrixName(), -301);
  if (getErrorCondition() < 0) { return; }

  ifstream inFile;
  inFile.open(m_InputFile.toLatin1().data());

  float zval;
  for (size_t iter = 0; iter < m->getZPoints() + 1; iter++)
  {
    inFile >> zval;
  }
  size_t zP = static_cast<size_t>(zval / getNewZRes());

  if(preflight == true)
  {
    m->setDimensions(m->getXPoints(), m->getYPoints(), zP);
    QVector<size_t> tDims(3, 0);
    tDims[0] = m->getXPoints();
    tDims[1] = m->getYPoints();
    tDims[2] = zP;
    cellAttrMat->resizeAttributeArrays(tDims);
  }

  inFile.close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck(true);
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegularizeZSpacing::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck(false);
  if(getErrorCondition() < 0) { return; }

  DREAM3D_RANDOMNG_NEW()
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getCellAttributeMatrixPath().getDataContainerName());

  size_t dims[3];
  m->getDimensions(dims);

  ifstream inFile;
  inFile.open(m_InputFile.toLatin1().data());

  float zval;
  QVector<float> zboundvalues(dims[2] + 1, 0.0);
  for (size_t iter = 0; iter < dims[2] + 1; iter++)
  {
    inFile >> zval;
    zboundvalues[iter] = zval;
  }
  inFile.close();

  float xRes = m->getXRes();
  float yRes = m->getYRes();

  float sizez = zboundvalues[dims[2]];
  size_t m_XP = dims[0];
  size_t m_YP = dims[1];
  size_t m_ZP = static_cast<size_t>(sizez / m_NewZRes);
  int64_t totalPoints = m_XP * m_YP * m_ZP;

  int index, oldindex;
  int plane;
  QVector<size_t> newindicies(totalPoints, 0);
  for (size_t i = 0; i < m_ZP; i++)
  {
    plane = 0;
    for (size_t iter = 1; iter < dims[2]; iter++)
    {
      if((i * m_NewZRes) > zboundvalues[iter]) { plane = iter; }
    }
    for (size_t j = 0; j < m_YP; j++)
    {
      for (size_t k = 0; k < m_XP; k++)
      {
        oldindex = (plane * dims[0] * dims[1]) + (j * dims[0]) + k;
        index = (i * dims[0] * dims[1]) + (j * dims[0]) + k;
        newindicies[index] = oldindex;
      }
    }
  }

  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  QVector<size_t> tDims(3, 0);
  tDims[0] = m_XP;
  tDims[1] = m_YP;
  tDims[2] = m_ZP;
  AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(tDims, cellAttrMat->getName(), cellAttrMat->getType());

  QList<QString> voxelArrayNames = cellAttrMat->getAttributeArrayNameList();
  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    QString name = *iter;
    IDataArray::Pointer p = cellAttrMat->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name. At least in theory
    IDataArray::Pointer data = p->createNewArray(p->getNumberOfTuples(), p->getComponentDimensions(), p->getName());
    data->resize(totalPoints);
    void* source = NULL;
    void* destination = NULL;
    size_t newIndicies_I = 0;
    int nComp = data->getNumberOfComponents();
    for (size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
    {
      newIndicies_I = newindicies[i];

      source = p->getVoidPointer((nComp * newIndicies_I));
      destination = data->getVoidPointer((data->getNumberOfComponents() * i));
      ::memcpy(destination, source, p->getTypeSize() * data->getNumberOfComponents());
    }
    cellAttrMat->removeAttributeArray(*iter);
    newCellAttrMat->addAttributeArray(*iter, data);
  }
  m->setResolution(xRes, yRes, m_NewZRes);
  m->setDimensions(m_XP, m_YP, m_ZP);
  m->removeAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName());
  m->addAttributeMatrix(getCellAttributeMatrixPath().getAttributeMatrixName(), newCellAttrMat);

  notifyStatusMessage(getHumanLabel(), "Changing Resolution Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RegularizeZSpacing::newFilterInstance(bool copyFilterParameters)
{
  /*
  * InputFile
  * NewZRes
  */
  RegularizeZSpacing::Pointer filter = RegularizeZSpacing::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
