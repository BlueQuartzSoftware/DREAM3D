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

#include "FindEllipsoidError.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindEllipsoidError::FindEllipsoidError()
: m_CellFeatureAttributeMatrixName(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, "")
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_CentroidsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Centroids)
, m_NumCellsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::NumCells)
, m_AxisLengthsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AxisLengths)
, m_AxisEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AxisEulerAngles)
, m_IdealFeatureIdsArrayName("IdealFeatureIds")
, m_EllipsoidErrorArrayName("EllipsoidError")
, m_WriteIdealEllipseFeatureIds(true)
, m_ScaleFator(1.0f)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindEllipsoidError::~FindEllipsoidError() = default;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEllipsoidError::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  QStringList linkedProps("IdealFeatureIdsArrayName");
  parameters.push_back(
      SIMPL_NEW_LINKED_BOOL_FP("Write Ideal Ellipse Feature Ids (Caution LONG calculation)", WriteIdealEllipseFeatureIds, FilterParameter::Parameter, FindEllipsoidError, linkedProps));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeatureIds", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindEllipsoidError, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Centroids", CentroidsArrayPath, FilterParameter::RequiredArray, FindEllipsoidError, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("NumCells", NumCellsArrayPath, FilterParameter::RequiredArray, FindEllipsoidError, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Axis Eulers", AxisEulerAnglesArrayPath, FilterParameter::RequiredArray, FindEllipsoidError, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("AxisLengths", AxisLengthsArrayPath, FilterParameter::RequiredArray, FindEllipsoidError, req));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req =
        AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Feature Attribute Matrix Name", CellFeatureAttributeMatrixName, FilterParameter::RequiredArray, FindEllipsoidError, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("EllipsoidError", EllipsoidErrorArrayName, FilterParameter::CreatedArray, FindEllipsoidError));
  parameters.push_back(SIMPL_NEW_STRING_FP("IdealFeatureIds", IdealFeatureIdsArrayName, FilterParameter::CreatedArray, FindEllipsoidError));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEllipsoidError::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setAxisEulerAnglesArrayPath(reader->readDataArrayPath("AxisEulerAnglesArrayPath", getAxisEulerAnglesArrayPath()));
  setNumCellsArrayPath(reader->readDataArrayPath("NumCellsArrayPath", getNumCellsArrayPath()));
  setAxisLengthsArrayPath(reader->readDataArrayPath("AxisLengthsArrayPath", getAxisLengthsArrayPath()));
  setIdealFeatureIdsArrayName(reader->readString("IdealFeatureIdsArrayName", getIdealFeatureIdsArrayName()));
  setEllipsoidErrorArrayName(reader->readString("EllipsoidErrrorArrayName", getEllipsoidErrorArrayName()));
  setWriteIdealEllipseFeatureIds(reader->readValue("WriteIdealEllipseFeatureIds", getWriteIdealEllipseFeatureIds()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEllipsoidError::initialize()
{
  m_ScaleFator = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEllipsoidError::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();
  DataArrayPath tempPath;

  std::vector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                        /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCode() < 0 || nullptr == image.get())
  {
    return;
  }

  dims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(),
                                                                                                     dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CentroidsPtr.lock())                                                                      /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  m_AxisEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAxisEulerAnglesArrayPath(), dims);
  if(nullptr != m_AxisEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  m_AxisLengthsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAxisLengthsArrayPath(), dims);
  if(nullptr != m_AxisLengthsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  m_NumCellsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getNumCellsArrayPath(), dims);
  if(nullptr != m_NumCellsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NumCells = m_NumCellsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_WriteIdealEllipseFeatureIds)
  {
    dims[0] = 1;
    tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getIdealFeatureIdsArrayName());
    m_IdealFeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
        this, tempPath, 0, dims);                    /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_IdealFeatureIdsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_IdealFeatureIds = m_IdealFeatureIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  dims[0] = 1;
  tempPath.update(m_NumCellsArrayPath.getDataContainerName(), m_NumCellsArrayPath.getAttributeMatrixName(), getEllipsoidErrorArrayName());
  m_EllipsoidErrorPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, dims);                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_EllipsoidErrorPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_EllipsoidError = m_EllipsoidErrorPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEllipsoidError::preflight()
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
void FindEllipsoidError::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  FloatVec3Type spacing = m->getGeometryAs<ImageGeom>()->getSpacing();

  m_ScaleFator = 1.0 / spacing[0];
  if(spacing[1] > spacing[0] && spacing[1] > spacing[2])
  {
    m_ScaleFator = 1.0 / spacing[1];
  }
  if(spacing[2] > spacing[0] && spacing[2] > spacing[1])
  {
    m_ScaleFator = 1.0 / spacing[2];
  }

  if(m->getGeometryAs<ImageGeom>()->getXPoints() > 1 && m->getGeometryAs<ImageGeom>()->getYPoints() > 1 && m->getGeometryAs<ImageGeom>()->getZPoints() > 1)
  {
  }
  if(m->getGeometryAs<ImageGeom>()->getXPoints() == 1 || m->getGeometryAs<ImageGeom>()->getYPoints() == 1 || m->getGeometryAs<ImageGeom>()->getZPoints() == 1)
  {
    find_error2D();
  }

  notifyStatusMessage("FindEllipsoidError Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindEllipsoidError::find_error2D()
{
  size_t numfeatures = m_NumCellsPtr.lock()->getNumberOfTuples();
  std::vector<std::vector<size_t>> featureCellList(numfeatures);
  std::vector<size_t> numInsideIdeal(numfeatures, 0); // Create a list for the ideal ellipses
  std::vector<size_t> numOutsideIdeal(numfeatures, 0);

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  size_t xPoints = m->getGeometryAs<ImageGeom>()->getXPoints();
  size_t yPoints = m->getGeometryAs<ImageGeom>()->getYPoints();
  size_t zPoints = m->getGeometryAs<ImageGeom>()->getZPoints();

  FloatVec3Type spacing = m->getGeometryAs<ImageGeom>()->getSpacing();

  float xsquared, ysquared, asquared, bsquared, xc, yc, theta;
  int32_t xcoord, ycoord;

  size_t totalPoints = xPoints * yPoints * zPoints;

  for(size_t k = 0; k < totalPoints; k++)
  {
    featureCellList[m_FeatureIds[k]].push_back(k);

    //        //calculate the x and y coordinate for each cell in the actual feature
    //        xcoord = int(k % xPoints);
    //        ycoord = int(k/xPoints) % yPoints;
    //        for (size_t i = 1; i<numfeatures; i++)
    //        {
    //            theta = -m_AxisEulerAngles[3*i]; //only need the first angle in 2D

    //            //Get the centroids (in pixels) for the ideal ellipse
    //            xc = m_Centroids[3*i]/spacing[0];
    //            yc = m_Centroids[3*i + 1]/spacing[1];

    //            //Get the axis lengths for the ideal ellipse
    //            asquared = (m_AxisLengths[3*i]*m_AxisLengths[3*i])/(spacing[0]*spacing[0]);
    //            bsquared = m_AxisLengths[3*i+1]*m_AxisLengths[3*i+1]/(spacing[1]*spacing[1]);

    //            //rotate and translate the current x, y pair into where the ideal ellipse is
    //            xsquared = ((xcoord-xc)*cosf(theta)-(ycoord-yc)*sinf(theta))*((xcoord-xc)*cosf(theta)-(ycoord-yc)*sinf(theta));
    //            ysquared = ((xcoord-xc)*sinf(theta)+(ycoord-yc)*cosf(theta))*((xcoord-xc)*sinf(theta)+(ycoord-yc)*cosf(theta));

    //            if( (xsquared/asquared + ysquared/bsquared) < 1 )
    //            {
    //                m_IdealFeatureIds[k] = i;
    //            }

    //        }
  }

  for(size_t i = 1; i < numfeatures; i++)
  {
    theta = -m_AxisEulerAngles[3 * i]; // only need the first angle in 2D

    // Get the centroids (in pixels) for the ideal ellipse
    xc = m_Centroids[3 * i] / spacing[0];
    yc = m_Centroids[3 * i + 1] / spacing[1];

    // Get the axis lengths for the ideal ellipse
    asquared = (m_AxisLengths[3 * i] * m_AxisLengths[3 * i]) / (spacing[0] * spacing[0]);
    bsquared = m_AxisLengths[3 * i + 1] * m_AxisLengths[3 * i + 1] / (spacing[1] * spacing[1]);

    // iterate over all the cells in each feature
    for(size_t j = 0; j < featureCellList[i].size(); j++)
    {
      // calculate the x and y coordinate for each cell in the actual feature
      xcoord = int(featureCellList[i][j] % xPoints);
      ycoord = int(featureCellList[i][j] / xPoints) % yPoints;

      // rotate and translate the current x, y pair into where the ideal ellipse is
      xsquared = ((xcoord - xc) * cosf(theta) - (ycoord - yc) * sinf(theta)) * ((xcoord - xc) * cosf(theta) - (ycoord - yc) * sinf(theta));
      ysquared = ((xcoord - xc) * sinf(theta) + (ycoord - yc) * cosf(theta)) * ((xcoord - xc) * sinf(theta) + (ycoord - yc) * cosf(theta));

      if((xsquared / asquared + ysquared / bsquared) < 1)
      {
        numInsideIdeal[i]++;
      }
      else
      {
        numOutsideIdeal[i]++;
      }
    }

    m_EllipsoidError[i] = float(numOutsideIdeal[i]) / float(m_NumCells[i]);
  }

  std::cout << "done" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindEllipsoidError::newFilterInstance(bool copyFilterParameters) const
{
  FindEllipsoidError::Pointer filter = FindEllipsoidError::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindEllipsoidError::getCompiledLibraryName() const
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindEllipsoidError::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindEllipsoidError::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindEllipsoidError::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindEllipsoidError::getUuid() const
{
  return QUuid("{583e0789-090a-5de2-b8f6-f3ef5baeab59}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindEllipsoidError::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindEllipsoidError::getHumanLabel() const
{
  return "Find Ellipsoid Error";
}

// -----------------------------------------------------------------------------
FindEllipsoidError::Pointer FindEllipsoidError::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindEllipsoidError> FindEllipsoidError::New()
{
  struct make_shared_enabler : public FindEllipsoidError
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindEllipsoidError::getNameOfClass() const
{
  return QString("FindEllipsoidError");
}

// -----------------------------------------------------------------------------
QString FindEllipsoidError::ClassName()
{
  return QString("FindEllipsoidError");
}

// -----------------------------------------------------------------------------
void FindEllipsoidError::setCellFeatureAttributeMatrixName(const DataArrayPath& value)
{
  m_CellFeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindEllipsoidError::getCellFeatureAttributeMatrixName() const
{
  return m_CellFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void FindEllipsoidError::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindEllipsoidError::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindEllipsoidError::setCentroidsArrayPath(const DataArrayPath& value)
{
  m_CentroidsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindEllipsoidError::getCentroidsArrayPath() const
{
  return m_CentroidsArrayPath;
}

// -----------------------------------------------------------------------------
void FindEllipsoidError::setNumCellsArrayPath(const DataArrayPath& value)
{
  m_NumCellsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindEllipsoidError::getNumCellsArrayPath() const
{
  return m_NumCellsArrayPath;
}

// -----------------------------------------------------------------------------
void FindEllipsoidError::setAxisLengthsArrayPath(const DataArrayPath& value)
{
  m_AxisLengthsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindEllipsoidError::getAxisLengthsArrayPath() const
{
  return m_AxisLengthsArrayPath;
}

// -----------------------------------------------------------------------------
void FindEllipsoidError::setAxisEulerAnglesArrayPath(const DataArrayPath& value)
{
  m_AxisEulerAnglesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindEllipsoidError::getAxisEulerAnglesArrayPath() const
{
  return m_AxisEulerAnglesArrayPath;
}

// -----------------------------------------------------------------------------
void FindEllipsoidError::setIdealFeatureIdsArrayName(const QString& value)
{
  m_IdealFeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindEllipsoidError::getIdealFeatureIdsArrayName() const
{
  return m_IdealFeatureIdsArrayName;
}

// -----------------------------------------------------------------------------
void FindEllipsoidError::setEllipsoidErrorArrayName(const QString& value)
{
  m_EllipsoidErrorArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindEllipsoidError::getEllipsoidErrorArrayName() const
{
  return m_EllipsoidErrorArrayName;
}

// -----------------------------------------------------------------------------
void FindEllipsoidError::setWriteIdealEllipseFeatureIds(bool value)
{
  m_WriteIdealEllipseFeatureIds = value;
}

// -----------------------------------------------------------------------------
bool FindEllipsoidError::getWriteIdealEllipseFeatureIds() const
{
  return m_WriteIdealEllipseFeatureIds;
}
