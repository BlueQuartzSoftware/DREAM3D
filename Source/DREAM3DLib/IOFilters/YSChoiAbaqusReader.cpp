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
  m_InputFile(""),
  m_InputGrainInfoFile(""),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_GrainIds(NULL),
  m_CellPhases(NULL),
  m_SurfaceFields(NULL),
  m_Quats(NULL),
  m_AvgQuats(NULL),
  m_CellEulerAngles(NULL)
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
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Grain Orientation File");
    option->setPropertyName("InputGrainInfoFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setInputFile( reader->readValue( "InputFile", getInputFile() ) );
  setInputGrainInfoFile( reader->readValue( "InputGrainInfoFile", getInputGrainInfoFile() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int YSChoiAbaqusReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("InputGrainInfoFile", getInputGrainInfoFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getVolumeDataContainer();

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
      notifyErrorMessage(msg, getErrorCondition());
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
        m->setOrigin(0,0,0);
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

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, float, FloatArrayType, 0, voxels, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Quats, float, FloatArrayType, 0, voxels, 4)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, AvgQuats, float, FloatArrayType, 0, fields, 4)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, int32_t, Int32ArrayType, 1, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, SurfaceFields, bool, BoolArrayType, false, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, int32_t, Int32ArrayType, 0, voxels, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellEnsembleData, CrystalStructures, unsigned int, XTalStructArrayType, Ebsd::CrystalStructure::Cubic_High, ensembles, 1)
}

void YSChoiAbaqusReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}
void YSChoiAbaqusReader::execute()
{
  VolumeDataContainer* m = getVolumeDataContainer();

  int xpoints, ypoints, zpoints, totalpoints = 0;
  float resx, resy, resz;
  float ***mat;
  //const unsigned int size(1024);
  // Read header from data file to figure out how many points there are
  QFile in(getInputFile());
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Abaqus file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(msg, getErrorCondition());
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
      size_t dims[3] = {xpoints, ypoints, zpoints};
      m->setDimensions(dims);
      m->setOrigin(0,0,0);

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
  // Read header from grian info file to figure out how many grains there are

  QFile in2(getInputGrainInfoFile());
  if (!in2.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("Abaqus Grain Info file could not be opened: %1").arg(getInputGrainInfoFile());
    setErrorCondition(-100);
    notifyErrorMessage(msg, getErrorCondition());
    return;
  }

  int numgrains;

  QByteArray buf = in2.readLine();
  numgrains = buf.toInt(&ok, 10);
  buf = in2.readLine();
  QList<QByteArray> tokens = buf.split(' ');
//  in2 >> word >> word >> word >> word >> word >> word;
  totalpoints = m->getNumCellTuples();
  dataCheck(false, totalpoints, numgrains+1, 2);
  //Read data file
  int gnum = 0;
  bool onedge = false;
  int col, row, plane;
  float value;
  for (int i = 0; i < totalpoints; i++)
  {
    mat[i] = new float *[3];
    for(int j=0;j<3;j++)
    {
      mat[i][j] = new float [3];
    }
    onedge = false;
    gnum = tokens[6].toInt(&ok, 10);
    col = i % xpoints;
    row = (i / xpoints) % ypoints;
    plane = i / (xpoints * ypoints);
    if (col == 0 || col == (xpoints - 1) || row == 0 || row == (ypoints - 1) || plane == 0 || plane == (zpoints - 1)) onedge = true;
    m_GrainIds[i] = gnum;
    m_SurfaceFields[gnum] = onedge;
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
  //Read grain info
  int numpoints;
  //float q0, q1, q2, q3;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  avgQuats[0].x = 0.0;
  avgQuats[0].y = 0.0;
  avgQuats[0].z = 0.0;
  avgQuats[0].w = 0.0;

  for (int i = 1; i < numgrains+1; i++)
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
  for(int i=0;i<(xpoints*ypoints*zpoints);i++)
  {
    for(int j=0;j<3;j++)
    {
      for(int k=0;k<3;k++)
      {
        g[j][k] = mat[i][j][k];
      }
    }
    MatrixMath::Normalize3x3(g);
    q.w = static_cast<float>( sqrt((1.0+g[0][0]+g[1][1]+g[2][2]))/2.0 );
    q.x = static_cast<float>( (g[1][2]-g[2][1])/(4.0*q.w) );
    q.y = static_cast<float>( (g[2][0]-g[0][2])/(4.0*q.w) );
    q.z = static_cast<float>( (g[0][1]-g[1][0])/(4.0*q.w) );
    QuaternionMathF::Copy(q, quats[i]);
    //    m_Quats[5*i+1] = q[1];
    //    m_Quats[5*i+2] = q[2];
    //    m_Quats[5*i+3] = q[3];
    //    m_Quats[5*i+4] = q[4];
    OrientationMath::QuattoEuler(q, ea1, ea2, ea3);
    m_CellEulerAngles[3*i] = ea1;
    m_CellEulerAngles[3*i + 1] = ea2;
    m_CellEulerAngles[3*i + 2] = ea3;
    delete[] mat[i];
  }
  delete[] mat;
}
