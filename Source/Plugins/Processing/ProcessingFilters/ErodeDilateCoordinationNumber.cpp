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

#include "ErodeDilateCoordinationNumber.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Processing/ProcessingConstants.h"
#include "Processing/ProcessingVersion.h"

// Include the MOC generated file for this class
#include "moc_ErodeDilateCoordinationNumber.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErodeDilateCoordinationNumber::ErodeDilateCoordinationNumber() :
  AbstractFilter(),
  m_Loop(false),
  m_CoordinationNumber(6),
  m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds),
  m_Neighbors(NULL),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErodeDilateCoordinationNumber::~ErodeDilateCoordinationNumber()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateCoordinationNumber::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Coordination Number to Consider", CoordinationNumber, FilterParameter::Parameter, ErodeDilateCoordinationNumber));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Loop Until Gone", Loop, FilterParameter::Parameter, ErodeDilateCoordinationNumber));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, ErodeDilateCoordinationNumber, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateCoordinationNumber::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setCoordinationNumber( reader->readValue("CoordinationNumber", getCoordinationNumber()) );
  setLoop( reader->readValue("Loop", getLoop()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateCoordinationNumber::initialize()
{
  m_Neighbors = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateCoordinationNumber::dataCheck()
{
  setErrorCondition(0);
  initialize();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  if (getCoordinationNumber() < 0 || getCoordinationNumber() > 6)
  {
    QString ss = QObject::tr("The coordination number (%1) must be on the interval [0,6]").arg(getCoordinationNumber());
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateCoordinationNumber::preflight()
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
void ErodeDilateCoordinationNumber::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValue(-1);

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] =
  {
    static_cast<int64_t>(udims[0]),
    static_cast<int64_t>(udims[1]),
    static_cast<int64_t>(udims[2]),
  };

  int32_t good = 1;
  int64_t point = 0;
  int64_t kstride = 0, jstride = 0;
  int32_t featurename = 0, feature = 0;
  int32_t coordination = 0;
  int32_t current = 0;
  int32_t most = 0;
  int64_t neighpoint = 0;
  size_t numfeatures = 0;

  for(size_t i = 0; i < totalPoints; i++)
  {
    featurename = m_FeatureIds[i];
    if (featurename > numfeatures) { numfeatures = featurename; }
  }

  int64_t neighpoints[6] = { 0, 0, 0, 0, 0, 0 };
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];

  QString attrMatName = m_FeatureIdsArrayPath.getAttributeMatrixName();
  QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();

  QVector<int32_t> n(numfeatures + 1, 0);
  QVector<int32_t> coordinationNumber(totalPoints, 0);
  bool keepgoing = true;
  int32_t counter = 1;

  while (counter > 0 && keepgoing == true)
  {
    counter = 0;
    if (m_Loop == false) { keepgoing = false; }

    for (int64_t k = 0; k < dims[2]; k++)
    {
      kstride = dims[0] * dims[1] * k;
      for (int64_t j = 0; j < dims[1]; j++)
      {
        jstride = dims[0] * j;
        for (int64_t i = 0; i < dims[0]; i++)
        {
          point = kstride + jstride + i;
          featurename = m_FeatureIds[point];
          coordination = 0;
          current = 0;
          most = 0;
          for (int32_t l = 0; l < 6; l++)
          {
            good = 1;
            neighpoint = point + neighpoints[l];
            if (l == 0 && k == 0) { good = 0; }
            if (l == 5 && k == (dims[2] - 1)) { good = 0; }
            if (l == 1 && j == 0) { good = 0; }
            if (l == 4 && j == (dims[1] - 1)) { good = 0; }
            if (l == 2 && i == 0) { good = 0; }
            if (l == 3 && i == (dims[0] - 1)) { good = 0; }
            if (good == 1)
            {
              feature = m_FeatureIds[neighpoint];
              if ((featurename > 0 && feature == 0) || (featurename == 0 && feature > 0))
              {
                coordination = coordination + 1;
                n[feature]++;
                current = n[feature];
                if (current > most)
                {
                  most = current;
                  m_Neighbors[point] = neighpoint;
                }
              }
            }
          }
          coordinationNumber[point] = coordination;
          int32_t neighbor = m_Neighbors[point];
          if (coordinationNumber[point] >= m_CoordinationNumber && coordinationNumber[point] > 0)
          {
            for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
            {
              IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
              p->copyTuple(neighbor, point);
            }
          }
          for (int32_t l = 0; l < 6; l++)
          {
            good = 1;
            neighpoint = point + neighpoints[l];
            if (l == 0 && k == 0) { good = 0; }
            if (l == 5 && k == (dims[2] - 1)) { good = 0; }
            if (l == 1 && j == 0) { good = 0; }
            if (l == 4 && j == (dims[1] - 1)) { good = 0; }
            if (l == 2 && i == 0) { good = 0; }
            if (l == 3 && i == (dims[0] - 1)) { good = 0; }
            if (good == 1)
            {
              feature = m_FeatureIds[neighpoint];
              if (feature > 0) { n[feature] = 0; }
            }
          }
        }
      }
    }
    for (int64_t k = 0; k < dims[2]; k++)
    {
      kstride = static_cast<int64_t>(dims[0] * dims[1] * k);
      for (int64_t j = 0; j < dims[1]; j++)
      {
        jstride = static_cast<int64_t>(dims[0] * j);
        for (int64_t i = 0; i < dims[0]; i++)
        {
          point = kstride + jstride + i;
          if (coordinationNumber[point] >= m_CoordinationNumber)
          {
            counter++;
          }
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ErodeDilateCoordinationNumber::newFilterInstance(bool copyFilterParameters)
{
  ErodeDilateCoordinationNumber::Pointer filter = ErodeDilateCoordinationNumber::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateCoordinationNumber::getCompiledLibraryName()
{
  return ProcessingConstants::ProcessingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateCoordinationNumber::getBrandingString()
{
  return "Processing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateCoordinationNumber::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  Processing::Version::Major() << "." << Processing::Version::Minor() << "." << Processing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateCoordinationNumber::getGroupName()
{ return SIMPL::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateCoordinationNumber::getSubGroupName()
{ return SIMPL::FilterSubGroups::CleanupFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateCoordinationNumber::getHumanLabel()
{ return "Smooth Bad Data (Coordination Number)"; }

