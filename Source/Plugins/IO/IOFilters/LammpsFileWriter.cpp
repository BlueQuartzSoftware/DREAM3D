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

#include "LammpsFileWriter.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QtEndian>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Utilities/SIMPLibEndian.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

// Include the MOC generated file for this class
#include "moc_LammpsFileWriter.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LammpsFileWriter::LammpsFileWriter()
: AbstractFilter()
, m_VertexDataContainerName(SIMPL::Defaults::VertexDataContainerName)
, m_LammpsFile("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LammpsFileWriter::~LammpsFileWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LammpsFileWriter::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Lammps File", LammpsFile, FilterParameter::Parameter, LammpsFileWriter));

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("Vertex Data Container", VertexDataContainerName, FilterParameter::RequiredArray, LammpsFileWriter, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LammpsFileWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setLammpsFile(reader->readString("LammpsFile", getLammpsFile()));
  setVertexDataContainerName(reader->readString("VertexDataContainerName", getVertexDataContainerName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LammpsFileWriter::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LammpsFileWriter::dataCheck()
{
  setErrorCondition(0);

  if(m_LammpsFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Lammps Output file is Not set correctly", -1003);
  }

  DataContainer::Pointer v = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_VertexDataContainerName);
  if(getErrorCondition() < 0)
  {
    return;
  }

  VertexGeom::Pointer vertices = v->getPrereqGeometry<VertexGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0)
  {
    return;
  }

  // We MUST have Nodes
  if(nullptr == vertices->getVertices().get())
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "VertexDataContainer missing Nodes", getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LammpsFileWriter::preflight()
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
void LammpsFileWriter::execute()
{
  // int err = 0;

  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer v = getDataContainerArray()->getDataContainer(getVertexDataContainerName());

  VertexGeom::Pointer vertices = v->getGeometryAs<VertexGeom>();

  int64_t numAtoms = vertices->getNumberOfVertices();

  // Open the output VTK File for writing
  FILE* lammpsFile = nullptr;
  lammpsFile = fopen(m_LammpsFile.toLatin1().data(), "wb");
  if(nullptr == lammpsFile)
  {
    QString ss = QObject::tr(": Error creating LAMMPS output file '%1'").arg(getLammpsFile());
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  float xMin = 1000000000.0;
  float xMax = 0.0;
  float yMin = 1000000000.0;
  float yMax = 0.0;
  float zMin = 1000000000.0;
  float zMax = 0.0;
  int dummy = 0;
  int atomType = 1;
  float pos[3] = {0.0f, 0.0f, 0.0f};

  for(int64_t i = 0; i < numAtoms; i++)
  {
    vertices->getCoords(i, pos);
    if(pos[0] < xMin)
    {
      xMin = pos[0];
    }
    if(pos[0] > xMax)
    {
      xMax = pos[0];
    }
    if(pos[1] < yMin)
    {
      yMin = pos[1];
    }
    if(pos[1] > yMax)
    {
      yMax = pos[1];
    }
    if(pos[2] < zMin)
    {
      zMin = pos[2];
    }
    if(pos[2] > zMax)
    {
      zMax = pos[2];
    }
  }

  fprintf(lammpsFile, "LAMMPS data file from restart file: timestep = 1, procs = 4\n");
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "%lld atoms\n", (long long int)(numAtoms));
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "1 atom types\n");
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "%f %f xlo xhi\n", xMin, xMax);
  fprintf(lammpsFile, "%f %f ylo yhi\n", yMin, yMax);
  fprintf(lammpsFile, "%f %f zlo zhi\n", zMin, zMax);
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "Masses\n");
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "1 63.546\n");
  fprintf(lammpsFile, "\n");
  fprintf(lammpsFile, "Atoms\n");
  fprintf(lammpsFile, "\n");

  // Write the Atom positions (Vertices)
  for(int64_t i = 0; i < numAtoms; i++)
  {
    vertices->getCoords(i, pos);
    fprintf(lammpsFile, "%lld %d %f %f %f %d %d %d\n", (long long int)(i), atomType, pos[0], pos[1], pos[2], dummy, dummy, dummy); // Write the positions to the output file
  }

  fprintf(lammpsFile, "\n");
  // Free the memory
  // Close the input and output files
  fclose(lammpsFile);

  setErrorCondition(0);
  notifyStatusMessage(getHumanLabel(), "Complete");

  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer LammpsFileWriter::newFilterInstance(bool copyFilterParameters)
{
  /*
  * NodesFile
  * TrianglesFile
  * OutputVtkFile
  * WriteBinaryFile
  * WriteConformalMesh
  */
  LammpsFileWriter::Pointer filter = LammpsFileWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LammpsFileWriter::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LammpsFileWriter::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LammpsFileWriter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LammpsFileWriter::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LammpsFileWriter::getSubGroupName()
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString LammpsFileWriter::getHumanLabel()
{
  return "Write Lammps File";
}
