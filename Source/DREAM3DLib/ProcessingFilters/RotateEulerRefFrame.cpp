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
#include "RotateEulerRefFrame.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/blocked_range3d.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Math/MatrixMath.h"




class RotateEulerRefFrameImpl
{
    float* m_CellEulerAngles;
    float angle;
    FloatVec3Widget_t axis;
  public:
    RotateEulerRefFrameImpl(float* data, float rotAngle, FloatVec3Widget_t rotAxis) :
      m_CellEulerAngles(data),
      angle(rotAngle),
      axis(rotAxis)
    {}
    virtual ~RotateEulerRefFrameImpl() {}

    void convert(size_t start, size_t end) const
    {
      float rotMat[3][3];

      OrientationMath::AxisAngletoMat(angle, axis.x, axis.y, axis.z, rotMat);
      float ea1 = 0, ea2 = 0, ea3 = 0;
      float ea1new = 0, ea2new = 0, ea3new = 0;
      float g[3][3];
      float gNew[3][3];
      for (size_t i = start; i < end; i++)
      {
        ea1 = m_CellEulerAngles[3 * i + 0];
        ea2 = m_CellEulerAngles[3 * i + 1];
        ea3 = m_CellEulerAngles[3 * i + 2];
        OrientationMath::EulertoMat(ea1, ea2, ea3, g);
        MatrixMath::Multiply3x3with3x3(g, rotMat, gNew);
        MatrixMath::Normalize3x3(gNew);
        OrientationMath::MattoEuler(gNew, ea1new, ea2new, ea3new);
        m_CellEulerAngles[3 * i + 0] = ea1new;
        m_CellEulerAngles[3 * i + 1] = ea2new;

        m_CellEulerAngles[3 * i + 2] = ea3new;
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif


};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateEulerRefFrame::RotateEulerRefFrame() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::HDF5::CellAttributeMatrixName),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_RotationAngle(0.0),
  m_CellEulerAngles(NULL)
{
  m_RotationAxis.x = 0.0;
  m_RotationAxis.y = 0.0;
  m_RotationAxis.z = 1.0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateEulerRefFrame::~RotateEulerRefFrame()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Rotation Axis");
    option->setPropertyName("RotationAxis");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("ijk");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Rotation Angle");
    option->setPropertyName("RotationAngle");
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
void RotateEulerRefFrame::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setRotationAxis( reader->readFloatVec3("RotationAxis", getRotationAxis() ) );
  setRotationAngle( reader->readValue("RotationAngle", getRotationAngle()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RotateEulerRefFrame::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("RotationAxis", getRotationAxis() );
  writer->writeValue("RotationAngle", getRotationAngle() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    QString ss = QObject::tr("The Volume Data Container with name '%1'' was not found in the Data Container Array.").arg(getDataContainerName());
    setErrorCondition(-1001);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  QVector<int> dims(1, 3);
  m_CellEulerAnglesPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellEulerAnglesArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, 0, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  m_RotationAngle = m_RotationAngle * DREAM3D::Constants::k_Pi / 180.0;

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      RotateEulerRefFrameImpl(m_CellEulerAngles, m_RotationAngle, m_RotationAxis), tbb::auto_partitioner());

  }
  else
#endif
  {
    RotateEulerRefFrameImpl serial(m_CellEulerAngles, m_RotationAngle, m_RotationAxis);
    serial.convert(0, totalPoints);
  }

  notifyStatusMessage("Complete");
}
