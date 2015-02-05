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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "GenerateMisorientationColors.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include "DREAM3DLib/Math/MatrixMath.h"
#include "OrientationLib/Math/OrientationMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Utilities/ColorTable.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"

#include "Generic/GenericConstants.h"


class GenerateMisorientationColorsImpl
{

  public:
    GenerateMisorientationColorsImpl(FloatVec3_t referenceAxis, float refAngle, QuatF* quats, int32_t* phases, unsigned int* crystalStructures,
                                     bool* goodVoxels, uint8_t* notSupported, uint8_t* colors) :
      m_ReferenceAxis(referenceAxis),
      m_ReferenceAngle(refAngle),
      m_Quats(quats),
      m_CellPhases(phases),
      m_CrystalStructures(crystalStructures),
      m_GoodVoxels(goodVoxels),
      m_NotSupported(notSupported),
      m_MisorientationColor(colors)
    {}
    virtual ~GenerateMisorientationColorsImpl() {}

    void convert(size_t start, size_t end) const
    {
      QVector<OrientationOps::Pointer> ops = OrientationOps::getOrientationOpsQVector();

      QuatF refQuat = {m_ReferenceAxis.x * sinf(m_ReferenceAngle), m_ReferenceAxis.y * sinf(m_ReferenceAngle), m_ReferenceAxis.z * sinf(m_ReferenceAngle), cosf(m_ReferenceAngle)};
      QuatF cellQuat = {0.0f, 0.0f, 0.0f, 1.0f};
      DREAM3D::Rgb argb = 0x00000000;

      bool missingGoodVoxels = false;
      if(NULL == m_GoodVoxels)
      {
        missingGoodVoxels = true;
      }
      int phase = 0;
      size_t index = 0;
      for (size_t i = start; i < end; i++)
      {
        phase = m_CellPhases[i];
        index = i * 3;
        m_MisorientationColor[index] = 0;
        m_MisorientationColor[index + 1] = 0;
        m_MisorientationColor[index + 2] = 0;
        cellQuat = m_Quats[i];

        if(m_CrystalStructures[phase] != Ebsd::CrystalStructure::Cubic_High &&
            m_CrystalStructures[phase] != Ebsd::CrystalStructure::Hexagonal_High)
        {
          uint32_t idx = m_CrystalStructures[phase];
          if(idx == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            idx = 12;
          }
          m_NotSupported[idx] = 1;
          m_MisorientationColor[index] = 0;
          m_MisorientationColor[index + 1] = 0;
          m_MisorientationColor[index + 2] = 0;

        }
        // Make sure we are using a valid Euler Angles with valid crystal symmetry
        else if( (missingGoodVoxels == true || m_GoodVoxels[i] == true)
                 && m_CrystalStructures[phase] < Ebsd::CrystalStructure::LaueGroupEnd )
        {
          argb = ops[m_CrystalStructures[phase]]->generateMisorientationColor(cellQuat, refQuat);
          m_MisorientationColor[index] = RgbColor::dRed(argb);
          m_MisorientationColor[index + 1] = RgbColor::dGreen(argb);
          m_MisorientationColor[index + 2] = RgbColor::dBlue(argb);
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    FloatVec3_t  m_ReferenceAxis;
    float m_ReferenceAngle;
    QuatF* m_Quats;
    int32_t* m_CellPhases;
    unsigned int* m_CrystalStructures;
    bool* m_GoodVoxels;
    uint8_t* m_NotSupported;
    uint8_t* m_MisorientationColor;

};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateMisorientationColors::GenerateMisorientationColors() :
  AbstractFilter(),
  m_ReferenceAngle(0.0f),
  m_CellPhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_QuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_MisorientationColorArrayName(DREAM3D::CellData::MisorientationColor),
  m_UseGoodVoxels(false),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL),
  m_MisorientationColor(NULL),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL)
{
  m_ReferenceAxis.x = 0.0f;
  m_ReferenceAxis.y = 0.0f;
  m_ReferenceAxis.z = 1.0f;
  m_ReferenceAngle = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateMisorientationColors::~GenerateMisorientationColors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMisorientationColors::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Reference Orientation Axis", "ReferenceAxis", FilterParameterWidgetType::FloatVec3Widget, getReferenceAxis(), false));
  parameters.push_back(FilterParameter::New("Reference Orientation Angle", "ReferenceAngle", FilterParameterWidgetType::DoubleWidget, getReferenceAngle(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Quats", "QuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Optional Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Apply to Good Voxels Only (Bad Voxels Will Be Black)", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, true));
  parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGoodVoxelsArrayPath(), true, ""));

  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("MisorientationColor", "MisorientationColorArrayName", FilterParameterWidgetType::StringWidget, getMisorientationColorArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMisorientationColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setMisorientationColorArrayName(reader->readString("MisorientationColorArrayName", getMisorientationColorArrayName() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setReferenceAxis( reader->readFloatVec3("ReferenceAxis", getReferenceAxis() ) );
  setReferenceAngle( reader->readValue("ReferenceAngle", getReferenceAngle() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateMisorientationColors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  DREAM3D_FILTER_WRITE_PARAMETER(MisorientationColorArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceAxis)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceAngle)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMisorientationColors::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  //typedef DataArray<unsigned int> XTalStructArrayType;
  dims[0] = 1;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  tempPath.update(m_CellPhasesArrayPath.getDataContainerName(), m_CellPhasesArrayPath.getAttributeMatrixName(), getMisorientationColorArrayName() );
  m_MisorientationColorPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MisorientationColorPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MisorientationColor = m_MisorientationColorPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
  dims[0] = 1;
  if (getUseGoodVoxels() == true)
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    dims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else
  {
    m_GoodVoxels = NULL;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMisorientationColors::preflight()
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
void GenerateMisorientationColors::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  int64_t totalPoints = m_CellPhasesPtr.lock()->getNumberOfTuples();

//  bool missingGoodVoxels = true;
//  if (NULL != m_GoodVoxels)
//  {
//    missingGoodVoxels = false;
//  }

  // Make sure we are dealing with a unit 1 vector.
  FloatVec3_t normRefDir = m_ReferenceAxis; // Make a copy of the reference Direction

  MatrixMath::Normalize3x1(normRefDir.x, normRefDir.y, normRefDir.z);
  // Create 1 of every type of Ops class. This condenses the code below
  UInt8ArrayType::Pointer notSupported = UInt8ArrayType::CreateArray(13, "NotSupportedArray");
  notSupported->initializeWithZeros();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      GenerateMisorientationColorsImpl( normRefDir, m_ReferenceAngle, reinterpret_cast<QuatF*>(m_Quats), m_CellPhases, m_CrystalStructures, m_GoodVoxels, notSupported->getPointer(0), m_MisorientationColor), tbb::auto_partitioner());

  }
  else
#endif
  {
    GenerateMisorientationColorsImpl serial( normRefDir, m_ReferenceAngle, reinterpret_cast<QuatF*>(m_Quats), m_CellPhases, m_CrystalStructures, m_GoodVoxels, notSupported->getPointer(0), m_MisorientationColor);
    serial.convert(0, totalPoints);
  }

  QVector<OrientationOps::Pointer> ops = OrientationOps::getOrientationOpsQVector();

  // Check and warn about unsupported crystal symmetries in the computation which will show as black
  for(size_t i = 0; i < notSupported->getNumberOfTuples() - 1; i++)
  {
    if (notSupported->getValue(i) == 1)
    {
      QString msg("The Symmetry of ");
      msg.append(ops[i]->getSymmetryName()).append(" is not currently supported for Misorientation Coloring. Voxels with this symmetry have been set to black.");
      notifyWarningMessage(getHumanLabel(), msg, -5000);
    }
  }

  // Check for bad voxels which will show up as black also.
  if (notSupported->getValue(12) == 1)
  {
    QString msg("There were voxels with an unknown crystal symmetry due most likely being marked as a 'Bad Voxel'. These voxels have been colored black BUT black is a valid color for Misorientation coloring. Please understand this when visualizing your data.");
    notifyWarningMessage(getHumanLabel(), msg, -5001);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateMisorientationColors::newFilterInstance(bool copyFilterParameters)
{
  GenerateMisorientationColors::Pointer filter = GenerateMisorientationColors::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateMisorientationColors::getCompiledLibraryName()
{ return Generic::GenericBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateMisorientationColors::getGroupName()
{ return DREAM3D::FilterGroups::GenericFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateMisorientationColors::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CrystallographyFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateMisorientationColors::getHumanLabel()
{ return "Generate Misorientation Colors"; }

