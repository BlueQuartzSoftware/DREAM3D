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

#include <memory>

#include "ImportEbsdMontage.h"

#include <QtCore/QFileInfo>

#include "EbsdLib/HKL/CtfFields.h"
#include "EbsdLib/HKL/CtfReader.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/TSL/AngReader.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationAnalysis/FilterParameters/EbsdMontageImportFilterParameter.h"
#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/GenerateIPFColors.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/ReadAngData.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/ReadCtfData.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID31 = 31
};

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
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportEbsdMontage::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_EbsdMontageListInfo_FP("Input File List", InputFileListInfo, FilterParameter::Parameter, ImportEbsdMontage));
  // parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ImportEbsdMontage));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, ImportEbsdMontage));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, ImportEbsdMontage));

  QStringList linkedProps("CellIPFColorsArrayName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Generate IPF Color Map", GenerateIPFColorMap, FilterParameter::Parameter, ImportEbsdMontage, linkedProps));
  parameters.push_back(SIMPL_NEW_STRING_FP("IPF Colors", CellIPFColorsArrayName, FilterParameter::CreatedArray, ImportEbsdMontage));

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
    QString msg = QString("Error: DataContainer '%1' already exists in the DataContainerArray.").arg(fname);
    filter->setErrorCondition(-74000, msg);
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
    reader->setDataContainerName(DataArrayPath(fname));
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
  if(reader->getErrorCode() < 0)
  {
    QString msg = QString("Sub filter (%1) caused an error.").arg(reader->getHumanLabel());
    filter->setErrorCondition(reader->getErrorCode(), msg);
    return;
  }

  DataContainer::Pointer dc = dca->getDataContainer(fname);
  filter->getDataContainerArray()->addOrReplaceDataContainer(dc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportEbsdMontage::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  DataArrayPath tempPath;
  QString ss;

  if(m_InputFileListInfo.InputPath.isEmpty())
  {
    ss = QObject::tr("The input directory must be set");
    setErrorCondition(-13, ss);
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

    setErrorCondition(-11, ss);
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
      QString phasesName;
      QString eulersName;
      QString xtalName;

      if(m_InputFileListInfo.FileExtension == Ebsd::Ang::FileExt)
      {
        readEbsdFile<ReadAngData>(this, tile2D.FileName, m_FilterCache, newFilterCache);
        phasesName = Ebsd::AngFile::Phases;
        eulersName = Ebsd::AngFile::EulerAngles;
        xtalName = Ebsd::AngFile::CrystalStructures;
      }
      if(m_InputFileListInfo.FileExtension == Ebsd::Ctf::FileExt)
      {
        readEbsdFile<ReadCtfData>(this, tile2D.FileName, m_FilterCache, newFilterCache);
        phasesName = Ebsd::CtfFile::Phases;
        eulersName = Ebsd::CtfFile::EulerAngles;
        xtalName = Ebsd::CtfFile::CrystalStructures;
      }
      if(!fi.exists())
      {
        QString msg = QString("Input EBSD file '%1' does not exist").arg(tile2D.FileName);
        setErrorCondition(-56500, msg);
      }
      if(getErrorCode() >= 0)
      {
        DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(fname);
        ImageGeom::Pointer imageGeom = dc->getGeometryAs<ImageGeom>();

        SizeVec3Type dims = imageGeom->getDimensions();
        FloatVec3Type spacing = imageGeom->getSpacing();
        FloatVec3Type origin = imageGeom->getOrigin();

        //
        origin[0] = globalTileOrigin[0];
        origin[1] = globalTileOrigin[1];
        imageGeom->setOrigin(origin);

        // Now update the globalTileOrigin values
        globalTileOrigin[0] += origin[0] + (dims[0] * spacing[0]);
        tileHeight = (dims[1] * spacing[1]);

        if(getCancel())
        {
          return;
        }
        tilesRead++;
        if(!getInPreflight())
        {
          QString msg = QString("==> [%1/%2] %3").arg(tilesRead).arg(totalTiles).arg(tile2D.FileName);
          notifyStatusMessage(msg);
        }

        if(getGenerateIPFColorMap())
        {
          if(getCellIPFColorsArrayName().isEmpty())
          {
            ss = QObject::tr("Generate IPF Colors is ENABLED. Please set name for the generated IPColors DataArray");
            setErrorCondition(-23500, ss);
          }
          else
          {
            DataArrayPath dap(fname, getCellAttributeMatrixName(), getCellIPFColorsArrayName());
            // QVector<size_t> cDims = {3};

            GenerateIPFColors::Pointer generateIPFColors = GenerateIPFColors::New();
            generateIPFColors->setDataContainerArray(getDataContainerArray());
            generateIPFColors->setReferenceDir(m_ReferenceDir);
            dap.setDataArrayName(phasesName);
            generateIPFColors->setCellPhasesArrayPath(dap);
            dap.setDataArrayName(eulersName);
            generateIPFColors->setCellEulerAnglesArrayPath(dap);

            dap.setAttributeMatrixName(getCellEnsembleAttributeMatrixName());
            dap.setDataArrayName(xtalName);
            generateIPFColors->setCrystalStructuresArrayPath(dap);
            generateIPFColors->setUseGoodVoxels(false);

            generateIPFColors->setCellIPFColorsArrayName(getCellIPFColorsArrayName());
            if(getInPreflight())
            {
              generateIPFColors->preflight();
              if(generateIPFColors->getErrorCode() < 0)
              {
                ss = QObject::tr("Preflight of GenerateIPFColors failed with error code ").arg(generateIPFColors->getErrorCode());
                setErrorCondition(generateIPFColors->getErrorCode(), ss);
              }
            }
            else
            {
              QString msg = QString("==> [%1/%2] %3 Generating IPF Colors").arg(tilesRead).arg(totalTiles).arg(tile2D.FileName);
              notifyStatusMessage(msg);
              generateIPFColors->execute();
              if(generateIPFColors->getErrorCode() < 0)
              {
                ss = QObject::tr("GenerateIPFColors failed with error code ").arg(generateIPFColors->getErrorCode());
                setErrorCondition(generateIPFColors->getErrorCode(), ss);
              }
            }
          }
        }
      }
    }
    globalTileOrigin[1] += tileHeight;
  }

  m_FilterCache = newFilterCache; // Swap our maps. This dumps any previous instantiations of the reader filter that are not used any more.
  clearWarningCode();
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
QString ImportEbsdMontage::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportEbsdMontage::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportEbsdMontage::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportEbsdMontage::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportEbsdMontage::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportEbsdMontage::getHumanLabel() const
{
  return "Import EBSD Montage";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ImportEbsdMontage::getUuid() const
{
  return QUuid("{179b0c7a-4e62-5070-ba49-ae58d5ccbfe8}");
}

// -----------------------------------------------------------------------------
ImportEbsdMontage::Pointer ImportEbsdMontage::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ImportEbsdMontage> ImportEbsdMontage::New()
{
  struct make_shared_enabler : public ImportEbsdMontage
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ImportEbsdMontage::getNameOfClass() const
{
  return QString("ImportEbsdMontage");
}

// -----------------------------------------------------------------------------
QString ImportEbsdMontage::ClassName()
{
  return QString("ImportEbsdMontage");
}

// -----------------------------------------------------------------------------
void ImportEbsdMontage::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ImportEbsdMontage::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void ImportEbsdMontage::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportEbsdMontage::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ImportEbsdMontage::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportEbsdMontage::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ImportEbsdMontage::setInputFileListInfo(const MontageFileListInfo& value)
{
  m_InputFileListInfo = value;
}

// -----------------------------------------------------------------------------
MontageFileListInfo ImportEbsdMontage::getInputFileListInfo() const
{
  return m_InputFileListInfo;
}

// -----------------------------------------------------------------------------
void ImportEbsdMontage::setGenerateIPFColorMap(bool value)
{
  m_GenerateIPFColorMap = value;
}

// -----------------------------------------------------------------------------
bool ImportEbsdMontage::getGenerateIPFColorMap() const
{
  return m_GenerateIPFColorMap;
}
// -----------------------------------------------------------------------------
void ImportEbsdMontage::setCellIPFColorsArrayName(const QString& value)
{
  m_CellIPFColorsArrayName = value;
}

// -----------------------------------------------------------------------------
QString ImportEbsdMontage::getCellIPFColorsArrayName() const
{
  return m_CellIPFColorsArrayName;
}
