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

#include <memory>

#include "YSChoiAbaqusReader.h"

// C Includes
#include <cstdio>

// C++ Includes
#include <iomanip>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/EbsdConstants.h"

#define DIMS "DIMENSIONS"
#define RES "SPACING"
#define LOOKUP "LOOKUP_TABLE"

using QuatF = Quaternion<float>;

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,
  AttributeMatrixID23 = 23,

  DataArrayID31 = 31,
  DataArrayID32 = 32,
  DataArrayID33 = 33,
  DataArrayID34 = 34,
  DataArrayID35 = 35,
  DataArrayID36 = 36,

  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
YSChoiAbaqusReader::YSChoiAbaqusReader()
: m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_CellFeatureAttributeMatrixName(SIMPL::Defaults::CellFeatureAttributeMatrixName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_InputFile("")
, m_InputFeatureInfoFile("")
, m_CellEulerAnglesArrayName(SIMPL::CellData::EulerAngles)
, m_QuatsArrayName(SIMPL::CellData::Quats)
, m_AvgQuatsArrayName(SIMPL::FeatureData::AvgQuats)
, m_CellPhasesArrayName(SIMPL::CellData::Phases)
, m_SurfaceFeaturesArrayName(SIMPL::FeatureData::SurfaceFeatures)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, m_CrystalStructuresArrayName(SIMPL::EnsembleData::CrystalStructures)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
YSChoiAbaqusReader::~YSChoiAbaqusReader() = default;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setupFilterParameters()
{
  FileReader::setupFilterParameters();
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input Feature Orientation File", InputFeatureInfoFile, FilterParameter::Parameter, YSChoiAbaqusReader));

  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container Name", CellEulerAnglesArrayName, FilterParameter::CreatedArray, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix Name", CellAttributeMatrixName, FilterParameter::CreatedArray, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Feature Attribute Matrix Name", CellFeatureAttributeMatrixName, FilterParameter::CreatedArray, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Ensemble Attribute Matrix Name", CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Quats", QuatsArrayName, FilterParameter::CreatedArray, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("AvgQuats", AvgQuatsArrayName, FilterParameter::CreatedArray, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Phases", CellPhasesArrayName, FilterParameter::CreatedArray, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("SurfaceFeatures", SurfaceFeaturesArrayName, FilterParameter::CreatedArray, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("FeatureIds", FeatureIdsArrayName, FilterParameter::CreatedArray, YSChoiAbaqusReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Crystal Structures", CrystalStructuresArrayName, FilterParameter::CreatedArray, YSChoiAbaqusReader));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setCrystalStructuresArrayName(reader->readString("CrystalStructuresArrayName", getCrystalStructuresArrayName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
  setSurfaceFeaturesArrayName(reader->readString("SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName()));
  setCellPhasesArrayName(reader->readString("CellPhasesArrayName", getCellPhasesArrayName()));
  setAvgQuatsArrayName(reader->readString("AvgQuatsArrayName", getAvgQuatsArrayName()));
  setQuatsArrayName(reader->readString("QuatsArrayName", getQuatsArrayName()));
  setCellEulerAnglesArrayName(reader->readString("CellEulerAnglesArrayName", getCellEulerAnglesArrayName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  setInputFeatureInfoFile(reader->readString("InputFeatureInfoFile", getInputFeatureInfoFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::updateCellInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_CellEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  }                                      /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_QuatsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  }                                           /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_CellPhasesPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  }                                           /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_FeatureIdsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::updateFeatureInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_AvgQuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  }                                                /* Now assign the raw pointer to data from the DataArray<T> object */
  if(nullptr != m_SurfaceFeaturesPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::updateEnsembleInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::dataCheck()
{
  DataArrayPath tempPath;
  clearErrorCode();
  clearWarningCode();

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  std::vector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == cellAttrMat.get())
  {
    return;
  }
  tDims.resize(1);
  tDims[0] = 0;

  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix(this, getCellFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature, AttributeMatrixID22);
  if(getErrorCode() < 0 || nullptr == cellFeatureAttrMat.get())
  {
    return;
  }

  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble, AttributeMatrixID23);
  if(getErrorCode() < 0 || nullptr == cellEnsembleAttrMat.get())
  {
    return;
  }

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty())
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388, ss);
  }
  else
  {
    bool ok = false;
    // const unsigned int size(1024);
    // Read header from data file to figure out how many points there are
    QFile in(getInputFile());
    if(!in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QString msg = QObject::tr("Abaqus file could not be opened: %1").arg(getInputFile());
      setErrorCondition(-100, "");
      return;
    }
    QString word;
    bool headerdone = false;
    int xpoints, ypoints, zpoints;
    float resx, resy, resz;
    while(!headerdone)
    {
      QByteArray buf = in.readLine();

      if(buf.startsWith(DIMS))
      {
        QList<QByteArray> tokens = buf.split(' ');
        xpoints = tokens[1].toInt(&ok, 10);
        ypoints = tokens[2].toInt(&ok, 10);
        zpoints = tokens[3].toInt(&ok, 10);
        size_t dims[3] = {static_cast<size_t>(xpoints), static_cast<size_t>(ypoints), static_cast<size_t>(zpoints)};
        m->getGeometryAs<ImageGeom>()->setDimensions(dims);
        m->getGeometryAs<ImageGeom>()->setOrigin(FloatVec3Type(0.0f, 0.0f, 0.0f));
      }
      if(RES == word)
      {
        QList<QByteArray> tokens = buf.split(' ');
        resx = tokens[1].toInt(&ok, 10);
        resy = tokens[2].toInt(&ok, 10);
        resz = tokens[3].toInt(&ok, 10);
        FloatVec3Type res = {resx, resy, resz};
        m->getGeometryAs<ImageGeom>()->setSpacing(res);
      }
    }
  }

  std::vector<size_t> dims(1, 3);
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getCellEulerAnglesArrayName());
  m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, dims);                    /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellEulerAnglesPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getQuatsArrayName());
  m_QuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims, "", DataArrayID31);
  if(nullptr != m_QuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getDataContainerName().getDataContainerName(), getCellFeatureAttributeMatrixName(), getAvgQuatsArrayName());
  m_AvgQuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims, "", DataArrayID32);
  if(nullptr != m_AvgQuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getCellPhasesArrayName());
  m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 1, dims, "", DataArrayID35); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getDataContainerName().getDataContainerName(), getCellFeatureAttributeMatrixName(), getSurfaceFeaturesArrayName());
  m_SurfaceFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims, "", DataArrayID33);
  if(nullptr != m_SurfaceFeaturesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, dims, "", DataArrayID36); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // typedef DataArray<unsigned int> XTalStructArrayType;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName());
  m_CrystalStructuresPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, tempPath, Ebsd::CrystalStructure::Cubic_High, dims, "", DataArrayID34);
  if(nullptr != m_CrystalStructuresPtr.lock())                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

void YSChoiAbaqusReader::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

void YSChoiAbaqusReader::execute()
{
  dataCheck();

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  int xpoints, ypoints, zpoints, totalpoints = 0;
  float resx, resy, resz;
  float*** mat = nullptr;
  // const unsigned int size(1024);
  // Read header from data file to figure out how many points there are
  QFile in(getInputFile());
  if(!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Abaqus file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100, "");
    return;
  }

  QString word;
  bool ok = false;
  bool headerdone = false;
  while(!headerdone)
  {
    QByteArray buf = in.readLine();

    if(buf.startsWith(DIMS))
    {
      QList<QByteArray> tokens = buf.split(' ');
      xpoints = tokens[1].toInt(&ok, 10);
      ypoints = tokens[2].toInt(&ok, 10);
      zpoints = tokens[3].toInt(&ok, 10);
      totalpoints = xpoints * ypoints * zpoints;
      size_t dims[3] = {static_cast<size_t>(xpoints), static_cast<size_t>(ypoints), static_cast<size_t>(zpoints)};
      m->getGeometryAs<ImageGeom>()->setDimensions(dims);
      m->getGeometryAs<ImageGeom>()->setOrigin(FloatVec3Type(0.0f, 0.0f, 0.0f));
    }
    if(buf.startsWith(RES))
    {
      QList<QByteArray> tokens = buf.split(' ');
      resx = tokens[1].toInt(&ok, 10);
      resy = tokens[2].toInt(&ok, 10);
      resz = tokens[3].toInt(&ok, 10);
      FloatVec3Type res = {resx, resy, resz};
      m->getGeometryAs<ImageGeom>()->setSpacing(res);
    }
    if(buf.startsWith(LOOKUP))
    {
      headerdone = true;
      word = QString(buf);
    }
  }
  // Read header from grain info file to figure out how many features there are

  QFile in2(getInputFeatureInfoFile());
  if(!in2.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Abaqus Feature Info file could not be opened: %1").arg(getInputFeatureInfoFile());
    setErrorCondition(-100, "");
    return;
  }

  int numfeatures;

  QByteArray buf = in2.readLine();
  numfeatures = buf.toInt(&ok, 10);
  buf = in2.readLine();
  QList<QByteArray> tokens = buf.split(' ');
  //  in2 >> word >> word >> word >> word >> word >> word;
  std::vector<size_t> tDims(3, 0);
  tDims[0] = xpoints;
  tDims[1] = ypoints;
  tDims[2] = zpoints;
  m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(tDims);
  tDims.resize(1);
  tDims[0] = numfeatures + 1;
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  tDims[0] = 2;
  m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateCellInstancePointers();
  updateFeatureInstancePointers();
  updateEnsembleInstancePointers();
  // Read data file
  int gnum = 0;
  bool onedge = false;
  int col, row, plane;
  float value;
  for(int i = 0; i < totalpoints; i++)
  {
    mat[i] = new float*[3];
    for(int j = 0; j < 3; j++)
    {
      mat[i][j] = new float[3];
    }
    onedge = false;
    gnum = tokens[6].toInt(&ok, 10);
    col = i % xpoints;
    row = (i / xpoints) % ypoints;
    plane = i / (xpoints * ypoints);
    if(col == 0 || col == (xpoints - 1) || row == 0 || row == (ypoints - 1) || plane == 0 || plane == (zpoints - 1))
    {
      onedge = true;
    }
    m_FeatureIds[i] = gnum;
    m_SurfaceFeatures[gnum] = onedge;
  }
  for(int iter1 = 0; iter1 < 3; iter1++)
  {
    for(int iter2 = 0; iter2 < 3; iter2++)
    {
      headerdone = false;
      while(!headerdone)
      {
        buf = in2.readLine();

        if(buf.startsWith(LOOKUP))
        {
          headerdone = true;
          // in >> word;
        }
      }
      for(int i = 0; i < totalpoints; i++)
      {
        onedge = false;
        value = buf.toInt(&ok, 10);
        mat[i][iter1][iter2] = value;
      }
    }
  }
  // Read feature info
  float* avgQuats = m_AvgQuatsPtr.lock()->getTuplePointer(0);
  // Initialize the first Quat to all zeros
  avgQuats[0] = 0.0;
  avgQuats[1] = 0.0;
  avgQuats[2] = 0.0;
  avgQuats[3] = 0.0;

  for(int i = 1; i < numfeatures + 1; i++)
  {
    buf = in2.readLine();
    tokens = buf.split(' ');
    gnum = tokens[0].toInt(&ok, 10);
    avgQuats[i * 4 + 0] = tokens[2].toFloat(&ok);
    avgQuats[i * 4 + 1] = tokens[3].toFloat(&ok);
    avgQuats[i * 4 + 2] = tokens[4].toFloat(&ok);
    avgQuats[i * 4 + 3] = tokens[5].toFloat(&ok);
  }
  //  QuatF q;
  //  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  float g[3][3];
  for(int i = 0; i < (xpoints * ypoints * zpoints); i++)
  {
    for(int j = 0; j < 3; j++)
    {
      for(int k = 0; k < 3; k++)
      {
        g[j][k] = mat[i][j][k];
      }
    }
    MatrixMath::Normalize3x3(g);
    QuatF q;
    q.w() = static_cast<float>(sqrt((1.0 + g[0][0] + g[1][1] + g[2][2])) / 2.0);
    q.x() = static_cast<float>((g[1][2] - g[2][1]) / (4.0 * q.w()));
    q.y() = static_cast<float>((g[2][0] - g[0][2]) / (4.0 * q.w()));
    q.z() = static_cast<float>((g[0][1] - g[1][0]) / (4.0 * q.w()));

    m_Quats[i * 4 + 0] = q[0];
    m_Quats[i * 4 + 1] = q[1];
    m_Quats[i * 4 + 2] = q[2];
    m_Quats[i * 4 + 3] = q[3];

    OrientationF eu(m_CellEulerAngles + (3 * i), 3);
    eu = OrientationTransformation::qu2eu<QuatF, OrientationF>(q);

    delete[] mat[i];
  }
  delete[] mat;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer YSChoiAbaqusReader::newFilterInstance(bool copyFilterParameters) const
{
  YSChoiAbaqusReader::Pointer filter = YSChoiAbaqusReader::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getBrandingString() const
{
  return "Orientation Analysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid YSChoiAbaqusReader::getUuid() const
{
  return QUuid("{917b66ac-35ba-56ac-b752-9c4c3c17266d}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getHumanLabel() const
{
  return "Import YS Choi Abaqus Vtk Output File";
}

// -----------------------------------------------------------------------------
YSChoiAbaqusReader::Pointer YSChoiAbaqusReader::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<YSChoiAbaqusReader> YSChoiAbaqusReader::New()
{
  struct make_shared_enabler : public YSChoiAbaqusReader
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getNameOfClass() const
{
  return QString("YSChoiAbaqusReader");
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::ClassName()
{
  return QString("YSChoiAbaqusReader");
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath YSChoiAbaqusReader::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setCellFeatureAttributeMatrixName(const QString& value)
{
  m_CellFeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getCellFeatureAttributeMatrixName() const
{
  return m_CellFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setInputFeatureInfoFile(const QString& value)
{
  m_InputFeatureInfoFile = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getInputFeatureInfoFile() const
{
  return m_InputFeatureInfoFile;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setCellEulerAnglesArrayName(const QString& value)
{
  m_CellEulerAnglesArrayName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getCellEulerAnglesArrayName() const
{
  return m_CellEulerAnglesArrayName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setQuatsArrayName(const QString& value)
{
  m_QuatsArrayName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getQuatsArrayName() const
{
  return m_QuatsArrayName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setAvgQuatsArrayName(const QString& value)
{
  m_AvgQuatsArrayName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getAvgQuatsArrayName() const
{
  return m_AvgQuatsArrayName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setCellPhasesArrayName(const QString& value)
{
  m_CellPhasesArrayName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getCellPhasesArrayName() const
{
  return m_CellPhasesArrayName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setSurfaceFeaturesArrayName(const QString& value)
{
  m_SurfaceFeaturesArrayName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getSurfaceFeaturesArrayName() const
{
  return m_SurfaceFeaturesArrayName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}

// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setCrystalStructuresArrayName(const QString& value)
{
  m_CrystalStructuresArrayName = value;
}

// -----------------------------------------------------------------------------
QString YSChoiAbaqusReader::getCrystalStructuresArrayName() const
{
  return m_CrystalStructuresArrayName;
}
