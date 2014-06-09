/* ============================================================================
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ConvertOrientation.h"

#include "DREAM3DLib/Math/OrientationMath.h"

#include "UCSBConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertOrientation::ConvertOrientation() :
  AbstractFilter(),
  m_EulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_RodriguesVectorsArrayName(DREAM3D::CellData::RodriguesVectors),
  m_AxisAnglesArrayName(DREAM3D::CellData::AxisAngles),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_EulerAngles(NULL),
  m_Quats(NULL),
  m_RodriguesVectors(NULL),
  m_AxisAngles(NULL),
  m_AvgQuats(NULL),
  m_OldType(0),
  m_NewType(0),
  m_FieldArray(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertOrientation::~ConvertOrientation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientation::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("From");
    parameter->setPropertyName("OldType");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Euler Angles");
    choices.push_back("Quaternions");
    choices.push_back("Rodrigues Vectors");
    choices.push_back("Axis Angle");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("To");
    parameter->setPropertyName("NewType");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Euler Angles");
    choices.push_back("Quaternions");
    choices.push_back("Rodrigues Vectors");
    choices.push_back("Axis Angle");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Field Array");
    parameter->setPropertyName("FieldArray");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOldType( reader->readValue( "OldType", getOldType() ) );
  setNewType( reader->readValue( "NewType", getNewType() ) );
  setFieldArray( reader->readValue( "FieldArray", getFieldArray() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ConvertOrientation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OldType", getOldType() );
  writer->writeValue("NewType", getNewType() );
  writer->writeValue("FieldArray", getFieldArray() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientation::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_OldType!=m_NewType)
  {
    if(m_FieldArray)//field values
    {
      switch(m_OldType)
      {
        case 0:{//euler angles
          GET_PREREQ_DATA(m, DREAM3D, FieldData, EulerAngles, ss, -302, float, FloatArrayType,  fields, 3)
        } break;

        case 1:{//quats
          GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -302, float, FloatArrayType,  fields, 4)
        } break;

        case 2:{//rods
          GET_PREREQ_DATA(m, DREAM3D, FieldData, RodriguesVectors, ss, -302, float, FloatArrayType,  fields, 3)
        } break;

        case 3:{//axis-angle
          GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisAngles, ss, -302, float, FloatArrayType,  fields, 4)
        } break;
      }
      if (!(getErrorCondition() < 0))
      {
        switch(m_NewType)
        {
          case 0:{//euler angles
            CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, EulerAngles, ss, float, FloatArrayType, 0, fields, 3)
          } break;

          case 1:{//quats
            CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, float, FloatArrayType, 0, fields, 4)
          } break;

          case 2:{//rods
            CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, RodriguesVectors, ss, float, FloatArrayType, 0, fields, 3)
          } break;

          case 3:{//axis-angle
            CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisAngles, ss, float, FloatArrayType, 0, fields, 4)
          } break;
        }
      }
    }
    else//voxel values
    {
      switch(m_OldType)
      {
        case 0:{//euler angles
          GET_PREREQ_DATA(m, DREAM3D, CellData, EulerAngles, ss, -302, float, FloatArrayType,  voxels, 3)
        } break;

        case 1:{//quats
          GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -302, float, FloatArrayType,  voxels, 4)
        } break;

        case 2:{//rods
          GET_PREREQ_DATA(m, DREAM3D, CellData, RodriguesVectors, ss, -302, float, FloatArrayType,  voxels, 3)
        } break;

        case 3:{//axis-angle
          GET_PREREQ_DATA(m, DREAM3D, CellData, AxisAngles, ss, -302, float, FloatArrayType,  voxels, 4)
        } break;
      }
      if (!(getErrorCondition() < 0))
      {
        switch(m_NewType)
        {
          case 0:{//euler angles
            CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, EulerAngles, ss, float, FloatArrayType, 0, voxels, 3)
          } break;

          case 1:{//quats
            CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, float, FloatArrayType, 0, voxels, 4)
          } break;

          case 2:{//rods
            CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, RodriguesVectors, ss, float, FloatArrayType, 0, voxels, 3)
          } break;

          case 3:{//axis-angle
            CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, AxisAngles, ss, float, FloatArrayType, 0, voxels, 4)
          } break;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientation::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientation::execute()
{
  int32_t err =0;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  if(m_OldType!=m_NewType)
  {
    int numPoints;
    QuatF* quats;
    if(m_FieldArray)
    {
      quats = reinterpret_cast<QuatF*>(m_AvgQuats);
      numPoints = numgrains;
    }
    else
    {
      quats = reinterpret_cast<QuatF*>(m_Quats);
      numPoints = totalPoints;
    }

    switch(m_OldType)
    {
      case 0:{
        switch(m_NewType)
        {
          case 1:{//Euler Angle -> Quats
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::EulertoQuat(quats[i], m_EulerAngles[3*i+0], m_EulerAngles[3*i+1], m_EulerAngles[3*i+2]);
            }
          } break;

          case 2:{//Euler Angle -> Rod
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::EulerToRod(m_EulerAngles[3*i+0], m_EulerAngles[3*i+1], m_EulerAngles[3*i+2], m_RodriguesVectors[3*i+0], m_RodriguesVectors[3*i+1], m_RodriguesVectors[3*i+2]);
            }
          } break;

          case 3:{//Euler Angle -> Axis Angle
            QuatF quat;
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::EulertoQuat(quat, m_EulerAngles[3*i+0], m_EulerAngles[3*i+1], m_EulerAngles[3*i+2]);
              OrientationMath::QuattoAxisAngle(quat, m_AxisAngles[4*i+3], m_AxisAngles[4*i+0], m_AxisAngles[4*i+1] ,m_AxisAngles[4*i+2]);
            }
          } break;
        }
      } break;

      case 1:{
        switch(m_NewType)
        {
          case 0:{//Quats -> Euler Angle
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::QuattoEuler(quats[i], m_EulerAngles[3*i+0], m_EulerAngles[3*i+1], m_EulerAngles[3*i+2]);
            }
          } break;

          case 2:{//Quats -> Rod
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::QuattoRod(quats[i], m_RodriguesVectors[3*i+0], m_RodriguesVectors[3*i+1], m_RodriguesVectors[3*i+2]);
            }
          } break;

          case 3:{//Quats -> Axis Angle
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::QuattoAxisAngle(quats[i], m_AxisAngles[4*i+3], m_AxisAngles[4*i+0], m_AxisAngles[4*i+1] ,m_AxisAngles[4*i+2]);
            }
          } break;
        }
      } break;

      case 2:{
        switch(m_NewType)
        {
          case 0:{//Rod -> Euler Angle
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::RodtoEuler(m_RodriguesVectors[3*i+0], m_RodriguesVectors[3*i+1], m_RodriguesVectors[3*i+2], m_EulerAngles[3*i+0], m_EulerAngles[3*i+1], m_EulerAngles[3*i+2]);
            }
          } break;

          case 1:{//Rod -> Quats
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::RodtoQuat(quats[i], m_RodriguesVectors[3*i+0], m_RodriguesVectors[3*i+1], m_RodriguesVectors[3*i+2]);
            }
          } break;

          case 3:{//Rod -> Axis Angle
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::RodtoAxisAngle(m_RodriguesVectors[3*i+0], m_RodriguesVectors[3*i+1], m_RodriguesVectors[3*i+2], m_AxisAngles[4*i+3], m_AxisAngles[4*i+0], m_AxisAngles[4*i+1] ,m_AxisAngles[4*i+2]);
            }
          } break;
        }
      } break;

      case 3:{
        switch(m_NewType)
        {
          case 0:{//Axis Angle -> Euler Angle
            QuatF quat;
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::AxisAngletoQuat(m_AxisAngles[4*i+3], m_AxisAngles[4*i+0], m_AxisAngles[4*i+1] ,m_AxisAngles[4*i+2], quat);
              OrientationMath::QuattoEuler(quat, m_EulerAngles[3*i+0], m_EulerAngles[3*i+1], m_EulerAngles[3*i+2]);
            }
          } break;

          case 1:{//Axis Angle -> Quats
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::AxisAngletoQuat(m_AxisAngles[4*i+3], m_AxisAngles[4*i+0], m_AxisAngles[4*i+1] ,m_AxisAngles[4*i+2], quats[i]);
            }
          } break;

          case 2:{//Axis Angle -> Rod
            for(int i=0; i<numPoints; i++)
            {
              OrientationMath::AxisAngletoRod(m_AxisAngles[4*i+3], m_AxisAngles[4*i+0], m_AxisAngles[4*i+1] ,m_AxisAngles[4*i+2], m_RodriguesVectors[3*i+0], m_RodriguesVectors[3*i+1], m_RodriguesVectors[3*i+2]);
            }
          } break;
        }
      } break;
    }
  }
  notifyStatusMessage("Complete");
}
