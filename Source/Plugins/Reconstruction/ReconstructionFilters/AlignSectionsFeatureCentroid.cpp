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

#include <fstream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

#include "moc_AlignSectionsFeatureCentroid.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsFeatureCentroid::AlignSectionsFeatureCentroid() :
  AlignSections(),
  m_ReferenceSlice(0),
  m_UseReferenceSlice(false),
  m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask),
  m_GoodVoxels(NULL)
{
  // only setting up the child parameters because the parent constructor has already been called
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsFeatureCentroid::~AlignSectionsFeatureCentroid()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsFeatureCentroid::setupFilterParameters()
{
  // getting the current parameters that were set by the parent and adding to it before resetting it
  FilterParameterVector parameters = getFilterParameters();
  QStringList linkedProps("ReferenceSlice");
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Reference Slice", "UseReferenceSlice", getUseReferenceSlice(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Reference Slice", "ReferenceSlice", getReferenceSlice(), FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Mask", "GoodVoxelsArrayPath", getGoodVoxelsArrayPath(), FilterParameter::RequiredArray, req));
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
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setUseReferenceSlice( reader->readValue("UseReferenceSlice", getUseReferenceSlice()) );
  setReferenceSlice( reader->readValue("ReferenceSlice", getReferenceSlice()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AlignSectionsFeatureCentroid::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  AlignSections::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(UseReferenceSlice)
  SIMPL_FILTER_WRITE_PARAMETER(ReferenceSlice)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
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

  // Set the DataContainerName and AttributematrixName for the Parent Class (AlignSections) to Use.
  // These are checked for validity in the Parent Class dataCheck
  setDataContainerName(m_GoodVoxelsArrayPath.getDataContainerName());
  setCellAttributeMatrixName(m_GoodVoxelsArrayPath.getAttributeMatrixName());

  AlignSections::dataCheck();
  if(getErrorCondition() < 0) { return; }


  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, m_GoodVoxelsArrayPath.getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  if (m_ReferenceSlice > image->getZPoints())
  {
    QString ss = QObject::tr("The Image Geometry extent (%1) is smaller than the supplied reference slice (%2)").arg(image->getZPoints()).arg(m_ReferenceSlice);
    setErrorCondition(-5556);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> cDims(1, 1);
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() < 0) { return; }
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
  if (getWriteAlignmentShifts() == true)
  {
    outFile.open(getAlignmentShiftFileName().toLatin1().data());
  }
  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] =
  {
    static_cast<int64_t>(udims[0]),
    static_cast<int64_t>(udims[1]),
    static_cast<int64_t>(udims[2]),
  };

  int64_t newxshift = 0;
  int64_t newyshift = 0;
  int64_t count = 0;
  int64_t slice = 0;
  int64_t point = 0;
  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  std::vector<float> xCentroid(dims[2], 0.0f);
  std::vector<float> yCentroid(dims[2], 0.0f);

  for (int64_t iter = 0; iter < dims[2]; iter++)
  {
    count = 0;
    xCentroid[iter] = 0;
    yCentroid[iter] = 0;

    QString ss = QObject::tr("Aligning Sections || Determining Shifts || %1% Complete").arg(((float)iter / dims[2]) * 100);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    slice = static_cast<int>( (dims[2] - 1) - iter );
    for (int64_t l = 0; l < dims[1]; l++)
    {
      for (int64_t n = 0; n < dims[0]; n++)
      {
        point = ((slice) * dims[0] * dims[1]) + (l * dims[0]) + n;
        if (m_GoodVoxels[point] == true)
        {
          xCentroid[iter] = xCentroid[iter] + (float(n) * xRes);
          yCentroid[iter] = yCentroid[iter] + (float(l) * yRes);
          count++;
        }
      }
    }
    xCentroid[iter] = xCentroid[iter] / float(count);
    yCentroid[iter] = yCentroid[iter] / float(count);
  }
  for (int64_t iter = 1; iter < dims[2]; iter++)
  {
    slice = (dims[2] - 1) - iter;
    if (m_UseReferenceSlice == true)
    {
      xshifts[iter] = static_cast<int64_t>((xCentroid[iter] - xCentroid[m_ReferenceSlice]) / xRes);
      yshifts[iter] = static_cast<int64_t>((yCentroid[iter] - yCentroid[m_ReferenceSlice]) / yRes);
    }
    else
    {
      xshifts[iter] = xshifts[iter - 1] + static_cast<int64_t>((xCentroid[iter] - xCentroid[iter - 1]) / xRes);
      yshifts[iter] = yshifts[iter - 1] + static_cast<int64_t>((yCentroid[iter] - yCentroid[iter - 1]) / yRes);
    }
    if (getWriteAlignmentShifts() == true)
    {
      outFile << slice << "	" << slice + 1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << " " << xCentroid[iter] << " " << yCentroid[iter] << std::endl;
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
void AlignSectionsFeatureCentroid::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  AlignSections::execute();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSectionsFeatureCentroid::newFilterInstance(bool copyFilterParameters)
{
  AlignSectionsFeatureCentroid::Pointer filter = AlignSectionsFeatureCentroid::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getCompiledLibraryName()
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getBrandingString()
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getGroupName()
{ return SIMPL::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getSubGroupName()
{return SIMPL::FilterSubGroups::AlignmentFilters;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsFeatureCentroid::getHumanLabel()
{ return "Align Sections (Feature Centroid)"; }
