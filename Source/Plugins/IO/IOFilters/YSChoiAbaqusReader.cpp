/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "YSChoiAbaqusReader.h"


// C Includes
#include <stdio.h>

// C++ Includes
#include <iomanip>

#include <QtCore/QtDebug>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Math/MatrixMath.h"

#include "OrientationLib/OrientationMath/OrientationMath.h"

#include "IO/IOConstants.h"


#define DIMS "DIMENSIONS"
#define RES "SPACING"
#define LOOKUP "LOOKUP_TABLE"

// Include the MOC generated file for this class
#include "moc_YSChoiAbaqusReader.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
YSChoiAbaqusReader::YSChoiAbaqusReader() :
  FileReader(),
  m_DataContainerName(DREAM3D::Defaults::ImageDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_InputFile(""),
  m_InputFeatureInfoFile(""),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_SurfaceFeatures(NULL),
  m_Quats(NULL),
  m_AvgQuats(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
YSChoiAbaqusReader::~YSChoiAbaqusReader()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(InputFileFilterParameter::New("Input File", "InputFile", getInputFile(), FilterParameter::Parameter));
  parameters.push_back(InputFileFilterParameter::New("Input Feature Orientation File", "InputFeatureInfoFile", getInputFeatureInfoFile(), FilterParameter::Parameter));

  parameters.push_back(StringFilterParameter::New("Data Container Name", "CellEulerAnglesArrayName", getCellEulerAnglesArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Attribute Matrix Name", "CellAttributeMatrixName", getCellAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Ensemble Attribute Matrix Name", "CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Quats", "QuatsArrayName", getQuatsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("AvgQuats", "AvgQuatsArrayName", getAvgQuatsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Phases", "CellPhasesArrayName", getCellPhasesArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("SurfaceFeatures", "SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("FeatureIds", "FeatureIdsArrayName", getFeatureIdsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Crystal Structures", "CrystalStructuresArrayName", getCrystalStructuresArrayName(), FilterParameter::CreatedArray));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName() ) );
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName() ) );
  setCrystalStructuresArrayName(reader->readString("CrystalStructuresArrayName", getCrystalStructuresArrayName() ) );
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setSurfaceFeaturesArrayName(reader->readString("SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName() ) );
  setCellPhasesArrayName(reader->readString("CellPhasesArrayName", getCellPhasesArrayName() ) );
  setAvgQuatsArrayName(reader->readString("AvgQuatsArrayName", getAvgQuatsArrayName() ) );
  setQuatsArrayName(reader->readString("QuatsArrayName", getQuatsArrayName() ) );
  setCellEulerAnglesArrayName(reader->readString("CellEulerAnglesArrayName", getCellEulerAnglesArrayName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setInputFeatureInfoFile( reader->readString( "InputFeatureInfoFile", getInputFeatureInfoFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int YSChoiAbaqusReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(DataContainerName)
  SIMPL_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceFeaturesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(CellPhasesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(AvgQuatsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(QuatsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(InputFile)
  SIMPL_FILTER_WRITE_PARAMETER(InputFeatureInfoFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::updateCellInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::updateEnsembleInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  m->setGeometry(image);

  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get()) { return; }
  tDims.resize(1);
  tDims[0] = 0;

  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0 || NULL == cellFeatureAttrMat.get()) { return; }

  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0 || NULL == cellEnsembleAttrMat.get()) { return; }

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    bool ok = false;
    //const unsigned int size(1024);
    // Read header from data file to figure out how many points there are
    QFile in(getInputFile());
    if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QString msg = QObject::tr("Abaqus file could not be opened: %1").arg(getInputFile());
      setErrorCondition(-100);
      notifyErrorMessage(getHumanLabel(), "", getErrorCondition());
      return;
    }
    QString word;
    bool headerdone = false;
    int xpoints, ypoints, zpoints;
    float resx, resy, resz;
    while (headerdone == false)
    {
      QByteArray buf = in.readLine();

      if (buf.startsWith(DIMS))
      {
        QList<QByteArray> tokens = buf.split(' ');
        xpoints = tokens[1].toInt(&ok, 10);
        ypoints = tokens[2].toInt(&ok, 10);
        zpoints = tokens[3].toInt(&ok, 10);
        size_t dims[3] = { static_cast<size_t>(xpoints), static_cast<size_t>(ypoints), static_cast<size_t>(zpoints) };
        m->getGeometryAs<ImageGeom>()->setDimensions(dims);
        m->getGeometryAs<ImageGeom>()->setOrigin(0, 0, 0);
      }
      if (RES == word)
      {
        QList<QByteArray> tokens = buf.split(' ');
        resx = tokens[1].toInt(&ok, 10);
        resy = tokens[2].toInt(&ok, 10);
        resz = tokens[3].toInt(&ok, 10);
        float res[3] = {resx, resy, resz};
        m->getGeometryAs<ImageGeom>()->setResolution(res);
      }
    }
  }

  QVector<size_t> dims(1, 3);
  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getCellEulerAnglesArrayName() );
  m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getQuatsArrayName() );
  m_QuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getDataContainerName(), getCellFeatureAttributeMatrixName(), getAvgQuatsArrayName() );
  m_AvgQuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getCellPhasesArrayName() );
  m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getDataContainerName(), getCellFeatureAttributeMatrixName(), getSurfaceFeaturesArrayName() );
  m_SurfaceFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this,  tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  //typedef DataArray<unsigned int> XTalStructArrayType;
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName() );
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  tempPath, Ebsd::CrystalStructure::Cubic_High, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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
  float** *mat = NULL;
  //const unsigned int size(1024);
  // Read header from data file to figure out how many points there are
  QFile in(getInputFile());
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Abaqus file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), "", getErrorCondition());
    return;
  }

  QString word;
  bool ok = false;
  bool headerdone = false;
  while (headerdone == false)
  {
    QByteArray buf = in.readLine();

    if (buf.startsWith(DIMS))
    {
      QList<QByteArray> tokens = buf.split(' ');
      xpoints = tokens[1].toInt(&ok, 10);
      ypoints = tokens[2].toInt(&ok, 10);
      zpoints = tokens[3].toInt(&ok, 10);
      totalpoints = xpoints * ypoints * zpoints;
      size_t dims[3] = { static_cast<size_t>(xpoints), static_cast<size_t>(ypoints), static_cast<size_t>(zpoints) };
      m->getGeometryAs<ImageGeom>()->setDimensions(dims);
      m->getGeometryAs<ImageGeom>()->setOrigin(0, 0, 0);

    }
    if (buf.startsWith(RES))
    {
      QList<QByteArray> tokens = buf.split(' ');
      resx = tokens[1].toInt(&ok, 10);
      resy = tokens[2].toInt(&ok, 10);
      resz = tokens[3].toInt(&ok, 10);
      float res[3] = {resx, resy, resz};
      m->getGeometryAs<ImageGeom>()->setResolution(res);
    }
    if (buf.startsWith(LOOKUP))
    {
      headerdone = true;
      word = QString(buf);
    }
  }
  // Read header from grain info file to figure out how many features there are

  QFile in2(getInputFeatureInfoFile());
  if (!in2.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Abaqus Feature Info file could not be opened: %1").arg(getInputFeatureInfoFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), "", getErrorCondition());
    return;
  }

  int numfeatures;

  QByteArray buf = in2.readLine();
  numfeatures = buf.toInt(&ok, 10);
  buf = in2.readLine();
  QList<QByteArray> tokens = buf.split(' ');
//  in2 >> word >> word >> word >> word >> word >> word;
  QVector<size_t> tDims(3, 0);
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
  //Read data file
  int gnum = 0;
  bool onedge = false;
  int col, row, plane;
  float value;
  for (int i = 0; i < totalpoints; i++)
  {
    mat[i] = new float *[3];
    for(int j = 0; j < 3; j++)
    {
      mat[i][j] = new float [3];
    }
    onedge = false;
    gnum = tokens[6].toInt(&ok, 10);
    col = i % xpoints;
    row = (i / xpoints) % ypoints;
    plane = i / (xpoints * ypoints);
    if (col == 0 || col == (xpoints - 1) || row == 0 || row == (ypoints - 1) || plane == 0 || plane == (zpoints - 1)) { onedge = true; }
    m_FeatureIds[i] = gnum;
    m_SurfaceFeatures[gnum] = onedge;
  }
  for (int iter1 = 0; iter1 < 3; iter1++)
  {
    for (int iter2 = 0; iter2 < 3; iter2++)
    {
      headerdone = false;
      while (headerdone == false)
      {
        buf = in2.readLine();

        if (buf.startsWith(LOOKUP))
        {
          headerdone = true;
          //in >> word;
        }
      }
      for (int i = 0; i < totalpoints; i++)
      {
        onedge = 0;
        value = buf.toInt(&ok, 10);
        mat[i][iter1][iter2] = value;
      }
    }
  }
  //Read feature info
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  avgQuats[0].x = 0.0;
  avgQuats[0].y = 0.0;
  avgQuats[0].z = 0.0;
  avgQuats[0].w = 0.0;

  for (int i = 1; i < numfeatures + 1; i++)
  {
    buf = in2.readLine();
    tokens = buf.split(' ');
    gnum = tokens[0].toInt(&ok, 10);
    avgQuats[i].x = tokens[2].toFloat(&ok);
    avgQuats[i].y = tokens[3].toFloat(&ok);
    avgQuats[i].z = tokens[4].toFloat(&ok);
    avgQuats[i].w = tokens[5].toFloat(&ok);
  }
  QuatF q;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
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
    q.w = static_cast<float>( sqrt((1.0 + g[0][0] + g[1][1] + g[2][2])) / 2.0 );
    q.x = static_cast<float>( (g[1][2] - g[2][1]) / (4.0 * q.w) );
    q.y = static_cast<float>( (g[2][0] - g[0][2]) / (4.0 * q.w) );
    q.z = static_cast<float>( (g[0][1] - g[1][0]) / (4.0 * q.w) );
    QuaternionMathF::Copy(q, quats[i]);
    FOrientArrayType eu(m_CellEulerAngles + (3 * i), 3);
    FOrientTransformsType::qu2eu(FOrientArrayType(q), eu);

    delete[] mat[i];
  }
  delete[] mat;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer YSChoiAbaqusReader::newFilterInstance(bool copyFilterParameters)
{
  YSChoiAbaqusReader::Pointer filter = YSChoiAbaqusReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString YSChoiAbaqusReader::getCompiledLibraryName()
{ return IOConstants::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString YSChoiAbaqusReader::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString YSChoiAbaqusReader::getSubGroupName()
{ return DREAM3D::FilterSubGroups::InputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString YSChoiAbaqusReader::getHumanLabel()
{ return "Read YS Choi Abaqus Vtk Output File"; }

