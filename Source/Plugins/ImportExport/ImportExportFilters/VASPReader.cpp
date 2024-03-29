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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "VASPReader.h"

#include <QtCore/QtDebug>
#include <fstream>

#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Math/MatrixMath.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID31 = 31,
  DataArrayID32 = 32,
  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VASPReader::VASPReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VASPReader::~VASPReader() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VASPReader::setupFilterParameters()
{
  FileReader::setupFilterParameters();
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Category::Parameter, VASPReader, "*"));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Vertex Data Container", VertexDataContainerName, FilterParameter::Category::CreatedArray, VASPReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName, FilterParameter::Category::CreatedArray, VASPReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("AtomVelocities", AtomVelocitiesArrayName, FilterParameter::Category::CreatedArray, VASPReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("AtomTypes", AtomTypesArrayName, FilterParameter::Category::CreatedArray, VASPReader));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void VASPReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVertexDataContainerName(reader->readDataArrayPath("VertexDataContainerName", getVertexDataContainerName()));
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName()));
  setAtomTypesArrayName(reader->readString("AtomTypesArrayName", getAtomTypesArrayName()));
  setAtomVelocitiesArrayName(reader->readString("AtomVelocitiesArrayName", getAtomVelocitiesArrayName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VASPReader::updateVertexInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_AtomVelocitiesPtr.lock())
  {
    m_AtomVelocities = m_AtomVelocitiesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_AtomTypesPtr.lock())
  {
    m_AtomTypes = m_AtomTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VASPReader::initialize()
{
  if(m_InStream.isOpen())
  {
    m_InStream.close();
  }
  latticeConstant = 0.0f;
  for(size_t i = 0; i < 3; i++)
  {
    for(size_t j = 0; j < 3; j++)
    {
      latticeVectors[i][j] = 0.0f;
    }
  }
  atomNumbers.clear();
  totalAtoms = -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VASPReader::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;
  initialize();

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer(this, getVertexDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }
  std::vector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer vertexAttrMat = m->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == vertexAttrMat.get())
  {
    return;
  }

  VertexGeom::Pointer vertex = VertexGeom::CreateGeometry(0, SIMPL::Geometry::VertexGeometry, !getInPreflight());
  m->setGeometry(vertex);

  QFileInfo fi(getInputFile());

  if(getInputFile().isEmpty())
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist.");
    setErrorCondition(-388, ss);
  }

  std::vector<size_t> dims(1, 3);
  tempPath.update(getVertexDataContainerName().getDataContainerName(), getVertexAttributeMatrixName(), getAtomVelocitiesArrayName());
  m_AtomVelocitiesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, tempPath, 0.0, dims, "", DataArrayID31);
  if(nullptr != m_AtomVelocitiesPtr.lock())
  {
    m_AtomVelocities = m_AtomVelocitiesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  tempPath.update(getVertexDataContainerName().getDataContainerName(), getVertexAttributeMatrixName(), getAtomTypesArrayName());
  m_AtomTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>>(this, tempPath, 0, dims, "", DataArrayID32);
  if(nullptr != m_AtomTypesPtr.lock())
  {
    m_AtomTypes = m_AtomTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(!getInputFile().isEmpty() && fi.exists())
  {
    // We need to read the header of the input file to get the dimensions
    m_InStream.setFileName(getInputFile());
    if(!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QString ss = QObject::tr("VASPReader Input file could not be opened: %1").arg(getInputFile());
      setErrorCondition(-100, ss);
      return;
    }

    int error = readHeader();
    m_InStream.close();
    if(error < 0)
    {
      QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file. Is the input file a VASP file?");
      setErrorCondition(error, ss);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VASPReader::execute()
{
  int err = 0;

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  m_InStream.setFileName(getInputFile());
  if(!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("VASPReader Input file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100, ss);
    return;
  }

  err = readHeader();
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

//-----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VASPReader::readHeader()
{
  QString ss;

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVertexDataContainerName());
  AttributeMatrix::Pointer vertexAttrMat = m->getAttributeMatrix(getVertexAttributeMatrixName());

  int error = 0;

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */

  bool ok = false;
  // Process the header information
  //  bool done = false;
  // read Title line
  buf = m_InStream.readLine();
  // read Lattice Constant line
  buf = m_InStream.readLine();
  buf = buf.trimmed();
  tokens = buf.split(' ');
  latticeConstant = tokens[0].toFloat(&ok);
  // read Lattice Vector lines
  buf = m_InStream.readLine();
  buf = buf.trimmed();
  buf = buf.simplified();
  tokens = buf.split(' ');
  latticeVectors[0][0] = tokens[0].toFloat(&ok);
  latticeVectors[0][1] = tokens[1].toFloat(&ok);
  latticeVectors[0][2] = tokens[2].toFloat(&ok);
  buf = m_InStream.readLine();
  buf = buf.trimmed();
  buf = buf.simplified();
  tokens = buf.split(' ');
  latticeVectors[1][0] = tokens[0].toFloat(&ok);
  latticeVectors[1][1] = tokens[1].toFloat(&ok);
  latticeVectors[1][2] = tokens[2].toFloat(&ok);
  buf = m_InStream.readLine();
  buf = buf.trimmed();
  buf = buf.simplified();
  tokens = buf.split(' ');
  latticeVectors[2][0] = tokens[0].toFloat(&ok);
  latticeVectors[2][1] = tokens[1].toFloat(&ok);
  latticeVectors[2][2] = tokens[2].toFloat(&ok);
  // read Atom Type line
  buf = m_InStream.readLine();
  // read Atom Number line
  buf = m_InStream.readLine();
  buf = buf.trimmed();
  buf = buf.simplified();
  tokens = buf.split(' ');
  atomNumbers.resize(tokens.size());
  totalAtoms = 0;
  for(int i = 0; i < tokens.size(); i++)
  {
    atomNumbers[i] = tokens[i].toInt(&ok, 10);
    totalAtoms += tokens[i].toInt(&ok, 10);
  }

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(totalAtoms, SIMPL::VertexData::SurfaceMeshNodes, !getInPreflight());
  m->setGeometry(vertices);

  std::vector<size_t> tDims(1, totalAtoms);
  vertexAttrMat->resizeAttributeArrays(tDims);
  updateVertexInstancePointers();

  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VASPReader::readFile()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVertexDataContainerName());

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */

  VertexGeom::Pointer verticesPtr = m->getGeometryAs<VertexGeom>();
  float* vertex = verticesPtr->getVertexPointer(0);

  // read the blank line
  buf = m_InStream.readLine();
  bool ok = false;

  MatrixMath::Multiply3x3withConstant(latticeVectors, latticeConstant);

  float pos[3];
  float posMod[3];
  int index = 0;
  for(int i = 0; i < atomNumbers.size(); i++)
  {
    for(int j = 0; j < atomNumbers[i]; j++)
    {
      buf = m_InStream.readLine();
      buf = buf.trimmed();
      buf = buf.simplified();
      tokens = buf.split(' ');
      pos[0] = tokens[0].toFloat(&ok);
      pos[1] = tokens[1].toFloat(&ok);
      pos[2] = tokens[2].toFloat(&ok);
      MatrixMath::Multiply3x3with3x1(latticeVectors, pos, posMod);
      vertex[index * 3] = posMod[0];
      vertex[index * 3 + 1] = posMod[1];
      vertex[index * 3 + 2] = posMod[2];
      m_AtomTypes[index] = i;
      index++;
    }
  }
  // read the blank line
  buf = m_InStream.readLine();

  float vel[3];
  float velMod[3];

  index = 0;
  for(int i = 0; i < atomNumbers.size(); i++)
  {
    for(int j = 0; j < atomNumbers[i]; j++)
    {
      buf = m_InStream.readLine();
      buf = buf.trimmed();
      buf = buf.simplified();
      tokens = buf.split(' ');
      vel[0] = tokens[0].toFloat(&ok);
      vel[1] = tokens[1].toFloat(&ok);
      vel[2] = tokens[2].toFloat(&ok);
      MatrixMath::Multiply3x3with3x1(latticeVectors, vel, velMod);
      m_AtomVelocities[3 * index + 0] = velMod[0];
      m_AtomVelocities[3 * index + 1] = velMod[1];
      m_AtomVelocities[3 * index + 2] = velMod[2];
      index++;
    }
  }

  tokens.clear();
  m_InStream.close();

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VASPReader::newFilterInstance(bool copyFilterParameters) const
{
  VASPReader::Pointer filter = VASPReader::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VASPReader::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VASPReader::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VASPReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VASPReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VASPReader::getUuid() const
{
  return QUuid("{24ed1c49-8cd3-5370-b1b2-6035b33a35ee}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VASPReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VASPReader::getHumanLabel() const
{
  return "Import VASP File";
}

// -----------------------------------------------------------------------------
VASPReader::Pointer VASPReader::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<VASPReader> VASPReader::New()
{
  struct make_shared_enabler : public VASPReader
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString VASPReader::getNameOfClass() const
{
  return QString("VASPReader");
}

// -----------------------------------------------------------------------------
QString VASPReader::ClassName()
{
  return QString("VASPReader");
}

// -----------------------------------------------------------------------------
void VASPReader::setVertexDataContainerName(const DataArrayPath& value)
{
  m_VertexDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath VASPReader::getVertexDataContainerName() const
{
  return m_VertexDataContainerName;
}

// -----------------------------------------------------------------------------
void VASPReader::setVertexAttributeMatrixName(const QString& value)
{
  m_VertexAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString VASPReader::getVertexAttributeMatrixName() const
{
  return m_VertexAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void VASPReader::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString VASPReader::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void VASPReader::setAtomVelocitiesArrayName(const QString& value)
{
  m_AtomVelocitiesArrayName = value;
}

// -----------------------------------------------------------------------------
QString VASPReader::getAtomVelocitiesArrayName() const
{
  return m_AtomVelocitiesArrayName;
}

// -----------------------------------------------------------------------------
void VASPReader::setAtomTypesArrayName(const QString& value)
{
  m_AtomTypesArrayName = value;
}

// -----------------------------------------------------------------------------
QString VASPReader::getAtomTypesArrayName() const
{
  return m_AtomTypesArrayName;
}
