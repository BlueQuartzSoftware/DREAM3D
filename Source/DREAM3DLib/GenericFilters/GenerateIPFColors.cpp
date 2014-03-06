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
#include "GenerateIPFColors.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/ColorTable.h"


class GenerateIPFColorsImpl
{

  public:
    GenerateIPFColorsImpl(FloatVec3Widget_t referenceDir, float* eulers, int32_t* phases, unsigned int* crystalStructures,
                          bool* goodVoxels, uint8_t* colors) :
      m_ReferenceDir(referenceDir),
      m_CellEulerAngles(eulers),
      m_CellPhases(phases),
      m_CrystalStructures(crystalStructures),
      m_GoodVoxels(goodVoxels),
      m_CellIPFColors(colors)
    {}
    virtual ~GenerateIPFColorsImpl(){}

    void convert(size_t start, size_t end) const
    {
      std::vector<OrientationOps::Pointer> ops = OrientationOps::getOrientationOpsVector();
      double refDir[3] = {m_ReferenceDir.x, m_ReferenceDir.y, m_ReferenceDir.z};
      double dEuler[3] = {0.0, 0.0, 0.0};
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
        m_CellIPFColors[index] = 0;
        m_CellIPFColors[index + 1] = 0;
        m_CellIPFColors[index + 2] = 0;
        dEuler[0] = m_CellEulerAngles[index];
        dEuler[1] = m_CellEulerAngles[index + 1];
        dEuler[2] = m_CellEulerAngles[index + 2];

        // Make sure we are using a valid Euler Angles with valid crystal symmetry
        if( (missingGoodVoxels == true || m_GoodVoxels[i] == true)
            && m_CrystalStructures[phase] < Ebsd::CrystalStructure::LaueGroupEnd )
        {
          argb = ops[m_CrystalStructures[phase]]->generateIPFColor(dEuler, refDir, false);
          m_CellIPFColors[index] = RgbColor::dRed(argb);
          m_CellIPFColors[index + 1] = RgbColor::dGreen(argb);
          m_CellIPFColors[index + 2] = RgbColor::dBlue(argb);
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
    FloatVec3Widget_t  m_ReferenceDir;
    float* m_CellEulerAngles;
    int32_t* m_CellPhases;
    unsigned int* m_CrystalStructures;
    bool* m_GoodVoxels;
    uint8_t* m_CellIPFColors;

};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIPFColors::GenerateIPFColors() :
  AbstractFilter(),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CellIPFColorsArrayName(DREAM3D::CellData::IPFColor),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL),
  m_CellIPFColors(NULL)
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
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();

    option->setHumanLabel("Reference Direction");
    option->setPropertyName("ReferenceDir");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setReferenceDir(reader->readValue("ReferenceDir", getReferenceDir() ));
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateIPFColors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->openFilterGroup(this, index);
  writer->writeValue("ReferenceDir", getReferenceDir() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
      GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -300, float, FloatArrayType, voxels, 3)
      typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellIPFColors, ss, uint8_t, UInt8ArrayType, 0, voxels, 3)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFColors::execute()
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

  //std::cout << "Start Time: " << MXA::convertMillisToHrsMinSecs(MXA::getMilliSeconds()) << std::endl;

  bool* goodVoxels = NULL;

  IDataArray::Pointer gvPtr = m->getCellData(m_GoodVoxelsArrayName);

  // Check to see if we have "GoodVoxels" or not
  if (gvPtr.get() != NULL)
  {
    BoolArrayType::Pointer goodVoxelsPtr = boost::dynamic_pointer_cast<BoolArrayType>(gvPtr);
    goodVoxels = goodVoxelsPtr->GetPointer(0);
  }


  // Make sure we are dealing with a unit 1 vector.
  FloatVec3Widget_t normRefDir = m_ReferenceDir; // Make a copy of the reference Direction

  MatrixMath::Normalize3x1(normRefDir.x, normRefDir.y, normRefDir.z);
  // Create 1 of every type of Ops class. This condenses the code below

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      GenerateIPFColorsImpl(normRefDir, m_CellEulerAngles, m_CellPhases, m_CrystalStructures, goodVoxels, m_CellIPFColors), tbb::auto_partitioner());

  }
  else
#endif
  {
    GenerateIPFColorsImpl serial(normRefDir, m_CellEulerAngles, m_CellPhases, m_CrystalStructures, goodVoxels, m_CellIPFColors);
    serial.convert(0, totalPoints);
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
