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

#include "MultiDataArraySelectionFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::MultiDataArraySelectionFilterParameter() :
  FilterParameter(),
  m_DefaultPaths(QVector<DataArrayPath>())
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::~MultiDataArraySelectionFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::Pointer MultiDataArraySelectionFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const QVector<DataArrayPath>& defaultValue, Category category, const RequirementType req, int groupIndex)
{

  MultiDataArraySelectionFilterParameter::Pointer ptr = MultiDataArraySelectionFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setDefaultGeometryTypes(req.dcGeometryTypes);
  ptr->setDefaultAttributeMatrixTypes(req.amTypes);
  ptr->setDefaultAttributeArrayTypes(req.daTypes);
  ptr->setDefaultComponentDimensions(req.componentDimensions);
  ptr->setGroupIndex(groupIndex);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiDataArraySelectionFilterParameter::getWidgetType()
{
  return QString("MultiDataArraySelectionWidget");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::RequirementType MultiDataArraySelectionFilterParameter::CreateCategoryRequirement(const QString& primitiveType,
    size_t allowedCompDim,
    unsigned int attributeMatrixCategory)
{
  typedef QVector<size_t> QVectorOfSizeType;
  MultiDataArraySelectionFilterParameter::RequirementType req;
  QVector<unsigned int> amTypes;
  if(attributeMatrixCategory == DREAM3D::AttributeMatrixObjectType::Element)
  {
    amTypes.push_back(DREAM3D::AttributeMatrixType::Cell);
    amTypes.push_back(DREAM3D::AttributeMatrixType::Face);
    amTypes.push_back(DREAM3D::AttributeMatrixType::Edge);
    amTypes.push_back(DREAM3D::AttributeMatrixType::Vertex);
  }
  else if(attributeMatrixCategory == DREAM3D::AttributeMatrixObjectType::Feature)
  {
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexFeature);
  }
  else if(attributeMatrixCategory == DREAM3D::AttributeMatrixObjectType::Ensemble)
  {
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexEnsemble);
  }
  req.amTypes = amTypes;
  if(primitiveType.compare(DREAM3D::Defaults::AnyPrimitive) != 0)
  {
    req.daTypes = QVector<QString>(1, primitiveType);
  }
  if(DREAM3D::Defaults::AnyComponentSize != allowedCompDim)
  {
    req.componentDimensions = QVector<QVectorOfSizeType>(1, QVectorOfSizeType(1, allowedCompDim));
  }
//  if(DREAM3D::GeometryType::UnknownGeometry != geometryType)
//  {
//    req.dcGeometryTypes = QVector<unsigned int>(1, geometryType);
//  }
  return req;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiDataArraySelectionFilterParameter::RequirementType MultiDataArraySelectionFilterParameter::CreateRequirement(const QString& primitiveType,
                                                                                                                  size_t allowedCompDim,
                                                                                                                  unsigned int attributeMatrixType,
                                                                                                                  unsigned int geometryType)
{
  typedef QVector<size_t> QVectorOfSizeType;
  MultiDataArraySelectionFilterParameter::RequirementType req;
  if(primitiveType.compare(DREAM3D::Defaults::AnyPrimitive) != 0)
  {
    req.daTypes = QVector<QString>(1, primitiveType);
  }
  if(DREAM3D::Defaults::AnyComponentSize != allowedCompDim)
  {
    req.componentDimensions = QVector<QVectorOfSizeType>(1, QVectorOfSizeType(1, allowedCompDim));
  }
  if(DREAM3D::Defaults::AnyAttributeMatrix != attributeMatrixType)
  {
    QVector<unsigned int> amTypes(1, attributeMatrixType);
    req.amTypes = amTypes;
  }
  if(DREAM3D::Defaults::AnyGeometry != geometryType)
  {
    req.dcGeometryTypes = QVector<unsigned int>(1, geometryType);
  }
  return req;
}
