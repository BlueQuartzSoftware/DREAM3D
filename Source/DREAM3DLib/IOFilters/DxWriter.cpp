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

#include "DxWriter.h"

#include <QtCore/QtDebug>
#include <fstream>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxWriter::DxWriter() :
FileWriter(), 
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName), 
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds), 
  m_AddSurfaceLayer(false), 
  m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxWriter::~DxWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.dx");
    option->setFileType("Open DX Visualization");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Add Surface Layer");
    option->setPropertyName("AddSurfaceLayer");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readValue( "OutputFile", getOutputFile() ) );
  setAddSurfaceLayer( reader->readValue("AddSurfaceLayer", getAddSurfaceLayer()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile());
  writer->writeValue("AddSurfaceLayer", getAddSurfaceLayer());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  QString ss;
  if (getOutputFile().isEmpty() == true)
  {
    ss = QObject::tr( ": The output file must be set before executing this filter.");
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }
  QFileInfo fi(getOutputFile());
  QDir parentPath = fi.path();
  if (parentPath.exists() == false)
  {
    ss = QObject::tr( "The directory path for the output file does not exist.");
    addWarningMessage(getHumanLabel(), ss, -1);
  }

  if (fi.suffix().compare("") == 0)
  {
    setOutputFile(getOutputFile().append(".dx"));
  }

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxWriter::writeFile()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if (NULL == m)
  {
    QString ss;
    ss = QObject::tr("DataContainer Pointer was NULL and Must be valid.%1(%2)").arg(__FILE__).arg(__LINE__);
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return -1;
  }

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, 1, 1);
  if(getErrorCondition() < 0)
  {
    return -40;
  }

  //GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(m, Cell, DREAM3D::CellData::GrainIds, Int32ArrayType, int32_t, totalPoints, grain_indicies);

  int err = 0;
  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };
  // qDebug() << "Write Dx Grain File:  x, y, z: " << dims[0] << " " << dims[1] << " " << dims[2] ;

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());

  QDir dir(fi.path());
  if(!dir.mkpath("."))
  {
    QString ss;
    ss = QObject::tr("Error creating parent path '%1'").arg(dir.path());
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return -1;
  }

  QFile file(getOutputFile());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("DxWriter Input file could not be opened: %1").arg(getOutputFile());
    setErrorCondition(-100);
    notifyErrorMessage(ss, getErrorCondition());
    return -100;
  }

  QTextStream out(&file);
  DimType fileXDim = dims[0];
  DimType fileYDim = dims[1];
  DimType fileZDim = dims[2];

  DimType posXDim = fileXDim + 1;
  DimType posYDim = fileYDim + 1;
  DimType posZDim = fileZDim + 1;

  if(m_AddSurfaceLayer)
  {
    fileXDim = dims[0] + 2;
    fileYDim = dims[1] + 2;
    fileZDim = dims[2] + 2;

    posXDim = fileXDim + 1;
    posYDim = fileYDim + 1;
    posZDim = fileZDim + 1;
  }
  totalPoints = fileXDim * fileYDim * fileZDim;
  //Write the header
  out << "object 1 class gridpositions counts " << posZDim << " " << posYDim << " " << posXDim << "\n";
  out << "origin 0 0 0" << "\n";
  out << "delta  1 0 0" << "\n";
  out << "delta  0 1 0" << "\n";
  out << "delta  0 0 1" << "\n";
  out << "\n";
  out << "object 2 class gridconnections counts " << posZDim << " " << posYDim << " " << posXDim << "\n";
  out << "\n";
  out << "object 3 class array type int rank 0 items " << totalPoints << " data follows" << "\n";

  // Add a complete layer of surface voxels
  size_t rnIndex = 1;
  if(m_AddSurfaceLayer)
  {
    for (int i = 0; i < (fileXDim * fileYDim); ++i)
    {
      out << "-3 ";
      if(rnIndex == 20)
      {
        rnIndex = 0;
        out << "\n";
      }
      rnIndex++;
    }
  }

  int index = 0;
  for (DimType z = 0; z < dims[2]; ++z)
  {
    // Add a leading surface Row for this plane if needed
    if(m_AddSurfaceLayer)
    {
      for (int i = 0; i < fileXDim; ++i)
      {
        out << "-4 ";
      }
      out << "\n";
    }
    for (DimType y = 0; y < dims[1]; ++y)
    {
      // write leading surface voxel for this row
      if(m_AddSurfaceLayer)
      {
        out << "-5 ";
      }
      // Write the actual voxel data
      for (DimType x = 0; x < dims[0]; ++x)
      {
        if(m_GrainIds[index] == 0)
        {
          out << "0" << " ";
        }
        else
        {
          out << m_GrainIds[index] << " ";
        }
        ++index;
      }
      // write trailing surface voxel for this row
      if(m_AddSurfaceLayer)
      {
        out << "-6 ";
      }
      out << "\n";
    }
    // Add a trailing surface Row for this plane if needed
    if(m_AddSurfaceLayer)
    {
      for (int i = 0; i < fileXDim; ++i)
      {
        out << "-7 ";
      }
      out << "\n";
    }
  }

  // Add a complete layer of surface voxels
  if(m_AddSurfaceLayer)
  {
    rnIndex = 1;
    for (int i = 0; i < (fileXDim * fileYDim); ++i)
    {
      out << "-8 ";
      if(rnIndex == 20)
      {
        out << "\n";
        rnIndex = 0;
      }
      rnIndex++;
    }
  }
  out << "\n";
  out << "attribute \"dep\" string \"connections\"" << "\n";
  out << "\n";
  out << "object \"DREAM3D Generated\" class field" << "\n";
  out << "component  \"positions\"    value 1" << "\n";
  out << "component  \"connections\"  value 2" << "\n";
  out << "component  \"data\"         value 3" << "\n";
  out << "" << "\n";
  out << "end" << "\n";

  file.close();
#if 0
  out.open("/tmp/m3cmesh.raw", std::ios_base::binary);
  out.write((const char*)(&dims[0]), 4);
  out.write((const char*)(&dims[1]), 4);
  out.write((const char*)(&dims[2]), 4);
  getTotalPoints = dims[0] * dims[1] * dims[2];
  int32_t d = 0;
  for(int index = 0; index < getTotalPoints; ++index)
  {
    d = grainIds[index];
    if (d == 0)
    { d = -3;}
    out.write((const char*)(&d), sizeof(d));
  }

  out.close();
#endif

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Complete");
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

