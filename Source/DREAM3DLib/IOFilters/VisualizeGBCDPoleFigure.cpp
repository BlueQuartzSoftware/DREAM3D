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
#include "VisualizeGBCDPoleFigure.h"

#include <cmath>
#include <algorithm>
#include <limits>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"

const static float m_piOver2 = static_cast<float>(M_PI/2.0);
const static float m_pi = static_cast<float>(M_PI);
const static float m_pi2 = static_cast<float>(2.0*M_PI);
const static float m_180Overpi = static_cast<float>(180.0/M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCDPoleFigure::VisualizeGBCDPoleFigure() :
  AbstractFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceEnsembleAttributeMatrixName(DREAM3D::Defaults::FaceEnsembleAttributeMatrixName),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_MisAngle(60.0f),
  m_OutputFile(""),
  m_CrystalStructure(Ebsd::CrystalStructure::UnknownCrystalStructure),
  m_GBCDArrayName(DREAM3D::EnsembleData::GBCD),
  m_GBCD(NULL),
  m_GBCDdimensionsArrayName(DREAM3D::EnsembleData::GBCDdimensions),
  m_GBCDdimensions(NULL)
{
  m_MisAxis.x = 1;
  m_MisAxis.y = 1;
  m_MisAxis.z = 1;

  m_OrientationOps = OrientationOps::getOrientationOpsVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCDPoleFigure::~VisualizeGBCDPoleFigure()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Crystal Structure");
    option->setPropertyName("CrystalStructure");
    option->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    option->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Hexagonal-High 6/mmm");
    choices.push_back("Cubic-High m-3m");
    choices.push_back("Hexagonal-Low 6/m");
    choices.push_back("Cubic-Low m-3 (Tetrahedral)");
    choices.push_back("TriClinic -1");
    choices.push_back("Monoclinic 2/m");
    choices.push_back("OrthoRhombic mmm");
    choices.push_back("Tetragonal-Low 4/m");
    choices.push_back("Tetragonal-High 4/mmm");
    choices.push_back("Trigonal-Low -3");
    choices.push_back("Trigonal-High -3m");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Misorientation Axis Angles");
    parameter->setPropertyName("MisorientationRotations");
    parameter->setWidgetType(FilterParameterWidgetType::AxisAngleWidget);
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setPropertyName("MisAngle");
    parameter->setHumanLabel("Misorientation Angle");
    parameter->setWidgetType(FilterParameterWidgetType::DoubleWidget);
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameter->setUnits("Degrees");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();

    parameter->setHumanLabel("Misorientation Axis");
    parameter->setPropertyName("MisAxis");
    parameter->setWidgetType(FilterParameterWidgetType::FloatVec3Widget);
    parameter->setValueType("FloatVec3_t");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Regular Grid Pole Figure");
    parameter->setPropertyName("OutputFile");
    parameter->setWidgetType(FilterParameterWidgetType::OutputFileWidget);
    parameter->setFileExtension("*.vtk");
    parameter->setFileType("VTK File");
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMisAngle( reader->readValue("MisAngle", getMisAngle()) );
  setMisAxis( reader->readFloatVec3("MisAxis", getMisAxis() ) );
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  setMisorientationRotations(reader->readAxisAngles("MisorientationRotations", getMisorientationRotations() ) );
  setCrystalStructure(reader->readValue("CrystalStructure", getCrystalStructure() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VisualizeGBCDPoleFigure::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("MisorientationAngle", getMisAngle() );
  writer->writeValue("MisorientationAxis", getMisAxis() );
  writer->writeValue("OutputFile", getOutputFile() );
  writer->writeValue("MisorientationRotations", getMisorientationRotations() );
  writer->writeValue("CrystalStructure", getCrystalStructure() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::dataCheckSurfaceMesh()
{
  setErrorCondition(0);

  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer attrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceEnsembleAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  if(getCrystalStructure() == Ebsd::CrystalStructure::UnknownCrystalStructure)
  {
    QString ss = QObject::tr("%1 needs a valid crystal structure set.").arg(ClassName());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-381);
  }

  if (getOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr( ": The output file must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -1000);
    setErrorCondition(-1);
  }
  QFileInfo fi(getOutputFile());
  QDir parentPath = fi.path();
  if (parentPath.exists() == false)
  {
    QString ss = QObject::tr( "The directory path for the output file does not exist.");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }

  if (fi.suffix().compare("") == 0)
  {
    setOutputFile(getOutputFile().append(".dx"));
  }

  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-385);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }
  else
  {
    IDataArray::Pointer iDataArray = sm->getAttributeMatrix(getFaceEnsembleAttributeMatrixName())->getAttributeArray(DREAM3D::EnsembleData::GBCD);
    if (NULL == iDataArray.get())
    {
      setErrorCondition(-387);
      notifyErrorMessage(getHumanLabel(), "The GBCD Array was not found in the Surface Mesh Ensemble Data. ", getErrorCondition());
    }
    else
    {
      QVector<size_t> dims(1, 5);
      m_GBCDdimensionsPtr = attrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_GBCDdimensionsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if( NULL != m_GBCDdimensionsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
      { m_GBCDdimensions = m_GBCDdimensionsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
      int numComp = iDataArray->getNumberOfComponents();
      dims[0] = numComp;
      m_GBCDPtr = attrMat->getPrereqArray<DataArray<double>, AbstractFilter>(this, m_GBCDArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if( NULL != m_GBCDPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
      { m_GBCD = m_GBCDPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::preflight()
{
  dataCheckSurfaceMesh();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());

  notifyStatusMessage(getHumanLabel(), "Starting");

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());

  QDir dir(fi.path());
  if(!dir.mkpath("."))
  {
    QString ss;
    ss = QObject::tr("Error creating parent path '%1'").arg(dir.path());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  QFile file(getOutputFile());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("DxWriter Input file could not be opened: %1").arg(getOutputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  VertexArray::Pointer nodesPtr = sm->getVertices();

  FaceArray::Pointer trianglesPtr = sm->getFaces();
  size_t totalFaces = trianglesPtr->getNumberOfTuples();

  FloatArrayType::Pointer gbcdDeltasArray = FloatArrayType::CreateArray(5, "GBCDDeltas");
  gbcdDeltasArray->initializeWithZeros();

  FloatArrayType::Pointer gbcdLimitsArray = FloatArrayType::CreateArray(10, "GBCDLimits");
  gbcdLimitsArray->initializeWithZeros();

  Int32ArrayType::Pointer gbcdSizesArray = Int32ArrayType::CreateArray(5, "GBCDSizes");
  gbcdSizesArray->initializeWithZeros();

  float* gbcdDeltas = gbcdDeltasArray->getPointer(0);
  int* gbcdSizes = gbcdSizesArray->getPointer(0);
  float* gbcdLimits = gbcdLimitsArray->getPointer(0);

  //Original Ranges from Dave R.
  //m_GBCDlimits[0] = 0.0;
  //m_GBCDlimits[1] = cosf(1.0*m_pi);
  //m_GBCDlimits[2] = 0.0;
  //m_GBCDlimits[3] = 0.0;
  //m_GBCDlimits[4] = cosf(1.0*m_pi);
  //m_GBCDlimits[5] = 2.0*m_pi;
  //m_GBCDlimits[6] = cosf(0.0);
  //m_GBCDlimits[7] = 2.0*m_pi;
  //m_GBCDlimits[8] = 2.0*m_pi;
  //m_GBCDlimits[9] = cosf(0.0);

  //Greg's Ranges
  gbcdLimits[0] = 0.0;
  gbcdLimits[1] = 0.0;
  gbcdLimits[2] = 0.0;
  gbcdLimits[3] = 0.0;
  gbcdLimits[4] = 0.0;
  gbcdLimits[5] = m_pi/2.0;
  gbcdLimits[6] = 1.0;
  gbcdLimits[7] = m_pi/2.0;
  gbcdLimits[8] = 1.0;
  gbcdLimits[9] = 2.0*m_pi;

  //get num components of GBCD
  QVector<size_t> cDims = m_GBCDPtr.lock()->getComponentDimensions();

  gbcdSizes[0] = cDims[0];
  gbcdSizes[1] = cDims[1];
  gbcdSizes[2] = cDims[2];
  gbcdSizes[3] = cDims[3];
  gbcdSizes[4] = cDims[4];

  float binsize = gbcdLimits[5]/float(gbcdSizes[0]);
  float binsize2 = binsize*(2.0/m_pi);
  gbcdDeltas[0] = binsize;
  gbcdDeltas[1] = binsize2;
  gbcdDeltas[2] = binsize;
  gbcdDeltas[3] = binsize2;
  gbcdDeltas[4] = binsize;

  float vec[3];
  float vec2[3];
  float rotNormal[3];
  float rotNormal2[3];
  float rotNormal_sc[2];
  float rotNormal2_sc[2];
  float dg[3][3];
  float dgOrig[3][3];
  float dgt[3][3];
  float dg1[3][3];
  float dg2[3][3];
  float sym1[3][3];
  float sym1t[3][3];
  float sym2[3][3];
  float sym2t[3][3];
  float mis_euler1[3];
  QuatF qtest;
  float w, n1, n2, n3;
  //  float dist;
  float theta, cosPhi, phi;

  m_MisAngle = m_MisAngle * m_pi/180.0f;
  //convert axis angle to matrix representation of misorientation
  OrientationMath::AxisAngletoMat(m_MisAngle, m_MisAxis.x, m_MisAxis.y, m_MisAxis.z, dg);
  //take inverse of misorientation variable to use for switching symmetry
  MatrixMath::Transpose3x3(dg, dgt);

  // Get our OrientationOps pointer for the selected crystal structure
  OrientationOps::Pointer orientOps = m_OrientationOps[m_CrystalStructure];

  //get number of symmetry operators
  int n_sym = orientOps->getNumSymOps();

  int xpoints = 100;
  int ypoints = 100;
  int zpoints = 1;
  int xpointshalf = xpoints/2;
  int ypointshalf = ypoints/2;
  float xres = 2.0/float(xpoints);
  float yres = 2.0/float(ypoints);
  float zres = (xres+yres)/2.0;
  float x, y, z;
  int xbin, ybin;
  float sum = 0;
  int count = 0;

  int shift1 = gbcdSizes[0];
  int shift2 = gbcdSizes[0]*gbcdSizes[1];
  int shift3 = gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2];
  int shift4 = gbcdSizes[0]*gbcdSizes[1]*gbcdSizes[2]*gbcdSizes[3];

  QVector<size_t> dims(1, 1);
  DoubleArrayType::Pointer poleFigureArray = DoubleArrayType::NullPointer();
  poleFigureArray = DoubleArrayType::CreateArray(xpoints*ypoints, dims, "PoleFigure");
  poleFigureArray->initializeWithZeros();
  double* poleFigure = poleFigureArray->getPointer(0);

  for (int64_t k = 0; k < ypoints; k++)
  {
    for (int64_t l = 0; l < xpoints; l++)
    {
      //get (x,y) for stereographic projection pixel
      x = float(l - xpointshalf) * xres + (xres / 2.0);
      y = float(k - ypointshalf) * yres + (yres / 2.0);
      if((x * x + y * y) <= 1.0)
      {
        sum = 0;
        count = 0;
        vec[2] = -((x * x + y * y) - 1) / ((x * x + y * y) + 1);
        vec[0] = x * (1 + vec[2]);
        vec[1] = y * (1 + vec[2]);
        MatrixMath::Multiply3x3with3x1(dgt, vec, vec2);

        // Loop over all the symetry operators in the given cystal symmetry
        for(int i=0; i<n_sym; i++)
        {
          //get symmetry operator1
          orientOps->getMatSymOp(i, sym1);
          for(int j=0; j<n_sym; j++)
          {
            //get symmetry operator2
            orientOps->getMatSymOp(j, sym2);
            MatrixMath::Transpose3x3(sym2,sym2t);
            //calculate symmetric misorientation
            MatrixMath::Multiply3x3with3x3(dg,sym2t,dg1);
            MatrixMath::Multiply3x3with3x3(sym1,dg1,dg2);
            //convert to euler angle
            OrientationMath::MattoEuler(dg2, mis_euler1[0], mis_euler1[1], mis_euler1[2]);
            if(mis_euler1[0] < m_piOver2 && mis_euler1[1] < m_piOver2 && mis_euler1[2] < m_piOver2)
            {
              mis_euler1[1] = cosf(mis_euler1[1]);
              //find bins in GBCD
              int location1 = int((mis_euler1[0]-gbcdLimits[0])/gbcdDeltas[0]);
              int location2 = int((mis_euler1[1]-gbcdLimits[1])/gbcdDeltas[1]);
              int location3 = int((mis_euler1[2]-gbcdLimits[2])/gbcdDeltas[2]);
              //find symmetric poles using the first symmetry operator
              MatrixMath::Multiply3x3with3x1(sym1, vec, rotNormal);
              if(rotNormal[2] < 0.0) MatrixMath::Multiply3x1withConstant(rotNormal, -1);
              //calculate the crystal normals in aspherical coordinates ->[theta, cos(phi) ]
              rotNormal_sc[0] = atan2f(rotNormal[1], rotNormal[0]);
              if (rotNormal_sc[0] < 0) rotNormal_sc[0] += m_pi2;
              rotNormal_sc[1] = rotNormal[2];
              //Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
              int location4 = int((rotNormal_sc[1]-gbcdLimits[3])/gbcdDeltas[3]);
              int location5 = int((rotNormal_sc[0]-gbcdLimits[4])/gbcdDeltas[4]);
              if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 &&
                location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] && location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
              {
                sum += m_GBCD[(location5*shift4)+(location4*shift3)+(location3*shift2)+(location2*shift1)+location1];
                count++;
              }
            }

            //again in second crystal reference frame
            //calculate symmetric misorientation
            MatrixMath::Multiply3x3with3x3(dgt,sym2,dg1);
            MatrixMath::Multiply3x3with3x3(sym1,dg1,dg2);
            //convert to euler angle
            OrientationMath::MattoEuler(dg2, mis_euler1[0], mis_euler1[1], mis_euler1[2]);
            if(mis_euler1[0] < m_piOver2 && mis_euler1[1] < m_piOver2 && mis_euler1[2] < m_piOver2)
            {
              mis_euler1[1] = cosf(mis_euler1[1]);
              //find bins in GBCD
              int location1 = int((mis_euler1[0]-gbcdLimits[0])/gbcdDeltas[0]);
              int location2 = int((mis_euler1[1]-gbcdLimits[1])/gbcdDeltas[1]);
              int location3 = int((mis_euler1[2]-gbcdLimits[2])/gbcdDeltas[2]);
              //find symmetric poles using the first symmetry operator
              MatrixMath::Multiply3x3with3x1(sym1, vec2, rotNormal2);
              if(rotNormal2[2] < 0.0) MatrixMath::Multiply3x1withConstant(rotNormal2, -1);
              //calculate the crystal normals in aspherical coordinates ->[theta, cos(phi) ]
              rotNormal2_sc[0] = atan2f(rotNormal2[1], rotNormal2[0]);
              if (rotNormal2_sc[0] < 0) rotNormal2_sc[0] += m_pi2;
              rotNormal2_sc[1] = rotNormal2[2];
              //Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
              int location4 = int((rotNormal2_sc[1]-gbcdLimits[3])/gbcdDeltas[3]);
              int location5 = int((rotNormal2_sc[0]-gbcdLimits[4])/gbcdDeltas[4]);
              if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 &&
                location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] && location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
              {
                sum += m_GBCD[(location5*shift4)+(location4*shift3)+(location3*shift2)+(location2*shift1)+location1];
                count++;
              }
            }
          }
        }
        poleFigure[(k*xpoints)+l] = sum/float(count);;
      }
    }
  }

  FILE* f = NULL;
  f = fopen(m_OutputFile.toLatin1().data(), "wb");
  if(NULL == f)
  {

    QString ss = QObject::tr("Could not open GBCD viz file %1 for writing. Please check access permissions and the path to the output location exists").arg(m_OutputFile);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Write the correct header
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "data set from DREAM3D\n");
  fprintf(f, "BINARY");
  fprintf(f, "\n");
  fprintf(f, "DATASET RECTILINEAR_GRID\n");
  fprintf(f, "DIMENSIONS %d %d %d\n", xpoints + 1, ypoints + 1, zpoints + 1);

  // Write the Coords
  writeCoords(f, "X_COORDINATES", "float", xpoints + 1, (-float(xpoints)*xres / 2.0), xres);
  writeCoords(f, "Y_COORDINATES", "float", ypoints + 1, (-float(ypoints)*yres / 2.0), yres);
  writeCoords(f, "Z_COORDINATES", "float", zpoints + 1, (-float(zpoints)*zres / 2.0), zres);

  size_t total = xpoints * ypoints * zpoints;
  fprintf(f, "CELL_DATA %d\n", (int)total);

  fprintf(f, "SCALARS %s %s 1\n", "Intensity", "float");
  fprintf(f, "LOOKUP_TABLE default\n");
  {
    float* gn = new float[total];
    float t;
    count = 0;
    for (int64_t j = 0; j < (ypoints); j++)
    {
      for (int64_t i = 0; i < (xpoints); i++)
      {
        t = float(poleFigure[(j * xpoints) + i]);
        DREAM3D::Endian::FromSystemToBig::convert(t);
        gn[count] = t;
        count++;
      }
    }
    int64_t totalWritten = fwrite(gn, sizeof(float), (total), f);
    delete[] gn;
    if (totalWritten != (total))
    {
      qDebug() << "Error Writing Binary VTK Data into file " << m_OutputFile ;
      fclose(f);
    }
  }
  fclose(f);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}