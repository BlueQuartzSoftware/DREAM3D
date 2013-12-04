/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "VASPReader.h"

#include <QtCore/QtDebug>
#include <fstream>

#include <QtCore/QFileInfo>

#include "DREAM3DLib/Math/MatrixMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VASPReader::VASPReader() :
  FileReader(),
  m_VertexDataContainerName(DREAM3D::Defaults::VertexDataContainerName),
  m_VertexAttributeMatrixName(DREAM3D::Defaults::VertexAttributeMatrixName),
  m_InputFile(""),
  m_AtomVelocitiesArrayName(DREAM3D::VertexData::AtomVelocities),
  m_AtomTypesArrayName(DREAM3D::VertexData::AtomTypes),
  m_AtomVelocities(NULL),
  m_AtomTypes(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VASPReader::~VASPReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VASPReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setFileExtension("*");
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void VASPReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VASPReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VASPReader::dataCheck()
{
  setErrorCondition(0);
  VertexDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VertexDataContainer, AbstractFilter>(this, getVertexDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* attrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  QFileInfo fi(getInputFile());

  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist.");
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  QVector<int> dims(1, 3);
  m_AtomVelocitiesPtr = attrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_AtomVelocitiesArrayName, 0.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AtomVelocitiesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AtomVelocities = m_AtomVelocitiesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_AtomTypesPtr = attrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_AtomTypesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AtomTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AtomTypes = m_AtomTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (m_InStream.isOpen() == true)
  {
    m_InStream.close();
  }

  if (getInputFile().isEmpty() == false && fi.exists() == true)
  {
    // We need to read the header of the input file to get the dimensions
    m_InStream.setFileName(getInputFile());
    if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QString ss = QObject::tr("VASPReader Input file could not be opened: %1").arg(getInputFile());
      setErrorCondition(-100);
      notifyErrorMessage(ss, getErrorCondition());
      return;
    }

    int error = readHeader();
    m_InStream.close();
    if (error < 0)
    {
      setErrorCondition(error);
      QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file. Is the input file a Dx file?");
      addErrorMessage(getHumanLabel(), ss, -11000);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VASPReader::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VASPReader::execute()
{
  int err = 0;

  m_InStream.setFileName(getInputFile());
  if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("VASPReader Input file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(ss, getErrorCondition());
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

  dataCheck();

  VertexDataContainer* m = getDataContainerArray()->getDataContainerAs<VertexDataContainer>(getVertexDataContainerName());

  int error = 0;

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */

  bool ok = false;
  // Process the header information
//  bool done = false;
  //read Title line
  buf = m_InStream.readLine();
  //read Lattice Constant line
  buf = m_InStream.readLine();
  buf = buf.trimmed();
  tokens = buf.split(' ');
  latticeConstant = tokens[0].toFloat(&ok);
  //read Lattice Vector lines
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
  //read Atom Type line
  buf = m_InStream.readLine();
  //read Atom Number line
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
  VertexArray::Pointer vertices = VertexArray::CreateArray(totalAtoms, DREAM3D::VertexData::SurfaceMeshNodes);
  m->setVertices(vertices);

  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VASPReader::readFile()
{
  dataCheck();

  VertexDataContainer* m = getDataContainerArray()->getDataContainerAs<VertexDataContainer>(getVertexDataContainerName());

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */

  VertexArray::Pointer verticesPtr = m->getVertices();
  VertexArray::Vert_t* vertex = verticesPtr.get()->getPointer(0);
  totalAtoms = verticesPtr->getNumberOfTuples();

  // Resize the vertex attribute matrix 
  m->getAttributeMatrix(getVertexAttributeMatrixName())->resizeAttributeArrays(totalAtoms);
  dataCheck();

  if (getErrorCondition() < 0)
  {
    m_InStream.close();
    return -1;
  }
  //read the blank line
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
      vertex[index].pos[0] = posMod[0];
      vertex[index].pos[1] = posMod[1];
      vertex[index].pos[2] = posMod[2];
      m_AtomTypes[index] = i;
      index++;
    }
  }
  //read the blank line
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

  notifyStatusMessage("Complete");
  return 0;
}

