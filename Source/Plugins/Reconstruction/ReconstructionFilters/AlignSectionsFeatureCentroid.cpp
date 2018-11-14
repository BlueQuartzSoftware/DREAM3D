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

#include "AlignSectionsFeatureCentroid.h"

#include <cmath>
#include <fstream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsFeatureCentroid::AlignSectionsFeatureCentroid()
: m_ReferenceSlice(0)
, m_UseReferenceSlice(false)
, m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsFeatureCentroid::~AlignSectionsFeatureCentroid() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::setupFilterParameters()
{
  AlignSections::setupFilterParameters();
  // getting the current parameters that were set by the parent and adding to it before resetting it
  FilterParameterVector parameters = getFilterParameters();
  QStringList linkedProps("ReferenceSlice");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Reference Slice", UseReferenceSlice, FilterParameter::Parameter, AlignSectionsFeatureCentroid, linkedProps));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Reference Slice", ReferenceSlice, FilterParameter::Parameter, AlignSectionsFeatureCentroid));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, AlignSectionsFeatureCentroid, req));
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AlignSections::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setUseReferenceSlice(reader->readValue("UseReferenceSlice", getUseReferenceSlice()));
  setReferenceSlice(reader->readValue("ReferenceSlice", getReferenceSlice()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  // Set the DataContainerName and AttributematrixName for the Parent Class (AlignSections) to Use.
  // These are checked for validity in the Parent Class dataCheck
  setDataContainerName(m_GoodVoxelsArrayPath.getDataContainerName());
  setCellAttributeMatrixName(m_GoodVoxelsArrayPath.getAttributeMatrixName());

  AlignSections::dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, m_GoodVoxelsArrayPath.getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_ReferenceSlice > image->getZPoints())
  {
    QString ss = QObject::tr("The Image Geometry extent (%1) is smaller than the supplied reference slice (%2)").arg(image->getZPoints()).arg(m_ReferenceSlice);
    setErrorCondition(-5556);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> cDims(1, 1);
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                     cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_GoodVoxelsPtr.lock())                                                                      /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::preflight()
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
void AlignSectionsFeatureCentroid::find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  std::ofstream outFile;
  if(getWriteAlignmentShifts())
  {
    outFile.open(getAlignmentShiftFileName().toLatin1().data());
    outFile << "#"
            << "Slice_A,Slice_B,New X Shift,New Y Shift,X Shift, Y Shift, X Centroid, Y Centroid" << std::endl;
  }
  size_t dims[3] = {0, 0, 0};
  std::tie(dims[0], dims[1], dims[2]) = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t sdims[3] = {
      static_cast<int64_t>(dims[0]),
      static_cast<int64_t>(dims[1]),
      static_cast<int64_t>(dims[2]),
  };

  size_t newxshift = 0;
  size_t newyshift = 0;
  size_t count = 0;
  size_t slice = 0;
  size_t point = 0;
  float xRes = 0.0f;
  float yRes = 0.0f;
  float zRes = 0.0f;
  std::tie(xRes, yRes, zRes) = m->getGeometryAs<ImageGeom>()->getResolution();
  std::vector<float> xCentroid(dims[2], 0.0f);
  std::vector<float> yCentroid(dims[2], 0.0f);

  for(size_t iter = 0; iter < dims[2]; iter++)
  {
    count = 0;
    xCentroid[iter] = 0;
    yCentroid[iter] = 0;

    QString ss = QObject::tr("Aligning Sections || Determining Shifts || %1% Complete").arg((static_cast<double>(iter) / dims[2]) * 100);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    slice = static_cast<size_t>((dims[2] - 1) - iter);
    for(size_t l = 0; l < dims[1]; l++)
    {
      for(size_t n = 0; n < dims[0]; n++)
      {
        point = ((slice)*dims[0] * dims[1]) + (l * dims[0]) + n;
        if(m_GoodVoxels[point])
        {
          xCentroid[iter] = xCentroid[iter] + (static_cast<float>(n) * xRes);
          yCentroid[iter] = yCentroid[iter] + (static_cast<float>(l) * yRes);
          count++;
        }
      }
    }
    xCentroid[iter] = xCentroid[iter] / static_cast<float>(count);
    yCentroid[iter] = yCentroid[iter] / static_cast<float>(count);
  }

  bool xWarning = false;
  bool yWarning = false;
  // Calculate the X&Y shifts based on the cetroid. Note the shifts are in real units
  for(size_t iter = 1; iter < dims[2]; iter++)
  {
    slice = (dims[2] - 1) - iter;
    if(m_UseReferenceSlice)
    {
      xshifts[iter] = static_cast<int64_t>((xCentroid[iter] - xCentroid[static_cast<size_t>(m_ReferenceSlice)]) / xRes);
      yshifts[iter] = static_cast<int64_t>((yCentroid[iter] - yCentroid[static_cast<size_t>(m_ReferenceSlice)]) / yRes);
    }
    else
    {
      xshifts[iter] = xshifts[iter - 1] + static_cast<int64_t>((xCentroid[iter] - xCentroid[iter - 1]) / xRes);
      yshifts[iter] = yshifts[iter - 1] + static_cast<int64_t>((yCentroid[iter] - yCentroid[iter - 1]) / yRes);
    }

    if((xshifts[iter] < -sdims[0] || xshifts[iter] > sdims[0]) && !xWarning)
    {
      setWarningCondition(100);
      QString msg;
      QTextStream ss(&msg);
      ss << "A shift was greater than the X dimension of the Image Geometry. All subsequent slices are probably wrong. Slice=" << iter << " X Dim=" << dims[0] << " X Shift=" << xshifts[iter]
         << " sDims[0]=" << sdims[0];
      notifyWarningMessage(getHumanLabel(), msg, getWarningCondition());
      xWarning = true;
    }
    if((yshifts[iter] < -sdims[1] || yshifts[iter] > sdims[1]) && !yWarning)
    {
      setWarningCondition(101);
      QString msg;
      QTextStream ss(&msg);
      ss << "A shift was greater than the Y dimension of the Image Geometry. All subsequent slices are probably wrong. Slice=" << iter << " Y Dim=" << dims[1] << " Y Shift=" << yshifts[iter]
         << " sDims[1]=" << sdims[1];
      notifyWarningMessage(getHumanLabel(), msg, getWarningCondition());
      yWarning = true;
    }
    if(std::isnan(xCentroid[iter]) && !xWarning)
    {
      setWarningCondition(102);
      QString msg;
      QTextStream ss(&msg);
      ss << "The X Centoird was NaN. All subsequent slices are probably wrong. Slice=" << iter;
      notifyWarningMessage(getHumanLabel(), msg, getWarningCondition());
      xWarning = true;
    }
    if(std::isnan(yCentroid[iter]) && !yWarning)
    {
      setWarningCondition(103);
      QString msg;
      QTextStream ss(&msg);
      ss << "The Y Centoird was NaN. All subsequent slices are probably wrong. Slice=" << iter;
      notifyWarningMessage(getHumanLabel(), msg, getWarningCondition());
      yWarning = true;
    }
    if(getWriteAlignmentShifts())
    {
      outFile << slice << "," << slice + 1 << "," << newxshift << "," << newyshift << "," << xshifts[iter] << "," << yshifts[iter] << "," << xCentroid[iter] << "," << yCentroid[iter] << std::endl;
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
void AlignSectionsFeatureCentroid::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  AlignSections::execute();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSectionsFeatureCentroid::newFilterInstance(bool copyFilterParameters) const
{
  AlignSectionsFeatureCentroid::Pointer filter = AlignSectionsFeatureCentroid::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid AlignSectionsFeatureCentroid::getUuid()
{
  return QUuid("{886f8b46-51b6-5682-a289-6febd10b7ef0}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::AlignmentFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getHumanLabel() const
{
  return "Align Sections (Feature Centroid)";
}
