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
#include "Hex2SqrConverter.h"

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

#include "DREAM3DLib/Common/Observable.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Hex2SqrConverter::Hex2SqrConverter() :
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_XResolution(1.0),
  m_YResolution(1.0),
  m_NumCols(0),
  m_NumRows(0),
  m_HeaderIsComplete(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Hex2SqrConverter::~Hex2SqrConverter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::setupFilterParameters()
{
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex() ) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex() ) );
  setXResolution( reader->readValue("XResolution", getXResolution() ) );
  setYResolution( reader->readValue("YResolution", getYResolution() ) );
  setEbsdFileList( reader->readStrings("EbsdFileList", getEbsdFileList() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Hex2SqrConverter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ZStartIndex", getZStartIndex() );
  writer->writeValue("ZEndIndex", getZEndIndex() );
  writer->writeValue("XResolution", getXResolution() );
  writer->writeValue("ZYResolution", getYResolution() );
  writer->writeValue("EbsdFileList", getEbsdFileList() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::dataCheck()
{
  setErrorCondition(0);

  if (m_EbsdFileList.size() == 0)
  {

    QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    addErrorMessage(getHumanLabel(), ss, -11);
    setErrorCondition(-1);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::execute()
{

  herr_t err = 0;

  dataCheck();
  if(getErrorCondition() < 0) { return; }

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
  for (QVector<QString>::iterator filepath = m_EbsdFileList.begin(); filepath != m_EbsdFileList.end(); ++filepath)
  {
    QString ebsdFName = *filepath;
    {
      progress = static_cast<int>( z - m_ZStartIndex );
      progress = (int)(100.0f * (float)(progress) / total);
      QString msg = "Converting File: " + ebsdFName;
      notifyStatusMessage(msg.toLatin1().data());
    }
    // Write the Manufacturer of the OIM file here
    // This list will grow to be the number of EBSD file formats we support
    QFileInfo fi(ebsdFName);
    QString ext = fi.suffix();
    QString base = fi.baseName();
    QDir path = fi.path();
    if(ext.compare(Ebsd::Ang::FileExt) == 0)
    {
      AngReader reader;
      reader.setFileName(ebsdFName);
      reader.setReadHexGrid(true);
      err = reader.readFile();
      if(err < 0)
      {
        addErrorMessage(getHumanLabel(), reader.getErrorMessage(), reader.getErrorCode());
        setErrorCondition(reader.getErrorCode());
        return;
      }
      else if(reader.getGrid().startsWith(Ebsd::Ang::SquareGrid) == true)
      {

        QString ss = QObject::tr("Ang File is already a square grid: %1").arg(ebsdFName);
        setErrorCondition(-55000);
        addErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      else
      {
        QString origHeader = reader.getOriginalHeader();
        if (origHeader.isEmpty() == true)
        {

          QString ss = QObject::tr("Header could not be retrieved: %1").arg(ebsdFName);
          setErrorCondition(-55001);
          addErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }

        QTextStream in(&origHeader);

        QString newEbsdFName = path.absolutePath() + "/Sqr_" + base + "." + ext;

        QFile outFile(newEbsdFName);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
          QString msg = QObject::tr("ANG Square Output file could not be opened for writing: %1").arg(newEbsdFName);
          setErrorCondition(-200);
          notifyErrorMessage(msg, getErrorCondition());
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
      addErrorMessage(getHumanLabel(), ss, err);
      setErrorCondition(-1);
      return;
    }

  }

  notifyStatusMessage("Import Complete");
}

// -----------------------------------------------------------------------------
//  Modify the Header line of the ANG file if necessary
// -----------------------------------------------------------------------------
QString Hex2SqrConverter::modifyAngHeaderLine(QString& buf)
{
  QString line = "";
  if (buf.at(0) != '#')
  {
    line = buf;
    m_HeaderIsComplete = true;
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


