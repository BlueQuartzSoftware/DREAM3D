/*
 * Your License or Copyright can go here
 */

#include "ImportOnscaleTableFile.h"

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
#include "SIMPLib/Geometry/ImageGeom.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID31 = 31,

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
//
// -----------------------------------------------------------------------------
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
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ImportOnscaleTableFile::flushCache()
{
  setInputFile_Cache("");
  std::vector<size_t> v;
  v.push_back(1);
  v.push_back(1);
  v.push_back(1);
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

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
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

  if(m_InStream.isOpen())
  {
    m_InStream.close();
  }
  if(!getInputFile().isEmpty() && fi.exists())
  {
    QDateTime lastModified(fi.lastModified());

    if(getInputFile() == getInputFile_Cache() && getLastRead().isValid() && lastModified.msecsTo(getLastRead()) >= 0)
    {
      // We are reading from the cache, so set the FileWasRead flag to false
      m_FileWasRead = false;
      std::vector<size_t> v = getDims();
      ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
      if(nullptr != imageGeom.get())
      {
        imageGeom->setDimensions(v.data());
      }
    }
    else
    {
      // We are reading from the file, so set the FileWasRead flag to true
      m_FileWasRead = true;

      // We need to read the header of the input file to get the dimensions
      m_InStream.setFileName(getInputFile());
      if(!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
      {
        QString ss = QObject::tr("Error opening input file: %1").arg(getInputFile());
        setErrorCondition(-100, ss);
        return;
      }

      int32_t error = readHeader();
      m_InStream.close();
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

  std::vector<size_t> tDims = getDims();
  m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> cDims(1, 1);
  tempPath.update(getVolumeDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType>(this, tempPath, 0, cDims, "", DataArrayID31);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  m->getGeometryAs<ImageGeom>()->setSpacing(std::make_tuple(m_Spacing[0], m_Spacing[1], m_Spacing[2]));
  m->getGeometryAs<ImageGeom>()->setOrigin(std::make_tuple(m_Origin[0], m_Origin[1], m_Origin[2]));
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

  m_InStream.setFileName(getInputFile());
  if(!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
    setErrorCondition(-100, ss);
    return;
  }

  int32_t err = readHeader();
  if(err < 0)
  {
    m_InStream.close();
    return;
  }
  err = readFile();
  m_InStream.close();
  if(err < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
int32_t ImportOnscaleTableFile::readHeader()
{
  QString ss;
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());

  int32_t error = 0;

  QByteArray buf;
  QStringList tokens; /* vector to store the split data */

  bool ok = false;
  // Process the header information and look for the QString "counts"
  // Then read the data size after that
  size_t nx = 1;
  size_t ny = 1;
  size_t nz = 1;
  bool done = false;
  bool nxDone = false;
  bool nyDone = false;
  bool nzDone = false;
  while(!m_InStream.atEnd() && !done)
  {
    QString line = m_InStream.readLine();

    if(line.startsWith("xcrd"))
    {
      line = line.trimmed();
      tokens = line.split(" ", Qt::SkipEmptyParts);
      nx = tokens[1].toULongLong(&ok);
      nxDone = true;
    }
    else if(line.startsWith("ycrd"))
    {
      line = line.trimmed();
      tokens = line.split(" ", Qt::SkipEmptyParts);
      ny = tokens[1].toULongLong(&ok);
      nyDone = true;
    }
    else if(line.startsWith("zcrd"))
    {
      line = line.trimmed();
      tokens = line.split(" ", Qt::SkipEmptyParts);
      nz = tokens[1].toULongLong(&ok);
      nzDone = true;
    }
    if(nxDone && nyDone && nzDone)
    {
      done = true;
    }
  }

  // Set the values into the cache, so that they can be used later
  std::vector<size_t> v;
  v.push_back(nx);
  v.push_back(ny);
  v.push_back(nz);

  setDims(v);

  if(nullptr != m.get())
  {
    ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
    if(nullptr != imageGeom.get())
    {
      imageGeom->setDimensions(v.data());
    }
  }

  return error;
}

// -----------------------------------------------------------------------------
int32_t parseValues(QFile& in, std::vector<float>& data)
{
  bool done = false;
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
int32_t parseValues(QFile& in, std::vector<int32_t>& data)
{
  bool done = false;
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
  bool done = false;
  bool ok = false;
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
int32_t ImportOnscaleTableFile::readFile()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
  SizeVec3Type tDims = imageGeom->getDimensions();

  bool ok = false;

  size_t xIdx = 0, yIdx = 0, zIdx = 0;
  size_t count = 0;

  std::vector<float> xValues(tDims[0], 0.0f);
  std::vector<float> yValues(tDims[1], 0.0f);
  std::vector<float> zValues(tDims[2], 0.0f);
  std::vector<QString> names;
  std::vector<int32_t> featuresIds;
  while(!m_InStream.atEnd())
  {
    QString line = m_InStream.readLine();

    if(line.startsWith("xcrd"))
    {
      parseValues(m_InStream, xValues);
    }
    else if(line.startsWith("ycrd"))
    {
      parseValues(m_InStream, yValues);
    }
    else if(line.startsWith("zcrd"))
    {
      parseValues(m_InStream, zValues);
    }
    else if(line.startsWith("name"))
    {
      line = line.trimmed();
      QStringList tokens = line.split(" ", Qt::SkipEmptyParts);
      size_t numNames = tokens[1].toULongLong(&ok);
      names.resize(numNames);
      parseValues(m_InStream, names);
    }
    else if(line.startsWith("matr"))
    {
      line = line.trimmed();
      QStringList tokens = line.split(" ", Qt::SkipEmptyParts);
      size_t numGridElements = tokens[1].toULongLong(&ok);
      featuresIds.resize(numGridElements);
      parseValues(m_InStream, featuresIds);
    }
  }
  m_InStream.close();

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
