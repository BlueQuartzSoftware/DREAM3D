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


#include "VisualizeGBCDGMT.h"

#include <QtCore/QDir>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/AxisAngleFilterParameter.h"
#include "DREAM3DLib/FilterParameters/OutputFileFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"

#include "DREAM3DLib/FilterParameters/IntFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "OrientationLib/OrientationMath/OrientationMath.h"

#include "IO/IOConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCDGMT::VisualizeGBCDGMT() :
  AbstractFilter(),
  m_OutputFile(""),
  m_PhaseOfInterest(1),
  m_GBCDArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceEnsembleAttributeMatrixName, DREAM3D::EnsembleData::GBCD),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_GBCD(NULL),
  m_CrystalStructures(NULL)
{
  m_MisorientationRotation.angle = 0.0f;
  m_MisorientationRotation.h = 0.0f;
  m_MisorientationRotation.k = 0.0f;
  m_MisorientationRotation.l = 0.0f;

  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCDGMT::~VisualizeGBCDGMT()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(IntFilterParameter::New("Phase of Interest", "PhaseOfInterest", getPhaseOfInterest(), FilterParameter::Parameter));
  parameters.push_back(AxisAngleFilterParameter::New("Misorientation Axis-Angle", "MisorientationRotation", getMisorientationRotation(), FilterParameter::Parameter));
  parameters.push_back(OutputFileFilterParameter::New("Output GMT File", "OutputFile", getOutputFile(), FilterParameter::Parameter, "*.dat", "DAT File"));
  parameters.push_back(SeparatorFilterParameter::New("Face Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Double, UINT32_MAX, DREAM3D::AttributeMatrixType::FaceEnsemble, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("GBCD", "GBCDArrayPath", getGBCDArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGBCDArrayPath(reader->readDataArrayPath("GBCDArrayPath", getGBCDArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setOutputFile(reader->readString("OutputFile", getOutputFile()));
  setMisorientationRotation(reader->readAxisAngle("MisorientationRotation", getMisorientationRotation(), -1) );
  setPhaseOfInterest(reader->readValue("PhaseOfInterest", getPhaseOfInterest()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VisualizeGBCDGMT::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(GBCDArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(MisorientationRotation)
  DREAM3D_FILTER_WRITE_PARAMETER(PhaseOfInterest)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getGBCDArrayPath().getDataContainerName());

  if (getOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr( "The output file must be set");
    notifyErrorMessage(getHumanLabel(), ss, -1000);
    setErrorCondition(-1);
  }

  QFileInfo fi(getOutputFile());
  QDir parentPath = fi.path();
  if (parentPath.exists() == false)
  {
    QString ss = QObject::tr( "The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }

  if (fi.suffix().compare("") == 0)
  {
    setOutputFile(getOutputFile().append(".dat"));
  }

  // Make sure the file name ends with _1 so the GMT scripts work correctly
  QString fName = fi.baseName();
  if (fName.endsWith("_1") == false)
  {
    fName = fName + "_1";
    QString absPath = fi.absolutePath() + "/" + fName + ".dat";
    setOutputFile(absPath);
  }

  QVector<size_t> cDims(1, 1);
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  IDataArray::Pointer tmpGBCDPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getGBCDArrayPath());
  if(getErrorCondition() < 0) { return; }

  if (NULL != tmpGBCDPtr.get())
  {
    QVector<size_t> cDims = tmpGBCDPtr->getComponentDimensions();
    m_GBCDPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getGBCDArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GBCDPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GBCD = m_GBCDPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if (NULL != m_GBCDPtr.lock().get() && getPhaseOfInterest() >= m_GBCDPtr.lock()->getNumberOfTuples())
  {
    QString ss = QObject::tr("The phase index is larger than the number of Ensembles").arg(ClassName());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-381);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::preflight()
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
void VisualizeGBCDGMT::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getGBCDArrayPath().getDataContainerName());

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());

  QDir dir(fi.path());
  if (!dir.mkpath("."))
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
    QString ss = QObject::tr("Error opening output file '%1'").arg(getOutputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  FloatArrayType::Pointer gbcdDeltasArray = FloatArrayType::CreateArray(5, "GBCDDeltas");
  gbcdDeltasArray->initializeWithZeros();

  FloatArrayType::Pointer gbcdLimitsArray = FloatArrayType::CreateArray(10, "GBCDLimits");
  gbcdLimitsArray->initializeWithZeros();

  Int32ArrayType::Pointer gbcdSizesArray = Int32ArrayType::CreateArray(5, "GBCDSizes");
  gbcdSizesArray->initializeWithZeros();

  float* gbcdDeltas = gbcdDeltasArray->getPointer(0);
  int32_t* gbcdSizes = gbcdSizesArray->getPointer(0);
  float* gbcdLimits = gbcdLimitsArray->getPointer(0);

  // Original Ranges from Dave R.
  //m_GBCDlimits[0] = 0.0f;
  //m_GBCDlimits[1] = cosf(1.0f*m_pi);
  //m_GBCDlimits[2] = 0.0f;
  //m_GBCDlimits[3] = 0.0f;
  //m_GBCDlimits[4] = cosf(1.0f*m_pi);
  //m_GBCDlimits[5] = 2.0f*m_pi;
  //m_GBCDlimits[6] = cosf(0.0f);
  //m_GBCDlimits[7] = 2.0f*m_pi;
  //m_GBCDlimits[8] = 2.0f*m_pi;
  //m_GBCDlimits[9] = cosf(0.0f);

  // Greg R. Ranges
  gbcdLimits[0] = 0.0f;
  gbcdLimits[1] = 0.0f;
  gbcdLimits[2] = 0.0f;
  gbcdLimits[3] = -sqrtf(DREAM3D::Constants::k_Pi / 2.0f);
  gbcdLimits[4] = -sqrtf(DREAM3D::Constants::k_Pi / 2.0f);
  gbcdLimits[5] = DREAM3D::Constants::k_Pi / 2.0f;
  gbcdLimits[6] = 1.0f;
  gbcdLimits[7] = DREAM3D::Constants::k_Pi / 2.0f;
  gbcdLimits[8] = sqrtf(DREAM3D::Constants::k_Pi / 2.0f);
  gbcdLimits[9] = sqrtf(DREAM3D::Constants::k_Pi / 2.0f);

  // get num components of GBCD
  QVector<size_t> cDims = m_GBCDPtr.lock()->getComponentDimensions();

  gbcdSizes[0] = cDims[0];
  gbcdSizes[1] = cDims[1];
  gbcdSizes[2] = cDims[2];
  gbcdSizes[3] = cDims[3];
  gbcdSizes[4] = cDims[4];

  gbcdDeltas[0] = (gbcdLimits[5] - gbcdLimits[0]) / float(gbcdSizes[0]);
  gbcdDeltas[1] = (gbcdLimits[6] - gbcdLimits[1]) / float(gbcdSizes[1]);
  gbcdDeltas[2] = (gbcdLimits[7] - gbcdLimits[2]) / float(gbcdSizes[2]);
  gbcdDeltas[3] = (gbcdLimits[8] - gbcdLimits[3]) / float(gbcdSizes[3]);
  gbcdDeltas[4] = (gbcdLimits[9] - gbcdLimits[4]) / float(gbcdSizes[4]);

  float vec[3] = { 0.0f, 0.0f, 0.0f };
  float vec2[3] = { 0.0f, 0.0f, 0.0f };
  float rotNormal[3] = { 0.0f, 0.0f, 0.0f };
  float rotNormal2[3] = { 0.0f, 0.0f, 0.0f };
  float sqCoord[2] = { 0.0f, 0.0f };
  float dg[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float dgt[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float dg1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float dg2[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float sym1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float sym2[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float sym2t[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
  float mis_euler1[3] = { 0.0f, 0.0f, 0.0f };

  float misAngle = m_MisorientationRotation.angle * DREAM3D::Constants::k_PiOver180;
  // convert axis angle to matrix representation of misorientation
  FOrientArrayType om(9);
  FOrientTransformsType::ax2om(FOrientArrayType(m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l, misAngle), om);
  om.toGMatrix(dg);

  // take inverse of misorientation variable to use for switching symmetry
  MatrixMath::Transpose3x3(dg, dgt);

  // Get our SpaceGroupOps pointer for the selected crystal structure
  SpaceGroupOps::Pointer orientOps = m_OrientationOps[m_CrystalStructures[m_PhaseOfInterest]];

  // get number of symmetry operators
  int32_t n_sym = orientOps->getNumSymOps();

  int32_t thetaPoints = 120;
  int32_t phiPoints = 30;
  float thetaRes = 360.0f / float(thetaPoints);
  float phiRes = 90.0f / float(phiPoints);
  float theta = 0.0f, phi = 0.0f;
  float thetaRad = 0.0f, phiRad = 0.0f;
  float degToRad = DREAM3D::Constants::k_PiOver180;
  float sum = 0.0f;
  int32_t count = 0;
  bool nhCheck = false;
  int32_t hemisphere = 0;

  int32_t shift1 = gbcdSizes[0];
  int32_t shift2 = gbcdSizes[0] * gbcdSizes[1];
  int32_t shift3 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2];
  int32_t shift4 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3];

  int64_t totalGBCDBins = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3] * gbcdSizes[4] * 2;

  std::vector<float> gmtValues;

  for (int32_t k = 0; k < phiPoints + 1; k++)
  {
    for (int32_t l = 0; l < thetaPoints + 1; l++)
    {
      // get (x,y) for stereographic projection pixel
      theta = float(l) * thetaRes;
      phi = float(k) * phiRes;
      thetaRad = theta * degToRad;
      phiRad = phi * degToRad;
      sum = 0.0f;
      count = 0;
      vec[0] = sinf(phiRad) * cosf(thetaRad);
      vec[1] = sinf(phiRad) * sinf(thetaRad);
      vec[2] = cosf(phiRad);
      MatrixMath::Multiply3x3with3x1(dgt, vec, vec2);

      // Loop over all the symetry operators in the given cystal symmetry
      for (int32_t i = 0; i < n_sym; i++)
      {
        // get symmetry operator1
        orientOps->getMatSymOp(i, sym1);
        for (int32_t j = 0; j < n_sym; j++)
        {
          // get symmetry operator2
          orientOps->getMatSymOp(j, sym2);
          MatrixMath::Transpose3x3(sym2, sym2t);
          // calculate symmetric misorientation
          MatrixMath::Multiply3x3with3x3(dg, sym2t, dg1);
          MatrixMath::Multiply3x3with3x3(sym1, dg1, dg2);
          // convert to euler angle
          FOrientArrayType mEuler(mis_euler1, 3);
          FOrientTransformsType::om2eu(FOrientArrayType(dg2), mEuler);
          if (mis_euler1[0] < DREAM3D::Constants::k_PiOver2 && mis_euler1[1] < DREAM3D::Constants::k_PiOver2 && mis_euler1[2] < DREAM3D::Constants::k_PiOver2)
          {
            mis_euler1[1] = cosf(mis_euler1[1]);
            // find bins in GBCD
            int32_t location1 = int32_t((mis_euler1[0] - gbcdLimits[0]) / gbcdDeltas[0]);
            int32_t location2 = int32_t((mis_euler1[1] - gbcdLimits[1]) / gbcdDeltas[1]);
            int32_t location3 = int32_t((mis_euler1[2] - gbcdLimits[2]) / gbcdDeltas[2]);
            // find symmetric poles using the first symmetry operator
            MatrixMath::Multiply3x3with3x1(sym1, vec, rotNormal);
            // get coordinates in square projection of crystal normal parallel to boundary normal
            nhCheck = getSquareCoord(rotNormal, sqCoord);
            // Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
            int32_t location4 = int32_t((sqCoord[0] - gbcdLimits[3]) / gbcdDeltas[3]);
            int32_t location5 = int32_t((sqCoord[1] - gbcdLimits[4]) / gbcdDeltas[4]);
            if (location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 &&
                location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] && location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
            {
              hemisphere = 0;
              if (nhCheck == false) { hemisphere = 1; }
              sum += m_GBCD[(m_PhaseOfInterest * totalGBCDBins) + 2 * ((location5 * shift4) + (location4 * shift3) + (location3 * shift2) + (location2 * shift1) + location1) + hemisphere];
              count++;
            }
          }

          // again in second crystal reference frame
          // calculate symmetric misorientation
          MatrixMath::Multiply3x3with3x3(dgt, sym2, dg1);
          MatrixMath::Multiply3x3with3x3(sym1, dg1, dg2);
          // convert to euler angle
          FOrientTransformsType::om2eu(FOrientArrayType(dg2), mEuler);
          if (mis_euler1[0] < DREAM3D::Constants::k_PiOver2 && mis_euler1[1] < DREAM3D::Constants::k_PiOver2 && mis_euler1[2] < DREAM3D::Constants::k_PiOver2)
          {
            mis_euler1[1] = cosf(mis_euler1[1]);
            // find bins in GBCD
            int32_t location1 = int32_t((mis_euler1[0] - gbcdLimits[0]) / gbcdDeltas[0]);
            int32_t location2 = int32_t((mis_euler1[1] - gbcdLimits[1]) / gbcdDeltas[1]);
            int32_t location3 = int32_t((mis_euler1[2] - gbcdLimits[2]) / gbcdDeltas[2]);
            // find symmetric poles using the first symmetry operator
            MatrixMath::Multiply3x3with3x1(sym1, vec2, rotNormal2);
            // get coordinates in square projection of crystal normal parallel to boundary normal
            nhCheck = getSquareCoord(rotNormal2, sqCoord);
            // Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
            int32_t location4 = int32_t((sqCoord[0] - gbcdLimits[3]) / gbcdDeltas[3]);
            int32_t location5 = int32_t((sqCoord[1] - gbcdLimits[4]) / gbcdDeltas[4]);
            if (location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 &&
                location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] && location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
            {
              hemisphere = 0;
              if (nhCheck == false) { hemisphere = 1; }
              sum += m_GBCD[(m_PhaseOfInterest * totalGBCDBins) + 2 * ((location5 * shift4) + (location4 * shift3) + (location3 * shift2) + (location2 * shift1) + location1) + hemisphere];
              count++;
            }
          }
        }
      }
      gmtValues.push_back(theta);
      gmtValues.push_back((90.0f - phi));
      gmtValues.push_back(sum / float(count));
    }
  }

  FILE* f = NULL;
  f = fopen(m_OutputFile.toLatin1().data(), "wb");
  if (NULL == f)
  {
    QString ss = QObject::tr("Error opening output file '%1'").arg(m_OutputFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Remember to use the original Angle in Degrees!!!!
  fprintf(f, "%.1f %.1f %.1f %.1f\n", m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l, m_MisorientationRotation.angle);
  size_t size = gmtValues.size() / 3;

  for (size_t i = 0; i < size; i++)
  {
    fprintf(f, "%f %f %f\n", gmtValues[3 * i], gmtValues[3 * i + 1], gmtValues[3 * i + 2]);
  }
  fclose(f);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VisualizeGBCDGMT::getSquareCoord(float* xstl1_norm1, float* sqCoord)
{
  bool nhCheck = false;
  float adjust = 1.0;
  if (xstl1_norm1[2] >= 0.0)
  {
    adjust = -1.0;
    nhCheck = true;
  }
  if (fabsf(xstl1_norm1[0]) >= fabsf(xstl1_norm1[1]))
  {
    sqCoord[0] = (xstl1_norm1[0] / fabsf(xstl1_norm1[0])) * sqrt(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * (DREAM3D::Constants::k_SqrtPi / 2.0f);
    sqCoord[1] = (xstl1_norm1[0] / fabsf(xstl1_norm1[0])) * sqrt(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * ((2.0f / DREAM3D::Constants::k_SqrtPi) * atanf(xstl1_norm1[1] / xstl1_norm1[0]));
  }
  else
  {
    sqCoord[0] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * ((2.0f / DREAM3D::Constants::k_SqrtPi) * atanf(xstl1_norm1[0] / xstl1_norm1[1]));
    sqCoord[1] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * (DREAM3D::Constants::k_SqrtPi / 2.0f);
  }
  return nhCheck;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VisualizeGBCDGMT::newFilterInstance(bool copyFilterParameters)
{
  VisualizeGBCDGMT::Pointer filter = VisualizeGBCDGMT::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VisualizeGBCDGMT::getCompiledLibraryName()
{ return IOConstants::IOBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VisualizeGBCDGMT::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VisualizeGBCDGMT::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VisualizeGBCDGMT::getHumanLabel()
{ return "Write GBCD Pole Figure (GMT 5)"; }
