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
    if(m_FieldArray)
    {
      switch(m_OldType)
      {
        case 0:{
          switch(m_NewType)
          {
            case 1:{//Euler Angle -> Quats
              DataArray<float>* pEA = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::EulerAngles)).get());
              float* ea = pEA->GetPointer(0);
              FloatArrayType::Pointer pQuats = FloatArrayType::CreateArray(numgrains, 4, DREAM3D::FieldData::AvgQuats);
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::EulertoQuat(quats[i], ea[3*i+0], ea[3*i+1], ea[3*i+2]);
              }
              m->addFieldData(pQuats->GetName(), pQuats);
            } break;

            case 2:{//Euler Angle -> Rod
              DataArray<float>* pEA = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::EulerAngles)).get());
              float* ea = pEA->GetPointer(0);
              FloatArrayType::Pointer pRods = FloatArrayType::CreateArray(numgrains, 3, DREAM3D::FieldData::RodriguesVectors);
              float* rods = pRods->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::EulerToRod(ea[3*i+0], ea[3*i+1], ea[3*i+2], rods[3*i+0], rods[3*i+1], rods[3*i+2]);
              }
              m->addFieldData(pRods->GetName(), pRods);
            } break;

            case 3:{//Euler Angle -> Axis Angle
              DataArray<float>* pEA = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::EulerAngles)).get());
              float* ea = pEA->GetPointer(0);
              FloatArrayType::Pointer pAxisAngle = FloatArrayType::CreateArray(numgrains, 4, DREAM3D::FieldData::AxisAngles);
              float* axisAngle = pAxisAngle->GetPointer(0);
              QuatF quat;
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::EulertoQuat(quat, ea[3*i+0], ea[3*i+1], ea[3*i+2]);
                OrientationMath::QuattoAxisAngle(quat, axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2]);
              }
              m->addFieldData(pAxisAngle->GetName(), pAxisAngle);
            } break;
          }
        } break;

        case 1:{
          switch(m_NewType)
          {
            case 0:{//Quats -> Euler Angle
              DataArray<float>* pQuats = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::AvgQuats)).get());
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              FloatArrayType::Pointer pEA = FloatArrayType::CreateArray(numgrains, 3, DREAM3D::FieldData::EulerAngles);
              float* ea = pEA->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::QuattoEuler(quats[i], ea[3*i+0], ea[3*i+1], ea[3*i+2]);
              }
              m->addFieldData(pEA->GetName(), pEA);
            } break;

            case 2:{//Quats -> Rod
              DataArray<float>* pQuats = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::AvgQuats)).get());
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              FloatArrayType::Pointer pRods = FloatArrayType::CreateArray(numgrains, 3, DREAM3D::FieldData::RodriguesVectors);
              float* rods = pRods->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::QuattoRod(quats[i], rods[3*i+0], rods[3*i+1], rods[3*i+2]);
              }
              m->addFieldData(pRods->GetName(), pRods);
            } break;

            case 3:{//Quats -> Axis Angle
              DataArray<float>* pQuats = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::AvgQuats)).get());
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              FloatArrayType::Pointer pAxisAngle = FloatArrayType::CreateArray(numgrains, 4, DREAM3D::FieldData::AxisAngles);
              float* axisAngle = pAxisAngle->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::QuattoAxisAngle(quats[i], axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2]);
              }
              m->addFieldData(pAxisAngle->GetName(), pAxisAngle);
            } break;
          }
        } break;

        case 2:{
          switch(m_NewType)
          {
            case 0:{//Rod -> Euler Angle
              DataArray<float>* pRods = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::RodriguesVectors)).get());
              float* rods = pRods->GetPointer(0);
              FloatArrayType::Pointer pEA = FloatArrayType::CreateArray(numgrains, 3, DREAM3D::FieldData::EulerAngles);
              float* ea = pEA->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::RodtoEuler(rods[3*i+0], rods[3*i+1], rods[3*i+2], ea[3*i+0], ea[3*i+1], ea[3*i+2]);
              }
              m->addFieldData(pEA->GetName(), pEA);
            } break;

            case 1:{//Rod -> Quats
              DataArray<float>* pRods = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::RodriguesVectors)).get());
              float* rods = pRods->GetPointer(0);
              FloatArrayType::Pointer pQuats = FloatArrayType::CreateArray(numgrains, 4, DREAM3D::FieldData::AvgQuats);
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::RodtoQuat(quats[i], rods[3*i+0], rods[3*i+1], rods[3*i+2]);
              }
              m->addFieldData(pQuats->GetName(), pQuats);
            } break;

            case 3:{//Rod -> Axis Angle
              DataArray<float>* pRods = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::RodriguesVectors)).get());
              float* rods = pRods->GetPointer(0);
              FloatArrayType::Pointer pAxisAngle = FloatArrayType::CreateArray(numgrains, 4, DREAM3D::FieldData::AxisAngles);
              float* axisAngle = pAxisAngle->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::RodtoAxisAngle(rods[3*i+0], rods[3*i+1], rods[3*i+2], axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2]);
              }
              m->addFieldData(pAxisAngle->GetName(), pAxisAngle);
            } break;
          }
        } break;

        case 3:{
          switch(m_NewType)
          {
            case 0:{//Axis Angle -> Euler Angle
              DataArray<float>* pAxisAngle = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::AxisAngles)).get());
              float* axisAngle = pAxisAngle->GetPointer(0);
              FloatArrayType::Pointer pEA = FloatArrayType::CreateArray(numgrains, 3, DREAM3D::FieldData::EulerAngles);
              float* ea = pEA->GetPointer(0);
              QuatF quat;
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::AxisAngletoQuat(axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2], quat);
                OrientationMath::QuattoEuler(quat, ea[3*i+0], ea[3*i+1], ea[3*i+2]);
              }
              m->addFieldData(pEA->GetName(), pEA);
            } break;

            case 1:{//Axis Angle -> Quats
              DataArray<float>* pAxisAngle = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::AxisAngles)).get());
              float* axisAngle = pAxisAngle->GetPointer(0);
              FloatArrayType::Pointer pQuats = FloatArrayType::CreateArray(numgrains, 4, DREAM3D::FieldData::AvgQuats);
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::AxisAngletoQuat(axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2], quats[i]);
              }
              m->addFieldData(pQuats->GetName(), pQuats);
            } break;

            case 2:{//Axis Angle -> Rod
              DataArray<float>* pAxisAngle = DataArray<float>::SafePointerDownCast((m->getFieldData(DREAM3D::FieldData::AxisAngles)).get());
              float* axisAngle = pAxisAngle->GetPointer(0);
              FloatArrayType::Pointer pRods = FloatArrayType::CreateArray(numgrains, 3, DREAM3D::FieldData::RodriguesVectors);
              float* rods = pRods->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::AxisAngletoRod(axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2], rods[3*i+0], rods[3*i+1], rods[3*i+2]);
              }
              m->addFieldData(pRods->GetName(), pRods);
            } break;
          }
        } break;
      }
    }
    else
    {
      switch(m_OldType)
      {
        case 0:{
          switch(m_NewType)
          {
            case 1:{//Euler Angle -> Quats
              DataArray<float>* pEA = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::EulerAngles)).get());
              float* ea = pEA->GetPointer(0);
              FloatArrayType::Pointer pQuats = FloatArrayType::CreateArray(totalPoints, 4, DREAM3D::CellData::Quats);
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::EulertoQuat(quats[i], ea[3*i+0], ea[3*i+1], ea[3*i+2]);
              }
              m->addCellData(pQuats->GetName(), pQuats);
            } break;

            case 2:{//Euler Angle -> Rod
              DataArray<float>* pEA = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::EulerAngles)).get());
              float* ea = pEA->GetPointer(0);
              FloatArrayType::Pointer pRods = FloatArrayType::CreateArray(totalPoints, 3, DREAM3D::CellData::RodriguesVectors);
              float* rods = pRods->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::EulerToRod(ea[3*i+0], ea[3*i+1], ea[3*i+2], rods[3*i+0], rods[3*i+1], rods[3*i+2]);
              }
              m->addCellData(pRods->GetName(), pRods);
            } break;

            case 3:{//Euler Angle -> Axis Angle
              DataArray<float>* pEA = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::EulerAngles)).get());
              float* ea = pEA->GetPointer(0);
              FloatArrayType::Pointer pAxisAngle = FloatArrayType::CreateArray(totalPoints, 4, DREAM3D::CellData::AxisAngles);
              float* axisAngle = pAxisAngle->GetPointer(0);
              QuatF quat;
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::EulertoQuat(quat, ea[3*i+0], ea[3*i+1], ea[3*i+2]);
                OrientationMath::QuattoAxisAngle(quat, axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2]);
              }
              m->addCellData(pAxisAngle->GetName(), pAxisAngle);
            } break;
          }
        } break;

        case 1:{
          switch(m_NewType)
          {
            case 0:{//Quats -> Euler Angle
              DataArray<float>* pQuats = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::Quats)).get());
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              FloatArrayType::Pointer pEA = FloatArrayType::CreateArray(totalPoints, 3, DREAM3D::CellData::EulerAngles);
              float* ea = pEA->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::QuattoEuler(quats[i], ea[3*i+0], ea[3*i+1], ea[3*i+2]);
              }
              m->addCellData(pEA->GetName(), pEA);
            } break;

            case 2:{//Quats -> Rod
              DataArray<float>* pQuats = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::Quats)).get());
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              FloatArrayType::Pointer pRods = FloatArrayType::CreateArray(totalPoints, 3, DREAM3D::CellData::RodriguesVectors);
              float* rods = pRods->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::QuattoRod(quats[i], rods[3*i+0], rods[3*i+1], rods[3*i+2]);
              }
              m->addCellData(pRods->GetName(), pRods);
            } break;

            case 3:{//Quats -> Axis Angle
              DataArray<float>* pQuats = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::Quats)).get());
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              FloatArrayType::Pointer pAxisAngle = FloatArrayType::CreateArray(totalPoints, 4, DREAM3D::CellData::AxisAngles);
              float* axisAngle = pAxisAngle->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::QuattoAxisAngle(quats[i], axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2]);
              }
              m->addCellData(pAxisAngle->GetName(), pAxisAngle);
            } break;
          }
        } break;

        case 2:{
          switch(m_NewType)
          {
            case 0:{//Rod -> Euler Angle
              DataArray<float>* pRods = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::RodriguesVectors)).get());
              float* rods = pRods->GetPointer(0);
              FloatArrayType::Pointer pEA = FloatArrayType::CreateArray(totalPoints, 3, DREAM3D::CellData::EulerAngles);
              float* ea = pEA->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::RodtoEuler(rods[3*i+0], rods[3*i+1], rods[3*i+2], ea[3*i+0], ea[3*i+1], ea[3*i+2]);
              }
              m->addCellData(pEA->GetName(), pEA);
            } break;

            case 1:{//Rod -> Quats
              DataArray<float>* pRods = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::RodriguesVectors)).get());
              float* rods = pRods->GetPointer(0);
              FloatArrayType::Pointer pQuats = FloatArrayType::CreateArray(totalPoints, 4, DREAM3D::CellData::Quats);
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::RodtoQuat(quats[i], rods[3*i+0], rods[3*i+1], rods[3*i+2]);
              }
              m->addCellData(pQuats->GetName(), pQuats);
            } break;

            case 3:{//Rod -> Axis Angle
              DataArray<float>* pRods = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::RodriguesVectors)).get());
              float* rods = pRods->GetPointer(0);
              FloatArrayType::Pointer pAxisAngle = FloatArrayType::CreateArray(totalPoints, 4, DREAM3D::CellData::AxisAngles);
              float* axisAngle = pAxisAngle->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::RodtoAxisAngle(rods[3*i+0], rods[3*i+1], rods[3*i+2], axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2]);
              }
              m->addCellData(pAxisAngle->GetName(), pAxisAngle);
            } break;
          }
        } break;

        case 3:{
          switch(m_NewType)
          {
            case 0:{//Axis Angle -> Euler Angle
              DataArray<float>* pAxisAngle = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::AxisAngles)).get());
              float* axisAngle = pAxisAngle->GetPointer(0);
              FloatArrayType::Pointer pEA = FloatArrayType::CreateArray(totalPoints, 3, DREAM3D::CellData::EulerAngles);
              float* ea = pEA->GetPointer(0);
              QuatF quat;
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::AxisAngletoQuat(axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2], quat);
                OrientationMath::QuattoEuler(quat, ea[3*i+0], ea[3*i+1], ea[3*i+2]);
              }
              m->addCellData(pEA->GetName(), pEA);
            } break;

            case 1:{//Axis Angle -> Quats
              DataArray<float>* pAxisAngle = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::AxisAngles)).get());
              float* axisAngle = pAxisAngle->GetPointer(0);
              FloatArrayType::Pointer pQuats = FloatArrayType::CreateArray(totalPoints, 4, DREAM3D::CellData::Quats);
              QuatF* quats = reinterpret_cast<QuatF*>(pQuats->GetPointer(0));
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::AxisAngletoQuat(axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2], quats[i]);
              }
              m->addCellData(pQuats->GetName(), pQuats);
            } break;

            case 2:{//Axis Angle -> Rod
              DataArray<float>* pAxisAngle = DataArray<float>::SafePointerDownCast((m->getCellData(DREAM3D::CellData::AxisAngles)).get());
              float* axisAngle = pAxisAngle->GetPointer(0);
              FloatArrayType::Pointer pRods = FloatArrayType::CreateArray(totalPoints, 3, DREAM3D::CellData::RodriguesVectors);
              float* rods = pRods->GetPointer(0);
              for(int i=0; i<numgrains; i++)
              {
                OrientationMath::AxisAngletoRod(axisAngle[4*i+3], axisAngle[4*i+0], axisAngle[4*i+1] ,axisAngle[4*i+2], rods[3*i+0], rods[3*i+1], rods[3*i+2]);
              }
              m->addCellData(pRods->GetName(), pRods);
            } break;
          }
        }
      }
    }
  }
  notifyStatusMessage("Complete");
}
