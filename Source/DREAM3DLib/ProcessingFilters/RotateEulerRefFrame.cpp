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

#if DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include "DREAM3DLib/Common/DREAM3DMath.h"

const static float m_pi = static_cast<float>(M_PI);


class RotateEulerRefFrameImpl
{
    float* m_CellEulerAngles;
	float phi1mod;
	float PHImod;
	float phi2mod;
  public:
    RotateEulerRefFrameImpl(float* data, float rotPhi1, float rotPHI, float rotPhi2) :
      m_CellEulerAngles(data),
	  phi1mod(rotPhi1),
	  PHImod(rotPHI),
	  phi2mod(rotPhi2)
    {}
    virtual ~RotateEulerRefFrameImpl(){}

    void convert(size_t start, size_t end) const
    {
      for (size_t i = start; i < end; i++)
      {
        m_CellEulerAngles[3*i+0] = m_CellEulerAngles[3*i+0] + phi1mod;
        m_CellEulerAngles[3*i+1] = m_CellEulerAngles[3*i+1] + PHImod;
        m_CellEulerAngles[3*i+2] = m_CellEulerAngles[3*i+2] + phi2mod;
      }
    }

#if DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
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
m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
m_RotationAngle(DREAM3D::EulerFrameRotationAngle::Zero),
m_RotationAxis(DREAM3D::EulerFrameRotationAxis::None),
m_CellEulerAngles(NULL)
{
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
  std::vector<FilterParameter::Pointer> parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Rotation Axis");
    option->setPropertyName("RotationAxis");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("RD");
    choices.push_back("TD");
    choices.push_back("ND");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Rotation Angle");
    option->setPropertyName("RotationAngle");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("90 Degrees");
    choices.push_back("180 Degrees");
    choices.push_back("270 Degrees");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("RotationAxis", getRotationAxis() );
  writer->writeValue("RotationAngle", getRotationAngle() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -301, float, FloatArrayType, voxels, 3)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateEulerRefFrame::execute()
{
  DataContainer* m = getDataContainer();
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

  float rotAngle = 0;
  float degToRad = m_pi/180.0;
  float angles[3];

  if (m_RotationAngle == DREAM3D::EulerFrameRotationAngle::Ninety) rotAngle = 90.0;
  else if (m_RotationAngle == DREAM3D::EulerFrameRotationAngle::oneEighty) rotAngle = 180.0;
  else if (m_RotationAngle == DREAM3D::EulerFrameRotationAngle::twoSeventy) rotAngle = 270.0;

  if (m_RotationAxis == DREAM3D::EulerFrameRotationAxis::RD)
  {
	angles[0] = 0.0;
	angles[1] = rotAngle*degToRad;
	angles[2] = 0.0;
  }
  else if (m_RotationAxis == DREAM3D::EulerFrameRotationAxis::TD)
  {
	if(rotAngle == 90) angles[0] = 270.0*degToRad, angles[1] = 90.0*degToRad, angles[2] = 90.0*degToRad;
	else if(rotAngle == 90) angles[0] = 180.0*degToRad, angles[1] = 180.0*degToRad, angles[2] = 0.0*degToRad;
	else if(rotAngle == 270) angles[0] = 90.0*degToRad, angles[1] = 90.0*degToRad, angles[2] = 270.0*degToRad;
  }
  else if (m_RotationAxis == DREAM3D::EulerFrameRotationAxis::ND)
  {
	angles[0] = 0.0;
	angles[1] = 0.0;
	angles[2] = rotAngle;
  }

//  std::cout << "RotateEulerRefFrame: " << m_ConversionFactor << std::endl;
#if DREAM3D_USE_PARALLEL_ALGORITHMS
//#if 0
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                    RotateEulerRefFrameImpl(m_CellEulerAngles, conversionFactor), tbb::auto_partitioner());

#else
  RotateEulerRefFrameImpl serial(m_CellEulerAngles, angles[0], angles[1], angles[2]);
  serial.convert(0, totalPoints);
#endif

 notifyStatusMessage("Complete");
}
