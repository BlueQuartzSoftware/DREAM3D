/* ============================================================================
 * Copyright (c) 2009-2022 BlueQuartz Software, LLC
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
#include "VisualizeGBCDPoleFigure.h"

#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"
#include "SIMPLib/Utilities/ParallelData2DAlgorithm.h"
#include "SIMPLib/Utilities/SIMPLibEndian.h"

#include "EbsdLib/LaueOps/LaueOps.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID31 = 31,

  DataContainerID = 1
};

namespace
{

class VisualizeGBCDPoleFigureImpl
{
private:
  DoubleArrayType::Pointer m_PoleFigureArray;
  std::array<int32_t, 2> m_Dimensions;
  LaueOps::Pointer orientOps;
  FloatArrayType::Pointer gbcdDeltasArray;
  FloatArrayType::Pointer gbcdLimitsArray;
  Int32ArrayType::Pointer gbcdSizesArray;
  DoubleArrayType::Pointer m_GBCDPtr;
  int32_t m_PhaseOfInterest = 0;
  AxisAngleInput m_MisorientationRotation;

public:
  VisualizeGBCDPoleFigureImpl(DoubleArrayType::Pointer& poleFigureArray, std::array<int32_t, 2> dimensions, LaueOps::Pointer& orient_Ops, FloatArrayType::Pointer& gbcd_DeltasArray,
                              FloatArrayType::Pointer& gbcd_LimitsArray, Int32ArrayType::Pointer& gbcd_SizesArray, DoubleArrayType::Pointer& gbcd, int32_t phaseOfInterest,
                              AxisAngleInput& misorientationRotation)
  : m_PoleFigureArray(poleFigureArray)
  , m_Dimensions(dimensions)
  , orientOps(orient_Ops)
  , gbcdDeltasArray(gbcd_DeltasArray)
  , gbcdLimitsArray(gbcd_LimitsArray)
  , gbcdSizesArray(gbcd_SizesArray)
  , m_GBCDPtr(gbcd)
  , m_PhaseOfInterest(phaseOfInterest)
  , m_MisorientationRotation(misorientationRotation)
  {
  }
  ~VisualizeGBCDPoleFigureImpl() = default;

  void generate(size_t xStart, size_t xEnd, size_t yStart, size_t yEnd) const
  {

    double* poleFigure = m_PoleFigureArray->getPointer(0);

    float* gbcdDeltas = gbcdDeltasArray->getPointer(0);
    int32_t* gbcdSizes = gbcdSizesArray->getPointer(0);
    float* gbcdLimits = gbcdLimitsArray->getPointer(0);
    double* gbcdValues = m_GBCDPtr->getPointer(0);

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

    float misAngle = m_MisorientationRotation.angle * SIMPLib::Constants::k_PiOver180F;
    float normAxis[3] = {m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l};
    MatrixMath::Normalize3x1(normAxis);
    // convert axis angle to matrix representation of misorientation
    OrientationTransformation::ax2om<OrientationF, OrientationF>(OrientationF(normAxis[0], normAxis[1], normAxis[2], misAngle)).toGMatrix(dg);

    // take inverse of misorientation variable to use for switching symmetry
    MatrixMath::Transpose3x3(dg, dgt);

    // get number of symmetry operators
    int32_t n_sym = orientOps->getNumSymOps();

    int32_t xpoints = m_Dimensions[0];
    int32_t ypoints = m_Dimensions[1];
    // int32_t zpoints = 1;
    int32_t xpointshalf = xpoints / 2;
    int32_t ypointshalf = ypoints / 2;
    float xres = 2.0f / float(xpoints);
    float yres = 2.0f / float(ypoints);
    // float zres = (xres + yres) / 2.0;
    bool nhCheck = false;
    int32_t hemisphere = 0;

    int32_t shift1 = gbcdSizes[0];
    int32_t shift2 = gbcdSizes[0] * gbcdSizes[1];
    int32_t shift3 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2];
    int32_t shift4 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3];

    int64_t totalGBCDBins = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3] * gbcdSizes[4] * 2;

    std::vector<size_t> dims = {1ULL};

    for(int32_t k = yStart; k < yEnd; k++)
    {
      for(int32_t l = xStart; l < xEnd; l++)
      {
        // get (x,y) for stereographic projection pixel
        float x = static_cast<float>(l - xpointshalf) * xres + (xres / 2.0F);
        float y = static_cast<float>(k - ypointshalf) * yres + (yres / 2.0F);

        if((x * x + y * y) <= 1.0)
        {
          double sum = 0.0;
          int32_t count = 0;
          vec[2] = -((x * x + y * y) - 1) / ((x * x + y * y) + 1);
          vec[0] = x * (1 + vec[2]);
          vec[1] = y * (1 + vec[2]);
          MatrixMath::Multiply3x3with3x1(dgt, vec, vec2);

          // Loop over all the symmetry operators in the given crystal symmetry
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
              OrientationF eu(mis_euler1, 3);
              eu = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(dg2));
              if(mis_euler1[0] < SIMPLib::Constants::k_PiOver2D && mis_euler1[1] < SIMPLib::Constants::k_PiOver2D && mis_euler1[2] < SIMPLib::Constants::k_PiOver2D)
              {
                mis_euler1[1] = cosf(mis_euler1[1]);
                // find bins in GBCD
                auto location1 = static_cast<int32_t>((mis_euler1[0] - gbcdLimits[0]) / gbcdDeltas[0]);
                auto location2 = static_cast<int32_t>((mis_euler1[1] - gbcdLimits[1]) / gbcdDeltas[1]);
                auto location3 = static_cast<int32_t>((mis_euler1[2] - gbcdLimits[2]) / gbcdDeltas[2]);
                // find symmetric poles using the first symmetry operator
                MatrixMath::Multiply3x3with3x1(sym1, vec, rotNormal);
                // get coordinates in square projection of crystal normal parallel to boundary normal
                nhCheck = getSquareCoord(rotNormal, sqCoord);
                // Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
                auto location4 = static_cast<int32_t>((sqCoord[0] - gbcdLimits[3]) / gbcdDeltas[3]);
                auto location5 = static_cast<int32_t>((sqCoord[1] - gbcdLimits[4]) / gbcdDeltas[4]);
                if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] &&
                   location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
                {
                  hemisphere = 0;
                  if(!nhCheck)
                  {
                    hemisphere = 1;
                  }
                  sum += gbcdValues[(m_PhaseOfInterest * totalGBCDBins) + 2 * ((location5 * shift4) + (location4 * shift3) + (location3 * shift2) + (location2 * shift1) + location1) + hemisphere];
                  count++;
                }
              }

              // again in second crystal reference frame
              // calculate symmetric misorientation
              MatrixMath::Multiply3x3with3x3(dgt, sym2, dg1);
              MatrixMath::Multiply3x3with3x3(sym1, dg1, dg2);
              // convert to euler angle
              eu = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(dg2));
              if(mis_euler1[0] < SIMPLib::Constants::k_PiOver2D && mis_euler1[1] < SIMPLib::Constants::k_PiOver2D && mis_euler1[2] < SIMPLib::Constants::k_PiOver2D)
              {
                mis_euler1[1] = cosf(mis_euler1[1]);
                // find bins in GBCD
                auto location1 = static_cast<int32_t>((mis_euler1[0] - gbcdLimits[0]) / gbcdDeltas[0]);
                auto location2 = static_cast<int32_t>((mis_euler1[1] - gbcdLimits[1]) / gbcdDeltas[1]);
                auto location3 = static_cast<int32_t>((mis_euler1[2] - gbcdLimits[2]) / gbcdDeltas[2]);
                // find symmetric poles using the first symmetry operator
                MatrixMath::Multiply3x3with3x1(sym1, vec2, rotNormal2);
                // get coordinates in square projection of crystal normal parallel to boundary normal
                nhCheck = getSquareCoord(rotNormal2, sqCoord);
                // Note the switch to have theta in the 4 slot and cos(Phi) int he 3 slot
                auto location4 = static_cast<int32_t>((sqCoord[0] - gbcdLimits[3]) / gbcdDeltas[3]);
                auto location5 = static_cast<int32_t>((sqCoord[1] - gbcdLimits[4]) / gbcdDeltas[4]);
                if(location1 >= 0 && location2 >= 0 && location3 >= 0 && location4 >= 0 && location5 >= 0 && location1 < gbcdSizes[0] && location2 < gbcdSizes[1] && location3 < gbcdSizes[2] &&
                   location4 < gbcdSizes[3] && location5 < gbcdSizes[4])
                {
                  hemisphere = 0;
                  if(!nhCheck)
                  {
                    hemisphere = 1;
                  }
                  sum += gbcdValues[(m_PhaseOfInterest * totalGBCDBins) + 2 * ((location5 * shift4) + (location4 * shift3) + (location3 * shift2) + (location2 * shift1) + location1) + hemisphere];
                  count++;
                }
              }
            }
          }
          if(count > 0)
          {
            poleFigure[(k * xpoints) + l] = sum / float(count);
          }
        }
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const SIMPLRange2D& r) const
  {
    generate(r.minCol(), r.maxCol(), r.minRow(), r.maxRow());
  }
#endif

private:
  /**
   * @brief getSquareCoord Computes the square based coordinate based on the incoming normal
   * @param crystalNormal Incoming normal
   * @param sqCoord Computed square coordinate
   * @return Boolean value for whether coordinate lies in the norther hemisphere
   */
  static bool getSquareCoord(float* crystalNormal, float* sqCoord)
  {
    bool nhCheck = false;
    float adjust = 1.0;
    if(crystalNormal[2] >= 0.0)
    {
      adjust = -1.0;
      nhCheck = true;
    }
    if(fabsf(crystalNormal[0]) >= fabsf(crystalNormal[1]))
    {
      sqCoord[0] = (crystalNormal[0] / fabsf(crystalNormal[0])) * sqrtf(2.0f * 1.0f * (1.0f + (crystalNormal[2] * adjust))) * (SIMPLib::Constants::k_SqrtPiF / 2.0f);
      sqCoord[1] = (crystalNormal[0] / fabsf(crystalNormal[0])) * sqrtf(2.0f * 1.0f * (1.0f + (crystalNormal[2] * adjust))) *
                   ((2.0f / SIMPLib::Constants::k_SqrtPiF) * atanf(crystalNormal[1] / crystalNormal[0]));
    }
    else
    {
      sqCoord[0] = (crystalNormal[1] / fabsf(crystalNormal[1])) * sqrtf(2.0f * 1.0f * (1.0f + (crystalNormal[2] * adjust))) *
                   ((2.0f / SIMPLib::Constants::k_SqrtPiF) * atanf(crystalNormal[0] / crystalNormal[1]));
      sqCoord[1] = (crystalNormal[1] / fabsf(crystalNormal[1])) * sqrtf(2.0f * 1.0f * (1.0f + (crystalNormal[2] * adjust))) * (SIMPLib::Constants::k_SqrtPiF / 2.0f);
    }
    return nhCheck;
  }
};

} // namespace

// -----------------------------------------------------------------------------
VisualizeGBCDPoleFigure::VisualizeGBCDPoleFigure()
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
VisualizeGBCDPoleFigure::~VisualizeGBCDPoleFigure() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Phase of Interest", PhaseOfInterest, FilterParameter::Category::Parameter, VisualizeGBCDPoleFigure));
  parameters.push_back(SIMPL_NEW_AXISANGLE_FP("Misorientation Axis-Angle", MisorientationRotation, FilterParameter::Category::Parameter, VisualizeGBCDPoleFigure));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output Regular Grid VTK File", OutputFile, FilterParameter::Category::Parameter, VisualizeGBCDPoleFigure, "*.vtk", "VTK File"));

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Output Image Dimension", OutputDimension, FilterParameter::Category::Parameter, VisualizeGBCDPoleFigure));

  parameters.push_back(SeparatorFilterParameter::Create("Input Face Ensemble Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::FaceEnsemble, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Input GBCD", GBCDArrayPath, FilterParameter::Category::RequiredArray, VisualizeGBCDPoleFigure, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::Category::RequiredArray, VisualizeGBCDPoleFigure, req));
  }

  parameters.push_back(SeparatorFilterParameter::Create("Output Pole Figure", FilterParameter::Category::CreatedArray));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Pole Figure Data Container", DataContainerName, FilterParameter::Category::CreatedArray, VisualizeGBCDPoleFigure));
  parameters.push_back(
      SIMPL_NEW_AM_WITH_LINKED_DC_FP("Pole Figure Cell Attribute Matrix", CellAttributeMatrixName, DataContainerName, FilterParameter::Category::CreatedArray, VisualizeGBCDPoleFigure));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Pole Figure Cell Intensity", IntensityArrayName, DataContainerName, CellAttributeMatrixName, FilterParameter::Category::CreatedArray, VisualizeGBCDPoleFigure));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
void VisualizeGBCDPoleFigure::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom>(this, getGBCDArrayPath().getDataContainerName());

  QFileInfo fi(m_OutputFile);
  if(fi.suffix().compare("") == 0)
  {
    m_OutputFile.append(".dream3d");
  }
  FileSystemPathHelper::CheckOutputFile(this, "Output VTK File", getOutputFile(), true);

  std::vector<size_t> cDims = {1};
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
    cDims = tmpGBCDPtr->getComponentDimensions();
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

  if(m_OutputDimension < 0)
  {
    QString ss = QObject::tr("Output Dimension must be greater than zero").arg(ClassName());
    setErrorCondition(-100, ss);
  }

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  auto outDim = static_cast<size_t>(m_OutputDimension);
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  image->setDimensions({outDim, outDim, 1});
  image->setSpacing({2.0f / outDim, 2.0f / outDim, 2.0f / outDim});
  image->setOrigin(-1.0F, -1.0F, 0.0F);
  m->setGeometry(image);

  std::vector<size_t> tDims = {outDim, outDim, 1ULL};
  m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0)
  {
    return;
  }

  cDims.assign({1});
  DataArrayPath arrayPath = getDataContainerName();
  arrayPath.setAttributeMatrixName(getCellAttributeMatrixName());
  arrayPath.setDataArrayName(getIntensityArrayName());
  m_IntensityPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DoubleArrayType>(this, arrayPath, 0, cDims, "", DataArrayID31);
  if(nullptr != m_IntensityPtr.lock())
  {
    m_Intensity = m_IntensityPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

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
  int* gbcdSizes = gbcdSizesArray->getPointer(0);
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
  gbcdLimits[3] = 0.0f;
  gbcdLimits[4] = 0.0f;
  gbcdLimits[5] = SIMPLib::Constants::k_PiOver2D;
  gbcdLimits[6] = 1.0f;
  gbcdLimits[7] = SIMPLib::Constants::k_PiOver2D;
  gbcdLimits[8] = 1.0f;
  gbcdLimits[9] = SIMPLib::Constants::k_2PiD;

  // reset the 3rd and 4th dimensions using the square grid approach
  gbcdLimits[3] = -sqrtf(SIMPLib::Constants::k_PiOver2D);
  gbcdLimits[4] = -sqrtf(SIMPLib::Constants::k_PiOver2D);
  gbcdLimits[8] = sqrtf(SIMPLib::Constants::k_PiOver2D);
  gbcdLimits[9] = sqrtf(SIMPLib::Constants::k_PiOver2D);

  // get num components of GBCD
  std::vector<size_t> cDims = m_GBCDPtr.lock()->getComponentDimensions();

  gbcdSizes[0] = static_cast<int32_t>(cDims[0]);
  gbcdSizes[1] = static_cast<int32_t>(cDims[1]);
  gbcdSizes[2] = static_cast<int32_t>(cDims[2]);
  gbcdSizes[3] = static_cast<int32_t>(cDims[3]);
  gbcdSizes[4] = static_cast<int32_t>(cDims[4]);

  gbcdDeltas[0] = (gbcdLimits[5] - gbcdLimits[0]) / float(gbcdSizes[0]);
  gbcdDeltas[1] = (gbcdLimits[6] - gbcdLimits[1]) / float(gbcdSizes[1]);
  gbcdDeltas[2] = (gbcdLimits[7] - gbcdLimits[2]) / float(gbcdSizes[2]);
  gbcdDeltas[3] = (gbcdLimits[8] - gbcdLimits[3]) / float(gbcdSizes[3]);
  gbcdDeltas[4] = (gbcdLimits[9] - gbcdLimits[4]) / float(gbcdSizes[4]);

  //  float vec[3] = {0.0f, 0.0f, 0.0f};
  //  float vec2[3] = {0.0f, 0.0f, 0.0f};
  //  float rotNormal[3] = {0.0f, 0.0f, 0.0f};
  //  float rotNormal2[3] = {0.0f, 0.0f, 0.0f};
  //  float sqCoord[2] = {0.0f, 0.0f};
  float dg[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float dgt[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  //  float dg1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  //  float dg2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  //  float sym1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  //  float sym2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  //  float sym2t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  //  float mis_euler1[3] = {0.0f, 0.0f, 0.0f};

  float misAngle = m_MisorientationRotation.angle * SIMPLib::Constants::k_PiOver180F;
  float normAxis[3] = {m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l};
  MatrixMath::Normalize3x1(normAxis);
  // convert axis angle to matrix representation of misorientation
  OrientationTransformation::ax2om<OrientationF, OrientationF>(OrientationF(normAxis[0], normAxis[1], normAxis[2], misAngle)).toGMatrix(dg);

  // take inverse of misorientation variable to use for switching symmetry
  MatrixMath::Transpose3x3(dg, dgt);

  // Get our LaueOps pointer for the selected crystal structure
  LaueOps::Pointer orientOps = m_OrientationOps[m_CrystalStructures[m_PhaseOfInterest]];

  // get number of symmetry operators
  //  int32_t n_sym = orientOps->getNumSymOps();

  int32_t xpoints = m_OutputDimension;
  int32_t ypoints = m_OutputDimension;
  int32_t zpoints = 1;
  //  int32_t xpointshalf = xpoints / 2;
  //  int32_t ypointshalf = ypoints / 2;
  float xres = 2.0f / static_cast<float>(xpoints);
  float yres = 2.0f / static_cast<float>(ypoints);
  float zres = (xres + yres) / 2.0F;

  //  bool nhCheck = false;
  //  int32_t hemisphere = 0;

  //  int32_t shift1 = gbcdSizes[0];
  //  int32_t shift2 = gbcdSizes[0] * gbcdSizes[1];
  //  int32_t shift3 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2];
  //  int32_t shift4 = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3];

  //  int64_t totalGBCDBins = gbcdSizes[0] * gbcdSizes[1] * gbcdSizes[2] * gbcdSizes[3] * gbcdSizes[4] * 2;

//  std::vector<size_t> dims = {1ULL};
//  DoubleArrayType::Pointer poleFigureArray = DoubleArrayType::NullPointer();
//  poleFigureArray = DoubleArrayType::CreateArray(xpoints * ypoints, dims, "PoleFigure", true);
//  poleFigureArray->initializeWithZeros();
//  double* poleFigure = poleFigureArray->getPointer(0);

  DoubleArrayType::Pointer poleFigurePtr = m_IntensityPtr.lock();
  double* poleFigure = poleFigurePtr->getPointer(0);

  DoubleArrayType::Pointer gbcdPtr = m_GBCDPtr.lock();

  QString ss = QObject::tr("Generating Intensity Plot for phase %1").arg(m_PhaseOfInterest);
  notifyStatusMessage(ss);

  ParallelData2DAlgorithm dataAlg;
  dataAlg.setRange(0, 0, ypoints, xpoints);
  dataAlg.execute(VisualizeGBCDPoleFigureImpl(poleFigurePtr, {xpoints, ypoints}, orientOps, gbcdDeltasArray, gbcdLimitsArray, gbcdSizesArray, gbcdPtr, m_PhaseOfInterest, m_MisorientationRotation));

#if 0
  for(int32_t k = 0; k < ypoints; k++)
  {
    for(int32_t l = 0; l < xpoints; l++)
    {
      // get (x,y) for stereographic projection pixel
      float x = float(l - xpointshalf) * xres + (xres / 2.0);
      float y = float(k - ypointshalf) * yres + (yres / 2.0);
      if((x * x + y * y) <= 1.0)
      {
        float sum = 0.0f;
        int32_t count = 0;
        vec[2] = -((x * x + y * y) - 1) / ((x * x + y * y) + 1);
        vec[0] = x * (1 + vec[2]);
        vec[1] = y * (1 + vec[2]);
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
            OrientationF eu(mis_euler1, 3);
            eu = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(dg2));
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
            eu = OrientationTransformation::om2eu<OrientationF, OrientationF>(OrientationF(dg2));
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
        if(count > 0)
        {
          poleFigure[(k * xpoints) + l] = sum / float(count);
        }
      }
    }
  }
#endif

  FILE* f = fopen(m_OutputFile.toLatin1().data(), "wb");
  if(nullptr == f)
  {
    ss = QObject::tr("Error opening output file '%1'").arg(m_OutputFile);
    setErrorCondition(-1, ss);
    return;
  }

  ss = QObject::tr("Writing output file %1").arg(m_OutputFile);
  notifyStatusMessage(ss);

  // Write the correct header
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "data set from DREAM3D\n");
  fprintf(f, "BINARY");
  fprintf(f, "\n");
  fprintf(f, "DATASET RECTILINEAR_GRID\n");
  fprintf(f, "DIMENSIONS %d %d %d\n", xpoints + 1, ypoints + 1, zpoints + 1);

  // Write the Coords
  writeCoords(f, "X_COORDINATES", "float", xpoints + 1, (-float(xpoints) * xres / 2.0f), xres);
  writeCoords(f, "Y_COORDINATES", "float", ypoints + 1, (-float(ypoints) * yres / 2.0f), yres);
  writeCoords(f, "Z_COORDINATES", "float", zpoints + 1, (-float(zpoints) * zres / 2.0f), zres);

  int32_t total = xpoints * ypoints * zpoints;
  fprintf(f, "CELL_DATA %d\n", total);

  fprintf(f, "SCALARS %s %s 1\n", "Intensity", "float");
  fprintf(f, "LOOKUP_TABLE default\n");
  {

    std::vector<float> gn(total);
    float t;
    int32_t count = 0;
    for(int32_t j = 0; j < ypoints; j++)
    {
      for(int32_t i = 0; i < xpoints; i++)
      {
        t = float(poleFigure[(j * xpoints) + i]);
        SIMPLib::Endian::FromSystemToBig::convert(t);
        gn[count] = t;
        count++;
      }
    }
    size_t totalWritten = fwrite(gn.data(), sizeof(float), (total), f);
    if(totalWritten != (total))
    {
      ss = QObject::tr("Error writing binary VTK data to file '%1'").arg(m_OutputFile);
      setErrorCondition(-1, ss);
      fclose(f);
      return;
    }
  }
  fclose(f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t VisualizeGBCDPoleFigure::writeCoords(FILE* f, const char* axis, const char* type, int64_t nPoints, float min, float step)
{
  int32_t err = 0;
  fprintf(f, "%s %lld %s\n", axis, (long long int)(nPoints), type);

  std::vector<float> data(nPoints, 0.0F);
  float d;
  for(int64_t idx = 0; idx < nPoints; ++idx)
  {
    d = static_cast<float>(idx) * step + min;
    SIMPLib::Endian::FromSystemToBig::convert(d);
    data[idx] = d;
  }
  size_t totalWritten = fwrite(static_cast<void*>(data.data()), sizeof(float), static_cast<size_t>(nPoints), f);

  if(totalWritten != static_cast<size_t>(nPoints))
  {
    QString ss = QObject::tr("Error writing binary VTK data to file '%1'").arg(m_OutputFile);
    setErrorCondition(-1, ss);
    fclose(f);
    return getErrorCode();
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VisualizeGBCDPoleFigure::newFilterInstance(bool copyFilterParameters) const
{
  VisualizeGBCDPoleFigure::Pointer filter = VisualizeGBCDPoleFigure::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDPoleFigure::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDPoleFigure::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDPoleFigure::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDPoleFigure::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VisualizeGBCDPoleFigure::getUuid() const
{
  return {"{85900eba-3da9-5985-ac71-1d9d290a5d31}"};
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDPoleFigure::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VisualizeGBCDPoleFigure::getHumanLabel() const
{
  return "Export GBCD Pole Figure (VTK)";
}

// -----------------------------------------------------------------------------
VisualizeGBCDPoleFigure::Pointer VisualizeGBCDPoleFigure::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<VisualizeGBCDPoleFigure> VisualizeGBCDPoleFigure::New()
{
  struct make_shared_enabler : public VisualizeGBCDPoleFigure
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString VisualizeGBCDPoleFigure::getNameOfClass() const
{
  return {"VisualizeGBCDPoleFigure"};
}

// -----------------------------------------------------------------------------
QString VisualizeGBCDPoleFigure::ClassName()
{
  return {"VisualizeGBCDPoleFigure"};
}

// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::setOutputFile(const QString& value)
{
  m_OutputFile = value;
}

// -----------------------------------------------------------------------------
QString VisualizeGBCDPoleFigure::getOutputFile() const
{
  return m_OutputFile;
}

// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::setPhaseOfInterest(int value)
{
  m_PhaseOfInterest = value;
}

// -----------------------------------------------------------------------------
int VisualizeGBCDPoleFigure::getPhaseOfInterest() const
{
  return m_PhaseOfInterest;
}

// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::setMisorientationRotation(const AxisAngleInput& value)
{
  m_MisorientationRotation = value;
}

// -----------------------------------------------------------------------------
AxisAngleInput VisualizeGBCDPoleFigure::getMisorientationRotation() const
{
  return m_MisorientationRotation;
}

// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::setGBCDArrayPath(const DataArrayPath& value)
{
  m_GBCDArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath VisualizeGBCDPoleFigure::getGBCDArrayPath() const
{
  return m_GBCDArrayPath;
}

// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath VisualizeGBCDPoleFigure::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void VisualizeGBCDPoleFigure::setOutputDimension(const int32_t value)
{
  m_OutputDimension = value;
}

// -----------------------------------------------------------------------------
int32_t VisualizeGBCDPoleFigure::getOutputDimension() const
{
  return m_OutputDimension;
}

void VisualizeGBCDPoleFigure::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

DataArrayPath VisualizeGBCDPoleFigure::getDataContainerName() const
{
  return m_DataContainerName;
}

void VisualizeGBCDPoleFigure::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}
QString VisualizeGBCDPoleFigure::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

void VisualizeGBCDPoleFigure::setIntensityArrayName(const QString& value)
{
  m_IntensityArrayName = value;
}

QString VisualizeGBCDPoleFigure::getIntensityArrayName() const
{
  return m_IntensityArrayName;
}
