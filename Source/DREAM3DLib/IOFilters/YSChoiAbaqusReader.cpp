/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"

#define DIMS "DIMENSIONS"
#define RES "SPACING"
#define LOOKUP "LOOKUP_TABLE"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
YSChoiAbaqusReader::YSChoiAbaqusReader() :
  FileReader(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_InputFile(""),
  m_InputFeatureInfoFile(""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_SurfaceFeatures(NULL),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
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
  parameters.push_back(FilterParameter::New("Input File", "InputFile", FilterParameterWidgetType::InputFileWidget,"QString", false));
  parameters.push_back(FilterParameter::New("Input Feature Orientation File", "InputFeatureInfoFile", FilterParameterWidgetType::InputFileWidget,"QString", false));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*##*/parameters.push_back(FilterParameter::New("CellEulerAngles", "CellEulerAnglesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("Quats", "QuatsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("CellPhases", "CellPhasesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("SurfaceFeatures", "SurfaceFeaturesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("CrystalStructures", "CrystalStructuresArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
/*[]*/setCrystalStructuresArrayName(reader->readString("CrystalStructuresArrayName", getCrystalStructuresArrayName() ) );
/*[]*/setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
/*[]*/setSurfaceFeaturesArrayName(reader->readString("SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName() ) );
/*[]*/setCellPhasesArrayName(reader->readString("CellPhasesArrayName", getCellPhasesArrayName() ) );
/*[]*/setAvgQuatsArrayName(reader->readString("AvgQuatsArrayName", getAvgQuatsArrayName() ) );
/*[]*/setQuatsArrayName(reader->readString("QuatsArrayName", getQuatsArrayName() ) );
/*[]*/setCellEulerAnglesArrayName(reader->readString("CellEulerAnglesArrayName", getCellEulerAnglesArrayName() ) );
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
/*[]*/writer->writeValue("CrystalStructuresArrayName", getCrystalStructuresArrayName() );
/*[]*/writer->writeValue("FeatureIdsArrayName", getFeatureIdsArrayName() );
/*[]*/writer->writeValue("SurfaceFeaturesArrayName", getSurfaceFeaturesArrayName() );
/*[]*/writer->writeValue("CellPhasesArrayName", getCellPhasesArrayName() );
/*[]*/writer->writeValue("AvgQuatsArrayName", getAvgQuatsArrayName() );
/*[]*/writer->writeValue("QuatsArrayName", getQuatsArrayName() );
/*[]*/writer->writeValue("CellEulerAnglesArrayName", getCellEulerAnglesArrayName() );
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("InputFeatureInfoFile", getInputFeatureInfoFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }
  tDims.resize(1);
  tDims[0] = 0;
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0) { return; }

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
        size_t dims[3] = {xpoints, ypoints, zpoints};
        m->setDimensions(dims);
        m->setOrigin(0, 0, 0);
      }
      if (RES == word)
      {
        QList<QByteArray> tokens = buf.split(' ');
        resx = tokens[1].toInt(&ok, 10);
        resy = tokens[2].toInt(&ok, 10);
        resz = tokens[3].toInt(&ok, 10);
        float res[3] = {resx, resy, resz};
        m->setResolution(res);
      }
    }
  }

  QVector<size_t> dims(1, 3);
  m_CellEulerAnglesPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_CellEulerAnglesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getCellEulerAnglesArrayName() );
////==>MIKE_GROEBER_FIX m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_QuatsPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_QuatsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getQuatsArrayName() );
////==>MIKE_GROEBER_FIX m_QuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_AvgQuatsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this,  m_AvgQuatsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getAvgQuatsArrayName() );
////==>MIKE_GROEBER_FIX m_AvgQuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_CellPhasesPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_CellPhasesArrayName, 1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getCellPhasesArrayName() );
////==>MIKE_GROEBER_FIX m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SurfaceFeaturesPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this,  m_SurfaceFeaturesArrayName, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getSurfaceFeaturesArrayName() );
////==>MIKE_GROEBER_FIX m_SurfaceFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this,  tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeatureIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getFeatureIdsArrayName() );
////==>MIKE_GROEBER_FIX m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->createNonPrereqArray<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  m_CrystalStructuresArrayName, Ebsd::CrystalStructure::Cubic_High, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getCrystalStructuresArrayName() );
////==>MIKE_GROEBER_FIX m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  tempPath, Ebsd::CrystalStructure::Cubic_High, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

void YSChoiAbaqusReader::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}
void YSChoiAbaqusReader::execute()
{
  dataCheck();

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

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
      size_t dims[3] = {xpoints, ypoints, zpoints};
      m->setDimensions(dims);
      m->setOrigin(0, 0, 0);

    }
    if (buf.startsWith(RES))
    {
      QList<QByteArray> tokens = buf.split(' ');
      resx = tokens[1].toInt(&ok, 10);
      resy = tokens[2].toInt(&ok, 10);
      resz = tokens[3].toInt(&ok, 10);
      float res[3] = {resx, resy, resz};
      m->setResolution(res);
    }
    if (buf.startsWith(LOOKUP))
    {
      headerdone = true;
      word = QString(buf);
    }
  }
  // Read header from grian info file to figure out how many features there are

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
  tDims[0] = numfeatures+1;
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  tDims[0] = 2;
  m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->resizeAttributeArrays(tDims);
  dataCheck();
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
  int numpoints;
  //float q0, q1, q2, q3;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  avgQuats[0].x = 0.0;
  avgQuats[0].y = 0.0;
  avgQuats[0].z = 0.0;
  avgQuats[0].w = 0.0;

  for (int i = 1; i < numfeatures + 1; i++)
  {
    buf = in2.readLine();
    tokens = buf.split(' ');
    //in2 >> gnum >> numpoints >> q0 >> q1 >> q2 >> q3;
    gnum = tokens[0].toInt(&ok, 10);
    numpoints = tokens[1].toInt(&ok, 10);
    avgQuats[i].x = tokens[2].toFloat(&ok);
    avgQuats[i].y = tokens[3].toFloat(&ok);
    avgQuats[i].z = tokens[4].toFloat(&ok);
    avgQuats[i].w = tokens[5].toFloat(&ok);
  }
  float ea1, ea2, ea3;
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
    OrientationMath::QuattoEuler(q, ea1, ea2, ea3);
    m_CellEulerAngles[3 * i] = ea1;
    m_CellEulerAngles[3 * i + 1] = ea2;
    m_CellEulerAngles[3 * i + 2] = ea3;
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
    filter->setCrystalStructuresArrayName(getCrystalStructuresArrayName());
    filter->setFeatureIdsArrayName(getFeatureIdsArrayName());
    filter->setSurfaceFeaturesArrayName(getSurfaceFeaturesArrayName());
    filter->setCellPhasesArrayName(getCellPhasesArrayName());
    filter->setAvgQuatsArrayName(getAvgQuatsArrayName());
    filter->setQuatsArrayName(getQuatsArrayName());
    filter->setCellEulerAnglesArrayName(getCellEulerAnglesArrayName());
    filter->setInputFile( getInputFile() );
    filter->setInputFeatureInfoFile( getInputFeatureInfoFile() );
  }
  return filter;
}
