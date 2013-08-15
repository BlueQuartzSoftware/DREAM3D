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
#include "ReadOrientationData.h"


#include <limits>
#include <vector>
#include <sstream>

#include "MXA/Utilities/MXAFileInfo.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfFields.h"
#include "EbsdLib/HKL/CtfReader.h"
#include "EbsdLib/HEDM/MicFields.h"
#include "EbsdLib/HEDM/MicReader.h"

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadOrientationData::ReadOrientationData() :
  AbstractFilter(),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_LatticeConstantsArrayName(DREAM3D::EnsembleData::LatticeConstants),
  m_MaterialNameArrayName(DREAM3D::EnsembleData::MaterialName),
  m_InputFile(""),
  m_RefFrameZDir(Ebsd::UnknownRefFrameZDirection),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadOrientationData::~ReadOrientationData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  /*   For an input file use this code*/
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Input File");
    parameter->setPropertyName("InputFile");
    parameter->setWidgetType(FilterParameter::InputFileWidget);
    parameter->setValueType("string");
    parameter->setFileExtension("*.ang *.ctf");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setInputFile( reader->readValue( "InputFile", getInputFile() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadOrientationData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if (NULL == m)
  {
    ss.str("");
    ss << getHumanLabel() << "The VoxelDataContainer was NULL and this is NOT allowed. There is an error in the programming. Please contact the developers";
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return;
  }

  if (m_InputFile.empty() == true && m_Manufacturer == Ebsd::UnknownManufacturer)
  {
    ss.str("");
    ss << getHumanLabel() << ": Either the H5Ebsd file must exist or the Manufacturer must be set";
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
  }
  else if (MXAFileInfo::exists(m_InputFile) == false)
  {
    ss << "The input file does not exist.";
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else if (m_InputFile.empty() == false)
  {
    int64_t dims[3];

    std::string ext = MXAFileInfo::extension(m_InputFile);
    std::vector<std::string> names;
    if(ext.compare(Ebsd::Ang::FileExt) == 0)
    {
      AngReader reader;
      reader.setFileName(m_InputFile);
      reader.readHeaderOnly();
      dims[0] = reader.getXDimension();
      dims[1] = reader.getYDimension();
      dims[2] = 1; // We are reading a single slice
      m->setDimensions(dims[0], dims[1], dims[2]);
      m->setResolution(reader.getXStep(), reader.getYStep(), 1.0);
      m->setOrigin(0.0f, 0.0f, 0.0f);
      AngFields fields;
      names = fields.getFilterFields<std::vector<std::string> > ();
      for (size_t i = 0; i < names.size(); ++i)
      {
        if (reader.getPointerType(names[i]) == Ebsd::Int32)
        {
          Int32ArrayType::Pointer array = Int32ArrayType::CreateArray(voxels, names[i]);
          m->addCellData(names[i], array);
        }
        else if (reader.getPointerType(names[i]) == Ebsd::Float)
        {
          FloatArrayType::Pointer array = FloatArrayType::CreateArray(voxels, names[i]);
          m->addCellData(names[i], array);
        }
      }
    }
    else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
    {
      CtfReader reader;
      reader.setFileName(m_InputFile);
      reader.readHeaderOnly();
      dims[0] = reader.getXDimension();
      dims[1] = reader.getYDimension();
      dims[2] = 1; // We are reading a single slice
      m->setDimensions(dims[0], dims[1], dims[2]);
      m->setResolution(reader.getXStep(), reader.getYStep(), 1.0);
      m->setOrigin(0.0f, 0.0f, 0.0f);
      CtfFields fields;
      names = fields.getFilterFields<std::vector<std::string> > ();
      for (size_t i = 0; i < names.size(); ++i)
      {
        if (reader.getPointerType(names[i]) == Ebsd::Int32)
        {
          Int32ArrayType::Pointer array = Int32ArrayType::CreateArray(voxels, names[i]);
          m->addCellData(names[i], array);
        }
        else if (reader.getPointerType(names[i]) == Ebsd::Float)
        {
          FloatArrayType::Pointer array = FloatArrayType::CreateArray(voxels, names[i]);
          m->addCellData(names[i], array);
        }
      }
    }
    else if(ext.compare(Ebsd::Mic::FileExt) == 0)
    {
      MicReader reader;
      reader.setFileName(m_InputFile);
      reader.readHeaderOnly();
      dims[0] = reader.getXDimension();
      dims[1] = reader.getYDimension();
      dims[2] = 1; // We are reading a single slice
      m->setDimensions(dims[0], dims[1], dims[2]);
      m->setResolution(reader.getXStep(), reader.getYStep(), 1.0);
      m->setOrigin(0.0f, 0.0f, 0.0f);
      MicFields fields;
      names = fields.getFilterFields<std::vector<std::string> > ();
      for (size_t i = 0; i < names.size(); ++i)
      {
        if (reader.getPointerType(names[i]) == Ebsd::Int32)
        {
          Int32ArrayType::Pointer array = Int32ArrayType::CreateArray(voxels, names[i]);
          m->addCellData(names[i], array);
        }
        else if (reader.getPointerType(names[i]) == Ebsd::Float)
        {
          FloatArrayType::Pointer array = FloatArrayType::CreateArray(voxels, names[i]);
          m->addCellData(names[i], array);
        }
      }
    }
    else
    {
      setErrorCondition(-997);
      ss.str("");
      ss << "The File extension '" << ext << "' was not recognized. Currently .ang or .ctf are the only recognized file extensions";
      addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());

      return;
    }

    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, float, FloatArrayType, 0, voxels, 3)
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, int32_t, Int32ArrayType, 0, voxels, 1)


    typedef DataArray<unsigned int> XTalStructArrayType;
    CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, unsigned int, XTalStructArrayType, Ebsd::CrystalStructure::UnknownCrystalStructure, ensembles, 1)
    CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, LatticeConstants, ss, float, FloatArrayType, 0.0, ensembles, 6)

    StringDataArray::Pointer materialNames = StringDataArray::CreateArray(1, DREAM3D::EnsembleData::MaterialName);
    m->addEnsembleData( DREAM3D::EnsembleData::MaterialName, materialNames);

    ADD_HELP_INDEX_ENTRY(EnsembleData, MaterialName, XTalStructArrayType, 1);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  std::string ext = MXAFileInfo::extension(m_InputFile);
  if(ext.compare(Ebsd::Ang::FileExt) == 0)
  {
    readAngFile();
  }
  else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
  {
    readCtfFile();
  }
  else if(ext.compare(Ebsd::Mic::FileExt) == 0)
  {
    readMicFile();
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::readAngFile()
{
  int err = 0;
  AngReader reader;
  reader.setFileName(m_InputFile);
  err = reader.readFile();
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage(reader.getErrorMessage(), err);
    notifyErrorMessage("AngReader could not read the .ang file.", getErrorCondition());
    return;
  }
  VoxelDataContainer* m = getVoxelDataContainer();

  int64_t dims[3];
  dims[0] = reader.getXDimension();
  dims[1] = reader.getYDimension();
  dims[2] = 1; // We are reading a single slice
  m->setDimensions(dims[0], dims[1], dims[2]);
  m->setResolution(reader.getXStep(), reader.getYStep(), 1.0);
  m->setOrigin(0.0f, 0.0f, 0.0f);

  err = loadInfo<AngReader, AngPhase>(&reader);

  float* f1 = NULL;
  float* f2 = NULL;
  float* f3 = NULL;
  int* phasePtr = NULL;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();


  int64_t totalPoints = m->getTotalPoints();

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ang::PhaseData));
    for (int64_t i = 0; i < totalPoints; i++)
    {
      if (phasePtr[i] < 1)
      {
        phasePtr[i] = 1;
      }
    }
    iArray = Int32ArrayType::CreateArray(totalPoints, DREAM3D::CellData::Phases);
    ::memcpy(iArray->GetPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(DREAM3D::CellData::Phases, iArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::Phi1));
    f2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::Phi));
    f3 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::Phi2));
    fArray = FloatArrayType::CreateArray(totalPoints, 3, DREAM3D::CellData::EulerAngles);
    float* cellEulerAngles = fArray->GetPointer(0);

    for (int64_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
    }
    m->addCellData(DREAM3D::CellData::EulerAngles, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::ImageQuality));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::ImageQuality);
    ::memcpy(fArray->GetPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ang::ImageQuality, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::ConfidenceIndex));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::ConfidenceIndex);
    ::memcpy(fArray->GetPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ang::ConfidenceIndex, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::SEMSignal));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::SEMSignal);
    ::memcpy(fArray->GetPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ang::SEMSignal, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::Fit));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::Fit);
    ::memcpy(fArray->GetPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ang::Fit, fArray);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::readCtfFile()
{
  int err = 0;
  CtfReader reader;
  reader.setFileName(m_InputFile);
  reader.readOnlySliceIndex(0);
  err = reader.readFile();
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage(reader.getErrorMessage(), getErrorCondition());
    return;
  }
  VoxelDataContainer* m = getVoxelDataContainer();

  int64_t dims[3];
  dims[0] = reader.getXDimension();
  dims[1] = reader.getYDimension();
  dims[2] = 1; // We are reading a single slice
  m->setDimensions(dims[0], dims[1], dims[2]);
  m->setResolution(reader.getXStep(), reader.getYStep(), 1.0);
  m->setOrigin(0.0f, 0.0f, 0.0f);

  err = loadInfo<CtfReader, CtfPhase>(&reader);

  float* f1 = NULL;
  float* f2 = NULL;
  float* f3 = NULL;
  int* phasePtr = NULL;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();


  int64_t totalPoints = m->getTotalPoints();
  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::Phase));
    for (int64_t i = 0; i < totalPoints; i++)
    {
      if (phasePtr[i] < 1)
      {
        phasePtr[i] = 1;
      }
    }
    iArray = Int32ArrayType::CreateArray(totalPoints, DREAM3D::CellData::Phases);
    ::memcpy(iArray->GetPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(DREAM3D::CellData::Phases, iArray);
  }
  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler1));
    f2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler2));
    f3 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler3));
    fArray = FloatArrayType::CreateArray(totalPoints, 3, DREAM3D::CellData::EulerAngles);
    float* cellEulerAngles = fArray->GetPointer(0);
    int* cellPhases = iArray->GetPointer(0);

    for (int64_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
      if(m_CrystalStructures[cellPhases[i]] == Ebsd::CrystalStructure::Hexagonal_High)
      {cellEulerAngles[3 * i + 2] = cellEulerAngles[3 * i + 2] + (30.0);}
    }
    m->addCellData(DREAM3D::CellData::EulerAngles, fArray);
  }

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::Bands));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::Bands);
    ::memcpy(iArray->GetPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(Ebsd::Ctf::Bands, iArray);
  }

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::Error));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::Error);
    ::memcpy(iArray->GetPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(Ebsd::Ctf::Error, iArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::MAD));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ctf::MAD);
    ::memcpy(fArray->GetPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ctf::MAD, fArray);
  }

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::BC));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::BC);
    ::memcpy(iArray->GetPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(Ebsd::Ctf::BC, iArray);
  }

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::BS));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::BS);
    ::memcpy(iArray->GetPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(Ebsd::Ctf::BS, iArray);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::readMicFile()
{
  int err = 0;
  MicReader reader;
  reader.setFileName(m_InputFile);
  err = reader.readFile();
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage(reader.getErrorMessage(), getErrorCondition());
    return;
  }
  VoxelDataContainer* m = getVoxelDataContainer();

  int64_t dims[3];
  dims[0] = reader.getXDimension();
  dims[1] = reader.getYDimension();
  dims[2] = 1; // We are reading a single slice
  m->setDimensions(dims[0], dims[1], dims[2]);
  m->setResolution(reader.getXStep(), reader.getYStep(), 1.0);
  m->setOrigin(0.0f, 0.0f, 0.0f);

  err = loadInfo<MicReader, MicPhase>(&reader);

  float* f1 = NULL;
  float* f2 = NULL;
  float* f3 = NULL;
  int* phasePtr = NULL;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  int64_t totalPoints = m->getTotalPoints();

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Mic::Phase));
    for (int64_t i = 0; i < totalPoints; i++)
    {
      if (phasePtr[i] < 1)
      {
        phasePtr[i] = 1;
      }
    }
    iArray = Int32ArrayType::CreateArray(totalPoints, DREAM3D::CellData::Phases);
    ::memcpy(iArray->GetPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(DREAM3D::CellData::Phases, iArray);
  }

  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::Euler1));
    f2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::Euler2));
    f3 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::Euler3));
    fArray = FloatArrayType::CreateArray(totalPoints * 3, DREAM3D::CellData::EulerAngles);
    fArray->SetNumberOfComponents(3);
    float* cellEulerAngles = fArray->GetPointer(0);
    for (int64_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
    }
    m->addCellData(DREAM3D::CellData::EulerAngles, fArray);
  }

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Mic::Phase));
    iArray = Int32ArrayType::CreateArray(totalPoints, DREAM3D::CellData::Phases);
    iArray->SetNumberOfComponents(1);
    ::memcpy(iArray->GetPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(DREAM3D::CellData::Phases, iArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::Confidence));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Mic::Confidence);
    fArray->SetNumberOfComponents(1);
    ::memcpy(fArray->GetPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Mic::Confidence, fArray);
  }

}
