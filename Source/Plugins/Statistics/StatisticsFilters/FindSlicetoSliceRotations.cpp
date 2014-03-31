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

#include "FindSlicetoSliceRotations.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/OrientationMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlicetoSliceRotations::FindSlicetoSliceRotations() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_SlicetoSliceRotationsFile(""),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSlicetoSliceRotations::~FindSlicetoSliceRotations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Slice to Slice Rotations File");
    parameter->setPropertyName("SlicetoSliceRotationsFile");
    parameter->setWidgetType(FilterParameterWidgetType::OutputFileWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindSlicetoSliceRotations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SlicetoSliceRotationsFile", getSlicetoSliceRotationsFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  QVector<size_t> dims(1, 1);
  m_CellPhasesPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_CellPhasesArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_GoodVoxelsPtr = cellAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_GoodVoxelsArrayName, -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = cellEnsembleAttrMat->getPrereqArray<DataArray<unsigned int>, AbstractFilter>(this, m_CrystalStructuresArrayName, -305, dims)
                           ; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 4;
  m_QuatsPtr = cellAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_QuatsArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSlicetoSliceRotations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QuatF q1;
  QuatF q2;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  bool good = false;

  float w, n1, n2, n3;
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType xPoints = static_cast<DimType>(udims[0]);
  DimType yPoints = static_cast<DimType>(udims[1]);
  DimType zPoints = static_cast<DimType>(udims[2]);

  size_t point;
  size_t inNeighbor = 0;
  size_t outNeighbor = 0;

  DimType neighpoints[6];
  neighpoints[0] = -(xPoints * yPoints);
  neighpoints[1] = -xPoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = (xPoints * yPoints);

  float inPlaneAngle, outPlaneAngle;
  float inPlaneAxisX, inPlaneAxisY, inPlaneAxisZ;
  float outPlaneAxisX, outPlaneAxisY, outPlaneAxisZ;
  int inPlaneCount, outPlaneCount;

  std::ofstream outFile;
  outFile.open(m_SlicetoSliceRotationsFile.toLatin1().data(), std::ios_base::binary);

  for (DimType plane = 0; plane < zPoints; plane++)
  {
    inPlaneAngle = 0;
    outPlaneAngle = 0;
    inPlaneAxisX = 0;
    outPlaneAxisX = 0;
    inPlaneAxisY = 0;
    outPlaneAxisY = 0;
    inPlaneAxisZ = 0;
    outPlaneAxisZ = 0;
    inPlaneCount = 0;
    outPlaneCount = 0;
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType col = 0; col < xPoints; col++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        QuaternionMathF::Copy(quats[point], q1);
        if (plane < zPoints - 1)
        {
          outNeighbor = point + neighpoints[5];
          if (m_CellPhases[point] == m_CellPhases[outNeighbor] && m_CellPhases[point] > 0 && m_GoodVoxels[point] == true && m_GoodVoxels[outNeighbor] == true)
          {
            QuaternionMathF::Copy(quats[outNeighbor], q2);
            w = m_OrientationOps[m_CrystalStructures[m_CellPhases[point]]]->getMisoQuat( q1, q2, n1, n2, n3);
            if(w < 5.0f * DREAM3D::Constants::k_Pi / 180.0f)
            {
              OrientationMath::ChangeAxisReferenceFrame(q1, n1, n2, n3);
              outPlaneAngle = outPlaneAngle + w;
              outPlaneAxisX = outPlaneAxisX + n1;
              outPlaneAxisY = outPlaneAxisY + n2;
              outPlaneAxisZ = outPlaneAxisZ + n3;
              outPlaneCount++;
            }
          }
        }
        for(size_t i = 1; i < 5; i++)
        {
          good = true;
          inNeighbor = point + neighpoints[i];
          if(i == 1 && row == 0) { good = false; }
          if(i == 4 && row == (yPoints - 1)) { good = false; }
          if(i == 2 && col == 0) { good = false; }
          if(i == 3 && col == (xPoints - 1)) { good = false; }
          if(good == true && m_CellPhases[point] == m_CellPhases[inNeighbor] && m_CellPhases[point] > 0 && m_GoodVoxels[point] == true && m_GoodVoxels[inNeighbor] == true)
          {
            QuaternionMathF::Copy(quats[inNeighbor], q2);
            w = m_OrientationOps[m_CrystalStructures[m_CellPhases[point]]]->getMisoQuat( q1, q2, n1, n2, n3);
            if(w < 5.0f * DREAM3D::Constants::k_Pi / 180.0f)
            {
              OrientationMath::ChangeAxisReferenceFrame(q1, n1, n2, n3);
              inPlaneAngle = inPlaneAngle + w;
              inPlaneAxisX = inPlaneAxisX + n1;
              inPlaneAxisY = inPlaneAxisY + n2;
              inPlaneAxisZ = inPlaneAxisZ + n3;
              inPlaneCount++;
            }
          }
        }
      }
    }
    if(inPlaneCount > 0)
    {
      inPlaneAngle = inPlaneAngle / inPlaneCount;
      inPlaneAxisX = inPlaneAxisX / inPlaneCount;
      inPlaneAxisY = inPlaneAxisY / inPlaneCount;
      inPlaneAxisZ = inPlaneAxisZ / inPlaneCount;
    }
    if(outPlaneCount > 0)
    {
      outPlaneAngle = outPlaneAngle / outPlaneCount;
      outPlaneAxisX = outPlaneAxisX / outPlaneCount;
      outPlaneAxisY = outPlaneAxisY / outPlaneCount;
      outPlaneAxisZ = outPlaneAxisZ / outPlaneCount;
    }
    outFile << plane << "	" << inPlaneCount << "	" << inPlaneAngle << "	" << inPlaneAxisX << "	" << inPlaneAxisY << "	" << inPlaneAxisZ << "	" << outPlaneCount << "	" << outPlaneAngle << "	" << outPlaneAxisX << "	" << outPlaneAxisY << "	" << outPlaneAxisZ << "\n";
  }

  notifyStatusMessage(getHumanLabel(), "FindSlicetoSliceRotations Completed");
}

