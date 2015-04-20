/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "ConvertOrientations.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "OrientationLib/Math/OrientationMath.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

/**
 * @brief The ConvertOrientationsImpl class implements a threaded algorithm to convert the incoming orientation
 * representation to the selected output orientation representation.
 */
class ConvertOrientationsImpl
{

  public:
    ConvertOrientationsImpl(float* inputEulers, float* inputQuats, float* inputRods, float* inputAxisAngles, float* outputEulers, float* outputQuats, float* outputRods, float* outputAxisAngles, int32_t* cellPhases, uint32_t* crystalStructures, int inputType, int outputType) :
      m_InputEulerAngles(inputEulers),
      m_InputQuats(inputQuats),
      m_InputRods(inputRods),
      m_InputAxisAngles(inputAxisAngles),
      m_OutputEulerAngles(outputEulers),
      m_OutputQuats(outputQuats),
      m_OutputRods(outputRods),
      m_OutputAxisAngles(outputAxisAngles),
      m_CellPhases(cellPhases),
      m_CrystalStructures(crystalStructures),
      m_InputType(inputType),
      m_OutputType(outputType)
    {}
    virtual ~ConvertOrientationsImpl() {}

    void convert(size_t start, size_t end) const
    {
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // quats -> ?
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      if ( 0 == m_InputType && 1 == m_OutputType ) // euler angles->quats
      {
        QuatF* quats = reinterpret_cast<QuatF*>(m_OutputQuats);
        QuatF q = QuaternionMathF::New();
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            QuaternionMathF::Identity(q);
          }
          else
          {
            OrientationMath::EulertoQuat( m_InputEulerAngles[3 * i], m_InputEulerAngles[3 * i + 1], m_InputEulerAngles[3 * i + 2], q );
            QuaternionMathF::UnitQuaternion(q);
          }
          QuaternionMathF::Copy(q, quats[i]);
        }
      }
      else if ( 0 == m_InputType && 2 == m_OutputType ) // euler angles->rods
      {
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            m_OutputRods[3 * i] = 0;
            m_OutputRods[3 * i + 1] = 0;
            m_OutputRods[3 * i + 2] = 0;
          }
          else
          {
            OrientationMath::EulertoRod( m_InputEulerAngles[3 * i], m_InputEulerAngles[3 * i + 1], m_InputEulerAngles[3 * i + 2], m_OutputRods[3 * i], m_OutputRods[3 * i + 1], m_OutputRods[3 * i + 2] );
          }
        }
      }
      else if ( 0 == m_InputType && 3 == m_OutputType ) // euler angles->axis angles
      {
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            m_OutputAxisAngles[4 * i] = 0;
            m_OutputAxisAngles[4 * i + 1] = 0;
            m_OutputAxisAngles[4 * i + 2] = 0;
            m_OutputAxisAngles[4 * i + 3] = 0;
          }
          else
          {
            OrientationMath::EulerToAxisAngle( m_InputEulerAngles[3 * i], m_InputEulerAngles[3 * i + 1], m_InputEulerAngles[3 * i + 2], m_OutputAxisAngles[4 * i], m_OutputAxisAngles[4 * i + 1], m_OutputAxisAngles[4 * i + 2], m_OutputAxisAngles[4 * i + 3] );
          }
        }
      }
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // quats -> ?
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      else if ( 1 == m_InputType && 0 == m_OutputType ) // quats->euler angles
      {
        QuatF* cellQuats = reinterpret_cast<QuatF*>(m_InputQuats);
        QuatF q = QuaternionMathF::New();
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            m_OutputEulerAngles[3 * i] = 0;
            m_OutputEulerAngles[3 * i + 1] = 0;
            m_OutputEulerAngles[3 * i + 2] = 0;
          }
          else
          {
            QuaternionMathF::Copy(cellQuats[i], q);
            OrientationMath::QuattoEuler(q, m_OutputEulerAngles[3 * i], m_OutputEulerAngles[3 * i + 1], m_OutputEulerAngles[3 * i + 2] );
          }
        }
      }
      else if ( 1 == m_InputType && 2 == m_OutputType ) // quats->rods
      {
        QuatF* cellQuats = reinterpret_cast<QuatF*>(m_InputQuats);
        QuatF q = QuaternionMathF::New();
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            m_OutputRods[3 * i] = 0;
            m_OutputRods[3 * i + 1] = 0;
            m_OutputRods[3 * i + 2] = 0;
          }
          else
          {
            QuaternionMathF::Copy(cellQuats[i], q);
            OrientationMath::QuattoRod(q, m_OutputRods[3 * i], m_OutputRods[3 * i + 1], m_OutputRods[3 * i + 2] );
          }
        }
      }
      else if ( 1 == m_InputType && 3 == m_OutputType ) // quats->axis angles
      {
        QuatF* cellQuats = reinterpret_cast<QuatF*>(m_InputQuats);
        QuatF q = QuaternionMathF::New();
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            m_OutputAxisAngles[4 * i] = 0;
            m_OutputAxisAngles[4 * i + 1] = 0;
            m_OutputAxisAngles[4 * i + 2] = 0;
            m_OutputAxisAngles[4 * i + 3] = 0;
          }
          else
          {
            QuaternionMathF::Copy(cellQuats[i], q);
            OrientationMath::QuattoAxisAngle(q, m_OutputAxisAngles[4 * i], m_OutputAxisAngles[4 * i + 1], m_OutputAxisAngles[4 * i + 2], m_OutputAxisAngles[4 * i + 3] );
          }
        }
      }
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // rods -> ?
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      else if( 2 == m_InputType && 0 == m_OutputType ) // rods->euler angles
      {
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            m_OutputEulerAngles[3 * i] = 0;
            m_OutputEulerAngles[3 * i + 1] = 0;
            m_OutputEulerAngles[3 * i + 2] = 0;
          }
          else
          {
            OrientationMath::RodtoEuler( m_InputRods[3 * i], m_InputRods[3 * i + 1], m_InputRods[3 * i + 2], m_OutputEulerAngles[3 * i], m_OutputEulerAngles[3 * i + 1], m_OutputEulerAngles[3 * i + 2] );
          }
        }
      }
      else if ( 2 == m_InputType && 1 == m_OutputType ) // rods->quats
      {
        QuatF* quats = reinterpret_cast<QuatF*>(m_OutputQuats);
        QuatF q = QuaternionMathF::New();
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            QuaternionMathF::Identity(q);
          }
          else
          {
            OrientationMath::RodtoQuat( q, m_InputRods[3 * i], m_InputRods[3 * i + 1], m_InputRods[3 * i + 2] );
            QuaternionMathF::UnitQuaternion(q);
          }
          QuaternionMathF::Copy(q, quats[i]);
        }
      }
      else if ( 2 == m_InputType && 3 == m_OutputType ) // rods->axis angles
      {
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            m_OutputAxisAngles[4 * i] = 0;
            m_OutputAxisAngles[4 * i + 1] = 0;
            m_OutputAxisAngles[4 * i + 2] = 0;
            m_OutputAxisAngles[4 * i + 3] = 0;
          }
          else
          {
            OrientationMath::RodtoAxisAngle( m_InputRods[3 * i], m_InputRods[3 * i + 1], m_InputRods[3 * i + 2], m_OutputAxisAngles[4 * i], m_OutputAxisAngles[4 * i + 1], m_OutputAxisAngles[4 * i + 2], m_OutputAxisAngles[4 * i + 3] );
          }
        }
      }
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // axis angles -> ?
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      else if ( 3 == m_InputType && 0 == m_OutputType ) // axis angles->euler angles (currently unsupported)
      {

      }
      else if ( 3 == m_InputType && 1 == m_OutputType ) // axis angles->quats
      {
        QuatF* quats = reinterpret_cast<QuatF*>(m_OutputQuats);
        QuatF q = QuaternionMathF::New();
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            QuaternionMathF::Identity(q);
          }
          else
          {
            OrientationMath::AxisAngletoQuat( m_InputAxisAngles[4 * i], m_InputAxisAngles[4 * i + 1], m_InputAxisAngles[4 * i + 2], m_InputAxisAngles[4 * i + 3], q );
            QuaternionMathF::UnitQuaternion(q);
          }
          QuaternionMathF::Copy(q, quats[i]);
        }
      }
      else if ( 3 == m_InputType && 2 == m_OutputType ) // axis angles->rods
      {
        for (size_t i = start; i < end; i++)
        {
          if (m_CrystalStructures[m_CellPhases[i]] == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            m_OutputRods[3 * i] = 0;
            m_OutputRods[3 * i + 1] = 0;
            m_OutputRods[3 * i + 2] = 0;
          }
          else
          {
            OrientationMath::AxisAngletoRod( m_InputAxisAngles[4 * i], m_InputAxisAngles[4 * i + 1], m_InputAxisAngles[4 * i + 2], m_InputAxisAngles[4 * i + 3], m_OutputRods[3 * i], m_OutputRods[3 * i + 1], m_OutputRods[3 * i + 2] );
          }
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
    float* m_InputEulerAngles;
    float* m_InputQuats;
    float* m_InputRods;
    float* m_InputAxisAngles;
    float* m_OutputEulerAngles;
    float* m_OutputQuats;
    float* m_OutputRods;
    float* m_OutputAxisAngles;
    int32_t* m_CellPhases;
    uint32_t* m_CrystalStructures;
    int m_InputType;
    int m_OutputType;

};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertOrientations::ConvertOrientations() :
  AbstractFilter(),
  m_InputType(0),
  m_OutputType(1),
  m_CellEulerAnglesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::EulerAngles),
  m_CellQuatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_CellRodriguesVectorsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::RodriguesVectors),
  m_CellAxisAnglesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::AxisAngles),
  m_CellPhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_EulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_RodriguesVectorsArrayName(DREAM3D::CellData::RodriguesVectors),
  m_AxisAnglesArrayName(DREAM3D::CellData::AxisAngles),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_EulerAngles(NULL),
  m_Quats(NULL),
  m_RodriguesVectors(NULL),
  m_AxisAngles(NULL),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL),
  m_CellQuatsArrayName(DREAM3D::CellData::Quats),
  m_CellQuats(NULL),
  m_CellRodriguesVectorsArrayName(DREAM3D::CellData::RodriguesVectors),
  m_CellAxisAnglesArrayName(DREAM3D::CellData::AxisAngles),
  m_CellAxisAngles(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsQVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertOrientations::~ConvertOrientations()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::setupFilterParameters()
{
  FilterParameterVector parameters;
  QVector<QString> choices;
  choices.push_back(DREAM3D::CellData::EulerAngles);
  choices.push_back(DREAM3D::CellData::Quats);
  choices.push_back(DREAM3D::CellData::RodriguesVectors);
  choices.push_back(DREAM3D::CellData::AxisAngles);

  LinkedChoicesFilterParameter::Pointer input = LinkedChoicesFilterParameter::New();
  input->setHumanLabel("Input Orientation Representation");
  input->setPropertyName("InputType");
  input->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
  input->setChoices(choices);
  QStringList inputLinkedProps;
  inputLinkedProps << "CellEulerAnglesArrayPath" << "CellQuatsArrayPath" << "CellRodriguesVectorsArrayPath" << "CellAxisAnglesArrayPath";
  input->setLinkedProperties(inputLinkedProps);
  parameters.push_back(input);

  LinkedChoicesFilterParameter::Pointer output = LinkedChoicesFilterParameter::New();
  output->setHumanLabel("Output Orientation Representation");
  output->setPropertyName("OutputType");
  output->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
  output->setChoices(choices);
  QStringList outputLinkedProps;
  outputLinkedProps << "EulerAnglesArrayName" << "QuatsArrayName" << "RodriguesVectorsArrayName" << "AxisAnglesArrayName";
  output->setLinkedProperties(outputLinkedProps);
  parameters.push_back(output);

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Euler Angles", "CellEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellEulerAnglesArrayPath(), true, "", 0));
  parameters.push_back(FilterParameter::New("Quaternions", "CellQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellQuatsArrayPath(), true, "", 1));
  parameters.push_back(FilterParameter::New("Rodrigues Vectors", "CellRodriguesVectorsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellRodriguesVectorsArrayPath(), true, "", 2));
  parameters.push_back(FilterParameter::New("Axis Angles", "CellAxisAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellAxisAnglesArrayPath(), true, "", 3));
  parameters.push_back(FilterParameter::New("Phases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Euler Angles", "EulerAnglesArrayName", FilterParameterWidgetType::StringWidget, getEulerAnglesArrayName(), true, "", 0));
  parameters.push_back(FilterParameter::New("Quaternions", "QuatsArrayName", FilterParameterWidgetType::StringWidget, getQuatsArrayName(), true, "", 1));
  parameters.push_back(FilterParameter::New("Rodrigues Vectors", "RodriguesVectorsArrayName", FilterParameterWidgetType::StringWidget, getRodriguesVectorsArrayName(), true, "", 2));
  parameters.push_back(FilterParameter::New("Axis Angles", "AxisAnglesArrayName", FilterParameterWidgetType::StringWidget, getAxisAnglesArrayName(), true, "", 3));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ConvertOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputType(reader->readValue("InputType", getInputType() ) );
  setOutputType(reader->readValue("OutputType", getOutputType() ) );
  setEulerAnglesArrayName(reader->readString("EulerAnglesArrayName", getEulerAnglesArrayName() ) );
  setQuatsArrayName(reader->readString("QuatsArrayName", getQuatsArrayName() ) );
  setRodriguesVectorsArrayName(reader->readString("RodriguesVectorsArrayName", getRodriguesVectorsArrayName() ) );
  setAxisAnglesArrayName(reader->readString("AxisAnglesArrayName", getAxisAnglesArrayName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setCellQuatsArrayPath(reader->readDataArrayPath("CellQuatsArrayPath", getCellQuatsArrayPath() ) );
  setCellRodriguesVectorsArrayPath(reader->readDataArrayPath("CellRodriguesVectorsArrayPath", getCellRodriguesVectorsArrayPath() ) );
  setCellAxisAnglesArrayPath(reader->readDataArrayPath("CellAxisAnglesArrayPath", getCellAxisAnglesArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ConvertOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(InputType)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputType)
  DREAM3D_FILTER_WRITE_PARAMETER(EulerAnglesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(RodriguesVectorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(AxisAnglesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellRodriguesVectorsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAxisAnglesArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  if(getInputType() == getOutputType())
  {
    QString ss = QObject::tr("Input and output orientation representation types must be different");
    notifyErrorMessage(getHumanLabel(), ss, -1000);
    setErrorCondition(-1000);
  }

  if(  (0 == getInputType() && 3 == getOutputType()) || (3 == getInputType() && 0 == getOutputType() ) )
  {
    QString ss = QObject::tr("Direct conversion between Axis Angle and Euler Angle is currently not implemented");
    notifyErrorMessage(getHumanLabel(), ss, -1001);
    setErrorCondition(-1001);
  }

  switch(getInputType())
  {
    case 0: // euler angles
    {
      QVector<size_t> dims(1, 3);
      m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), dims);
      if( NULL != m_CellEulerAnglesPtr.lock().get() )
      { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); }
      tempPath.update(getCellEulerAnglesArrayPath().getDataContainerName(), getCellEulerAnglesArrayPath().getAttributeMatrixName(), "" );
    }
    break;

    case 1: // quats
    {
      QVector<size_t> dims(1, 4);
      m_CellQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellQuatsArrayPath(), dims);
      if( NULL != m_CellQuatsPtr.lock().get() )
      { m_CellQuats = m_CellQuatsPtr.lock()->getPointer(0); }
      tempPath.update(getCellQuatsArrayPath().getDataContainerName(), getCellQuatsArrayPath().getAttributeMatrixName(), "" );
    }
    break;

    case 2: // rods
    {
      QVector<size_t> dims(1, 3);
      m_CellRodriguesVectorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellRodriguesVectorsArrayPath(), dims);
      if( NULL != m_CellRodriguesVectorsPtr.lock().get() )
      { m_CellRodriguesVectors = m_CellRodriguesVectorsPtr.lock()->getPointer(0); }
      tempPath.update(getCellRodriguesVectorsArrayPath().getDataContainerName(), getCellRodriguesVectorsArrayPath().getAttributeMatrixName(), "" );
    }
    break;

    case 3: // axis angle
    {
      QVector<size_t> dims(1, 4);
      m_CellAxisAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellAxisAnglesArrayPath(), dims);
      if( NULL != m_CellAxisAnglesPtr.lock().get() )
      { m_CellAxisAngles = m_CellAxisAnglesPtr.lock()->getPointer(0); }
      tempPath.update(getCellAxisAnglesArrayPath().getDataContainerName(), getCellAxisAnglesArrayPath().getAttributeMatrixName(), "" );
    }
    break;
  }

  switch(getOutputType())
  {
    case 0: // euler angles
    {
      QVector<size_t> dims(1, 3);
      tempPath.update(tempPath.getDataContainerName(), tempPath.getAttributeMatrixName(), getEulerAnglesArrayName() );
      m_EulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims);
      if( NULL != m_EulerAnglesPtr.lock().get() )
      { m_EulerAngles = m_EulerAnglesPtr.lock()->getPointer(0); }
      if(getErrorCondition() >= 0) { dataArrayPaths.push_back(tempPath); }
    }
    break;

    case 1: // quats
    {
      QVector<size_t> dims(1, 4);
      tempPath.update(tempPath.getDataContainerName(), tempPath.getAttributeMatrixName(), getQuatsArrayName() );
      m_QuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims);
      if( NULL != m_QuatsPtr.lock().get() )
      { m_Quats = m_QuatsPtr.lock()->getPointer(0); }
      if(getErrorCondition() >= 0) { dataArrayPaths.push_back(tempPath); }
    }
    break;

    case 2: // rods
    {
      QVector<size_t> dims(1, 3);
      tempPath.update(tempPath.getDataContainerName(), tempPath.getAttributeMatrixName(), getRodriguesVectorsArrayName() );
      m_RodriguesVectorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims);
      if( NULL != m_RodriguesVectorsPtr.lock().get() )
      { m_RodriguesVectors = m_RodriguesVectorsPtr.lock()->getPointer(0); }
      if(getErrorCondition() >= 0) { dataArrayPaths.push_back(tempPath); }
    }
    break;

    case 3: // axis angle
    {
      QVector<size_t> dims(1, 4);
      tempPath.update(tempPath.getDataContainerName(), tempPath.getAttributeMatrixName(), getAxisAnglesArrayName() );
      m_AxisAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims);
      if( NULL != m_AxisAnglesPtr.lock().get() )
      { m_AxisAngles = m_AxisAnglesPtr.lock()->getPointer(0); }
      if(getErrorCondition() >= 0) { dataArrayPaths.push_back(tempPath); }
    }
    break;
  }

  // get phase + crystal structure
  QVector<size_t> dims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims);
  if( NULL != m_CellPhasesPtr.lock().get() )
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); }
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCellPhasesArrayPath()); }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims);
  if( NULL != m_CrystalStructuresPtr.lock().get() )
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::preflight()
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
void ConvertOrientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  size_t totalPoints = 0;
  switch(getInputType())
  {
    case 0: // euler angles
    {
      totalPoints = m_CellEulerAnglesPtr.lock()->getNumberOfTuples();
    }
    break;

    case 1: // quats
    {
      totalPoints = m_CellQuatsPtr.lock()->getNumberOfTuples();
    }
    break;

    case 2: // rods
    {
      totalPoints = m_CellRodriguesVectorsPtr.lock()->getNumberOfTuples();
    }
    break;

    case 3: // axis angle
    {
      totalPoints = m_CellAxisAnglesPtr.lock()->getNumberOfTuples();
    }
    break;
    default:
      totalPoints = 0;
  }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      ConvertOrientationsImpl(m_CellEulerAngles, m_CellQuats, m_CellRodriguesVectors, m_CellAxisAngles, m_EulerAngles, m_Quats, m_RodriguesVectors, m_AxisAngles, m_CellPhases, m_CrystalStructures, m_InputType, m_OutputType), tbb::auto_partitioner());
  }
  else
#endif
  {
    ConvertOrientationsImpl serial(m_CellEulerAngles, m_CellQuats, m_CellRodriguesVectors, m_CellAxisAngles, m_EulerAngles, m_Quats, m_RodriguesVectors, m_AxisAngles, m_CellPhases, m_CrystalStructures, m_InputType, m_OutputType);
    serial.convert(0, totalPoints);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertOrientations::newFilterInstance(bool copyFilterParameters)
{
  ConvertOrientations::Pointer filter = ConvertOrientations::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getCompiledLibraryName()
{ return OrientationAnalysis::OrientationAnalysisBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getGroupName()
{ return DREAM3D::FilterGroups::OrientationAnalysisFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getSubGroupName()
{ return DREAM3D::FilterSubGroups::ConversionFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getHumanLabel()
{ return "Convert Orientations"; }

