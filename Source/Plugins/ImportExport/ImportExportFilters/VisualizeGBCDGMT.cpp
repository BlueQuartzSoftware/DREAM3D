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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VisualizeGBCDGMT.h"

#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

#include "EbsdLib/Core/Orientation.hpp"
#include "EbsdLib/Core/OrientationTransformation.hpp"
#include "EbsdLib/Core/Quaternion.hpp"
#include "EbsdLib/LaueOps/LaueOps.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCDGMT::VisualizeGBCDGMT()
{
  m_MisorientationRotation.angle = 0.0f;
  m_MisorientationRotation.h = 0.0f;
  m_MisorientationRotation.k = 0.0f;
  m_MisorientationRotation.l = 0.0f;

  m_OrientationOps = LaueOps::GetAllOrientationOps();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VisualizeGBCDGMT::~VisualizeGBCDGMT() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Phase of Interest", PhaseOfInterest, FilterParameter::Category::Parameter, VisualizeGBCDGMT));
  parameters.push_back(SIMPL_NEW_AXISANGLE_FP("Misorientation Axis-Angle", MisorientationRotation, FilterParameter::Category::Parameter, VisualizeGBCDGMT));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output GMT File", OutputFile, FilterParameter::Category::Parameter, VisualizeGBCDGMT, "*.dat", "DAT File"));
  parameters.push_back(SeparatorFilterParameter::Create("Face Ensemble Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::FaceEnsemble, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("GBCD", GBCDArrayPath, FilterParameter::Category::RequiredArray, VisualizeGBCDGMT, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::Category::RequiredArray, VisualizeGBCDGMT, req));
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
  setMisorientationRotation(reader->readAxisAngle("MisorientationRotation", getMisorientationRotation(), -1));
  setPhaseOfInterest(reader->readValue("PhaseOfInterest", getPhaseOfInterest()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::initialize()
{
  gmtValues.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom>(this, getGBCDArrayPath().getDataContainerName());

  QFileInfo fi(getOutputFile());
  if(fi.suffix().compare("") == 0)
  {
    setOutputFile(getOutputFile().append(".dat"));
  }

  FileSystemPathHelper::CheckOutputFile(this, "Output GBCD File", getOutputFile(), true);

  // Make sure the file name ends with _1 so the GMT scripts work correctly
  QString fName = fi.baseName();
  if(!fName.endsWith("_1"))
  {
    fName = fName + "_1";
    QString absPath = fi.absolutePath() + "/" + fName + ".dat";
    setOutputFile(absPath);
  }

  std::vector<size_t> cDims(1, 1);
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>>(this, getCrystalStructuresArrayPath(), cDims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  IDataArray::Pointer tmpGBCDPtr = getDataContainerArray()->getPrereqIDataArrayFromPath(this, getGBCDArrayPath());
  if(getErrorCode() < 0)
  {
    return;
  }

  if(nullptr != tmpGBCDPtr.get())
  {
    std::vector<size_t> cDims = tmpGBCDPtr->getComponentDimensions();
    m_GBCDPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>>(this, getGBCDArrayPath(), cDims);
    if(nullptr != m_GBCDPtr.lock())
    {
      m_GBCD = m_GBCDPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if(nullptr != m_GBCDPtr.lock() && getPhaseOfInterest() >= m_GBCDPtr.lock()->getNumberOfTuples())
  {
    QString ss = QObject::tr("The phase index is larger than the number of Ensembles").arg(ClassName());
    setErrorCondition(-1, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getGBCDArrayPath().getDataContainerName());

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());

  QDir dir(fi.path());
  if(!dir.mkpath("."))
  {
    QString ss;
    ss = QObject::tr("Error creating parent path '%1'").arg(dir.path());
    setErrorCondition(-1, ss);
    return;
  }

  QFile file(getOutputFile());
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Error opening output file '%1'").arg(getOutputFile());
    setErrorCondition(-100, ss);
    return;
  }

  FloatArrayType::Pointer gbcdDeltasArray = FloatArrayType::CreateArray(5, std::string("GBCDDeltas"), true);
  gbcdDeltasArray->initializeWithZeros();

  FloatArrayType::Pointer gbcdLimitsArray = FloatArrayType::CreateArray(10, std::string("GBCDLimits"), true);
  gbcdLimitsArray->initializeWithZeros();

  Int32ArrayType::Pointer gbcdSizesArray = Int32ArrayType::CreateArray(5, std::string("GBCDSizes"), true);
  gbcdSizesArray->initializeWithZeros();

  float* gbcdDeltas = gbcdDeltasArray->getPointer(0);
  int32_t* gbcdSizes = gbcdSizesArray->getPointer(0);
  float* gbcdLimits = gbcdLimitsArray->getPointer(0);

  // Original Ranges from Dave R.
  // m_GBCDlimits[0] = 0.0f;
  // m_GBCDlimits[1] = cosf(1.0f*m_pi);
  // m_GBCDlimits[2] = 0.0f;
  // m_GBCDlimits[3] = 0.0f;
  // m_GBCDlimits[4] = cosf(1.0f*m_pi);
  // m_GBCDlimits[5] = 2.0f*m_pi;
  // m_GBCDlimits[6] = cosf(0.0f);
  // m_GBCDlimits[7] = 2.0f*m_pi;
  // m_GBCDlimits[8] = 2.0f*m_pi;
  // m_GBCDlimits[9] = cosf(0.0f);

  // Greg R. Ranges
  gbcdLimits[0] = 0.0f;
  gbcdLimits[1] = 0.0f;
  gbcdLimits[2] = 0.0f;
  gbcdLimits[3] = -sqrtf(SIMPLib::Constants::k_PiOver2D);
  gbcdLimits[4] = -sqrtf(SIMPLib::Constants::k_PiOver2D);
  gbcdLimits[5] = SIMPLib::Constants::k_PiD / 2.0f;
  gbcdLimits[6] = 1.0f;
  gbcdLimits[7] = SIMPLib::Constants::k_PiD / 2.0f;
  gbcdLimits[8] = sqrtf(SIMPLib::Constants::k_PiOver2D);
  gbcdLimits[9] = sqrtf(SIMPLib::Constants::k_PiOver2D);

  // get num components of GBCD
  std::vector<size_t> cDims = m_GBCDPtr.lock()->getComponentDimensions();

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

  float vec[3] = {0.0f, 0.0f, 0.0f};
  float vec2[3] = {0.0f, 0.0f, 0.0f};
  float rotNormal[3] = {0.0f, 0.0f, 0.0f};
  float rotNormal2[3] = {0.0f, 0.0f, 0.0f};
  float sqCoord[2] = {0.0f, 0.0f};
  float dg[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float dgt[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float dg1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float dg2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float sym1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float sym2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float sym2t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float mis_euler1[3] = {0.0f, 0.0f, 0.0f};

  float misAngle = m_MisorientationRotation.angle * SIMPLib::Constants::k_PiOver180D;
  float normAxis[3] = {m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l};
  MatrixMath::Normalize3x1(normAxis);
  // convert axis angle to matrix representation of misorientation
  OrientationTransformation::ax2om<OrientationF, OrientationF>(OrientationF(normAxis[0], normAxis[1], normAxis[2], misAngle)).toGMatrix(dg);

  // take inverse of misorientation variable to use for switching symmetry
  MatrixMath::Transpose3x3(dg, dgt);

  // Get our LaueOps pointer for the selected crystal structure
  LaueOps::Pointer orientOps = m_OrientationOps[m_CrystalStructures[m_PhaseOfInterest]];

  // get number of symmetry operators
  int32_t n_sym = orientOps->getNumSymOps();

  int32_t thetaPoints = 120;
  int32_t phiPoints = 30;
  float thetaRes = 360.0f / float(thetaPoints);
  float phiRes = 90.0f / float(phiPoints);
  float theta = 0.0f, phi = 0.0f;
  float thetaRad = 0.0f, phiRad = 0.0f;
  float degToRad = SIMPLib::Constants::k_PiOver180D;
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

  for(int32_t k = 0; k < phiPoints + 1; k++)
  {
    for(int32_t l = 0; l < thetaPoints + 1; l++)
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
      for(int32_t i = 0; i < n_sym; i++)
      {
        // get symmetry operator1
        orientOps->getMatSymOp(i, sym1);
        for(int32_t j = 0; j < n_sym; j++)
        {
          // get symmetry operator2
          orientOps->getMatSymOp(j, sym2);
          MatrixMath::Transpose3x3(sym2, sym2t);
          // calculate symmetric misorientation
          MatrixMath::Multiply3x3with3x3(dg, sym2t, dg1);
          MatrixMath::Multiply3x3with3x3(sym1, dg1, dg2);
          // convert to euler angle
          OrientationF mEuler(mis_euler1, 3);
          mEuler = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(dg2));

          if(mis_euler1[0] < SIMPLib::Constants::k_PiOver2D && mis_euler1[1] < SIMPLib::Constants::k_PiOver2D && mis_euler1[2] < SIMPLib::Constants::k_PiOver2D)
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
            if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] &&
               location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
            {
              hemisphere = 0;
              if(!nhCheck)
              {
                hemisphere = 1;
              }
              sum += m_GBCD[(m_PhaseOfInterest * totalGBCDBins) + 2 * ((location5 * shift4) + (location4 * shift3) + (location3 * shift2) + (location2 * shift1) + location1) + hemisphere];
              count++;
            }
          }

          // again in second crystal reference frame
          // calculate symmetric misorientation
          MatrixMath::Multiply3x3with3x3(dgt, sym2, dg1);
          MatrixMath::Multiply3x3with3x3(sym1, dg1, dg2);
          // convert to euler angle
          mEuler = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(dg2));
          if(mis_euler1[0] < SIMPLib::Constants::k_PiOver2D && mis_euler1[1] < SIMPLib::Constants::k_PiOver2D && mis_euler1[2] < SIMPLib::Constants::k_PiOver2D)
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
            if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] &&
               location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
            {
              hemisphere = 0;
              if(!nhCheck)
              {
                hemisphere = 1;
              }
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

  FILE* f = nullptr;
  f = fopen(m_OutputFile.toLatin1().data(), "wb");
  if(nullptr == f)
  {
    QString ss = QObject::tr("Error opening output file '%1'").arg(m_OutputFile);
    setErrorCondition(-1, ss);
    return;
  }

  // Remember to use the original Angle in Degrees!!!!
  fprintf(f, "%.1f %.1f %.1f %.1f\n", m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l, m_MisorientationRotation.angle);
  size_t size = gmtValues.size() / 3;

  for(size_t i = 0; i < size; i++)
  {
    fprintf(f, "%f %f %f\n", gmtValues[3 * i], gmtValues[3 * i + 1], gmtValues[3 * i + 2]);
  }
  fclose(f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VisualizeGBCDGMT::getSquareCoord(float* xstl1_norm1, float* sqCoord)
{
  bool nhCheck = false;
  float adjust = 1.0;
  if(xstl1_norm1[2] >= 0.0)
  {
    adjust = -1.0;
    nhCheck = true;
  }
  if(fabsf(xstl1_norm1[0]) >= fabsf(xstl1_norm1[1]))
  {
    sqCoord[0] = (xstl1_norm1[0] / fabsf(xstl1_norm1[0])) * sqrt(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * (SIMPLib::Constants::k_SqrtPiD / 2.0f);
    sqCoord[1] = (xstl1_norm1[0] / fabsf(xstl1_norm1[0])) * sqrt(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * ((2.0f / SIMPLib::Constants::k_SqrtPiD) * atanf(xstl1_norm1[1] / xstl1_norm1[0]));
  }
  else
  {
    sqCoord[0] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * ((2.0f / SIMPLib::Constants::k_SqrtPiD) * atanf(xstl1_norm1[0] / xstl1_norm1[1]));
    sqCoord[1] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * (SIMPLib::Constants::k_SqrtPiD / 2.0f);
  }
  return nhCheck;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VisualizeGBCDGMT::newFilterInstance(bool copyFilterParameters) const
{
  VisualizeGBCDGMT::Pointer filter = VisualizeGBCDGMT::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDGMT::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDGMT::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDGMT::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDGMT::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VisualizeGBCDGMT::getUuid() const
{
  return QUuid("{f62065b4-54e9-53b1-bed7-2178a57d3c7a}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDGMT::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDGMT::getHumanLabel() const
{
  return "Export GBCD Pole Figure (GMT 5)";
}

// -----------------------------------------------------------------------------
VisualizeGBCDGMT::Pointer VisualizeGBCDGMT::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<VisualizeGBCDGMT> VisualizeGBCDGMT::New()
{
  struct make_shared_enabler : public VisualizeGBCDGMT
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString VisualizeGBCDGMT::getNameOfClass() const
{
  return QString("VisualizeGBCDGMT");
}

// -----------------------------------------------------------------------------
QString VisualizeGBCDGMT::ClassName()
{
  return QString("VisualizeGBCDGMT");
}

// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::setOutputFile(const QString& value)
{
  m_OutputFile = value;
}

// -----------------------------------------------------------------------------
QString VisualizeGBCDGMT::getOutputFile() const
{
  return m_OutputFile;
}

// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::setPhaseOfInterest(int value)
{
  m_PhaseOfInterest = value;
}

// -----------------------------------------------------------------------------
int VisualizeGBCDGMT::getPhaseOfInterest() const
{
  return m_PhaseOfInterest;
}

// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::setMisorientationRotation(const AxisAngleInput& value)
{
  m_MisorientationRotation = value;
}

// -----------------------------------------------------------------------------
AxisAngleInput VisualizeGBCDGMT::getMisorientationRotation() const
{
  return m_MisorientationRotation;
}

// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::setGBCDArrayPath(const DataArrayPath& value)
{
  m_GBCDArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath VisualizeGBCDGMT::getGBCDArrayPath() const
{
  return m_GBCDArrayPath;
}

// -----------------------------------------------------------------------------
void VisualizeGBCDGMT::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath VisualizeGBCDGMT::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}
