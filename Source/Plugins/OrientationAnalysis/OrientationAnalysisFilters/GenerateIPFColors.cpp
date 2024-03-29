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

#include "GenerateIPFColors.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLRange.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Utilities/ColorTable.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"

#include "EbsdLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/Core/EbsdLibConstants.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID31 = 31
};

/**
 * @brief The GenerateIPFColorsImpl class implements a threaded algorithm that computes the IPF
 * colors for each element in a geometry
 */
class GenerateIPFColorsImpl
{
public:
  GenerateIPFColorsImpl(GenerateIPFColors* filter, FloatVec3Type referenceDir, float* eulers, int32_t* phases, uint32_t* crystalStructures, int32_t numPhases, bool* goodVoxels, uint8_t* colors)
  : m_Filter(filter)
  , m_ReferenceDir(referenceDir)
  , m_CellEulerAngles(eulers)
  , m_CellPhases(phases)
  , m_CrystalStructures(crystalStructures)
  , m_NumPhases(numPhases)
  , m_GoodVoxels(goodVoxels)
  , m_CellIPFColors(colors)
  {
  }

  virtual ~GenerateIPFColorsImpl() = default;

  void convert(size_t start, size_t end) const
  {
    std::vector<LaueOps::Pointer> ops = LaueOps::GetAllOrientationOps();
    double refDir[3] = {m_ReferenceDir[0], m_ReferenceDir[1], m_ReferenceDir[2]};
    double dEuler[3] = {0.0, 0.0, 0.0};
    SIMPL::Rgb argb = 0x00000000;
    int32_t phase = 0;
    bool calcIPF = false;
    size_t index = 0;
    for(size_t i = start; i < end; i++)
    {
      phase = m_CellPhases[i];
      index = i * 3;
      m_CellIPFColors[index] = 0;
      m_CellIPFColors[index + 1] = 0;
      m_CellIPFColors[index + 2] = 0;
      dEuler[0] = m_CellEulerAngles[index];
      dEuler[1] = m_CellEulerAngles[index + 1];
      dEuler[2] = m_CellEulerAngles[index + 2];

      // Make sure we are using a valid Euler Angles with valid crystal symmetry
      calcIPF = true;
      if(nullptr != m_GoodVoxels)
      {
        calcIPF = m_GoodVoxels[i];
      }
      // Sanity check the phase data to make sure we do not walk off the end of the array
      if(phase >= m_NumPhases)
      {
        m_Filter->incrementPhaseWarningCount();
      }

      if(phase < m_NumPhases && calcIPF && m_CrystalStructures[phase] < EbsdLib::CrystalStructure::LaueGroupEnd)
      {
        argb = ops[m_CrystalStructures[phase]]->generateIPFColor(dEuler, refDir, false);
        m_CellIPFColors[index] = static_cast<uint8_t>(RgbColor::dRed(argb));
        m_CellIPFColors[index + 1] = static_cast<uint8_t>(RgbColor::dGreen(argb));
        m_CellIPFColors[index + 2] = static_cast<uint8_t>(RgbColor::dBlue(argb));
      }
    }
  }

  void operator()(const SIMPLRange& range) const
  {
    convert(range.min(), range.max());
  }

private:
  GenerateIPFColors* m_Filter = nullptr;
  FloatVec3Type m_ReferenceDir;
  float* m_CellEulerAngles;
  int32_t* m_CellPhases;
  unsigned int* m_CrystalStructures;
  int32_t m_NumPhases = 0;
  bool* m_GoodVoxels;
  uint8_t* m_CellIPFColors;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIPFColors::GenerateIPFColors()
{
  m_ReferenceDir[0] = 0.0f;
  m_ReferenceDir[1] = 0.0f;
  m_ReferenceDir[2] = 1.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIPFColors::~GenerateIPFColors() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Reference Direction", ReferenceDir, FilterParameter::Category::Parameter, GenerateIPFColors));

  std::vector<QString> linkedProps = {"GoodVoxelsArrayPath"};
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Apply to Good Elements Only (Bad Elements Will Be Black)", UseGoodVoxels, FilterParameter::Category::Parameter, GenerateIPFColors, linkedProps));
  parameters.push_back(SeparatorFilterParameter::Create("Element Data", FilterParameter::Category::RequiredArray));
  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::Category::RequiredArray, GenerateIPFColors, req));
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::Category::RequiredArray, GenerateIPFColors, req));
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::Category::RequiredArray, GenerateIPFColors, req));

  parameters.push_back(SeparatorFilterParameter::Create("Ensemble Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::Category::RequiredArray, GenerateIPFColors, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Element Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("IPF Colors", CellIPFColorsArrayName, CellEulerAnglesArrayPath, CellEulerAnglesArrayPath, FilterParameter::Category::CreatedArray, GenerateIPFColors));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels()));
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setCellIPFColorsArrayName(reader->readString("CellIPFColorsArrayName", getCellIPFColorsArrayName()));
  setReferenceDir(reader->readFloatVec3("ReferenceDir", getReferenceDir()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::initialize()
{
  m_PhaseWarningCount = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArraypaths;

  std::vector<size_t> cDims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getCellPhasesArrayPath(), cDims);
  if(nullptr != m_CellPhasesPtr.lock())
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArraypaths.push_back(getCellPhasesArrayPath());
  }

  cDims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getCellEulerAnglesArrayPath(), cDims);
  if(nullptr != m_CellEulerAnglesPtr.lock())
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArraypaths.push_back(getCellEulerAnglesArrayPath());
  }

  cDims[0] = 1;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>>(this, getCrystalStructuresArrayPath(), cDims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(m_CellEulerAnglesArrayPath.getDataContainerName(), getCellEulerAnglesArrayPath().getAttributeMatrixName(), getCellIPFColorsArrayName());
  m_CellIPFColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>>(this, tempPath, 0, cDims, "", DataArrayID31);
  if(nullptr != m_CellIPFColorsPtr.lock())
  {
    m_CellIPFColors = m_CellIPFColorsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getUseGoodVoxels())
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    cDims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>>(this, getGoodVoxelsArrayPath(), cDims);
    if(nullptr != m_GoodVoxelsPtr.lock())
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArraypaths.push_back(getGoodVoxelsArrayPath());
    }
  }
  else
  {
    m_GoodVoxels = nullptr;
  }

  getDataContainerArray()->validateNumberOfTuples(this, dataArraypaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::execute()
{
  initialize();
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  m_PhaseWarningCount = 0;
  size_t totalPoints = m_CellEulerAnglesPtr.lock()->getNumberOfTuples();

  int32_t numPhases = static_cast<int32_t>(m_CrystalStructuresPtr.lock()->getNumberOfTuples());

  // Make sure we are dealing with a unit 1 vector.
  FloatVec3Type normRefDir = m_ReferenceDir; // Make a copy of the reference Direction
  MatrixMath::Normalize3x1(normRefDir[0], normRefDir[1], normRefDir[2]);

  // Allow data-based parallelization
  ParallelDataAlgorithm dataAlg;
  dataAlg.setRange(0, totalPoints);
  dataAlg.execute(GenerateIPFColorsImpl(this, normRefDir, m_CellEulerAngles, m_CellPhases, m_CrystalStructures, numPhases, m_GoodVoxels, m_CellIPFColors));

  if(m_PhaseWarningCount > 0)
  {
    QString ss = QObject::tr("The Ensemble Phase information only references %2 phase(s) but %1 cell(s) had a phase value greater than %2. \
This indicates a problem with the input cell phase data. DREAM.3D will give INCORRECT RESULTS.")
                     .arg(m_PhaseWarningCount)
                     .arg(numPhases - 1);
    setErrorCondition(-48000, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::incrementPhaseWarningCount()
{
  ++m_PhaseWarningCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateIPFColors::newFilterInstance(bool copyFilterParameters) const
{
  GenerateIPFColors::Pointer filter = GenerateIPFColors::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIPFColors::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIPFColors::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIPFColors::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIPFColors::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateIPFColors::getUuid() const
{
  return QUuid("{a50e6532-8075-5de5-ab63-945feb0de7f7}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIPFColors::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIPFColors::getHumanLabel() const
{
  return "Generate IPF Colors";
}

// -----------------------------------------------------------------------------
GenerateIPFColors::Pointer GenerateIPFColors::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateIPFColors> GenerateIPFColors::New()
{
  struct make_shared_enabler : public GenerateIPFColors
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateIPFColors::getNameOfClass() const
{
  return QString("GenerateIPFColors");
}

// -----------------------------------------------------------------------------
QString GenerateIPFColors::ClassName()
{
  return QString("GenerateIPFColors");
}

// -----------------------------------------------------------------------------
void GenerateIPFColors::setReferenceDir(const FloatVec3Type& value)
{
  m_ReferenceDir = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type GenerateIPFColors::getReferenceDir() const
{
  return m_ReferenceDir;
}

// -----------------------------------------------------------------------------
void GenerateIPFColors::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateIPFColors::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateIPFColors::setCellEulerAnglesArrayPath(const DataArrayPath& value)
{
  m_CellEulerAnglesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateIPFColors::getCellEulerAnglesArrayPath() const
{
  return m_CellEulerAnglesArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateIPFColors::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateIPFColors::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateIPFColors::setUseGoodVoxels(bool value)
{
  m_UseGoodVoxels = value;
}

// -----------------------------------------------------------------------------
bool GenerateIPFColors::getUseGoodVoxels() const
{
  return m_UseGoodVoxels;
}

// -----------------------------------------------------------------------------
void GenerateIPFColors::setGoodVoxelsArrayPath(const DataArrayPath& value)
{
  m_GoodVoxelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateIPFColors::getGoodVoxelsArrayPath() const
{
  return m_GoodVoxelsArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateIPFColors::setCellIPFColorsArrayName(const QString& value)
{
  m_CellIPFColorsArrayName = value;
}

// -----------------------------------------------------------------------------
QString GenerateIPFColors::getCellIPFColorsArrayName() const
{
  return m_CellIPFColorsArrayName;
}
