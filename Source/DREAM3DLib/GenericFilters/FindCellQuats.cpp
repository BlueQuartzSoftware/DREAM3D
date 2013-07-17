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

#include "FindCellQuats.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/IDataArray.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindCellQuats::FindCellQuats() :
  AbstractFilter(),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CellPhases(NULL),
  m_Quats(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationMath::getOrientationOpsVector();
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindCellQuats::~FindCellQuats()
{
}

// -----------------------------------------------------------------------------
void FindCellQuats::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindCellQuats::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCellQuats::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -300, float, FloatArrayType, voxels, 3)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -301, int32_t, Int32ArrayType, voxels, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, float, FloatArrayType, 0, voxels, 5)

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCellQuats::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCellQuats::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  std::stringstream ss;

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  float qr[5];
  int phase = -1;
  for (int i = 0; i < totalPoints; i++)
  {
    phase = m_CellPhases[i];
    OrientationMath::eulertoQuat(qr, m_CellEulerAngles[3*i], m_CellEulerAngles[3*i + 1], m_CellEulerAngles[3*i + 2]);
    OrientationMath::normalizeQuat(qr);
    if (m_CrystalStructures[phase] == Ebsd::CrystalStructure::UnknownCrystalStructure)
    {
      qr[1] = 0.0;
      qr[2] = 0.0;
      qr[3] = 0.0;
      qr[4] = 1.0;
    }
    else
    {
      uint32_t xtalStruct = m_CrystalStructures[phase];
      if (xtalStruct >= m_OrientationOps.size())
      {
        setErrorCondition(-55000);
        ss.str("");
        ss << "The value for the Crystal Structure is " << xtalStruct << " which is a value that is not understood by DREAM3D. The "
        << "symmetry operations have not been implemented. Please report this to dream3d@bluequartz.net along with any data that you "
        << "can provide.";
        notifyErrorMessage(ss.str(), getErrorCondition());
        return;
      }
      else
      {
        m_OrientationOps[m_CrystalStructures[phase]]->getFZQuat(qr);
        OrientationMath::normalizeQuat(qr);
      }
    }

    m_Quats[i*5 + 0] = 1.0f;
    m_Quats[i*5 + 1] = qr[1];
    m_Quats[i*5 + 2] = qr[2];
    m_Quats[i*5 + 3] = qr[3];
    m_Quats[i*5 + 4] = qr[4];
  }

  notifyStatusMessage("Complete");
}


