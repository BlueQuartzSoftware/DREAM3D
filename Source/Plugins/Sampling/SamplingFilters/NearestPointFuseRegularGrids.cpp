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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "NearestPointFuseRegularGrids.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Sampling/SamplingConstants.h"
#include "Sampling/SamplingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NearestPointFuseRegularGrids::NearestPointFuseRegularGrids() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NearestPointFuseRegularGrids::~NearestPointFuseRegularGrids() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Reference Cell Attribute Matrix", ReferenceCellAttributeMatrixPath, FilterParameter::Category::RequiredArray, NearestPointFuseRegularGrids, req));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Sampling Cell Attribute Matrix", SamplingCellAttributeMatrixPath, FilterParameter::Category::RequiredArray, NearestPointFuseRegularGrids, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setReferenceCellAttributeMatrixPath(reader->readDataArrayPath("ReferenceCellAttributeMatrixPath", getReferenceCellAttributeMatrixPath()));
  setSamplingCellAttributeMatrixPath(reader->readDataArrayPath("SamplingCellAttributeMatrixPath", getSamplingCellAttributeMatrixPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getReferenceCellAttributeMatrixPath().getDataContainerName());
  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getSamplingCellAttributeMatrixPath().getDataContainerName());

  AttributeMatrix::Pointer refAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, getReferenceCellAttributeMatrixPath(), -301);
  AttributeMatrix::Pointer sampleAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, getSamplingCellAttributeMatrixPath(), -301);
  if(getErrorCode() < 0)
  {
    return;
  }

  // Create arrays on the reference grid to hold data present on the sampling grid
  QList<QString> sampleArrayNames = sampleAttrMat->getAttributeArrayNames();
  QList<QString> refArrayNames = refAttrMat->getAttributeArrayNames();
  for(QList<QString>::iterator iter = sampleArrayNames.begin(); iter != sampleArrayNames.end(); ++iter)
  {
    tempPath.update(getReferenceCellAttributeMatrixPath().getDataContainerName(), getReferenceCellAttributeMatrixPath().getAttributeMatrixName(), *iter);
    IDataArray::Pointer tmpDataArray = sampleAttrMat->getPrereqIDataArray(this, *iter, -90001);
    if(getErrorCode() >= 0)
    {
      if(refArrayNames.contains(*iter))
      {
        QString ss = QObject::tr("There is already an attribute array with the name %1 in the reference attribute matrix").arg(*iter);
        setErrorCondition(-5559, ss);
      }
      else
      {
        std::vector<size_t> cDims = tmpDataArray->getComponentDimensions();
        TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, tmpDataArray);
      }
    }
  }

  // Get the list of all attribute matrices in the sampling data container and add them to the reference data container if they are feature or ensemble type
  DataContainer::Pointer mR = getDataContainerArray()->getDataContainer(getReferenceCellAttributeMatrixPath().getDataContainerName());
  DataContainer::Pointer mS = getDataContainerArray()->getDataContainer(getSamplingCellAttributeMatrixPath().getDataContainerName());
  QList<QString> m_AttrMatList = mS->getAttributeMatrixNames();
  AttributeMatrix::Type tempAttrMatType = AttributeMatrix::Type::Vertex;

  QList<QString> refAttrMatNames = mR->getAttributeMatrixNames();

  // Loop through all the attribute matrices in the sampling data container
  // We are only copying feature/ensemble attribute matrices here with a deep copy.
  for(QList<QString>::Iterator it = m_AttrMatList.begin(); it != m_AttrMatList.end(); ++it)
  {
    AttributeMatrix::Pointer tmpAttrMat = mS->getPrereqAttributeMatrix(this, *it, -301);
    if(getErrorCode() >= 0)
    {
      tempAttrMatType = tmpAttrMat->getType();
      if(tempAttrMatType > AttributeMatrix::Type::Cell)
      {
        if(refAttrMatNames.contains(tmpAttrMat->getName()))
        {
          QString ss = QObject::tr("There is already an attribute matrix with the name %1 in the reference data container").arg(tmpAttrMat->getName());
          setErrorCondition(-5559, ss);
        }
        else
        {
          AttributeMatrix::Pointer attrMat = tmpAttrMat->deepCopy(getInPreflight());
          mR->addOrReplaceAttributeMatrix(attrMat);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer refDC = getDataContainerArray()->getDataContainer(m_ReferenceCellAttributeMatrixPath.getDataContainerName());
  DataContainer::Pointer sampleDC = getDataContainerArray()->getDataContainer(m_SamplingCellAttributeMatrixPath.getDataContainerName());

  AttributeMatrix::Pointer refAttrMat = refDC->getAttributeMatrix(m_ReferenceCellAttributeMatrixPath.getAttributeMatrixName());
  AttributeMatrix::Pointer sampleAttrMat = sampleDC->getAttributeMatrix(m_SamplingCellAttributeMatrixPath.getAttributeMatrixName());

  // Get dimensions and resolutions of two grids
  SizeVec3Type _refDims = refDC->getGeometryAs<ImageGeom>()->getDimensions();
  SizeVec3Type _sampleDims = sampleDC->getGeometryAs<ImageGeom>()->getDimensions();
  FloatVec3Type refRes = refDC->getGeometryAs<ImageGeom>()->getSpacing();
  FloatVec3Type sampleRes = sampleDC->getGeometryAs<ImageGeom>()->getSpacing();
  FloatVec3Type refOrigin = refDC->getGeometryAs<ImageGeom>()->getOrigin();
  FloatVec3Type sampleOrigin = sampleDC->getGeometryAs<ImageGeom>()->getOrigin();

  // Further down we divide by sampleRes, so here check to make sure that no components of the resolution are 0
  // This would be incredible unusual behavior if it were to occur, hence why we don't spend the time
  // doing the validation up in the dataCheck
  bool zeroRes = false;
  for(size_t i = 0; i < 3; i++)
  {
    if(sampleRes[i] == 0.0f)
    {
      zeroRes = true;
      break;
    }
  }

  if(zeroRes)
  {
    QString ss = QObject::tr("A component of the resolution for the Image Geometry associated with DataContainer '%1' is 0. This would result in a division by 0 operation")
                     .arg(m_SamplingCellAttributeMatrixPath.getDataContainerName());
    setErrorCondition(-5555, ss);
    return;
  }

  int64_t refDims[3] = {0, 0, 0};
  int64_t sampleDims[3] = {0, 0, 0};
  for(size_t i = 0; i < 3; i++)
  {
    refDims[i] = static_cast<int64_t>(_refDims[i]);
    sampleDims[i] = static_cast<int64_t>(_sampleDims[i]);
  }

  int64_t numRefTuples = refDims[0] * refDims[1] * refDims[2];

  float x = 0.0f, y = 0.0f, z = 0.0f;
  int64_t col = 0, row = 0, plane = 0;
  int64_t refIndex = 0;
  int64_t sampleIndex = 0;
  int64_t planeComp = 0, rowComp = 0;

  // Create arrays on the reference grid to hold data present on the sampling grid
  QList<QString> voxelArrayNames = sampleAttrMat->getAttributeArrayNames();
  for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    IDataArray::Pointer p = sampleAttrMat->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name. At least in theory
    IDataArray::Pointer data = p->createNewArray(numRefTuples, p->getComponentDimensions(), p->getName());
    refAttrMat->insertOrAssign(data);
  }

  bool outside = false;
  for(int64_t i = 0; i < refDims[2]; i++)
  {
    planeComp = i * refDims[0] * refDims[1];
    for(int64_t j = 0; j < refDims[1]; j++)
    {
      rowComp = j * refDims[0];
      for(int64_t k = 0; k < refDims[0]; k++)
      {
        outside = false;
        x = (k * refRes[0] + refOrigin[0]);
        y = (j * refRes[1] + refOrigin[1]);
        z = (i * refRes[2] + refOrigin[2]);
        if((x - sampleOrigin[0]) < 0)
        {
          outside = true;
        }
        else
        {
          col = int64_t((x - sampleOrigin[0]) / sampleRes[0]);
        }
        if((y - sampleOrigin[1]) < 0)
        {
          outside = true;
        }
        else
        {
          row = int64_t((y - sampleOrigin[1]) / sampleRes[1]);
        }
        if((z - sampleOrigin[2]) < 0)
        {
          outside = true;
        }
        else
        {
          plane = int64_t((z - sampleOrigin[2]) / sampleRes[2]);
        }
        if(col >= sampleDims[0] || row >= sampleDims[1] || plane >= sampleDims[2])
        {
          outside = true;
        }
        if(!outside)
        {
          sampleIndex = (plane * sampleDims[0] * sampleDims[1]) + (row * sampleDims[0]) + col;
          refIndex = planeComp + rowComp + k;
          for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            IDataArray::Pointer p = sampleAttrMat->getAttributeArray(*iter);
            // Make a copy of the 'p' array that has the same name. When placed into
            // the data container this will over write the current array with
            // the same name. At least in theory
            IDataArray::Pointer data = refAttrMat->getAttributeArray(*iter);
            void* source = nullptr;
            void* destination = nullptr;
            int nComp = data->getNumberOfComponents();

            source = p->getVoidPointer((nComp * sampleIndex));
            destination = data->getVoidPointer((nComp * refIndex));
            ::memcpy(destination, source, p->getTypeSize() * nComp);
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer NearestPointFuseRegularGrids::newFilterInstance(bool copyFilterParameters) const
{
  NearestPointFuseRegularGrids::Pointer filter = NearestPointFuseRegularGrids::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NearestPointFuseRegularGrids::getCompiledLibraryName() const
{
  return SamplingConstants::SamplingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NearestPointFuseRegularGrids::getBrandingString() const
{
  return "Sampling";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NearestPointFuseRegularGrids::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Sampling::Version::Major() << "." << Sampling::Version::Minor() << "." << Sampling::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NearestPointFuseRegularGrids::getGroupName() const
{
  return SIMPL::FilterGroups::SamplingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid NearestPointFuseRegularGrids::getUuid() const
{
  return QUuid("{cbaf9e68-5ded-560c-9440-509289100ea8}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NearestPointFuseRegularGrids::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ResolutionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString NearestPointFuseRegularGrids::getHumanLabel() const
{
  return "Fuse Regular Grids (Nearest Point)";
}

// -----------------------------------------------------------------------------
NearestPointFuseRegularGrids::Pointer NearestPointFuseRegularGrids::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<NearestPointFuseRegularGrids> NearestPointFuseRegularGrids::New()
{
  struct make_shared_enabler : public NearestPointFuseRegularGrids
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString NearestPointFuseRegularGrids::getNameOfClass() const
{
  return QString("NearestPointFuseRegularGrids");
}

// -----------------------------------------------------------------------------
QString NearestPointFuseRegularGrids::ClassName()
{
  return QString("NearestPointFuseRegularGrids");
}

// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::setReferenceCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_ReferenceCellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath NearestPointFuseRegularGrids::getReferenceCellAttributeMatrixPath() const
{
  return m_ReferenceCellAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::setSamplingCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_SamplingCellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath NearestPointFuseRegularGrids::getSamplingCellAttributeMatrixPath() const
{
  return m_SamplingCellAttributeMatrixPath;
}
