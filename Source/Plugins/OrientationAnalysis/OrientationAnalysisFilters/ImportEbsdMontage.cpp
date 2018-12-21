/* ============================================================================
 * Copyright (c) 2018 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportEbsdMontage.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

#include "EbsdLib/HKL/CtfFields.h"
#include "EbsdLib/HKL/CtfReader.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/TSL/AngReader.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"

#include "OrientationAnalysis/FilterParameters/EbsdMontageImportFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/ReadAngData.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/ReadCtfData.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportEbsdMontage::ImportEbsdMontage()
: m_DataContainerName("OIM Data Container")
, m_CellEnsembleAttributeMatrixName("Phase Data")
, m_CellAttributeMatrixName("Scan Data")
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportEbsdMontage::~ImportEbsdMontage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportEbsdMontage::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportEbsdMontage::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_EbsdMontageListInfo_FP("Input File List", InputFileListInfo, FilterParameter::Parameter, ImportEbsdMontage));
  // parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ImportEbsdMontage));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, ImportEbsdMontage));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, ImportEbsdMontage));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <class EbsdReaderClass>
void readEbsdFile(ImportEbsdMontage* filter, const QString& fileName, std::map<QString, AbstractFilter::Pointer>& prevFilterCache, std::map<QString, AbstractFilter::Pointer>& newFilterCache)
{
  QFileInfo fi(fileName);
  QString fname = fi.completeBaseName();
  if(filter->getDataContainerArray()->doesDataContainerExist(fname))
  {
    filter->setErrorCondition(-74000);
    QString msg = QString("Error: DataContainer '%1' already exists in the DataContainerArray.").arg(fname);
    filter->notifyErrorMessage(filter->getHumanLabel(), msg, filter->getErrorCondition());
    return;
  }

  DataContainerArray::Pointer dca = DataContainerArray::New();

  typename EbsdReaderClass::Pointer reader = EbsdReaderClass::NullPointer();
  if(prevFilterCache.find(fileName) != prevFilterCache.end())
  {
    reader = std::dynamic_pointer_cast<EbsdReaderClass>(prevFilterCache[fileName]);
  }
  else
  {
    reader = EbsdReaderClass::New();
    reader->setInputFile(fileName);
    reader->setDataContainerName(fname);
  }
  newFilterCache[fileName] = reader;
  reader->setDataContainerArray(dca);
  reader->setCellEnsembleAttributeMatrixName(filter->getCellEnsembleAttributeMatrixName());
  reader->setCellAttributeMatrixName(filter->getCellAttributeMatrixName());
  if(filter->getInPreflight())
  {
    reader->preflight();
  }
  else
  {
    reader->execute();
  }
  if(reader->getErrorCondition() < 0)
  {
    filter->setErrorCondition(reader->getErrorCondition());
    QString msg = QString("Sub filter (%1) caused an error during preflight.").arg(reader->getHumanLabel());
    filter->notifyErrorMessage(filter->getHumanLabel(), msg, filter->getErrorCondition());
    return;
  }

  DataContainer::Pointer dc = dca->getDataContainer(fname);
  filter->getDataContainerArray()->addDataContainer(dc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportEbsdMontage::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setWarningCondition(0);

  DataArrayPath tempPath;
  QString ss;

  if(m_InputFileListInfo.InputPath.isEmpty())
  {
    ss = QObject::tr("The input directory must be set");
    setErrorCondition(-13);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    m_FilterCache.clear();
  }
  bool hasMissingFiles = false;
  // Now generate all the file names the user is asking for and populate the table
  FilePathGenerator::TileRCIncexLayout2D tileLayout2d = FilePathGenerator::GenerateRCIndexMontageFileList(
      m_InputFileListInfo.RowStart, m_InputFileListInfo.RowEnd, m_InputFileListInfo.ColStart, m_InputFileListInfo.ColEnd, hasMissingFiles, (m_InputFileListInfo.Ordering == 0),
      m_InputFileListInfo.InputPath, m_InputFileListInfo.FilePrefix, m_InputFileListInfo.FileSuffix, m_InputFileListInfo.FileExtension, m_InputFileListInfo.PaddingDigits);
  if(tileLayout2d.empty())
  {
    ss.clear();
    QTextStream out(&ss);
    out << " No files have been selected for import. Have you set the input directory and other values so that input files will be generated?\n";
    out << "InputPath: " << m_InputFileListInfo.InputPath << "\n";
    out << "FilePrefix: " << m_InputFileListInfo.FilePrefix << "\n";
    out << "FileSuffix: " << m_InputFileListInfo.FileSuffix << "\n";
    out << "FileExtension: " << m_InputFileListInfo.FileExtension << "\n";
    out << "PaddingDigits: " << m_InputFileListInfo.PaddingDigits << "\n";
    out << "RowStart: " << m_InputFileListInfo.RowStart << "\n";
    out << "RowEnd: " << m_InputFileListInfo.RowEnd << "\n";
    out << "ColStart: " << m_InputFileListInfo.ColStart << "\n";
    out << "ColEnd: " << m_InputFileListInfo.ColEnd << "\n";

    m_FilterCache.clear();

    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  int32_t numRows = tileLayout2d.size();
  int32_t numCols = tileLayout2d[0].size();
  int32_t totalTiles = numRows * numCols;
  int32_t tilesRead = 0;

  std::array<double, 2> globalTileOrigin = {{0.0, 0.0}};

  std::map<QString, AbstractFilter::Pointer> oldCache = m_FilterCache;

  std::map<QString, AbstractFilter::Pointer> newFilterCache;

  for(const FilePathGenerator::TileRCIndexRow2D& tileRow2D : tileLayout2d)
  {
    globalTileOrigin[0] = 0.0; // Reset the X Coord back to Zero for each row.
    double tileHeight = 0.0;
    for(const FilePathGenerator::TileRCIndex2D& tile2D : tileRow2D)
    {
      QFileInfo fi(tile2D.FileName);
      QString fname = fi.completeBaseName();

      if(m_InputFileListInfo.FileExtension == Ebsd::Ang::FileExt)
      {
        readEbsdFile<ReadAngData>(this, tile2D.FileName, m_FilterCache, newFilterCache);
      }
      if(m_InputFileListInfo.FileExtension == Ebsd::Ctf::FileExt)
      {
        readEbsdFile<ReadCtfData>(this, tile2D.FileName, m_FilterCache, newFilterCache);
      }
      if(getErrorCondition() >= 0)
      {
        DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(fname);
        ImageGeom::Pointer imageGeom = dc->getGeometryAs<ImageGeom>();

        std::array<size_t, 3> dims = {{0, 0, 0}};
        std::tie(dims[0], dims[1], dims[2]) = imageGeom->getDimensions();
        std::array<float, 3> res = {{0.0f, 0.0f, 0.0f}};
        std::tie(res[0], res[1], res[2]) = imageGeom->getResolution();
        std::array<float, 3> origin = {{0.0f, 0.0f, 0.0f}};
        std::tie(origin[0], origin[1], origin[2]) = imageGeom->getOrigin();

        //
        origin[0] = globalTileOrigin[0];
        origin[1] = globalTileOrigin[1];
        imageGeom->setOrigin(origin.data());

        // Now update the globalTileOrigin values
        globalTileOrigin[0] += origin[0] + (dims[0] * res[0]);
        tileHeight = (dims[1] * res[1]);

        if(getCancel())
        {
          return;
        }
        tilesRead++;
        if(!getInPreflight())
        {
          QString msg = QString("==> [%1/%2] %3").arg(tilesRead).arg(totalTiles).arg(tile2D.FileName);
          notifyStatusMessage(getHumanLabel(), msg);
        }
      }
    }
    globalTileOrigin[1] += tileHeight;
  }

  m_FilterCache = newFilterCache; // Swap our maps. This dumps any previous instantiations of the reader filter that are not used any more.
  setWarningCondition(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportEbsdMontage::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportEbsdMontage::execute()
{
  initialize();
  dataCheck();

  // The entire Reading of the data, whether that is in prefligth (so just read the header) or in execute is performed
  // in the dataCheck() method so we essentially do nothing here.

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportEbsdMontage::newFilterInstance(bool copyFilterParameters) const
{
  ImportEbsdMontage::Pointer filter = ImportEbsdMontage::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportEbsdMontage::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportEbsdMontage::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportEbsdMontage::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportEbsdMontage::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportEbsdMontage::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportEbsdMontage::getHumanLabel() const
{
  return "Import EBSD Montage";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ImportEbsdMontage::getUuid()
{
  return QUuid("{179b0c7a-4e62-5070-ba49-ae58d5ccbfe8}");
}
