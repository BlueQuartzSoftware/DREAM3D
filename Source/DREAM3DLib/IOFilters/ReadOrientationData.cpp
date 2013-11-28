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

#include <QtCore/QFileInfo>

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
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
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
  FilterParameterVector parameters;

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
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
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
void ReadOrientationData::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QFileInfo fi(m_InputFile);
  if (m_InputFile.isEmpty() == true && m_Manufacturer == Ebsd::UnknownManufacturer)
  {

    QString ss = QObject::tr("%1: Either the H5Ebsd file must exist or the Manufacturer must be set").arg(getHumanLabel());
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss, -1);
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (m_InputFile.isEmpty() == false)
  {
    int64_t dims[3];

    QString ext = fi.suffix();
    QVector<QString> names;
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
      AngFields angfeatures;
      names = angfeatures.getFilterFeatures<QVector<QString> > ();
      for (size_t i = 0; i < names.size(); ++i)
      {
        if (reader.getPointerType(names[i]) == Ebsd::Int32)
        {
          Int32ArrayType::Pointer array = Int32ArrayType::CreateArray(voxels, names[i]);
          m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(names[i], array);
        }
        else if (reader.getPointerType(names[i]) == Ebsd::Float)
        {
          FloatArrayType::Pointer array = FloatArrayType::CreateArray(voxels, names[i]);
          m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(names[i], array);
        }
      }
    }
    else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
    {
      CtfReader reader;
      reader.setFileName(m_InputFile);
      reader.readHeaderOnly();
      dims[0] = reader.getXCells();
      dims[1] = reader.getYCells();
      dims[2] = reader.getZCells(); // With CTF files there can be more than a single slice
      m->setDimensions(dims[0], dims[1], dims[2]);
      if (reader.getZStep() != 0.0f)
      {
        m->setResolution(reader.getXStep(), reader.getYStep(), reader.getZStep());
      }
      else
      {
        m->setResolution(reader.getXStep(), reader.getYStep(), 1.0);
      }
      m->setOrigin(0.0f, 0.0f, 0.0f);
      CtfFields ctffeatures;
      names = ctffeatures.getFilterFeatures<QVector<QString> > ();
      for (size_t i = 0; i < names.size(); ++i)
      {
        if (reader.getPointerType(names[i]) == Ebsd::Int32)
        {
          Int32ArrayType::Pointer array = Int32ArrayType::CreateArray(voxels, names[i]);
          m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(names[i], array);
        }
        else if (reader.getPointerType(names[i]) == Ebsd::Float)
        {
          FloatArrayType::Pointer array = FloatArrayType::CreateArray(voxels, names[i]);
          m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(names[i], array);
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
      MicFields micfeatures;
      names = micfeatures.getFilterFeatures<QVector<QString> > ();
      for (size_t i = 0; i < names.size(); ++i)
      {
        if (reader.getPointerType(names[i]) == Ebsd::Int32)
        {
          Int32ArrayType::Pointer array = Int32ArrayType::CreateArray(voxels, names[i]);
          m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(names[i], array);
        }
        else if (reader.getPointerType(names[i]) == Ebsd::Float)
        {
          FloatArrayType::Pointer array = FloatArrayType::CreateArray(voxels, names[i]);
          m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(names[i], array);
        }
      }
    }
    else
    {
      setErrorCondition(-997);
      QString ss = QObject::tr("The File extension '%1' was not recognized. Currently .ang or .ctf are the only recognized file extensions").arg(ext);
      addErrorMessage(getHumanLabel(), ss, getErrorCondition());

      return;
    }

    QVector<int> dim(1, 3);
    m_CellEulerAnglesPtr = m->createNonPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellEulerAnglesArrayName, 0, voxels, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dim[0] = 1;
    m_CellPhasesPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellPhasesArrayName, 0, voxels, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    typedef DataArray<unsigned int> XTalStructArrayType;
    m_CrystalStructuresPtr = m->createNonPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, Ebsd::CrystalStructure::UnknownCrystalStructure, ensembles, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dim[0] = 6;
    m_LatticeConstantsPtr = m->createNonPrereqArray<float, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_LatticeConstantsArrayName, 0.0, ensembles, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_LatticeConstantsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    StringDataArray::Pointer materialNames = StringDataArray::CreateArray(1, DREAM3D::EnsembleData::MaterialName);
    m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->addAttributeArray( DREAM3D::EnsembleData::MaterialName, materialNames);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
    vdc->setName(getDataContainerName());
    getDataContainerArray()->pushBack(vdc);
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadOrientationData::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
    vdc->setName(getDataContainerName());
    getDataContainerArray()->pushBack(vdc);
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  }
  setErrorCondition(0);
  QFileInfo fi(getInputFile());
  QString ext = fi.suffix();
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

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
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(DREAM3D::CellData::Phases, iArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::Phi1));
    f2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::Phi));
    f3 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::Phi2));
    QVector<int> dims(1, 3);
    fArray = FloatArrayType::CreateArray(totalPoints, dims, DREAM3D::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);

    for (int64_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
    }
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(DREAM3D::CellData::EulerAngles, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::ImageQuality));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::ImageQuality);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Ang::ImageQuality, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::ConfidenceIndex));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::ConfidenceIndex);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Ang::ConfidenceIndex, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::SEMSignal));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::SEMSignal);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Ang::SEMSignal, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ang::Fit));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::Fit);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Ang::Fit, fArray);
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
  // reader.readOnlySliceIndex(0);
  err = reader.readFile();
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage(reader.getErrorMessage(), getErrorCondition());
    return;
  }
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t dims[3];
  dims[0] = reader.getXCells();
  dims[1] = reader.getYCells();
  dims[2] = reader.getZCells(); // With CTF files there can be more than a single slice
  m->setDimensions(dims[0], dims[1], dims[2]);
  if (reader.getZStep() != 0.0f)
  {
    m->setResolution(reader.getXStep(), reader.getYStep(), reader.getZStep());
  }
  else
  {
    m->setResolution(reader.getXStep(), reader.getYStep(), 1.0);
  }
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
    /* Take from H5CtfVolumeReader.cpp
    * For HKL OIM Files if there is a single phase then the value of the phase
    * data is one (1). If there are 2 or more phases then the lowest value
    * of phase is also one (1). However, if there are "zero solutions" in the data
    * then those points are assigned a phase of zero.  Since those points can be identified
    * by other methods, the phase of these points should be changed to one since in the rest
    * of the reconstruction code we follow the convention that the lowest value is One (1)
    * even if there is only a single phase. The next if statement converts all zeros to ones
    * if there is a single phase in the OIM data.
    */
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::Phase));
    for (int64_t i = 0; i < totalPoints; i++)
    {
      if (phasePtr[i] < 1)
      {
        phasePtr[i] = 1;
      }
    }
    iArray = Int32ArrayType::CreateArray(totalPoints, DREAM3D::CellData::Phases);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(DREAM3D::CellData::Phases, iArray);
  }
  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler1));
    f2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler2));
    f3 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::Euler3));
    QVector<int> dims(1, 3);
    fArray = FloatArrayType::CreateArray(totalPoints, dims, DREAM3D::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);
    int* cellPhases = iArray->getPointer(0);

    for (int64_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
      if(m_CrystalStructures[cellPhases[i]] == Ebsd::CrystalStructure::Hexagonal_High)
      {cellEulerAngles[3 * i + 2] = cellEulerAngles[3 * i + 2] + (30.0);}
    }
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(DREAM3D::CellData::EulerAngles, fArray);
  }

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::Bands));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::Bands);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Ctf::Bands, iArray);
  }

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::Error));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::Error);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Ctf::Error, iArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Ctf::MAD));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ctf::MAD);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Ctf::MAD, fArray);
  }

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::BC));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::BC);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Ctf::BC, iArray);
  }

  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Ctf::BS));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::BS);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Ctf::BS, iArray);
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

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

  float x, y;
  float xMin = 10000000;
  float yMin = 10000000;
  f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::X));
  f2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::Y));
  for (int64_t i = 0; i < totalPoints; i++)
  {
    x = f1[i];
    y = f2[i];
    if(x < xMin) { xMin = x; }
    if(y < yMin) { yMin = y; }
  }
  m->setOrigin(xMin, yMin, 0.0);

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
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(DREAM3D::CellData::Phases, iArray);
  }

  QVector<int> compDims(1, 3); // Initially set this up for the Euler Angle 1x3
  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::Euler1));
    f2 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::Euler2));
    f3 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::Euler3));
    fArray = FloatArrayType::CreateArray(totalPoints, compDims, DREAM3D::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);
    for (int64_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
    }
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(DREAM3D::CellData::EulerAngles, fArray);
  }

  compDims[0] = 1; // Now reset the size of the first dimension to 1
  {
    phasePtr = reinterpret_cast<int*>(reader.getPointerByName(Ebsd::Mic::Phase));
    iArray = Int32ArrayType::CreateArray(totalPoints, compDims, DREAM3D::CellData::Phases);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(DREAM3D::CellData::Phases, iArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader.getPointerByName(Ebsd::Mic::Confidence));
    fArray = FloatArrayType::CreateArray(totalPoints, compDims, Ebsd::Mic::Confidence);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(Ebsd::Mic::Confidence, fArray);
  }

}
