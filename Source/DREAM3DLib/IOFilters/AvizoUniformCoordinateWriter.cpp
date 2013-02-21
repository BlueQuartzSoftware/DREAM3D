/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "AvizoUniformCoordinateWriter.h"

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"

#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AvizoUniformCoordinateWriter::AvizoUniformCoordinateWriter() :
    AbstractFilter(), m_GrainIdsArrayName(DREAM3D::CellData::GrainIds), m_WriteGrainIds(true), m_WriteBinaryFile(false), m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AvizoUniformCoordinateWriter::~AvizoUniformCoordinateWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.am");
    option->setFileType("Amira Mesh");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Binary File");
    option->setPropertyName("WriteBinaryFile");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("OutputFile", getOutputFile());
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_OutputFile.empty() == true)
  {
    ss << "The output file must be set before executing this filter.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-1);
  }
  if(m_WriteGrainIds == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
   * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AvizoUniformCoordinateWriter::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(m_OutputFile);
  if(!MXADir::mkdir(parentPath, true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembleTuples = m->getNumEnsembleTuples();

  dataCheck(false, totalPoints, totalFields, totalEnsembleTuples);

  MXAFileWriter64 writer(m_OutputFile);
  if(false == writer.initWriter())
  {
    std::stringstream ss;
    ss << "Error opening file '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }

  std::string header = generateHeader();
  writer.writeString(header);

  err = writeData(writer);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AvizoUniformCoordinateWriter::generateHeader()
{
  std::stringstream ss;
  if(m_WriteBinaryFile == true)
  {
#ifdef CMP_WORDS_BIGENDIAN
    ss << "# AmiraMesh BINARY 2.1\n";
#else
    ss << "# AmiraMesh BINARY-LITTLE-ENDIAN 2.1\n";
#endif
  }
  else
  {
    ss << "# AmiraMesh 3D ASCII 2.0\n";
  }
  ss << "\n";
  ss << "# Dimensions in x-, y-, and z-direction\n";
  size_t x = 0, y = 0, z = 0;
  getVoxelDataContainer()->getDimensions(x, y, z);
  ss << "define Lattice " << x << " " << y << " " << z << "\n\n";

  ss << "Parameters {\n";
  ss << "     DREAM3DParams {\n";
  ss << "         Author \"DREAM3D\",\n";
  ss << "         DateTime \"" << tifDateTime() << "\"\n";
  ss << "     }\n";

  ss << "     Units {\n";
  ss << "         Coordinates \"microns\"\n";
  ss << "     }\n";
  ss << "     Content \"" << x << "x" << y << "x" << z << " int, uniform coordinates\",\n";
  float origin[3];
  getVoxelDataContainer()->getOrigin(origin);
  float res[3];
  getVoxelDataContainer()->getResolution(res);
  ss << "     # Bounding Box is xmin xmax ymin ymax zmin zmax\n";
  ss << "     BoundingBox " << origin[0] << " " << origin[0] + (res[0] * x);
  ss << " " << origin[1] << " " << origin[1] + (res[1] * x);
  ss << " " << origin[2] << " " << origin[2] + (res[2] * x);
  ss << ",\n";
  ss << "     CoordType \"uniform\"\n";
  ss << "}\n\n";

  ss << "Lattice { int GrainIds } = @1\n\n";

  ss << "# Data section follows\n";

  return ss.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AvizoUniformCoordinateWriter::writeData(MXAFileWriter64 &writer)
{
  std::string start("@1\n");
  writer.writeString(start);
  if(true == m_WriteBinaryFile)
  {
    writer.writeArray(m_GrainIds, getVoxelDataContainer()->getTotalPoints());
  }
  else
  {
    // The "20 Items" is purely arbitrary and is put in to try and save some space in the ASCII file
    int64_t totalPoints = getVoxelDataContainer()->getTotalPoints();
    int count = 0;
    std::stringstream ss;
    for (int64_t i = 0; i < totalPoints; ++i)
    {
      ss << m_GrainIds[i];
      if(count < 20)
      {
        ss << " ";
        count++;
      }
      else
      {
        ss << "\n";
        writer.writeString(ss.str());
        ss.str("");
        count = 0;
      }
    }
    // Pick up any remaining data that was not written because we did not have 20 items on a line.
    writer.writeString(ss.str());
  }
  return 1;
}

