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
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/CubicLowOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalLowOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/OrientationOps/MonoclinicOps.h"
#include "DREAM3DLib/OrientationOps/TriclinicOps.h"
#include "DREAM3DLib/Utilities/ColorTable.h"



class GenerateMisorientationColorsImpl
{

  public:
    GenerateMisorientationColorsImpl(FloatVec3Widget_t referenceAxis, float refAngle, QuatF* quats, int32_t* phases, unsigned int* crystalStructures,
                                     bool* goodVoxels, uint8_t* notSupported, uint8_t* colors) :
      m_ReferenceAxis(referenceAxis),
      m_ReferenceAngle(refAngle*M_PI/180.0f),
      m_Quats(quats),
      m_CellPhases(phases),
      m_CrystalStructures(crystalStructures),
      m_GoodVoxels(goodVoxels),
      m_NotSupported(notSupported),
      m_MisorientationColor(colors)
    {}
    virtual ~GenerateMisorientationColorsImpl(){}

    void convert(size_t start, size_t end) const
    {
      std::vector<OrientationOps::Pointer> ops = OrientationOps::getOrientationOpsVector();

      QuatF refQuat = {m_ReferenceAxis.x * sin(m_ReferenceAngle), m_ReferenceAxis.y * sin(m_ReferenceAngle), m_ReferenceAxis.z * sin(m_ReferenceAngle), cos(m_ReferenceAngle)};
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

        if(m_CrystalStructures[phase] != Ebsd::CrystalStructure::Cubic_High)
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
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    FloatVec3Widget_t  m_ReferenceAxis;
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
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_MisorientationColorArrayName(DREAM3D::CellData::MisorientationColor),
  m_CellPhases(NULL),
  m_Quats(NULL),
  m_CrystalStructures(NULL),
  m_MisorientationColor(NULL)
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
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();

    option->setHumanLabel("Reference Orientation Axis");
    option->setPropertyName("ReferenceAxis");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Reference Orientation Angle");
    option->setPropertyName("ReferenceAngle");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Degrees");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMisorientationColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  setReferenceAxis( reader->readValue( "ReferenceAxis", getReferenceAxis() ) );
  setReferenceAngle( reader->readValue( "ReferenceAngle", getReferenceAngle() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateMisorientationColors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->openFilterGroup(this, index);
  writer->writeValue("ReferenceAxis", getReferenceAxis() );
  writer->writeValue("ReferenceAngle", getReferenceAngle() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMisorientationColors::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if (NULL == m)
  {
    std::stringstream ss;
    ss << getHumanLabel() << "The VolumeDataContainer was NULL and this is NOT allowed. There is an error in the programming. Please contact the developers";
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    return;
  }

  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType,  voxels, 1)
      GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -303, float, FloatArrayType, voxels, 4)

      typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)

      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, MisorientationColor, ss, uint8_t, UInt8ArrayType, 0, voxels, 3)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMisorientationColors::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateMisorientationColors::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  bool* goodVoxels = NULL;

  IDataArray::Pointer gvPtr = m->getCellData(m_GoodVoxelsArrayName);

  // Check to see if we have "GoodVoxels" or not
  if (gvPtr.get() != NULL)
  {
    BoolArrayType::Pointer goodVoxelsPtr = boost::dynamic_pointer_cast<BoolArrayType>(gvPtr);
    goodVoxels = goodVoxelsPtr->GetPointer(0);
  }


  // Make sure we are dealing with a unit 1 vector.
  FloatVec3Widget_t normRefDir = m_ReferenceAxis; // Make a copy of the reference Direction

  MatrixMath::Normalize3x1(normRefDir.x, normRefDir.y, normRefDir.z);
  // Create 1 of every type of Ops class. This condenses the code below
  UInt8ArrayType::Pointer notSupported = UInt8ArrayType::CreateArray(13, 1, "NotSupportedArray");
  notSupported->initializeWithZeros();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      GenerateMisorientationColorsImpl( normRefDir, m_ReferenceAngle, reinterpret_cast<QuatF*>(m_Quats), m_CellPhases, m_CrystalStructures, goodVoxels, notSupported->GetPointer(0), m_MisorientationColor), tbb::auto_partitioner());

  }
  else
#endif
  {
    GenerateMisorientationColorsImpl serial( normRefDir, m_ReferenceAngle, reinterpret_cast<QuatF*>(m_Quats), m_CellPhases, m_CrystalStructures, goodVoxels, notSupported->GetPointer(0), m_MisorientationColor);
    serial.convert(0, totalPoints);
  }

   std::vector<OrientationOps::Pointer> ops = OrientationOps::getOrientationOpsVector();

  // Check and warn about unsupported crystal symmetries in the computation which will show as black
  for(size_t i = 0; i < notSupported->GetNumberOfTuples() - 1; i++)
  {
    if (notSupported->GetValue(i) == 1)
    {
      std::string msg("The Symmetry of ");
      msg.append(ops[i]->getSymmetryName()).append(" is not currently supported for Misorientation Coloring. Voxels with this symmetry have been set to black.");
      notifyWarningMessage(msg, -500);
    }
  }

  // Check for bad voxels which will show up as black also.
  if (notSupported->GetValue(12) == 1)
  {
    std::string msg("There were voxels with an unknown crystal symmetry due most likely being marked as a 'Bad Voxel'. These voxels have been colored black BUT black is a valid color for Misorientation coloring. Please understand this when visualizing your data.");
    notifyWarningMessage(msg, -500);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
