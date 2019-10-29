/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <memory>

#include "AlignSectionsMisorientation.h"

#include <fstream>

#include <QtCore/QDateTime>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationLib/Core/Quaternion.hpp"
#include "OrientationLib/LaueOps/LaueOps.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsMisorientation::AlignSectionsMisorientation()
: m_MisorientationTolerance(5.0f)
, m_UseGoodVoxels(true)
, m_QuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
{
  m_RandomSeed = QDateTime::currentMSecsSinceEpoch();


  // only setting up the child parameters because the parent constructor has already been called
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsMisorientation::~AlignSectionsMisorientation() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::setupFilterParameters()
{
  // getting the current parameters that were set by the parent and adding to it before resetting it
  AlignSections::setupFilterParameters();
  FilterParameterVectorType parameters = getFilterParameters();
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Misorientation Tolerance (Degrees)", MisorientationTolerance, FilterParameter::Parameter, AlignSectionsMisorientation));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Mask Array", UseGoodVoxels, FilterParameter::Parameter, AlignSectionsMisorientation, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Quaternions", QuatsArrayPath, FilterParameter::RequiredArray, AlignSectionsMisorientation, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, AlignSectionsMisorientation, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, AlignSectionsMisorientation, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);

    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, AlignSectionsMisorientation, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AlignSections::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath()));
  setMisorientationTolerance(reader->readValue("MisorientationTolerance", getMisorientationTolerance()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::initialize()
{
  m_RandomSeed = QDateTime::currentMSecsSinceEpoch();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  // Set the DataContainerName and AttributematrixName for the Parent Class (AlignSections) to Use.
  setDataContainerName(DataArrayPath(m_QuatsArrayPath.getDataContainerName(), "", ""));
  setCellAttributeMatrixName(m_QuatsArrayPath.getAttributeMatrixName());

  AlignSections::dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  QVector<DataArrayPath> dataArrayPaths;

  std::vector<size_t> cDims(1, 4);
  m_QuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_QuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Quats = m_QuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getQuatsArrayPath());
  }

  cDims[0] = 1;
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  if(m_UseGoodVoxels)
  {
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                       cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_GoodVoxelsPtr.lock())                                                                      /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArrayPaths.push_back(getGoodVoxelsArrayPath());
    }
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

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
  if(getWriteAlignmentShifts())
  {
    outFile.open(getAlignmentShiftFileName().toLatin1().data());
  }

  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
  };

  QVector<LaueOps::Pointer> m_OrientationOps = LaueOps::getOrientationOpsQVector();

  float disorientation = 0.0f;
  float mindisorientation = std::numeric_limits<float>::max();
  int64_t newxshift = 0;
  int64_t newyshift = 0;
  int64_t oldxshift = 0;
  int64_t oldyshift = 0;
  float count = 0.0f;
  int64_t slice = 0;
  float w = 0.0f;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;

  int64_t refposition = 0;
  int64_t curposition = 0;

  uint32_t phase1 = 0, phase2 = 0;
  int64_t progInt = 0;

  // Allocate a 2D Array which will be reused from slice to slice
  BoolArrayType::Pointer misorientsPtr = BoolArrayType::CreateArray(dims[0] * dims[1], "_INTERNAL_USE_ONLY_Misorients", true);
  misorientsPtr->initializeWithValue(false);
  bool* misorients = misorientsPtr->getPointer(0); // Get the raw pointer to use in our calculations for speed.

  int64_t idx = 0; // This will be used to compute the index into the flat array
  int64_t xIdx = 0;
  int64_t yIdx = 0;

  const int64_t halfDim0 = static_cast<int64_t>(dims[0] * 0.5f);
  const int64_t halfDim1 = static_cast<int64_t>(dims[1] * 0.5f);

  // Loop over the Z Direction
  for(int64_t iter = 1; iter < dims[2]; iter++)
  {
    progInt = ((float)iter / dims[2]) * 100.0f;
    QString ss = QObject::tr("Aligning Sections || Determining Shifts || %1% Complete").arg(progInt);
    notifyStatusMessage(ss);
    if(getCancel())
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

    float misorientationTolerance = m_MisorientationTolerance * SIMPLib::Constants::k_Pif / 180.0f;

    while(newxshift != oldxshift || newyshift != oldyshift)
    {
      oldxshift = newxshift;
      oldyshift = newyshift;
      for(int32_t j = -3; j < 4; j++)
      {
        for(int32_t k = -3; k < 4; k++)
        {
          disorientation = 0.0f;
          count = 0.0f;
          xIdx = k + oldxshift + halfDim0;
          yIdx = j + oldyshift + halfDim1;
          idx = (dims[0] * yIdx) + xIdx;
          if(!misorients[idx] && llabs(k + oldxshift) < halfDim0 && llabs(j + oldyshift) < halfDim1)
          {
            for(int64_t l = 0; l < dims[1]; l = l + 4)
            {
              for(int64_t n = 0; n < dims[0]; n = n + 4)
              {
                if((l + j + oldyshift) >= 0 && (l + j + oldyshift) < dims[1] && (n + k + oldxshift) >= 0 && (n + k + oldxshift) < dims[0])
                {
                  count++;
                  refposition = ((slice + 1) * dims[0] * dims[1]) + (l * dims[0]) + n;
                  curposition = (slice * dims[0] * dims[1]) + ((l + j + oldyshift) * dims[0]) + (n + k + oldxshift);
                  if(!m_UseGoodVoxels || (m_GoodVoxels[refposition] && m_GoodVoxels[curposition]))
                  {
                    w = std::numeric_limits<float>::max();
                    if(m_CellPhases[refposition] > 0 && m_CellPhases[curposition] > 0)
                    {

                      QuatF q1(m_Quats + refposition * 4); // BEWARE POINTER MATH!!
                      phase1 = m_CrystalStructures[m_CellPhases[refposition]];
                      QuatF q2(m_Quats + curposition * 4); // BEWARE POINTER MATH!!
                      phase2 = m_CrystalStructures[m_CellPhases[curposition]];
                      if(phase1 == phase2 && phase1 < static_cast<uint32_t>(m_OrientationOps.size()))
                      {
                        w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
                      }
                    }
                    if(w > misorientationTolerance)
                    {
                      disorientation++;
                    }
                  }
                  if(m_UseGoodVoxels)
                  {
                    if(m_GoodVoxels[refposition] && !m_GoodVoxels[curposition])
                    {
                      disorientation++;
                    }
                    if(!m_GoodVoxels[refposition] && m_GoodVoxels[curposition])
                    {
                      disorientation++;
                    }
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
            if(disorientation < mindisorientation || (disorientation == mindisorientation && ((llabs(k + oldxshift) < llabs(newxshift)) || (llabs(j + oldyshift) < llabs(newyshift)))))
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
    if(getWriteAlignmentShifts())
    {
      outFile << slice << "	" << slice + 1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << "\n";
    }
  }
  if(getWriteAlignmentShifts())
  {
    outFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::execute()
{
  clearErrorCode();
  clearWarningCode();

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  AlignSections::execute();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSectionsMisorientation::newFilterInstance(bool copyFilterParameters) const
{
  AlignSectionsMisorientation::Pointer filter = AlignSectionsMisorientation::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsMisorientation::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsMisorientation::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsMisorientation::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsMisorientation::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid AlignSectionsMisorientation::getUuid() const
{
  return QUuid("{4fb2b9de-3124-534b-b914-dbbbdbc14604}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsMisorientation::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::AlignmentFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AlignSectionsMisorientation::getHumanLabel() const
{
  return "Align Sections (Misorientation)";
}

// -----------------------------------------------------------------------------
AlignSectionsMisorientation::Pointer AlignSectionsMisorientation::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<AlignSectionsMisorientation> AlignSectionsMisorientation::New()
{
  struct make_shared_enabler : public AlignSectionsMisorientation
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString AlignSectionsMisorientation::getNameOfClass() const
{
  return QString("AlignSectionsMisorientation");
}

// -----------------------------------------------------------------------------
QString AlignSectionsMisorientation::ClassName()
{
  return QString("AlignSectionsMisorientation");
}

// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::setMisorientationTolerance(float value)
{
  m_MisorientationTolerance = value;
}

// -----------------------------------------------------------------------------
float AlignSectionsMisorientation::getMisorientationTolerance() const
{
  return m_MisorientationTolerance;
}

// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::setUseGoodVoxels(bool value)
{
  m_UseGoodVoxels = value;
}

// -----------------------------------------------------------------------------
bool AlignSectionsMisorientation::getUseGoodVoxels() const
{
  return m_UseGoodVoxels;
}

// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::setQuatsArrayPath(const DataArrayPath& value)
{
  m_QuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath AlignSectionsMisorientation::getQuatsArrayPath() const
{
  return m_QuatsArrayPath;
}

// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath AlignSectionsMisorientation::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}

// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::setGoodVoxelsArrayPath(const DataArrayPath& value)
{
  m_GoodVoxelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath AlignSectionsMisorientation::getGoodVoxelsArrayPath() const
{
  return m_GoodVoxelsArrayPath;
}

// -----------------------------------------------------------------------------
void AlignSectionsMisorientation::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath AlignSectionsMisorientation::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}
