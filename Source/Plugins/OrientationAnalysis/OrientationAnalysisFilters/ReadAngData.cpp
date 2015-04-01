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
#include "ReadAngData.h"


#include <limits>
#include <vector>
#include <sstream>

#include <QtCore/QFileInfo>
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngFields.h"

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();


/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ReadAngDataPrivate
{
  Q_DISABLE_COPY(ReadAngDataPrivate)
    Q_DECLARE_PUBLIC(ReadAngData)
    ReadAngData* const q_ptr;
  ReadAngDataPrivate(ReadAngData* ptr);

  Ang_Private_Data m_Data;

  QString m_InputFile_Cache;
  QDateTime m_TimeStamp_Cache;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadAngDataPrivate::ReadAngDataPrivate(ReadAngData* ptr) :
q_ptr(ptr),
m_InputFile_Cache(""),
m_TimeStamp_Cache(QDateTime())
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadAngData::ReadAngData() :
AbstractFilter(),
m_DataContainerName(DREAM3D::Defaults::DataContainerName),
m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
m_PhaseNameArrayName(""),
m_MaterialNameArrayName(DREAM3D::EnsembleData::MaterialName),
m_InputFile(""),
d_ptr(new ReadAngDataPrivate(this)),
m_FileWasRead(false),
m_RefFrameZDir(Ebsd::RefFrameZDir::UnknownRefFrameZDirection),
m_Manufacturer(Ebsd::UnknownManufacturer),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_CellPhases(NULL),
m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
m_CellEulerAngles(NULL),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_CrystalStructures(NULL),
m_LatticeConstantsArrayName(DREAM3D::EnsembleData::LatticeConstants),
m_LatticeConstants(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadAngData::~ReadAngData()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_PIMPL_PROPERTY_DEF(ReadAngData, Ang_Private_Data, Data)
DREAM3D_PIMPL_PROPERTY_DEF(ReadAngData, QString, InputFile_Cache)
DREAM3D_PIMPL_PROPERTY_DEF(ReadAngData, QDateTime, TimeStamp_Cache)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Input File", "InputFile", FilterParameterWidgetType::InputFileWidget, getInputFile(), false, "", "*.ang *.ctf"));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Data Container Name", "DataContainerName", FilterParameterWidgetType::StringWidget, getDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix Name", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Ensemble Attribute Matrix Name", "CellEnsembleAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellEnsembleAttributeMatrixName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadAngData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
    DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
    DREAM3D_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixName)
    DREAM3D_FILTER_WRITE_PARAMETER(InputFile)
    writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::flushCache()
{
  setInputFile_Cache("");
  setTimeStamp_Cache(QDateTime());
  setData(Ang_Private_Data());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::populateAngData(AngReader* reader, DataContainer::Pointer m, QVector<size_t> dims, ANG_READ_FLAG flag)
{
  QFileInfo fi(m_InputFile);
  QDateTime timeStamp(fi.lastModified());

  // Drop into this if statement if we need to read from a file
  if (m_InputFile != getInputFile_Cache() || getTimeStamp_Cache().isValid() == false || getTimeStamp_Cache() < timeStamp)
  {
    float zStep = 1.0, xOrigin = 0.0f, yOrigin = 0.0f, zOrigin = 0.0f;
    int zDim = 1;

    reader->setFileName(m_InputFile);

    if (flag == ANG_HEADER_ONLY)
    {
      int err = reader->readHeaderOnly();
      if (err < 0)
      {
        setErrorCondition(err);
        notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), err);
        notifyErrorMessage(getHumanLabel(), "AngReader could not read the .ang file header.", getErrorCondition());
        m_FileWasRead = false;
        return;
      }
      else
      {
        m_FileWasRead = true;
      }
    }
    else
    {
      int err = reader->readFile();
      if (err < 0)
      {
        setErrorCondition(err);
        notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), err);
        notifyErrorMessage(getHumanLabel(), "AngReader could not read the .ang file.", getErrorCondition());
        return;
      }
    }
    dims[0] = reader->getXDimension();
    dims[1] = reader->getYDimension();
    dims[2] = zDim; // We are reading a single slice

    // Set Cache with values from the file
    {
      Ang_Private_Data data;
      data.dims = dims;
      data.resolution.push_back(reader->getXStep());
      data.resolution.push_back(reader->getYStep());
      data.resolution.push_back(zStep);
      data.origin.push_back(xOrigin);
      data.origin.push_back(yOrigin);
      data.origin.push_back(zOrigin);
      data.phases = reader->getPhaseVector();
      setData(data);

      setInputFile_Cache(m_InputFile);

      QFileInfo newFi(m_InputFile);
      QDateTime timeStamp(newFi.lastModified());
      setTimeStamp_Cache(timeStamp);
    }
  }
  else
  {
    m_FileWasRead = false;
  }

  // Read from cache
      {
        dims[0] = getData().dims[0];
        dims[1] = getData().dims[1];
        dims[2] = getData().dims[2];
        m->getGeometryAs<ImageGeom>()->setDimensions(dims[0], dims[1], dims[2]);
        m->getGeometryAs<ImageGeom>()->setResolution(getData().resolution[0], getData().resolution[1], getData().resolution[2]);
        m->getGeometryAs<ImageGeom>()->setOrigin(getData().origin[0], getData().origin[1], getData().origin[2]);
      }

      if (flag == ANG_FULL_FILE)
      {
        loadInfo(reader);
      }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::dataCheck()
{
  // Reset FileWasRead flag
  m_FileWasRead = false;

  DataArrayPath tempPath;
  setErrorCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if (getErrorCondition() < 0) { return; }

  // Create the Image Geometry
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  m->setGeometry(image);

  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if (getErrorCondition() < 0) { return; }
  tDims.resize(1);
  tDims[0] = 0;
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
  if (getErrorCondition() < 0) { return; }

  QFileInfo fi(m_InputFile);
  if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getInputFile());
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (m_InputFile.isEmpty() == true && m_Manufacturer == Ebsd::UnknownManufacturer)
  {
    QString ss = QObject::tr("%1: The InputFile must be set. It is empty.").arg(getHumanLabel());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }

  if (m_InputFile.isEmpty() == false) // User set a filename, so lets check it
  {
    QVector<size_t> dims(3, 0);

    QString ext = fi.suffix();
    QVector<QString> names;
    if (ext.compare(Ebsd::Ang::FileExt) == 0)
    {
      AngReader* reader = new AngReader();

      populateAngData(reader, m, dims, ANG_HEADER_ONLY);

      //Update the size of the Cell Attribute Matrix now that the dimensions of the volume are known
      cellAttrMat->resizeAttributeArrays(dims);
      AngFields angfeatures;
      names = angfeatures.getFilterFeatures<QVector<QString> >();
      dims.resize(1);
      dims[0] = 1;
      for (qint32 i = 0; i < names.size(); ++i)
      {
        if (reader->getPointerType(names[i]) == Ebsd::Int32)
        {
          cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, names[i], 0, dims);
        }
        else if (reader->getPointerType(names[i]) == Ebsd::Float)
        {
          cellAttrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, names[i], 0, dims);
        }
      }

      delete reader;
    }
    else
    {
      setErrorCondition(-997);
      QString ss = QObject::tr("The File extension '%1' was not recognized. The reader only recognizes the .ang file extension").arg(ext);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    QVector<size_t> dim(1, 3);
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getCellEulerAnglesArrayName());
    m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_CellEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    dim[0] = 1;
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getCellPhasesArrayName());
    m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_CellPhasesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    //typedef DataArray<unsigned int> XTalStructArrayType;
    tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName());
    m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    dim[0] = 6;
    tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getLatticeConstantsArrayName());
    m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0.0, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_LatticeConstantsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    StringDataArray::Pointer materialNames = StringDataArray::CreateArray(cellEnsembleAttrMat->getNumTuples(), DREAM3D::EnsembleData::MaterialName);
    cellEnsembleAttrMat->addAttributeArray(DREAM3D::EnsembleData::MaterialName, materialNames);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::preflight()
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
void ReadAngData::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);

  dataCheck();
  if (getErrorCondition() < 0) { return; }

  readAngFile();

  // Set the file name and time stamp into the cache, if we are reading from the file and after all the reading has been done
  {
    QFileInfo newFi(m_InputFile);
    QDateTime timeStamp(newFi.lastModified());

    if (m_InputFile == getInputFile_Cache() && getTimeStamp_Cache().isValid() && getTimeStamp_Cache() >= timeStamp)
    {
      setTimeStamp_Cache(timeStamp);
      setInputFile_Cache(m_InputFile);
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::readAngFile()
{
  int err = 0;
  AngReader* reader = new AngReader();
  QVector<size_t> tDims(3, 0);
  QVector<size_t> cDims(1, 1);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer ebsdAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  ebsdAttrMat->setType(DREAM3D::AttributeMatrixType::Cell);
  ebsdAttrMat->setTupleDimensions(tDims);

  populateAngData(reader, m, tDims, ANG_FULL_FILE);

  float* f1 = NULL;
  float* f2 = NULL;
  float* f3 = NULL;
  int* phasePtr = NULL;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();

  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();

  //// Adjust the values of the 'phase' data to correct for invalid values
  {
    phasePtr = reinterpret_cast<int*>(reader->getPointerByName(Ebsd::Ang::PhaseData));
    for (size_t i = 0; i < totalPoints; i++)
    {
      if (phasePtr[i] < 1)
      {
        phasePtr[i] = 1;
      }
    }
    iArray = Int32ArrayType::CreateArray(tDims, cDims, DREAM3D::CellData::Phases);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    ebsdAttrMat->addAttributeArray(DREAM3D::CellData::Phases, iArray);
  }

  //// Condense the Euler Angles from 3 separate arrays into a single 1x3 array
  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi1));
    f2 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi));
    f3 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi2));
    cDims[0] = 3;
    fArray = FloatArrayType::CreateArray(tDims, cDims, DREAM3D::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);

    for (size_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
    }
    ebsdAttrMat->addAttributeArray(DREAM3D::CellData::EulerAngles, fArray);
  }

  cDims[0] = 1;
  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::ImageQuality));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::ImageQuality);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->addAttributeArray(Ebsd::Ang::ImageQuality, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::ConfidenceIndex));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::ConfidenceIndex);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->addAttributeArray(Ebsd::Ang::ConfidenceIndex, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::SEMSignal));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::SEMSignal);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->addAttributeArray(Ebsd::Ang::SEMSignal, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Fit));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::Fit);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->addAttributeArray(Ebsd::Ang::Fit, fArray);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadAngData::newFilterInstance(bool copyFilterParameters)
{
  ReadAngData::Pointer filter = ReadAngData::New();
  if (true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadAngData::getCompiledLibraryName()
{
  return OrientationAnalysis::OrientationAnalysisBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadAngData::getGroupName()
{
  return DREAM3D::FilterGroups::IOFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadAngData::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::InputFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadAngData::getHumanLabel()
{
  return "Read EDAX EBSD Data (.ang)";
}

