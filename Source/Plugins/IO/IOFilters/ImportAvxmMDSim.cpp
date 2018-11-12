/*
 * Your License or Copyright can go here
 */

#include "ImportAvxmMDSim.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAvxmMDSim::ImportAvxmMDSim()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportAvxmMDSim::~ImportAvxmMDSim() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAvxmMDSim::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAvxmMDSim::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_FILELISTINFO_FP("Input File List", InputFileListInfo, FilterParameter::Parameter, ImportAvxmMDSim));

  QVector<QString> choices;
  choices.push_back("Tab");
  choices.push_back("Space");
  parameters.push_back(SIMPL_NEW_CHOICE_FP("Separator", SeparatorChoice, FilterParameter::Parameter, ImportAvxmMDSim, choices, false));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAvxmMDSim::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QString ss;

  if(m_SeparatorChoice < 0 || m_SeparatorChoice > 1)
  {
    ss = QObject::tr("The separator is set to an unknown type.");
    setErrorCondition(-13000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_InputFileListInfo.InputPath.isEmpty())
  {
    ss = QObject::tr("The input directory must be set.");
    setErrorCondition(-13001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  bool hasMissingFiles = false;
  bool orderAscending = false;
  if(m_InputFileListInfo.Ordering == 0)
  {
    orderAscending = true;
  }
  else if(m_InputFileListInfo.Ordering == 1)
  {
    orderAscending = false;
  }

  // Now generate all the file names the user is asking for and populate the table
  m_FilePathList = FilePathGenerator::GenerateFileList(m_InputFileListInfo.StartIndex, m_InputFileListInfo.EndIndex, m_InputFileListInfo.IncrementIndex, hasMissingFiles, orderAscending,
                                                       m_InputFileListInfo.InputPath, m_InputFileListInfo.FilePrefix, m_InputFileListInfo.FileSuffix, m_InputFileListInfo.FileExtension,
                                                       m_InputFileListInfo.PaddingDigits);
  if(m_FilePathList.empty())
  {
    QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    setErrorCondition(-13002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(hasMissingFiles)
  {
    QString ss = QObject::tr("Red Dot File(s) on the list do NOT exist on the filesystem. Please make sure all files exist.");
    setErrorCondition(-13003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  for(int i = 0; i < m_FilePathList.size(); i++)
  {
    QString filePath = m_FilePathList[i];
    QFileInfo fi(filePath);
    DataArrayPath typeDAPath(fi.baseName(), "CellData", "Type");

    QVector<size_t> tDims(1, 0);
    QVector<size_t> cDims(1, 1);

    DataContainerShPtr dc = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, typeDAPath.getDataContainerName());
    AttributeMatrixShPtr am = dc->createNonPrereqAttributeMatrix(this, typeDAPath, tDims, AttributeMatrix::Type::Cell);
    UInt8ArrayType::Pointer da = am->createNonPrereqArray<UInt8ArrayType, AbstractFilter, uint8_t>(this, typeDAPath.getDataArrayName(), 0, cDims);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportAvxmMDSim::preflight()
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
void ImportAvxmMDSim::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  for(int i = 0; i < m_FilePathList.size(); i++)
  {
    if(getCancel())
    {
      return;
    }

    QString filePath = m_FilePathList[i];

    notifyStatusMessage(getHumanLabel(), tr("Importing file %1 of %2").arg(i + 1).arg(m_FilePathList.size()));

    QFileInfo fi(filePath);
    DataArrayPath typeDAPath(fi.baseName(), "CellData", "Type");

    QStringList lines = readLines(filePath);
    QVector<size_t> tDims(1, lines.size());

    DataContainerShPtr dc = getDataContainerArray()->getDataContainer(typeDAPath);

    AttributeMatrixShPtr am = dc->getAttributeMatrix(typeDAPath);
    am->setTupleDimensions(tDims);

    UInt8ArrayType::Pointer da = am->getAttributeArrayAs<UInt8ArrayType>(typeDAPath.getDataArrayName());

    FloatArrayType::Pointer verts = FloatArrayType::CreateArray(lines.size(), QVector<size_t>(1, 3), "SharedVertexList");

    for(size_t j = 0; j < lines.size(); j++)
    {
      if(getCancel())
      {
        return;
      }

      QString line = lines[j];

      QStringList tokens;
      if(m_SeparatorChoice == static_cast<int>(SeparatorType::Tab))
      {
        tokens = line.split('\t');
      }
      else
      {
        tokens = line.split(' ');
      }

      if(tokens.size() != 4)
      {
        QString ss = QObject::tr("Unexpected column size in file '%1'.  Expected 4 columns, but found %2 at line %3.").arg(filePath).arg(tokens.size()).arg(j + 1);
        setErrorCondition(-13004);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      std::string typeStr = tokens[0].toStdString();
      if(typeStr.size() != 1)
      {
        QString ss = QObject::tr("Unexpected type specifier in file '%1'.  Found '%2' at line %3, column %4.  Expected 'l' or 's'.").arg(filePath).arg(typeStr.c_str()).arg(i + 1).arg(j + 1);
        setErrorCondition(-13004);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      uint8_t type = static_cast<uint8_t>(typeStr[0]);
      da->setValue(j, type);

      bool ok = false;

      float x = tokens[1].toFloat(&ok);
      if(ok)
      {
        verts->setComponent(j, 0, x);
      }

      ok = false;

      float y = tokens[2].toFloat(&ok);
      if(ok)
      {
        verts->setComponent(j, 1, y);
      }

      ok = false;

      float z = tokens[3].toFloat(&ok);
      if(ok)
      {
        verts->setComponent(j, 2, z);
      }
    }

    VertexGeom::Pointer vertex = VertexGeom::CreateGeometry(verts, SIMPL::Geometry::VertexGeometry);
    dc->setGeometry(vertex);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ImportAvxmMDSim::readLines(const QString& filePath)
{
  QStringList result;
  if(filePath.isEmpty())
  {
    return result;
  }
  QFile inputFile(filePath);
  if(inputFile.open(QIODevice::ReadOnly))
  {
    QTextStream in(&inputFile);

    while(!in.atEnd())
    {
      QString line = in.readLine();
      result.push_back(line);
    }
    inputFile.close();
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportAvxmMDSim::newFilterInstance(bool copyFilterParameters) const
{
  ImportAvxmMDSim::Pointer filter = ImportAvxmMDSim::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAvxmMDSim::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAvxmMDSim::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAvxmMDSim::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAvxmMDSim::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAvxmMDSim::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportAvxmMDSim::getHumanLabel() const
{
  return "ImportAvxmMDSim";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ImportAvxmMDSim::getUuid()
{
  return QUuid("{71e18332-4db1-5554-85e6-ae8db5826587}");
}
