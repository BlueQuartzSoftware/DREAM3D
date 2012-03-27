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

#include "FindAxisODF.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/StatisticsFilters/FindShapes.h"

const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAxisODF::FindAxisODF() :
AbstractFilter(),
m_SurfaceFields(NULL),
m_PhasesF(NULL),
m_AxisEulerAngles(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
  setupFilterOptions();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindAxisODF::~FindAxisODF()
{
}
// -----------------------------------------------------------------------------
void FindAxisODF::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAxisODF::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, -307, float, FloatArrayType, fields, 3);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, -303, bool, BoolArrayType, fields, 1);
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, -303,  int32_t, Int32ArrayType, fields, 1);

  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    ss << "Stats Array Not Initialized At Beginning of '" << getNameOfClass() << "' Filter" << std::endl;
    setErrorCondition(-308);
  }

  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAxisODF::preflight()
{
  dataCheck(true, 1,1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindAxisODF::execute()
{
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  setErrorCondition(0);

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }


  typedef DataArray<unsigned int> XTalType;
  XTalType* crystruct
      = XTalType::SafeObjectDownCast<IDataArray*, XTalType*>(m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructures).get());

  float r1, r2, r3;
  int bin;
  std::vector<FloatArrayType::Pointer> axisodf;
  std::vector<float> totalaxes;
  size_t numXTals = crystruct->GetNumberOfTuples();
  axisodf.resize(numXTals);
  totalaxes.resize(numXTals);
  for(size_t i=1;i<numXTals;i++)
  {
    totalaxes[i] = 0.0;
	axisodf[i] = FloatArrayType::CreateArray((36*36*36), DREAM3D::HDF5::AxisOrientation);
    for (int j = 0; j < (36 * 36 * 36); j++)
    {
		axisodf[i]->SetValue(j, 0.0);
    }
  }
  size_t numgrains = m->getNumFieldTuples();
  for (int i = 0; i < numgrains; i++)
  {
    if (m_SurfaceFields[i] == false)
    {
      totalaxes[m_PhasesF[i]]++;
	}
  }
  for (size_t i = 1; i < numgrains; i++)
  {
    float ea1 = m_AxisEulerAngles[3 * i];
    float ea2 = m_AxisEulerAngles[3 * i + 1];
    float ea3 = m_AxisEulerAngles[3 * i + 2];
    if(m_SurfaceFields[i] == 0)
    {
      OrientationMath::eulertoRod(r1, r2, r3, ea1, ea2, ea3);
      m_OrientationOps[Ebsd::CrystalStructure::OrthoRhombic]->getFZRod(r1, r2, r3);
      bin = m_OrientationOps[Ebsd::CrystalStructure::OrthoRhombic]->getOdfBin(r1, r2, r3);
	  axisodf[m_PhasesF[i]]->SetValue(bin, (axisodf[m_PhasesF[i]]->GetValue(bin) + (1.0/totalaxes[m_PhasesF[i]])));
    }
  }
 // int err;
  for(size_t i=1;i<numXTals;i++)
  {
	  statsDataArray[i]->setAxisOrientation(axisodf[i]);
  }

  notify("Completed", 0, Observable::UpdateProgressMessage);
}
