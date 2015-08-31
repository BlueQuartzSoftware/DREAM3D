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


#include "ChangeAngleRepresentation.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/ChoiceFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

/**
 * @brief The ChangeAngleRepresentationImpl class implements a threaded algorithm to convert an array of
 * Euler angles from raidans to degrees or degrees to radians, depending upon the incoming
 * conversion factor.
 */
class ChangeAngleRepresentationImpl
{

  public:
    ChangeAngleRepresentationImpl(float* data, float factor) :
      m_CellEulerAngles(data),
      convFactor(factor)
    {}
    virtual ~ChangeAngleRepresentationImpl() {}

    void convert(size_t start, size_t end) const
    {
      for (size_t i = start; i < end; i++)
      {
        m_CellEulerAngles[i] = m_CellEulerAngles[i] * convFactor;
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    float* m_CellEulerAngles;
    float  convFactor;

};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeAngleRepresentation::ChangeAngleRepresentation() :
  AbstractFilter(),
  m_ConversionType(DREAM3D::EulerAngleConversionType::DegreesToRadians),
  m_CellEulerAnglesArrayPath("", "", ""),
  m_CellEulerAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChangeAngleRepresentation::~ChangeAngleRepresentation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeAngleRepresentation::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Conversion Type");
    parameter->setPropertyName("ConversionType");

    QVector<QString> choices;
    choices.push_back("Degrees to Radians");
    choices.push_back("Radians to Degrees");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, DREAM3D::Defaults::AnyComponentSize, DREAM3D::AttributeMatrixObjectType::Element);
    parameters.push_back(DataArraySelectionFilterParameter::New("Angles", "CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath(), FilterParameter::RequiredArray, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeAngleRepresentation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setConversionType( reader->readValue("ConversionType", getConversionType()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ChangeAngleRepresentation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ConversionType)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeAngleRepresentation::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> cDims(1, 3);
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChangeAngleRepresentation::preflight()
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
void ChangeAngleRepresentation::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  int64_t totalPoints = static_cast<int64_t>(m_CellEulerAnglesPtr.lock()->getNumberOfTuples());

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  float conversionFactor = 1.0f;
  if (m_ConversionType == DREAM3D::EulerAngleConversionType::DegreesToRadians)
  {
    conversionFactor = static_cast<float>( M_PI / 180.0f );
  }
  else if (conversionFactor == DREAM3D::EulerAngleConversionType::RadiansToDegrees)
  {
    conversionFactor = static_cast<float>( 180.0f / M_PI );
  }

  totalPoints *= 3;
  //  qDebug() << "ChangeAngleRepresentation: " << m_ConversionFactor << "\n";
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      ChangeAngleRepresentationImpl(m_CellEulerAngles, conversionFactor), tbb::auto_partitioner());
  }
  else
#endif
  {
    ChangeAngleRepresentationImpl serial(m_CellEulerAngles, conversionFactor);
    serial.convert(0, totalPoints);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ChangeAngleRepresentation::newFilterInstance(bool copyFilterParameters)
{
  ChangeAngleRepresentation::Pointer filter = ChangeAngleRepresentation::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ChangeAngleRepresentation::getCompiledLibraryName()
{ return OrientationAnalysisConstants::OrientationAnalysisBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ChangeAngleRepresentation::getGroupName()
{ return DREAM3D::FilterGroups::OrientationAnalysisFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ChangeAngleRepresentation::getSubGroupName()
{ return DREAM3D::FilterSubGroups::ConversionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ChangeAngleRepresentation::getHumanLabel()
{ return "Convert Angles to Degrees or Radians"; }
