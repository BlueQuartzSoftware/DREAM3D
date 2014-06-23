/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "ConvertHexGridToSquareGrid.h"

#include <QtCore/QtDebug>
#include <fstream>

#include "H5Support/QH5Utilities.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>


#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/HKL/CtfReader.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/EbsdMath.h"

#include "DREAM3DLib/Utilities/FilePathGenerator.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertHexGridToSquareGrid::ConvertHexGridToSquareGrid() :
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_XResolution(1.0),
  m_YResolution(1.0),
  m_InputPath(),
  m_OutputPath(),
  m_OutputPrefix("Sqr_"),
  m_FilePrefix(),
  m_FileSuffix(),
  m_FileExtension("ang"),
  m_PaddingDigits(1),
  m_HexGridStack(0), // this is just a dummy variable,
  m_NumCols(0),
  m_NumRows(0),
  m_HeaderIsComplete(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertHexGridToSquareGrid::~ConvertHexGridToSquareGrid()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Convert Hex Grid ANG Files", "HexGridStack", FilterParameterWidgetType::ConvertHexGridToSquareGridWidget, getHexGridStack(), false));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex() ) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex() ) );
  setXResolution( reader->readValue("XResolution", getXResolution() ) );
  setYResolution( reader->readValue("YResolution", getYResolution() ) );
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex()) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex()) );
  setPaddingDigits( reader->readValue("PaddingDigits", getPaddingDigits()) );
  setInputPath( reader->readString("InputPath", getInputPath()) );
  setOutputPath( reader->readString("OutputPath", getOutputPath()) );
  setOutputPrefix( reader->readString("OutputPrefix", getOutputPrefix()) );
  setFilePrefix( reader->readString("FilePrefix", getFilePrefix()) );
  setFileSuffix( reader->readString("FileSuffix", getFileSuffix()) );
  setFileExtension( reader->readString("FileExtension", getFileExtension()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ConvertHexGridToSquareGrid::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ZStartIndex", getZStartIndex() );
  writer->writeValue("ZEndIndex", getZEndIndex() );
  writer->writeValue("PaddingDigits", getPaddingDigits() );
  writer->writeValue("InputPath", getInputPath() );
  writer->writeValue("OutputPath", getOutputPath() );
  writer->writeValue("OutputPrefix", getOutputPrefix() );
  writer->writeValue("FilePrefix", getFilePrefix() );
  writer->writeValue("FileSuffix", getFileSuffix() );
  writer->writeValue("FileExtension", getFileExtension() );
  writer->writeValue("XResolution", getXResolution() );
  writer->writeValue("YResolution", getYResolution() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  QString ss;

  if(m_OutputPath.isEmpty() == true)
  {
    ss = QObject::tr("The Output Directory must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -12);
    setErrorCondition(-12);
  }

  if(m_InputPath.isEmpty() == true)
  {
    ss = QObject::tr("The Input Directory must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -13);
    setErrorCondition(-13);
  }

  bool hasMissingFiles = false;

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_ZStartIndex, m_ZEndIndex,
                                                                  hasMissingFiles, true, m_InputPath,
                                                                  m_FilePrefix, m_FileSuffix, m_FileExtension,
                                                                  m_PaddingDigits);
  if (fileList.size() == 0)
  {
    QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::preflight()
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
void ConvertHexGridToSquareGrid::execute()
{

  herr_t err = 0;

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  bool hasMissingFiles = false;
  bool stackLowToHigh = true;
  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_ZStartIndex, m_ZEndIndex,
                                                                  hasMissingFiles, stackLowToHigh, m_InputPath,
                                                                  m_FilePrefix, m_FileSuffix, m_FileExtension,
                                                                  m_PaddingDigits);


  QVector<int> indices;
  // Loop on Each EBSD File
  float total = static_cast<float>( m_ZEndIndex - m_ZStartIndex );
  int progress = 0;
  int64_t z = m_ZStartIndex;
  /* There is a frailness about the z index and the file list. The programmer
   * using this code MUST ensure that the list of files that is sent into this
   * class is in the appropriate order to match up with the z index (slice index)
   * otherwise the import will have subtle errors. The programmer is urged NOT to
   * simply gather a list from the file system as those lists are sorted in such
   * a way that if the number of digits appearing in the filename are NOT the same
   * then the list will be wrong, ie, this example:
   *
   * slice_1.ang
   * slice_2.ang
   * ....
   * slice_10.ang
   *
   * Most, if not ALL C++ libraries when asked for that list will return the list
   * sorted like the following:
   *
   * slice_1.ang
   * slice_10.ang
   * slice_2.ang
   *
   * which is going to cause problems because the data is going to be placed
   * into the HDF5 file at the wrong index. YOU HAVE BEEN WARNED.
   */
  // int totalSlicesImported = 0;
  for (QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
  {
    QString ebsdFName = *filepath;
    {
      progress = static_cast<int>( z - m_ZStartIndex );
      progress = (int)(100.0f * (float)(progress) / total);
      QString msg = "Converting File: " + ebsdFName;
      notifyStatusMessage(getHumanLabel(), msg.toLatin1().data());
    }
    // Write the Manufacturer of the OIM file here
    // This list will grow to be the number of EBSD file formats we support
    QFileInfo fi(ebsdFName);
    QString ext = fi.suffix();
    QString base = fi.baseName();
    QDir path(getOutputPath());
    if(ext.compare(Ebsd::Ang::FileExt) == 0)
    {
      AngReader reader;
      reader.setFileName(ebsdFName);
      reader.setReadHexGrid(true);
      err = reader.readFile();
      if(err < 0 && err != -600)
      {
        setErrorCondition(reader.getErrorCode());
        notifyErrorMessage(getHumanLabel(), reader.getErrorMessage(), reader.getErrorCode());
        return;
      }
      else if(reader.getGrid().startsWith(Ebsd::Ang::SquareGrid) == true)
      {

        QString ss = QObject::tr("Ang File is already a square grid: %1").arg(ebsdFName);
        setErrorCondition(-55000);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      else
      {
        if (err == -600)
        {
          notifyWarningMessage(getHumanLabel(), reader.getErrorMessage(), reader.getErrorCode() );
        }
        QString origHeader = reader.getOriginalHeader();
        if (origHeader.isEmpty() == true)
        {

          QString ss = QObject::tr("Header could not be retrieved: %1").arg(ebsdFName);
          setErrorCondition(-55001);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }

        QTextStream in(&origHeader);

        //QString newEbsdFName = path.absolutePath() + "/Sqr_" + base + "." + ext;
        QString newEbsdFName = path.absolutePath() + "/" + getOutputPrefix() +  base + "." + ext;

        if(newEbsdFName.compare(ebsdFName) == 0)
        {
          QString msg = QObject::tr("New ANG File is the same as the old Ang file. Overwriting is NOT allowed.");
          setErrorCondition(-201);
          notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
          return;
        }


        QFile outFile(newEbsdFName);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
          QString msg = QObject::tr("ANG Square Output file could not be opened for writing: %1").arg(newEbsdFName);
          setErrorCondition(-200);
          notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
          return;
        }

        QTextStream dStream(&outFile);

        m_HeaderIsComplete = false;

        float HexXStep = reader.getXStep();
        float HexYStep = reader.getYStep();
        int HexNumColsOdd = reader.getNumOddCols();
        int HexNumColsEven = reader.getNumEvenCols();
        int HexNumRows = reader.getNumRows();
        m_NumCols = (HexNumColsOdd * HexXStep) / m_XResolution;
        m_NumRows = (HexNumRows * HexYStep) / m_YResolution;
        float xSqr, ySqr, xHex1, yHex1, xHex2, yHex2;
        int point, point1, point2;
        int row1, row2, col1, col2;
        float dist1, dist2;
        float* phi1 = reader.getPhi1Pointer();
        float* PHI = reader.getPhiPointer();
        float* phi2 = reader.getPhi2Pointer();
        float* ci = reader.getConfidenceIndexPointer();
        float* iq = reader.getImageQualityPointer();
        float* semsig = reader.getSEMSignalPointer();
        float* fit = reader.getFitPointer();
        int* phase = reader.getPhaseDataPointer();
        while (!in.atEnd())
        {
          QString buf = in.readLine();
          QString line = modifyAngHeaderLine(buf);
          if(m_HeaderIsComplete == false) { dStream << line ; }
        }
        for(int j = 0; j < m_NumRows; j++)
        {
          for(int i = 0; i < m_NumCols; i++)
          {
            xSqr = float(i) * m_XResolution;
            ySqr = float(j) * m_YResolution;
            row1 = ySqr / (HexYStep);
            yHex1 = row1 * HexYStep;
            row2 = row1 + 1;
            yHex2 = row2 * HexYStep;
            if(row1 % 2 == 0)
            {
              col1 = xSqr / (HexXStep);
              xHex1 = col1 * HexXStep;
              point1 = ((row1 / 2) * HexNumColsEven) + ((row1 / 2) * HexNumColsOdd) + col1;
              col2 = (xSqr - (HexXStep / 2.0)) / (HexXStep);
              xHex2 = col2 * HexXStep + (HexXStep / 2.0);
              point2 = ((row1 / 2) * HexNumColsEven) + (((row1 / 2) + 1) * HexNumColsOdd) + col2;
            }
            else
            {
              col1 = (xSqr - (HexXStep / 2.0)) / (HexXStep);
              xHex1 = col1 * HexXStep + (HexXStep / 2.0);
              point1 = ((row1 / 2) * HexNumColsEven) + (((row1 / 2) + 1) * HexNumColsOdd) + col1;
              col2 = xSqr / (HexXStep);
              xHex2 = col2 * HexXStep;
              point2 = (((row1 / 2) + 1) * HexNumColsEven) + (((row1 / 2) + 1) * HexNumColsOdd) + col2;
            }
            dist1 = ((xSqr - xHex1) * (xSqr - xHex1)) + ((ySqr - yHex1) * (ySqr - yHex1));
            dist2 = ((xSqr - xHex2) * (xSqr - xHex2)) + ((ySqr - yHex2) * (ySqr - yHex2));
            if(dist1 <= dist2 || row1 == (HexNumRows - 1)) {point = point1;}
            else {point = point2;}
            dStream << "  " << phi1[point] << "	" << PHI[point] << "	" << phi2[point] << "	" << xSqr << "	" << ySqr << "	" << iq[point] << "	" << ci[point] << "	" << phase[point] << "	" << semsig[point] << "	" << fit[point] << "	" << "\n";
          }
        }
      }
    }
    else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
    {
      qDebug() << "Ctf files are not on a hexagonal grid and do not need to be converted." << "\n";
    }
    else
    {
      err = -1;

      QString ss = QObject::tr("The File extension was not detected correctly");
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());

      return;
    }

  }

  notifyStatusMessage(getHumanLabel(), "Import Complete");
}

// -----------------------------------------------------------------------------
//  Modify the Header line of the ANG file if necessary
// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::modifyAngHeaderLine(QString& buf)
{
  QString line = "";
  if (buf.at(0) != '#')
  {
    line = buf;
    m_HeaderIsComplete = true;
    return line;
  }
  else if(buf.at(0) == '#' && buf.size() == 1)
  {
    line = buf;
    return line;
  }
  // Start at the first character and walk until you find another non-space character
  size_t i = 1;
  while(buf.at(i) == ' ')
  {
    ++i;
  }
  size_t wordStart = i;
  size_t wordEnd = i + 1;
  while(1)
  {
    if (buf.at(i) == 45 || buf.at(i) == 95) { ++i; } // "-" or "_" character
    else if (buf.at(i) >= 65 && buf.at(i) <= 90) { ++i; } // Upper case alpha character
    else if (buf.at(i) >= 97 && buf.at(i) <= 122) {++i; } // Lower case alpha character
    else { break;}
  }
  wordEnd = i;

  QString word( buf.mid(wordStart) );

  if (word.size() == 0)
  {
    line = buf;
    return line;
  }
  if (word.compare(Ebsd::Ang::Grid) == 0)
  {
    line = "# " + word + ": SqrGrid";
  }
  else if (word.compare(Ebsd::Ang::XStep) == 0)
  {
    line = "# " + word + ": " + QString::number( (double)m_XResolution);
  }
  else if (word.compare(Ebsd::Ang::YStep) == 0)
  {
    line = "# " + word + ": " + QString::number((double)m_YResolution);
  }
  else if (word.compare(Ebsd::Ang::NColsOdd) == 0)
  {
    line = "# " + word + ": " + QString::number(m_NumCols);
  }
  else if (word.compare(Ebsd::Ang::NColsEven) == 0)
  {
    line = "# " + word + ": " + QString::number(m_NumCols);
  }
  else if (word.compare(Ebsd::Ang::NRows) == 0)
  {
    line = "# " + word + ": " + QString::number(m_NumRows);
  }
  else
  {
    line = buf;
  }
  return line;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertHexGridToSquareGrid::newFilterInstance(bool copyFilterParameters)
{
  ConvertHexGridToSquareGrid::Pointer filter = ConvertHexGridToSquareGrid::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    // We are going to hand copy all of the parameters because the other way of copying the parameters are going to
    // miss some of them because we are not enumerating all of them.
    DREAM3D_COPY_INSTANCEVAR(ZStartIndex)
        DREAM3D_COPY_INSTANCEVAR(ZEndIndex)
        DREAM3D_COPY_INSTANCEVAR(XResolution)
        DREAM3D_COPY_INSTANCEVAR(YResolution)
        DREAM3D_COPY_INSTANCEVAR(InputPath)
        DREAM3D_COPY_INSTANCEVAR(OutputPath)
        DREAM3D_COPY_INSTANCEVAR(OutputPrefix)
        DREAM3D_COPY_INSTANCEVAR(FilePrefix)
        DREAM3D_COPY_INSTANCEVAR(FileSuffix)
        DREAM3D_COPY_INSTANCEVAR(FileExtension)
        DREAM3D_COPY_INSTANCEVAR(PaddingDigits)
        DREAM3D_COPY_INSTANCEVAR(HexGridStack)
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertHexGridToSquareGrid::getCompiledLibraryName()
{ return Sampling::SamplingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertHexGridToSquareGrid::getGroupName()
{ return DREAM3D::FilterGroups::SamplingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertHexGridToSquareGrid::getHumanLabel()
{ return "Convert Hexagonal Grid Data to Square Grid Data (TSL - .ang)"; }

