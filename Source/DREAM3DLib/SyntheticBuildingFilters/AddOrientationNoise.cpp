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

#include "AddOrientationNoise.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Math/MatrixMath.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddOrientationNoise::AddOrientationNoise() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::HDF5::CellAttributeMatrixName),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_Magnitude(1.0f),
  m_CellEulerAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddOrientationNoise::~AddOrientationNoise()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddOrientationNoise::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Magnitude of Orientation Noise");
    option->setPropertyName("Magnitude");
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
void AddOrientationNoise::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setMagnitude( reader->readValue("Magnitude", getMagnitude()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AddOrientationNoise::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("Magnitude", getMagnitude() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddOrientationNoise::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
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
  // Cell Data
  QVector<int> dims(1, 3);
  m_CellEulerAnglesPtr = m->getPrereqArray<float, FloatArrayType, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellEulerAnglesArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CellEulerAngles */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddOrientationNoise::preflight()
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
void AddOrientationNoise::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getAttributeMatrix(m_CellAttributeMatrixName)->getNumTuples();

  dataCheck(false, totalPoints, 0, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  m_Magnitude = m_Magnitude * DREAM3D::Constants::k_Pi / 180.0;

  add_orientation_noise();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("AddOrientationNoises Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  AddOrientationNoise::add_orientation_noise()
{
  notifyStatusMessage("Adding Orientation Noise");
  DREAM3D_RANDOMNG_NEW()

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  //float ea1, ea2, ea3;
  float g[3][3];
  float newg[3][3];
  float rot[3][3];
  float w, n1, n2, n3;
  int64_t totalPoints = m->getTotalPoints();
  for (size_t i = 0; i < static_cast<size_t>(totalPoints); ++i)
  {
    float ea1 = m_CellEulerAngles[3 * i + 0];
    float ea2 = m_CellEulerAngles[3 * i + 1];
    float ea3 = m_CellEulerAngles[3 * i + 2];
    OrientationMath::EulertoMat(ea1, ea2, ea3, g);
    n1 = static_cast<float>( rg.genrand_res53() );
    n2 = static_cast<float>( rg.genrand_res53() );
    n3 = static_cast<float>( rg.genrand_res53() );
    w = static_cast<float>( rg.genrand_res53() );
    w = 2.0 * (w - 0.5);
    w = (m_Magnitude * w);
    OrientationMath::AxisAngletoMat(w, n1, n2, n3, rot);
    MatrixMath::Multiply3x3with3x3(g, rot, newg);
    OrientationMath::MattoEuler(newg, ea1, ea2, ea3);
    m_CellEulerAngles[3 * i + 0] = ea1;
    m_CellEulerAngles[3 * i + 1] = ea2;
    m_CellEulerAngles[3 * i + 2] = ea3;
  }
}

