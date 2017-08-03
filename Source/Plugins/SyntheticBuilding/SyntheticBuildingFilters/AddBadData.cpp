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

#include "AddBadData.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

// Include the MOC generated file for this class
#include "moc_AddBadData.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddBadData::AddBadData()
: AbstractFilter()
, m_GBEuclideanDistancesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::ElementAttributeMatrixName, SIMPL::CellData::GBEuclideanDistances)
, m_PoissonNoise(true)
, m_PoissonVolFraction(0.0f)
, m_BoundaryNoise(false)
, m_BoundaryVolFraction(0.0f)
, m_GBEuclideanDistances(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AddBadData::~AddBadData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps("PoissonVolFraction");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Add Random Noise", PoissonNoise, FilterParameter::Parameter, AddBadData, linkedProps));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Volume Fraction of Random Noise", PoissonVolFraction, FilterParameter::Parameter, AddBadData));
  linkedProps.clear();
  linkedProps << "BoundaryVolFraction";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Add Boundary Noise", BoundaryNoise, FilterParameter::Parameter, AddBadData, linkedProps));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Volume Fraction of Boundary Noise", BoundaryVolFraction, FilterParameter::Parameter, AddBadData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Boundary Euclidean Distances", GBEuclideanDistancesArrayPath, FilterParameter::RequiredArray, AddBadData, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGBEuclideanDistancesArrayPath(reader->readDataArrayPath("GBEuclideanDistancesArrayPath", getGBEuclideanDistancesArrayPath()));
  setPoissonNoise(reader->readValue("PoissonNoise", getPoissonNoise()));
  setPoissonVolFraction(reader->readValue("PoissonVolFraction", getPoissonVolFraction()));
  setBoundaryNoise(reader->readValue("BoundaryNoise", getBoundaryNoise()));
  setBoundaryVolFraction(reader->readValue("BoundaryVolFraction", getBoundaryVolFraction()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getGBEuclideanDistancesArrayPath().getDataContainerName());

  if((m_PoissonNoise == false) && (m_BoundaryNoise == false))
  {
    QString ss = QObject::tr("At least one type of noise must be selected");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> cDims(1, 1);
  m_GBEuclideanDistancesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getGBEuclideanDistancesArrayPath(),
                                                                                                                  cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_GBEuclideanDistancesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_GBEuclideanDistances = m_GBEuclideanDistancesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::preflight()
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
void AddBadData::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  add_noise();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddBadData::add_noise()
{
  notifyStatusMessage(getHumanLabel(), "Adding Noise");
  SIMPL_RANDOMNG_NEW()

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getGBEuclideanDistancesArrayPath().getDataContainerName());

  QString attMatName = getGBEuclideanDistancesArrayPath().getAttributeMatrixName();
  QList<QString> voxelArrayNames = m->getAttributeMatrix(attMatName)->getAttributeArrayNames();

  float random = 0.0f;
  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
  for(size_t i = 0; i < totalPoints; ++i)
  {
    if(m_BoundaryNoise == true && m_GBEuclideanDistances[i] < 1)
    {
      random = static_cast<float>(rg.genrand_res53());
      if(random < m_BoundaryVolFraction)
      {
        for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          IDataArray::Pointer p = m->getAttributeMatrix(attMatName)->getAttributeArray(*iter);
          int var = 0;
          p->initializeTuple(i, &var);
        }
      }
    }
    if(m_PoissonNoise == true)
    {
      random = static_cast<float>(rg.genrand_res53());
      if(random < m_PoissonVolFraction)
      {
        for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
        {
          IDataArray::Pointer p = m->getAttributeMatrix(attMatName)->getAttributeArray(*iter);
          int var = 0;
          p->initializeTuple(i, &var);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AddBadData::newFilterInstance(bool copyFilterParameters)
{
  AddBadData::Pointer filter = AddBadData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddBadData::getCompiledLibraryName()
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddBadData::getBrandingString()
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddBadData::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddBadData::getGroupName()
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddBadData::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AddBadData::getHumanLabel()
{
  return "Add Bad Data";
}
