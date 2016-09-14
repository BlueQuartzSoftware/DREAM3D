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

#include "GenerateIPFColors.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Utilities/ColorTable.h"

#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/EbsdConstants.h"


/**
 * @brief The GenerateIPFColorsImpl class implements a threaded algorithm that computes the IPF
 * colors for each element in a geometry
 */
class GenerateIPFColorsImpl
{
  public:
    GenerateIPFColorsImpl(FloatVec3_t referenceDir, float* eulers, int32_t* phases, uint32_t* crystalStructures,
                          bool* goodVoxels, uint8_t* colors) :
      m_ReferenceDir(referenceDir),
      m_CellEulerAngles(eulers),
      m_CellPhases(phases),
      m_CrystalStructures(crystalStructures),
      m_GoodVoxels(goodVoxels),
      m_CellIPFColors(colors)
    {}
    virtual ~GenerateIPFColorsImpl() {}

    void convert(size_t start, size_t end) const
    {
      QVector<SpaceGroupOps::Pointer> ops = SpaceGroupOps::getOrientationOpsQVector();
      double refDir[3] = {m_ReferenceDir.x, m_ReferenceDir.y, m_ReferenceDir.z};
      double dEuler[3] = {0.0, 0.0, 0.0};
      SIMPL::Rgb argb = 0x00000000;
      int32_t phase = 0;
      bool calcIPF = false;
      size_t index = 0;
      for (size_t i = start; i < end; i++)
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
        if (nullptr != m_GoodVoxels) { calcIPF = m_GoodVoxels[i]; }

        if (calcIPF && m_CrystalStructures[phase] < Ebsd::CrystalStructure::LaueGroupEnd)
        {
          argb = ops[m_CrystalStructures[phase]]->generateIPFColor(dEuler, refDir, false);
          m_CellIPFColors[index] = RgbColor::dRed(argb);
          m_CellIPFColors[index + 1] = RgbColor::dGreen(argb);
          m_CellIPFColors[index + 2] = RgbColor::dBlue(argb);
        }
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    FloatVec3_t  m_ReferenceDir;
    float* m_CellEulerAngles;
    int32_t* m_CellPhases;
    unsigned int* m_CrystalStructures;
    bool* m_GoodVoxels;
    uint8_t* m_CellIPFColors;

};

// Include the MOC generated file for this class
#include "moc_GenerateIPFColors.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIPFColors::GenerateIPFColors() :
  AbstractFilter(),
  m_CellPhasesArrayPath("", "", ""),
  m_CellEulerAnglesArrayPath("", "", ""),
  m_CrystalStructuresArrayPath("", "", ""),
  m_UseGoodVoxels(false),
  m_GoodVoxelsArrayPath("", "", ""),
  m_CellIPFColorsArrayName(SIMPL::CellData::IPFColor),
  m_CellPhases(nullptr),
  m_CellEulerAngles(nullptr),
  m_CrystalStructures(nullptr),
  m_GoodVoxels(nullptr),
  m_CellIPFColors(nullptr)
{
  m_ReferenceDir.x = 0.0f;
  m_ReferenceDir.y = 0.0f;
  m_ReferenceDir.z = 1.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIPFColors::~GenerateIPFColors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Reference Direction", ReferenceDir, FilterParameter::Parameter, GenerateIPFColors));

  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Apply to Good Elements Only (Bad Elements Will Be Black)", UseGoodVoxels, FilterParameter::Parameter, GenerateIPFColors, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, SIMPL::AttributeMatrixObjectType::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::RequiredArray, GenerateIPFColors, req));
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixObjectType::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, GenerateIPFColors, req));
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, SIMPL::AttributeMatrixObjectType::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, GenerateIPFColors, req));

  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, SIMPL::AttributeMatrixObjectType::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, GenerateIPFColors, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("IPF Colors", CellIPFColorsArrayName, FilterParameter::CreatedArray, GenerateIPFColors));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setCellIPFColorsArrayName(reader->readString("CellIPFColorsArrayName", getCellIPFColorsArrayName() ) );
  setReferenceDir( reader->readFloatVec3("ReferenceDir", getReferenceDir() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArraypaths;

  QVector<size_t> cDims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( nullptr != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArraypaths.push_back(getCellPhasesArrayPath()); }

  cDims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( nullptr != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArraypaths.push_back(getCellEulerAnglesArrayPath()); }

  cDims[0] = 1;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( nullptr != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(m_CellEulerAnglesArrayPath.getDataContainerName(), getCellEulerAnglesArrayPath().getAttributeMatrixName(), getCellIPFColorsArrayName() );
  m_CellIPFColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( nullptr != m_CellIPFColorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  { m_CellIPFColors = m_CellIPFColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (getUseGoodVoxels() == true)
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    cDims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( nullptr != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArraypaths.push_back(getGoodVoxelsArrayPath()); }
  }
  else
  {
    m_GoodVoxels = nullptr;
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArraypaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::preflight()
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
void GenerateIPFColors::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalPoints = m_CellEulerAnglesPtr.lock()->getNumberOfTuples();

  // Make sure we are dealing with a unit 1 vector.
  FloatVec3_t normRefDir = m_ReferenceDir; // Make a copy of the reference Direction
  MatrixMath::Normalize3x1(normRefDir.x, normRefDir.y, normRefDir.z);

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      GenerateIPFColorsImpl(normRefDir, m_CellEulerAngles, m_CellPhases, m_CrystalStructures, m_GoodVoxels, m_CellIPFColors), tbb::auto_partitioner());
  }
  else
#endif
  {
    GenerateIPFColorsImpl serial(normRefDir, m_CellEulerAngles, m_CellPhases, m_CrystalStructures, m_GoodVoxels, m_CellIPFColors);
    serial.convert(0, totalPoints);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateIPFColors::newFilterInstance(bool copyFilterParameters)
{
  GenerateIPFColors::Pointer filter = GenerateIPFColors::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateIPFColors::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateIPFColors::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateIPFColors::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateIPFColors::getGroupName()
{ return SIMPL::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateIPFColors::getSubGroupName()
{ return SIMPL::FilterSubGroups::CrystallographyFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateIPFColors::getHumanLabel()
{ return "Generate IPF Colors"; }
