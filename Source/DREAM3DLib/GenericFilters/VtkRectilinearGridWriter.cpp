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


#include "VtkRectilinearGridWriter.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::VtkRectilinearGridWriter() :
AbstractFilter(),
m_WriteGrainIds(true),
m_WritePhaseIds(false),
m_WriteGoodVoxels(false),
m_WriteIPFColors(false),
m_WriteBinaryFile(false)
{
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::~VtkRectilinearGridWriter()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterOption::InputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Write Grain Ids");
    option->setPropertyName("WriteGrainIds");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Write Phase Ids");
    option->setPropertyName("WritePhaseIds");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Write Good Voxels");
    option->setPropertyName("WriteGoodVoxels");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Write IPF Colors");
    option->setPropertyName("WriteIPFColors");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Write Binary File");
    option->setPropertyName("WriteBinaryFile");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  setFilterOptions(options);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

  if(m_OutputFile.empty() == true)
  {
    ss << getNameOfClass() << ": The output file must be set before executing this filter.";
    setErrorCondition(-1);
  }

  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::execute()
{
  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->totalPoints();
  size_t totalFields = m->getTotalFields();
  size_t totalEnsembleTuples = m->getNumEnsembleTuples();


  dataCheck(false, totalPoints, totalFields, totalEnsembleTuples);



  // Setup all the classes that will help us write the Scalars to the VTK File
  std::vector<VtkScalarWriter*> scalarsToWrite;

  if (m_WriteGrainIds == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<DataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if(m_WritePhaseIds == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<DataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteGoodVoxels == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGoodVoxelScalarWriter<DataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteIPFColors == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelIPFColorScalarWriter<DataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }


  int err = write(m_OutputFile, m, scalarsToWrite);
  // Now Delete all the Scalar Helpers that we just created and used.
  for (std::vector<VtkScalarWriter*>::iterator iter = scalarsToWrite.begin(); iter != scalarsToWrite.end(); ++iter)
  {
    delete (*iter);
  }

  if (err < 0)
  {
    std::stringstream ss;
    ss << getNameOfClass() << ": Error writing output vtk file '" << m_OutputFile << "'\n "
        << getErrorMessage();
    setErrorMessage(ss.str());
    setErrorCondition(-1);
  }

  notify("VtkRectilinearGridWriter Complete", 0, Observable::UpdateProgressMessage);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkRectilinearGridWriter::write(const std::string &file, DataContainer* r, std::vector<VtkScalarWriter*> &scalars)
{
  int err = 0;
  FILE* f = NULL;
  f = fopen(file.c_str(), "wb");
  if(NULL == f)
  {
    return -1;
  }
  // Write the correct header
  if(m_WriteBinaryFile == true)
  {
    WRITE_RECTILINEAR_GRID_HEADER("BINARY", r, r->getXPoints() + 1, r->getYPoints()+1, r->getZPoints()+1)
  }
  else
  {
    WRITE_RECTILINEAR_GRID_HEADER("ASCII", r, r->getXPoints() + 1, r->getYPoints()+1, r->getZPoints()+1)
  }

  // Write the XCoords
  writeCoords(f, "X_COORDINATES", "float", r->getXPoints() + 1, 0.0f - r->getXRes() * 0.5f, (float)(r->getXPoints() + 1 * r->getXRes()), r->getXRes());
  writeCoords(f, "Y_COORDINATES", "float", r->getYPoints() + 1, 0.0f - r->getYRes() * 0.5f, (float)(r->getYPoints() + 1 * r->getYRes()), r->getYRes());
  writeCoords(f, "Z_COORDINATES", "float", r->getZPoints() + 1, 0.0f - r->getZRes() * 0.5f, (float)(r->getZPoints() + 1 * r->getZRes()), r->getZRes());

  size_t total = r->getXPoints() * r->getYPoints() * r->getZPoints();
  fprintf(f, "CELL_DATA %d\n", (int)total);

  // Now loop on all of our Scalars and write those arrays as CELL_DATA
  for (std::vector<VtkScalarWriter*>::iterator iter = scalars.begin(); iter != scalars.end(); ++iter)
  {
    err = (*iter)->writeScalars(f);
    if(err < 0)
    {
      setErrorCondition((*iter)->getErrorCondition());
      setErrorMessage((*iter)->getErrorMessage());
      break;
    }
  }
  // Close the file
  fclose(f);
  return err;
}

