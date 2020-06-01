/* ============================================================================
 * Copyright (c) 2020 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportOnscaleTableFile.h"

#include <cstddef>
#include <memory>

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/IGeometry.h"
#include "SIMPLib/Geometry/RectGridGeom.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
  DataContainerID = 1
};

/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ImportOnscaleTableFilePrivate
{
  Q_DISABLE_COPY(ImportOnscaleTableFilePrivate)
  Q_DECLARE_PUBLIC(ImportOnscaleTableFile)
  ImportOnscaleTableFile* const q_ptr;
  ImportOnscaleTableFilePrivate(ImportOnscaleTableFile* ptr);
  std::vector<size_t> m_Dims;
  std::vector<FloatArrayType::Pointer> m_Coords;
  std::vector<QString> m_Names;
  QString m_InputFile_Cache;
  QDateTime m_LastRead;
};

// -----------------------------------------------------------------------------
ImportOnscaleTableFilePrivate::ImportOnscaleTableFilePrivate(ImportOnscaleTableFile* ptr)
: q_ptr(ptr)
, m_InputFile_Cache("")
{
}

// -----------------------------------------------------------------------------
ImportOnscaleTableFile::ImportOnscaleTableFile()
: d_ptr(new ImportOnscaleTableFilePrivate(this))
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportOnscaleTableFile::~ImportOnscaleTableFile() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setDims(const std::vector<size_t>& value)
{
  Q_D(ImportOnscaleTableFile);
  d->m_Dims = value;
}

// -----------------------------------------------------------------------------
std::vector<size_t> ImportOnscaleTableFile::getDims() const
{
  Q_D(const ImportOnscaleTableFile);
  return d->m_Dims;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setCoords(const std::vector<FloatArrayType::Pointer>& value)
{
  Q_D(ImportOnscaleTableFile);
  d->m_Coords = value;
}

// -----------------------------------------------------------------------------
std::vector<FloatArrayType::Pointer> ImportOnscaleTableFile::getCoords() const
{
  Q_D(const ImportOnscaleTableFile);
  return d->m_Coords;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setNames(const std::vector<QString>& value)
{
  Q_D(ImportOnscaleTableFile);
  d->m_Names = value;
}

// -----------------------------------------------------------------------------
std::vector<QString> ImportOnscaleTableFile::getNames() const
{
  Q_D(const ImportOnscaleTableFile);
  return d->m_Names;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setInputFile_Cache(const QString& value)
{
  Q_D(ImportOnscaleTableFile);
  d->m_InputFile_Cache = value;
}

// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getInputFile_Cache() const
{
  Q_D(const ImportOnscaleTableFile);
  return d->m_InputFile_Cache;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setLastRead(const QDateTime& value)
{
  Q_D(ImportOnscaleTableFile);
  d->m_LastRead = value;
}

// -----------------------------------------------------------------------------
QDateTime ImportOnscaleTableFile::getLastRead() const
{
  Q_D(const ImportOnscaleTableFile);
  return d->m_LastRead;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, ImportOnscaleTableFile, "*.dx"));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, ImportOnscaleTableFile));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, ImportOnscaleTableFile));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", VolumeDataContainerName, FilterParameter::CreatedArray, ImportOnscaleTableFile));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, VolumeDataContainerName, FilterParameter::CreatedArray, ImportOnscaleTableFile));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Cell Feature Ids", FeatureIdsArrayName, VolumeDataContainerName, CellAttributeMatrixName, FilterParameter::CreatedArray, ImportOnscaleTableFile));
  parameters.push_back(SeparatorFilterParameter::New("Material Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Phase Attribute Matrix", PhaseAttributeMatrixName, VolumeDataContainerName, FilterParameter::CreatedArray, ImportOnscaleTableFile));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Material Names", MaterialNameArrayName, VolumeDataContainerName, PhaseAttributeMatrixName, FilterParameter::CreatedArray, ImportOnscaleTableFile));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::flushCache()
{
  setInputFile_Cache("");
  std::vector<size_t> v;
  v.push_back(2);
  v.push_back(2);
  v.push_back(2);
  setDims(v);
  setLastRead(QDateTime());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer(this, getVolumeDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  RectGridGeom::Pointer image = RectGridGeom::CreateGeometry(SIMPL::Geometry::RectGridGeometry);
  image->setUnits(IGeometry::LengthUnit::Meter);
  m->setGeometry(image);

  QFileInfo fi(getInputFile());

  if(getInputFile().isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387, ss);
    return;
  }
  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: %1").arg(getInputFile());
    setErrorCondition(-388, ss);
    return;
  }

  if(!getInputFile().isEmpty() && fi.exists())
  {
    QDateTime lastModified(fi.lastModified());

    if(getInputFile() == getInputFile_Cache() && getLastRead().isValid() && lastModified.msecsTo(getLastRead()) >= 0)
    {
      // We are reading from the cache, so set the FileWasRead flag to false
      m_FileWasRead = false;
      std::vector<size_t> v = getDims();
      v[0] -= 1;
      v[1] -= 1;
      v[2] -= 1;
      RectGridGeom::Pointer rectGridGeom = std::dynamic_pointer_cast<RectGridGeom>(m->getGeometry());
      if(nullptr != rectGridGeom.get())
      {
        rectGridGeom->setDimensions(v.data());
      }
    }
    else
    {
      // We are reading from the file, so set the FileWasRead flag to true
      m_FileWasRead = true;

      // We need to read the header of the input file to get the dimensions
      QFile fileStream(getInputFile());
      if(!fileStream.open(QIODevice::ReadOnly | QIODevice::Text))
      {
        QString ss = QObject::tr("Error opening input file: %1").arg(getInputFile());
        setErrorCondition(-100, ss);
        return;
      }

      int32_t error = readHeader(fileStream);
      fileStream.close();
      if(error < 0)
      {
        QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file. Is the input file a Dx file?");
        setErrorCondition(error, ss);
      }

      // Set the file path and time stamp into the cache
      setLastRead(QDateTime::currentDateTime());
      setInputFile_Cache(getInputFile());
    }
  }

  // Create the Cell Data AM and DA
  RectGridGeom::Pointer rectGridGeom = std::dynamic_pointer_cast<RectGridGeom>(m->getGeometry());
  std::vector<size_t> tDims = rectGridGeom->getDimensions().toContainer<std::vector<size_t>>();
  m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> cDims = {1};
  tempPath.update(getVolumeDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType>(this, tempPath, 0, cDims, "", DataArrayID31);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  // Create the Phase Data AM and DA
  std::vector<QString> names = getNames();
  m->createNonPrereqAttributeMatrix(this, getPhaseAttributeMatrixName(), {names.size()}, AttributeMatrix::Type::CellEnsemble, AttributeMatrixID22);
  if(getErrorCode() < 0)
  {
    return;
  }

  tempPath.update(getVolumeDataContainerName().getDataContainerName(), getPhaseAttributeMatrixName(), getMaterialNameArrayName());
  m_MaterialNamesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<StringDataArray>(this, tempPath, nullptr, cDims, "", DataArrayID32);
  if(getErrorCode() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  QFile fileStream(getInputFile());
  if(!fileStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
    setErrorCondition(-100, ss);
    return;
  }

  int32_t err = readFile(fileStream);
  fileStream.close();
  if(err < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
int32_t parseValues(QFile& in, FloatArrayType& data)
{
  bool ok = false;
  size_t count = 0;
  while(count < data.size())
  {
    QString line = in.readLine().trimmed();
    QStringList tokens = line.split(" ", Qt::SkipEmptyParts);
    for(const auto& token : tokens)
    {
      float value = token.toFloat(&ok);
      if(ok)
      {
        data[count] = value;
        count++;
      }
      else
      {
        return -1;
      }
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
int32_t parseValues(QFile& in, Int32ArrayType& data)
{
  bool ok = false;
  size_t count = 0;
  while(count < data.size())
  {
    QString line = in.readLine().trimmed();
    QStringList tokens = line.split(" ", Qt::SkipEmptyParts);
    for(const auto& token : tokens)
    {
      int32_t value = token.toInt(&ok);
      if(ok)
      {
        data[count] = value;
        count++;
      }
      else
      {
        return -1;
      }
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
int32_t parseValues(QFile& in, std::vector<QString>& data)
{
  size_t count = 0;
  while(count < data.size())
  {
    QString line = in.readLine().trimmed();
    QStringList tokens = line.split(" ", Qt::SkipEmptyParts);
    for(const auto& token : tokens)
    {
      data[count] = token;
      count++;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
int32_t ImportOnscaleTableFile::readHeader(QFile& fileStream)
{
  QString ss;
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());
  RectGridGeom::Pointer rectGridGeom = std::dynamic_pointer_cast<RectGridGeom>(m->getGeometry());

  FloatArrayType::Pointer xValues;
  FloatArrayType::Pointer yValues;
  FloatArrayType::Pointer zValues;
  std::vector<QString> names;

  int32_t error = 0;

  QByteArray buf;
  QStringList tokens; /* vector to store the split data */

  bool ok = false;
  // Process the header information and look for the QString "counts"
  // Then read the data size after that
  size_t nx = 2;
  size_t ny = 2;
  size_t nz = 2;
  bool done = false;
  bool nxDone = false;
  bool nyDone = false;
  bool nzDone = false;
  while(!fileStream.atEnd() && !done)
  {
    QString line = fileStream.readLine();

    if(line.startsWith("xcrd"))
    {
      line = line.trimmed();
      tokens = line.split(" ", Qt::SkipEmptyParts);
      nx = tokens[1].toULongLong(&ok);
      nxDone = true;
      xValues = FloatArrayType::CreateArray(nx, "X Bounds", true);
      rectGridGeom->setXBounds(xValues);
      parseValues(fileStream, *xValues);
    }
    else if(line.startsWith("ycrd"))
    {
      line = line.trimmed();
      tokens = line.split(" ", Qt::SkipEmptyParts);
      ny = tokens[1].toULongLong(&ok);
      nyDone = true;
      yValues = FloatArrayType::CreateArray(ny, "Y Bounds", true);
      rectGridGeom->setYBounds(yValues);
      parseValues(fileStream, *yValues);
    }
    else if(line.startsWith("zcrd"))
    {
      line = line.trimmed();
      tokens = line.split(" ", Qt::SkipEmptyParts);
      nz = tokens[1].toULongLong(&ok);
      nzDone = true;
      zValues = FloatArrayType::CreateArray(nx, "Z Bounds", true);
      rectGridGeom->setZBounds(zValues);
      parseValues(fileStream, *zValues);
    }
    else if(line.startsWith("name"))
    {
      line = line.trimmed();
      QStringList tokens = line.split(" ", Qt::SkipEmptyParts);
      size_t numNames = tokens[1].toULongLong(&ok);
      names.resize(numNames);
      parseValues(fileStream, names);

      if(!nxDone)
      {
        nxDone = true;
      }
      if(!nyDone)
      {
        nyDone = true;
      }
      if(!nzDone)
      {
        nzDone = true;
      }
    }
    else if(line.startsWith("matr"))
    {
      if(!nxDone)
      {
        nxDone = true;
      }
      if(!nyDone)
      {
        nyDone = true;
      }
      if(!nzDone)
      {
        nzDone = true;
      }
    }

    if(nxDone && nyDone && nzDone)
    {
      done = true;
    }
  }

  // Set the values into the cache, so that they can be used later
  setDims({nx, ny, nz});
  setCoords({xValues, yValues, zValues});
  setNames(names);

  if(nullptr != m.get())
  {
    RectGridGeom::Pointer rectGridGeom = std::dynamic_pointer_cast<RectGridGeom>(m->getGeometry());
    if(nullptr != rectGridGeom.get())
    {
      std::vector<size_t> v;
      v.push_back(nx - 1);
      v.push_back(ny - 1);
      v.push_back(nz - 1);
      rectGridGeom->setDimensions(v.data());
      rectGridGeom->setXBounds(xValues);
      rectGridGeom->setYBounds(yValues);
      rectGridGeom->setZBounds(zValues);
    }
  }

  return error;
}

// -----------------------------------------------------------------------------
int32_t ImportOnscaleTableFile::readFile(QFile& fileStream)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());

  RectGridGeom::Pointer rectGridGeom = std::dynamic_pointer_cast<RectGridGeom>(m->getGeometry());
  SizeVec3Type tDims = getDims();
  AttributeMatrix::Pointer am = m->getAttributeMatrix(getCellAttributeMatrixName());

  AttributeMatrix::Pointer enAm = m->getAttributeMatrix(getPhaseAttributeMatrixName());
  bool ok = false;

  FloatArrayType::Pointer xValues = FloatArrayType::CreateArray(tDims[0], "X Bounds", true);
  rectGridGeom->setXBounds(xValues);

  FloatArrayType::Pointer yValues = FloatArrayType::CreateArray(tDims[1], "Y Bounds", true);
  rectGridGeom->setYBounds(yValues);

  FloatArrayType::Pointer zValues = FloatArrayType::CreateArray(tDims[2], "Z Bounds", true);
  rectGridGeom->setZBounds(zValues);

  Int32ArrayType::Pointer featureIds = std::dynamic_pointer_cast<Int32ArrayType>(am->getAttributeArray(getFeatureIdsArrayName()));
  if(featureIds->allocate() < 0)
  {
    QString ss = QObject::tr("Error trying to allocate the Material array.");
    setErrorCondition(-1, ss);
    return -1;
  }

  StringDataArray::Pointer matNames = std::dynamic_pointer_cast<StringDataArray>(enAm->getAttributeArray(getMaterialNameArrayName()));
  std::array<bool, 3> boundsRead = {false, false, false};
  std::vector<QString> names;

  while(!fileStream.atEnd())
  {
    QString line = fileStream.readLine();

    if(line.startsWith("xcrd"))
    {
      QString ss = QObject::tr("Reading X Axis Values");
      notifyStatusMessage(ss);
      parseValues(fileStream, *xValues);
      boundsRead[0] = true;
    }
    else if(line.startsWith("ycrd"))
    {
      QString ss = QObject::tr("Reading Y Axis Values");
      notifyStatusMessage(ss);
      parseValues(fileStream, *yValues);
      boundsRead[1] = true;
    }
    else if(line.startsWith("zcrd"))
    {
      QString ss = QObject::tr("Reading Z Axis Values");
      notifyStatusMessage(ss);
      parseValues(fileStream, *zValues);
      boundsRead[2] = true;
    }
    else if(line.startsWith("name"))
    {
      QString ss = QObject::tr("Reading Names");
      notifyStatusMessage(ss);
      line = line.trimmed();
      QStringList tokens = line.split(" ", Qt::SkipEmptyParts);
      size_t numNames = tokens[1].toULongLong(&ok);
      names.resize(numNames);
      parseValues(fileStream, names);
      for(size_t i = 0; i < names.size(); i++)
      {
        matNames->setValue(i, names[i]);
      }
    }
    else if(line.startsWith("matr"))
    {
      QString ss = QObject::tr("Reading Material Values");
      notifyStatusMessage(ss);
      line = line.trimmed();
      QStringList tokens = line.split(" ", Qt::SkipEmptyParts);
      size_t numGridElements = tokens[1].toULongLong(&ok);
      // featureIds->resizeTuples(numGridElements);
      parseValues(fileStream, *featureIds);
    }
  }

  // Sanity check the X, Y or Z Bounds
  if(!boundsRead[0])
  {
    xValues->resizeTuples(2);
    float value = 0.0f;
    xValues->setTuple(0, &value);
    value = value + m_Spacing[0];
    xValues->setTuple(1, &value);
  }
  if(!boundsRead[1])
  {
    yValues->resizeTuples(2);
    float value = 0.0f;
    yValues->setTuple(0, &value);
    value = value + m_Spacing[1];
    yValues->setTuple(1, &value);
  }
  if(!boundsRead[2])
  {
    zValues->resizeTuples(2);
    float value = 0.0f;
    zValues->setTuple(0, &value);
    value = value + m_Spacing[2];
    zValues->setTuple(1, &value);
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportOnscaleTableFile::newFilterInstance(bool copyFilterParameters) const
{
  ImportOnscaleTableFile::Pointer filter = ImportOnscaleTableFile::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getBrandingString() const
{
  return "ImportExport";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getSubGroupName() const
{
  return "ImportExport";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getHumanLabel() const
{
  return "Import Onscale Table File (.flxtbl)";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ImportOnscaleTableFile::getUuid() const
{
  return QUuid("{06dd6e66-84fb-5170-a923-d925dc39bb94}");
}

// -----------------------------------------------------------------------------
ImportOnscaleTableFile::Pointer ImportOnscaleTableFile::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ImportOnscaleTableFile> ImportOnscaleTableFile::New()
{
  struct make_shared_enabler : public ImportOnscaleTableFile
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getNameOfClass() const
{
  return QString("ImportOnscaleTableFile");
}

// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::ClassName()
{
  return QString("ImportOnscaleTableFile");
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setVolumeDataContainerName(const DataArrayPath& value)
{
  m_VolumeDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ImportOnscaleTableFile::getVolumeDataContainerName() const
{
  return m_VolumeDataContainerName;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type ImportOnscaleTableFile::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type ImportOnscaleTableFile::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setFileWasRead(bool value)
{
  m_FileWasRead = value;
}

// -----------------------------------------------------------------------------
bool ImportOnscaleTableFile::getFileWasRead() const
{
  return m_FileWasRead;
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}
// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setPhaseAttributeMatrixName(const QString& value)
{
  m_PhaseAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getPhaseAttributeMatrixName() const
{
  return m_PhaseAttributeMatrixName;
}
// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::setMaterialNameArrayName(const QString& value)
{
  m_MaterialNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString ImportOnscaleTableFile::getMaterialNameArrayName() const
{
  return m_MaterialNameArrayName;
}
