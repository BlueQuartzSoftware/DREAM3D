/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "AlignSectionsMisorientation.h"

#include <QtCore/QDateTime>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DoubleFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "Reconstruction/ReconstructionConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsMisorientation::AlignSectionsMisorientation() :
  AlignSections(),
  m_MisorientationTolerance(5.0f),
  m_UseGoodVoxels(true),
  m_QuatsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_CellPhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Mask),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_Quats(NULL),
  m_CellPhases(NULL),
  m_GoodVoxels(NULL),
  m_CrystalStructures(NULL)
{
  Seed = QDateTime::currentMSecsSinceEpoch();

  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  // only setting up the child parameters because the parent constructor has already been called
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsMisorientation::~AlignSectionsMisorientation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::setupFilterParameters()
{
  // getting the current parameters that were set by the parent and adding to it before resetting it
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_front(DoubleFilterParameter::New("Misorientation Tolerance (Degrees)", "MisorientationTolerance", getMisorientationTolerance(), FilterParameter::Parameter));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Mask Array", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Quaternions", "QuatsArrayPath", getQuatsArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "CellPhasesArrayPath", getCellPhasesArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Mask", "GoodVoxelsArrayPath", getGoodVoxelsArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AlignSections::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AlignSectionsMisorientation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  AlignSections::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MisorientationTolerance)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::dataCheck()
{
  setErrorCondition(0);

  // Set the DataContainerName and AttributematrixName for the Parent Class (AlignSections) to Use.
  setDataContainerName(m_QuatsArrayPath.getDataContainerName());
  setCellAttributeMatrixName(m_QuatsArrayPath.getAttributeMatrixName());

  AlignSections::dataCheck();
  if(getErrorCondition() < 0) { return; }

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 4);
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getQuatsArrayPath()); }

  cDims[0] = 1;
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray< int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCellPhasesArrayPath()); }

  if (m_UseGoodVoxels == true)
  {
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getGoodVoxelsArrayPath()); }
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::preflight()
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
void AlignSectionsMisorientation::find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  std::ofstream outFile;
  if (getWriteAlignmentShifts() == true)
  {
    outFile.open(getAlignmentShiftFileName().toLatin1().data());
  }

  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  float disorientation = 0.0f;
  float mindisorientation = std::numeric_limits<float>::max();
  int64_t newxshift = 0;
  int64_t newyshift = 0;
  int64_t oldxshift = 0;
  int64_t oldyshift = 0;
  float count = 0.0f;
  DimType slice = 0;
  float w = 0.0f;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  DimType refposition = 0;
  DimType curposition = 0;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

  uint32_t phase1 = 0, phase2 = 0;
  DimType progInt = 0;

  // Allocate a 2D Array which will be reused from slice to slice
  BoolArrayType::Pointer misorientsPtr = BoolArrayType::CreateArray(dims[0] * dims[1], "_INTERNAL_USE_ONLY_Misorients");
  misorientsPtr->initializeWithValue(false);
  bool* misorients = misorientsPtr->getPointer(0); // Get the raw pointer to use in our calculations for speed.

  DimType idx = 0; // This will be used to compute the index into the flat array
  DimType xIdx = 0;
  DimType yIdx = 0;

  const DimType halfDim0 = static_cast<DimType>(dims[0] * 0.5f);
  const DimType halfDim1 = static_cast<DimType>(dims[1] * 0.5f);

  // Loop over the Z Direction
  for (DimType iter = 1; iter < dims[2]; iter++)
  {
    progInt = ((float)iter / dims[2]) * 100.0f;
    QString ss = QObject::tr("Aligning Sections || Determining Shifts || %1% Complete").arg(progInt);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    if (getCancel() == true)
    {
      return;
    }
    mindisorientation = std::numeric_limits<float>::max();
    slice = (dims[2] - 1) - iter;
    oldxshift = -1;
    oldyshift = -1;
    newxshift = 0;
    newyshift = 0;

    misorientsPtr->initializeWithValue(false); // Initialize everything to false

    while (newxshift != oldxshift || newyshift != oldyshift)
    {
      oldxshift = newxshift;
      oldyshift = newyshift;
      for (int32_t j = -3; j < 4; j++)
      {
        for (int32_t k = -3; k < 4; k++)
        {
          disorientation = 0.0f;
          count = 0.0f;
          xIdx = k + oldxshift + halfDim0;
          yIdx = j + oldyshift + halfDim1;
          idx = (dims[0] * yIdx) + xIdx;
          if (misorients[idx] == false && llabs(k + oldxshift) < halfDim0 && llabs(j + oldyshift) < halfDim1)
          {
            for (DimType l = 0; l < dims[1]; l = l + 4)
            {
              for (DimType n = 0; n < dims[0]; n = n + 4)
              {
                if ((l + j + oldyshift) >= 0 && (l + j + oldyshift) < dims[1] && (n + k + oldxshift) >= 0 && (n + k + oldxshift) < dims[0])
                {
                  count++;
                  refposition = ((slice + 1) * dims[0] * dims[1]) + (l * dims[0]) + n;
                  curposition = (slice * dims[0] * dims[1]) + ((l + j + oldyshift) * dims[0]) + (n + k + oldxshift);
                  if (m_UseGoodVoxels == false || (m_GoodVoxels[refposition] == true && m_GoodVoxels[curposition] == true))
                  {
                    w = std::numeric_limits<float>::max();
                    if (m_CellPhases[refposition] > 0 && m_CellPhases[curposition] > 0)
                    {
                      QuaternionMathF::Copy(quats[refposition], q1);
                      phase1 = m_CrystalStructures[m_CellPhases[refposition]];
                      QuaternionMathF::Copy(quats[curposition], q2);
                      phase2 = m_CrystalStructures[m_CellPhases[curposition]];
                      if (phase1 == phase2 && phase1 < static_cast<uint32_t>(m_OrientationOps.size()) )
                      {
                        w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
                      }
                    }
                    if (w > m_MisorientationTolerance) { disorientation++; }
                  }
                  if (m_UseGoodVoxels == true)
                  {
                    if (m_GoodVoxels[refposition] == true && m_GoodVoxels[curposition] == false) { disorientation++; }
                    if (m_GoodVoxels[refposition] == false && m_GoodVoxels[curposition] == true) { disorientation++; }
                  }
                }
                else
                {

                }
              }
            }
            disorientation = disorientation / count;
            xIdx = k + oldxshift + halfDim0;
            yIdx = j + oldyshift + halfDim1;
            idx = (dims[0] * yIdx) + xIdx;
            misorients[idx] = true;
            if (disorientation < mindisorientation || (disorientation == mindisorientation && ((llabs(k + oldxshift) < llabs(newxshift)) || (llabs(j + oldyshift) < llabs(newyshift)))))
            {
              newxshift = k + oldxshift;
              newyshift = j + oldyshift;
              mindisorientation = disorientation;
            }
          }
        }
      }
    }
    xshifts[iter] = xshifts[iter - 1] + newxshift;
    yshifts[iter] = yshifts[iter - 1] + newyshift;
    if (getWriteAlignmentShifts() == true)
    {
      outFile << slice << "	" << slice + 1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << "\n";
    }
  }
  if (getWriteAlignmentShifts() == true)
  {
    outFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Converting the user defined tolerance to radians.
  m_MisorientationTolerance = m_MisorientationTolerance * DREAM3D::Constants::k_Pi / 180.0f;

  AlignSections::execute();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSectionsMisorientation::newFilterInstance(bool copyFilterParameters)
{
  AlignSectionsMisorientation::Pointer filter = AlignSectionsMisorientation::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsMisorientation::getCompiledLibraryName()
{ return ReconstructionConstants::ReconstructionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsMisorientation::getGroupName()
{ return DREAM3D::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsMisorientation::getSubGroupName()
{return DREAM3D::FilterSubGroups::AlignmentFilters;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsMisorientation::getHumanLabel()
{ return "Align Sections (Misorientation)"; }
