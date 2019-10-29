/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <memory>

#include "ConvertHexGridToSquareGrid.h"

#include <QtCore/QDir>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"

#include "EbsdLib/HKL/CtfReader.h"
#include "EbsdLib/TSL/AngReader.h"

#include "OrientationAnalysis/FilterParameters/ConvertHexGridToSquareGridFilterParameter.h"
#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertHexGridToSquareGrid::ConvertHexGridToSquareGrid()
: m_ZStartIndex(0)
, m_ZEndIndex(0)
, m_XResolution(1.0f)
, m_YResolution(1.0f)
, m_InputPath("")
, m_OutputPath("")
, m_OutputPrefix("Sqr_")
, m_FilePrefix("")
, m_FileSuffix("")
, m_FileExtension("ang")
, m_PaddingDigits(1)
, m_NumCols(0)
, m_NumRows(0)
, m_HeaderIsComplete(false)
, m_HexGridStack(0) // this is just a dummy variable
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertHexGridToSquareGrid::~ConvertHexGridToSquareGrid() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(ConvertHexGridToSquareGridFilterParameter::New("Convert Hex Grid ANG Files", "HexGridStack", getHexGridStack(), FilterParameter::Parameter, this));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setZStartIndex(reader->readValue("ZStartIndex", getZStartIndex()));
  setZEndIndex(reader->readValue("ZEndIndex", getZEndIndex()));
  setXResolution(reader->readValue("XResolution", getXResolution()));
  setYResolution(reader->readValue("YResolution", getYResolution()));
  setPaddingDigits(reader->readValue("PaddingDigits", getPaddingDigits()));
  setInputPath(reader->readString("InputPath", getInputPath()));
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setOutputPrefix(reader->readString("OutputPrefix", getOutputPrefix()));
  setFilePrefix(reader->readString("FilePrefix", getFilePrefix()));
  setFileSuffix(reader->readString("FileSuffix", getFileSuffix()));
  setFileExtension(reader->readString("FileExtension", getFileExtension()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;
  QString ss;

  QDir dir(getOutputPath());

  if(getOutputPath().isEmpty())
  {
    setErrorCondition(-1003, "The output directory must be set");
  }
  else if(!dir.exists())
  {
    QString ss = QObject::tr("The output directory path does not exist. DREAM.3D will attempt to create this path during execution");
    setWarningCondition(-1004, ss);
  }

  if(m_InputPath.isEmpty())
  {
    ss = QObject::tr("The input directory must be set");
    setErrorCondition(-13, ss);
  }

  bool hasMissingFiles = false;
  int increment = 1;

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList =
      FilePathGenerator::GenerateFileList(m_ZStartIndex, m_ZEndIndex, increment, hasMissingFiles, true, m_InputPath, m_FilePrefix, m_FileSuffix, m_FileExtension, m_PaddingDigits);
  if(fileList.empty())
  {
    QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    setErrorCondition(-11, ss);
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
QString ConvertHexGridToSquareGrid::modifyAngHeaderLine(QString& buf)
{
  QString line = "";
  if(buf.at(0) != '#')
  {
    line = buf;
    m_HeaderIsComplete = true;
    return line;
  }
  if(buf.at(0) == '#' && buf.size() == 1)
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
  // size_t wordEnd = i + 1;
  while(true)
  {
    if(buf.at(i) == 45 || buf.at(i) == 95)
    {
      ++i;
    } // "-" or "_" character
    else if(buf.at(i) >= 65 && buf.at(i) <= 90)
    {
      ++i;
    } // Upper case alpha character
    else if(buf.at(i) >= 97 && buf.at(i) <= 122)
    {
      ++i;
    } // Lower case alpha character
    else
    {
      break;
    }
  }
  // wordEnd = i;

  QString word(buf.mid(wordStart));

  if(word.size() == 0)
  {
    line = buf;
    return line;
  }
  if(buf.contains(Ebsd::Ang::HexGrid))
  {
    line = buf.replace(Ebsd::Ang::HexGrid, Ebsd::Ang::SquareGrid);
    // line = "# " + word + ": SqrGrid";
  }
  else if(buf.contains(Ebsd::Ang::XStep))
  {
    line = "# " + Ebsd::Ang::XStep + ": " + QString::number((double)m_XResolution);
  }
  else if(buf.contains(Ebsd::Ang::YStep))
  {
    line = "# " + Ebsd::Ang::YStep + ": " + QString::number((double)m_YResolution);
  }
  else if(buf.contains(Ebsd::Ang::NColsOdd))
  {
    line = "# " + Ebsd::Ang::NColsOdd + ": " + QString::number(m_NumCols);
  }
  else if(buf.contains(Ebsd::Ang::NColsEven))
  {
    line = "# " + Ebsd::Ang::NColsEven + ": " + QString::number(m_NumCols);
  }
  else if(buf.contains(Ebsd::Ang::NRows))
  {
    line = "# " + Ebsd::Ang::NRows + ": " + QString::number(m_NumRows);
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
void ConvertHexGridToSquareGrid::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  herr_t err = 0;
  bool hasMissingFiles = false;
  bool stackLowToHigh = true;
  int increment = 1;

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList =
      FilePathGenerator::GenerateFileList(m_ZStartIndex, m_ZEndIndex, increment, hasMissingFiles, stackLowToHigh, m_InputPath, m_FilePrefix, m_FileSuffix, m_FileExtension, m_PaddingDigits);

  // Loop on Each EBSD File
  float total = static_cast<float>(m_ZEndIndex - m_ZStartIndex);
  int32_t progress = 0;
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
  for(QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
  {
    QString ebsdFName = *filepath;
    {
      progress = static_cast<int32_t>(z - m_ZStartIndex);
      progress = (int32_t)(100.0f * (float)(progress) / total);
      QString msg = "Converting File: " + ebsdFName;
      notifyStatusMessage(msg.toLatin1().data());
    }
    if(getCancel())
    {
      break;
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
        setErrorCondition(reader.getErrorCode(), reader.getErrorMessage());
        return;
      }
      if(reader.getGrid().startsWith(Ebsd::Ang::SquareGrid))
      {
        QString ss = QObject::tr("Ang file is already a square grid: %1").arg(ebsdFName);
        setErrorCondition(-55000, ss);
        return;
      }

        if(err == -600)
        {
          setWarningCondition(reader.getErrorCode(), reader.getErrorMessage());
        }
        QString origHeader = reader.getOriginalHeader();
        if(origHeader.isEmpty())
        {

          QString ss = QObject::tr("Header could not be retrieved: %1").arg(ebsdFName);
          setErrorCondition(-55001, ss);
        }

        QTextStream in(&origHeader);
        // QString newEbsdFName = path.absolutePath() + "/Sqr_" + base + "." + ext;
        QString newEbsdFName = path.absolutePath() + "/" + getOutputPrefix() + base + "." + ext;

        if(newEbsdFName.compare(ebsdFName) == 0)
        {
          QString msg = QObject::tr("New ang file is the same as the old ang file. Overwriting is NOT allowed");
          setErrorCondition(-201, msg);
          return;
        }

        QFile outFile(newEbsdFName);
        QFileInfo fi(newEbsdFName);
        // Ensure the output path exists by creating it if necessary
        QDir parent(fi.absolutePath());
        if(!parent.exists())
        {
          parent.mkpath(fi.absolutePath());
        }

        if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
          QString msg = QObject::tr("Ang square output file could not be opened for writing: %1").arg(newEbsdFName);
          setErrorCondition(-200, msg);
          return;
        }

        QTextStream dStream(&outFile);

        m_HeaderIsComplete = false;

        float HexXStep = reader.getXStep();
        float HexYStep = reader.getYStep();
        int32_t HexNumColsOdd = reader.getNumOddCols();
        int32_t HexNumColsEven = reader.getNumEvenCols();
        int32_t HexNumRows = reader.getNumRows();
        m_NumCols = (HexNumColsOdd * HexXStep) / m_XResolution;
        m_NumRows = (HexNumRows * HexYStep) / m_YResolution;
        float xSqr = 0.0f, ySqr = 0.0f, xHex1 = 0.0f, yHex1 = 0.0f, xHex2 = 0.0f, yHex2 = 0.0f;
        int32_t point = 0, point1 = 0, point2 = 0;
        int32_t row1 = 0, row2 = 0, col1 = 0, col2 = 0;
        float dist1 = 0.0f, dist2 = 0.0f;
        float* phi1 = reader.getPhi1Pointer();
        float* PHI = reader.getPhiPointer();
        float* phi2 = reader.getPhi2Pointer();
        float* ci = reader.getConfidenceIndexPointer();
        float* iq = reader.getImageQualityPointer();
        float* semsig = reader.getSEMSignalPointer();
        float* fit = reader.getFitPointer();
        int32_t* phase = reader.getPhaseDataPointer();
        while(!in.atEnd())
        {
          QString buf = in.readLine();
          QString line = modifyAngHeaderLine(buf);
          if(!m_HeaderIsComplete)
          {
            dStream << line << "\n";
          }
        }
        for(int32_t j = 0; j < m_NumRows; j++)
        {
          for(int32_t i = 0; i < m_NumCols; i++)
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
            if(dist1 <= dist2 || row1 == (HexNumRows - 1))
            {
              point = point1;
            }
            else
            {
              point = point2;
            }
            dStream << "  " << phi1[point] << "	" << PHI[point] << "	" << phi2[point] << "	" << xSqr << "	" << ySqr << "	" << iq[point] << "	" << ci[point] << "	" << phase[point] << "	"
                    << semsig[point] << "	" << fit[point] << "	"
                    << "\n";
          }
        }
    }
    else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
    {
      QString ss = QObject::tr("Ctf files are not on a hexagonal grid and do not need to be converted");
      setErrorCondition(-1, ss);
      return;
    }
    else
    {
      err = -1;
      QString ss = QObject::tr("The file extension was not detected correctly");
      setErrorCondition(-1, ss);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertHexGridToSquareGrid::newFilterInstance(bool copyFilterParameters) const
{
  ConvertHexGridToSquareGrid::Pointer filter = ConvertHexGridToSquareGrid::New();
  if(copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    // We are going to hand copy all of the parameters because the other way of copying the parameters are going to
    // miss some of them because we are not enumerating all of them.
    filter->setZStartIndex(getZStartIndex());
    filter->setZEndIndex(getZEndIndex());
    filter->setXResolution(getXResolution());
    filter->setYResolution(getYResolution());
    filter->setInputPath(getInputPath());
    filter->setOutputPath(getOutputPath());
    filter->setOutputPrefix(getOutputPrefix());
    filter->setFilePrefix(getFilePrefix());
    filter->setFileSuffix(getFileSuffix());
    filter->setFileExtension(getFileExtension());
    filter->setPaddingDigits(getPaddingDigits());
    filter->setHexGridStack(getHexGridStack());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ConvertHexGridToSquareGrid::getUuid() const
{
  return QUuid("{e1343abe-e5ad-5eb1-a89d-c209e620e4de}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ResolutionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getHumanLabel() const
{
  return "Convert Hexagonal Grid Data to Square Grid Data (TSL - .ang)";
}

// -----------------------------------------------------------------------------
ConvertHexGridToSquareGrid::Pointer ConvertHexGridToSquareGrid::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ConvertHexGridToSquareGrid> ConvertHexGridToSquareGrid::New()
{
  struct make_shared_enabler : public ConvertHexGridToSquareGrid
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getNameOfClass() const
{
  return QString("ConvertHexGridToSquareGrid");
}

// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::ClassName()
{
  return QString("ConvertHexGridToSquareGrid");
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setZStartIndex(int64_t value)
{
  m_ZStartIndex = value;
}

// -----------------------------------------------------------------------------
int64_t ConvertHexGridToSquareGrid::getZStartIndex() const
{
  return m_ZStartIndex;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setZEndIndex(int64_t value)
{
  m_ZEndIndex = value;
}

// -----------------------------------------------------------------------------
int64_t ConvertHexGridToSquareGrid::getZEndIndex() const
{
  return m_ZEndIndex;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setXResolution(float value)
{
  m_XResolution = value;
}

// -----------------------------------------------------------------------------
float ConvertHexGridToSquareGrid::getXResolution() const
{
  return m_XResolution;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setYResolution(float value)
{
  m_YResolution = value;
}

// -----------------------------------------------------------------------------
float ConvertHexGridToSquareGrid::getYResolution() const
{
  return m_YResolution;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setInputPath(const QString& value)
{
  m_InputPath = value;
}

// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getInputPath() const
{
  return m_InputPath;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setOutputPath(const QString& value)
{
  m_OutputPath = value;
}

// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getOutputPath() const
{
  return m_OutputPath;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setOutputPrefix(const QString& value)
{
  m_OutputPrefix = value;
}

// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getOutputPrefix() const
{
  return m_OutputPrefix;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setFilePrefix(const QString& value)
{
  m_FilePrefix = value;
}

// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getFilePrefix() const
{
  return m_FilePrefix;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setFileSuffix(const QString& value)
{
  m_FileSuffix = value;
}

// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getFileSuffix() const
{
  return m_FileSuffix;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setFileExtension(const QString& value)
{
  m_FileExtension = value;
}

// -----------------------------------------------------------------------------
QString ConvertHexGridToSquareGrid::getFileExtension() const
{
  return m_FileExtension;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setPaddingDigits(int value)
{
  m_PaddingDigits = value;
}

// -----------------------------------------------------------------------------
int ConvertHexGridToSquareGrid::getPaddingDigits() const
{
  return m_PaddingDigits;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setNumCols(int value)
{
  m_NumCols = value;
}

// -----------------------------------------------------------------------------
int ConvertHexGridToSquareGrid::getNumCols() const
{
  return m_NumCols;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setNumRows(int value)
{
  m_NumRows = value;
}

// -----------------------------------------------------------------------------
int ConvertHexGridToSquareGrid::getNumRows() const
{
  return m_NumRows;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setHeaderIsComplete(bool value)
{
  m_HeaderIsComplete = value;
}

// -----------------------------------------------------------------------------
bool ConvertHexGridToSquareGrid::getHeaderIsComplete() const
{
  return m_HeaderIsComplete;
}

// -----------------------------------------------------------------------------
void ConvertHexGridToSquareGrid::setHexGridStack(int value)
{
  m_HexGridStack = value;
}

// -----------------------------------------------------------------------------
int ConvertHexGridToSquareGrid::getHexGridStack() const
{
  return m_HexGridStack;
}
