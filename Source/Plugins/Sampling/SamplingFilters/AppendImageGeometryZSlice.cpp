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

#include "AppendImageGeometryZSlice.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppendImageGeometryZSlice::AppendImageGeometryZSlice()
: m_CheckResolution(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AppendImageGeometryZSlice::~AppendImageGeometryZSlice() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Input Cell Data", InputAttributeMatrix, FilterParameter::RequiredArray, AppendImageGeometryZSlice, req));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Destination Cell Data", DestinationAttributeMatrix, FilterParameter::RequiredArray, AppendImageGeometryZSlice, req));
  }

  parameters.push_back(SIMPL_NEW_BOOL_FP("Check Spacing", CheckResolution, FilterParameter::Parameter, AppendImageGeometryZSlice));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputAttributeMatrix(reader->readDataArrayPath("InputAttributeMatrix", getInputAttributeMatrix()));
  setDestinationAttributeMatrix(reader->readDataArrayPath("DestinationAttributeMatrix", getDestinationAttributeMatrix()));
  setCheckResolution(reader->readValue("CheckResolution", getCheckResolution()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  // Validate the Source & Destination Attribute Matrix are available
  AttributeMatrix::Pointer inputCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getInputAttributeMatrix(), -8201);
  if(getErrorCode() < 0)
  {
    return;
  }

  AttributeMatrix::Pointer destCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getDestinationAttributeMatrix(), -8200);
  if(getErrorCode() < 0)
  {
    return;
  }

  // Validate each AttributeMatrix is associated with an Image Geometry.
  ImageGeom::Pointer inputGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getInputAttributeMatrix().getDataContainerName());
  if(nullptr == inputGeometry.get() || getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer destGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getDestinationAttributeMatrix().getDataContainerName());
  if(nullptr == destGeometry.get() || getErrorCode() < 0)
  {
    return;
  }

  // Get the Dimensions of the ImageGeometries

  SizeVec3Type inputGeomDims = inputGeometry->getDimensions();

  SizeVec3Type destGeomDims = destGeometry->getDimensions();

  if(getCheckResolution())
  {
    FloatVec3Type inputRes = inputGeometry->getSpacing();

    FloatVec3Type destRes = destGeometry->getSpacing();

    if(inputRes[0] != destRes[0])
    {
      QString ss = QObject::tr("Input X Spacing (%1) not equal to Destination X Spacing (%2)").arg(inputRes[0]).arg(destRes[0]);
      setErrorCondition(-8205, ss);
    }
    if(inputRes[1] != destRes[1])
    {
      QString ss = QObject::tr("Input Y Spacing (%1) not equal to Destination Y Spacing (%2)").arg(inputRes[1]).arg(destRes[1]);
      setErrorCondition(-8206, ss);
    }
    if(inputRes[2] != destRes[2])
    {
      QString ss = QObject::tr("Input Z Spacing (%1) not equal to Destination Z Spacing (%2)").arg(inputRes[2]).arg(destRes[2]);
      setErrorCondition(-8207, ss);
    }
  }

  if(destGeomDims[0] != inputGeomDims[0])
  {
    QString ss = QObject::tr("Input X Dim (%1) not equal to Destination X Dim (%2)").arg(inputGeomDims[0]).arg(destGeomDims[0]);
    setErrorCondition(-8202, ss);
  }

  if(destGeomDims[0] != inputGeomDims[0])
  {
    QString ss = QObject::tr("Input Y Dim (%1) not equal to Destination Y Dim (%2)").arg(inputGeomDims[1]).arg(destGeomDims[1]);
    setErrorCondition(-8203, ss);
  }

  if(getInPreflight())
  {
    // We are only appending in the Z direction
    destGeomDims[2] = destGeomDims[2] + inputGeomDims[2];

    // Update the existing z dimension of the image geometry and set that value back into the Image Geometry
    destGeometry->setDimensions(destGeomDims);

    size_t totalPoints = 1;
    for(int i = 0; i < 3; i++)
    {
      if(destGeomDims[i] != 0)
      {
        totalPoints *= destGeomDims[i];
      }
    }
    std::vector<size_t> dgd = {destGeomDims[0], destGeomDims[1], destGeomDims[2]};
    AttributeMatrix::Pointer newCellAttrMat = AttributeMatrix::New(dgd, destCellAttrMat->getName(), destCellAttrMat->getType());
    // Create new DataArrays with the new sizes and place into a temp AttributeMatrix which
    // will be inserted into the data container as a replacement for the existing
    // AttributeMatrix
    QList<QString> voxelArrayNames = destCellAttrMat->getAttributeArrayNames();
    for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
    {
      IDataArray::Pointer p = destCellAttrMat->getAttributeArray(*iter);
      //
      IDataArray::Pointer data = p->createNewArray(totalPoints, p->getComponentDimensions(), p->getName(), false);

      destCellAttrMat->removeAttributeArray(*iter);
      newCellAttrMat->insertOrAssign(data);
    }
    getDataContainerArray()->getDataContainer(getDestinationAttributeMatrix().getDataContainerName())->removeAttributeMatrix(destCellAttrMat->getName());
    getDataContainerArray()->getDataContainer(getDestinationAttributeMatrix().getDataContainerName())->addOrReplaceAttributeMatrix(newCellAttrMat);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::preflight()
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
void AppendImageGeometryZSlice::execute()
{
  clearErrorCode();
  clearWarningCode();

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // Validate each AttributeMatrix is associated with an Image Geometry.
  ImageGeom::Pointer inputGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getInputAttributeMatrix().getDataContainerName());

  ImageGeom::Pointer destGeometry = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getDestinationAttributeMatrix().getDataContainerName());

  AttributeMatrix::Pointer inputCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getInputAttributeMatrix(), -8201);

  AttributeMatrix::Pointer destCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getDestinationAttributeMatrix(), -8200);

  SizeVec3Type inputGeomDims = inputGeometry->getDimensions();

  SizeVec3Type destGeomDims = destGeometry->getDimensions();

  size_t tupleOffset = 1;
  for(int i = 0; i < 3; i++)
  {
    if(destGeomDims[i] != 0)
    {
      tupleOffset *= destGeomDims[i];
    }
  }

  // We are only appending in the Z direction
  destGeomDims[2] = destGeomDims[2] + inputGeomDims[2];

  // Now update the geometry with new dimension
  // Update the existing z dimension of the image geometry and set that value back into the Image Geometry
  destGeometry->setDimensions(destGeomDims);

  std::vector<size_t> dgd = {destGeomDims[0], destGeomDims[1], destGeomDims[2]};
  destCellAttrMat->resizeAttributeArrays(dgd);

  QList<QString> voxelArrayNames = destCellAttrMat->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = destCellAttrMat->getAttributeArray(*iter);
    IDataArray::Pointer inputArray = inputCellAttrMat->getAttributeArray(*iter);
    if(nullptr != inputArray.get())
    {
      p->copyFromArray(tupleOffset, inputArray);
    }
    else
    {
      QString ss = QObject::tr("Data Array '%1' does not exist in the Input Cell AttributeMatrix.").arg(*iter);
      setWarningCondition(-8203, ss);
    }
  }

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AppendImageGeometryZSlice::newFilterInstance(bool copyFilterParameters) const
{
  AppendImageGeometryZSlice::Pointer filter = AppendImageGeometryZSlice::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AppendImageGeometryZSlice::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AppendImageGeometryZSlice::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AppendImageGeometryZSlice::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AppendImageGeometryZSlice::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid AppendImageGeometryZSlice::getUuid() const
{
  return QUuid("{52b2918a-4fb5-57aa-97d4-ccc084b89572}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AppendImageGeometryZSlice::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AppendImageGeometryZSlice::getHumanLabel() const
{
  return "Append Z Slice (Image Geometry)";
}

// -----------------------------------------------------------------------------
AppendImageGeometryZSlice::Pointer AppendImageGeometryZSlice::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<AppendImageGeometryZSlice> AppendImageGeometryZSlice::New()
{
  struct make_shared_enabler : public AppendImageGeometryZSlice
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString AppendImageGeometryZSlice::getNameOfClass() const
{
  return QString("AppendImageGeometryZSlice");
}

// -----------------------------------------------------------------------------
QString AppendImageGeometryZSlice::ClassName()
{
  return QString("AppendImageGeometryZSlice");
}

// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::setInputAttributeMatrix(const DataArrayPath& value)
{
  m_InputAttributeMatrix = value;
}

// -----------------------------------------------------------------------------
DataArrayPath AppendImageGeometryZSlice::getInputAttributeMatrix() const
{
  return m_InputAttributeMatrix;
}

// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::setDestinationAttributeMatrix(const DataArrayPath& value)
{
  m_DestinationAttributeMatrix = value;
}

// -----------------------------------------------------------------------------
DataArrayPath AppendImageGeometryZSlice::getDestinationAttributeMatrix() const
{
  return m_DestinationAttributeMatrix;
}

// -----------------------------------------------------------------------------
void AppendImageGeometryZSlice::setCheckResolution(bool value)
{
  m_CheckResolution = value;
}

// -----------------------------------------------------------------------------
bool AppendImageGeometryZSlice::getCheckResolution() const
{
  return m_CheckResolution;
}
