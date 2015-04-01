/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "WriteStatsGenOdfAngleFile.h"

#include <stdio.h>

#include <iostream>

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtGui/QImage>
#include <QtGui/QColor>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfReader.h"


#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DEndian.h"
#include "DREAM3DLib/Utilities/ColorTable.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "OrientationLib/Math/OrientationMath.h"
#include "OrientationLib/OrientationOps/CubicOps.h"
#include "OrientationLib/OrientationOps/CubicLowOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.h"
#include "OrientationLib/OrientationOps/HexagonalLowOps.h"
#include "OrientationLib/OrientationOps/OrthoRhombicOps.h"
#include "OrientationLib/OrientationOps/TrigonalOps.h"
#include "OrientationLib/OrientationOps/TetragonalOps.h"
#include "OrientationLib/OrientationOps/TrigonalLowOps.h"
#include "OrientationLib/OrientationOps/TetragonalLowOps.h"
#include "OrientationLib/OrientationOps/TriclinicOps.h"
#include "OrientationLib/OrientationOps/MonoclinicOps.h"
#include "OrientationLib/Utilities/PoleFigureUtilities.h"
#include "OrientationLib/Utilities/ModifiedLambertProjection.h"

#include "QtSupport/PoleFigureImageUtilities.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"


#define SET_DIRECTION(i, j, k)\
  direction[0] = i; direction[1] = j; direction[2] = k;



#define WRITE_EULERS_TEXT_FILE 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteStatsGenOdfAngleFile::WriteStatsGenOdfAngleFile() :
  AbstractFilter(),
  m_OutputFile(""),
  m_CellPhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::CellPhases),
  m_CellEulerAnglesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::EulerAngles),
  m_ConvertToDegrees(true),
  m_UseGoodVoxels(false),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteStatsGenOdfAngleFile::~WriteStatsGenOdfAngleFile()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::setupFilterParameters()
{
  FilterParameterVector parameters;

  /* For String input use this code */
  parameters.push_back(FileSystemFilterParameter::New("Output File", "OutputFile", FilterParameterWidgetType::OutputFileWidget, getOutputFile(), false));

  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Euler Angles", "CellEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellEulerAnglesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Convert to Degrees", "ConvertToDegrees", FilterParameterWidgetType::BooleanWidget, getConvertToDegrees(), true, ""));

  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Apply to Good Voxels Only (Bad Voxels Will Be Black)", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, true));
  parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGoodVoxelsArrayPath(), true, ""));


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setConvertToDegrees(reader->readValue("ConvertToDegrees", getConvertToDegrees() ) );
  setOutputFile( reader->readString("OutputFile", getOutputFile()));
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteStatsGenOdfAngleFile::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(ConvertToDegrees)
  DREAM3D_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayPath)

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::dataCheck()
{
  setErrorCondition(0);

  QString ss;
  if (getOutputFile().isEmpty() == true)
  {
    ss = QObject::tr( "The output file must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }
  QFileInfo fi(getOutputFile());
  QDir parentPath = fi.path();
  if (parentPath.exists() == false)
  {
    ss = QObject::tr( "The directory path for the output file does not exist.");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }


  QVector<size_t> dims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


  if (getUseGoodVoxels() == true)
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    dims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else
  {
    m_GoodVoxels = NULL;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::preflight()
{
  setInPreflight(true);
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteStatsGenOdfAngleFile::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }


  // Figure out how many unique phase values we have by looping over all the phase values
  int64_t totalPoints = m_CellPhasesPtr.lock()->getNumberOfTuples();
  std::set<int32_t> uniquePhases;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    uniquePhases.insert(m_CellPhases[i]);
  }

  uniquePhases.erase(0); // remove Phase 0 as this is a Special Phase for DREAM3D
  std::vector<QFile*> oFiles;

  QFileInfo fi(getOutputFile());
  QString absPath = fi.absolutePath();
  QString fname = fi.fileName();
  int pos = fname.lastIndexOf('.');
  fname = fname.mid(0, pos - 1);
  QString suffix = fi.suffix();

  for(std::set<int32_t>::iterator iter = uniquePhases.begin(); iter != uniquePhases.end(); iter++)
  {
    /* Let the GUI know we are done with this filter */
    QString ss = QObject::tr("Writing file for phase '%1'").arg(*iter);

    notifyStatusMessage(getHumanLabel(), ss);

    QString absFilePath = absPath + "/" + fname + "_Phase_" + QString::number(*iter) + "." + suffix;

    QFile file(absFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      setErrorCondition(-99000);
      QString ss = QObject::tr("Error creating output file '%1'").arg(absFilePath);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    QTextStream out(&file);

    // Dry run to figure out how many lines we are going to have
    int32_t lineCount = determineOutputLineCount(totalPoints, *iter);

    int err = writeOutputFile(out, lineCount, totalPoints, *iter);
    if(err < 0)
    {
      setErrorCondition(-99001);
      QString ss = QObject::tr("Error writing output file '%1'").arg(absFilePath);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    out.flush();
    file.close(); // Close the file

  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteStatsGenOdfAngleFile::determineOutputLineCount(int64_t totalPoints, int32_t phase)
{
  int32_t lineCount = 0;
  bool countLine = false;
  for(int64_t i = 0; i < totalPoints; i++)
  {
    countLine = false;

    if(m_UseGoodVoxels == true && m_GoodVoxels[i] == true) { countLine = true; }
    if(m_CellPhases[i] == phase) { countLine = true; }

    if(countLine == true)
    {
      lineCount++;
    }
  }

  return lineCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteStatsGenOdfAngleFile::writeOutputFile(QTextStream& out, int32_t lineCount, int64_t totalPoints, int32_t phase)
{
  bool writeLine = false;
  // write out the total number of lines
  out << lineCount << "\n";

  float weight = 1.0f;
  float sigma = 1.0f;

  for(int64_t i = 0; i < totalPoints; i++)
  {
    writeLine = false;

    if(m_UseGoodVoxels == true && m_GoodVoxels[i] == true) { writeLine = true; }
    if(m_CellPhases[i] == phase) { writeLine = true; }

    if(writeLine == true)
    {
      float e0 = m_CellEulerAngles[i * 3 + 0];
      float e1 = m_CellEulerAngles[i * 3 + 1];
      float e2 = m_CellEulerAngles[i * 3 + 2];
      if(m_ConvertToDegrees == true)
      {
        e0 = e0 * DREAM3D::Constants::k_180OverPi;
        e1 = e1 * DREAM3D::Constants::k_180OverPi;
        e2 = e2 * DREAM3D::Constants::k_180OverPi;
      }
      out << e0 << " " << e1 << " " << e2 << " " << weight << " " << sigma << "\n";
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteStatsGenOdfAngleFile::newFilterInstance(bool copyFilterParameters)
{
  WriteStatsGenOdfAngleFile::Pointer filter = WriteStatsGenOdfAngleFile::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteStatsGenOdfAngleFile::getCompiledLibraryName()
{ return OrientationAnalysis::OrientationAnalysisBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteStatsGenOdfAngleFile::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteStatsGenOdfAngleFile::getSubGroupName()
{ return "Output"; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteStatsGenOdfAngleFile::getHumanLabel()
{ return "Write StatsGenerator ODF Angle File"; }

