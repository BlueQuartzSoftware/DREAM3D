/* ============================================================================
* Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
* All rights reserved.
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
* BlueQuartz Software nor the names of its contributors may be used to endorse
* or promote products derived from this software without specific prior written
* permission.
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
*  This code was written under United States Air Force Contract number
*                           FA8650-10-D-5210
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
  const QVector<DataArrayPath>& defaultValue, Category category, const DataStructureRequirements req, int groupIndex)
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
MultiDataArraySelectionFilterParameter::DataStructureRequirements MultiDataArraySelectionFilterParameter::CreateCategoryRequirement(const QString& primitiveType,
                                                                               size_t allowedCompDim,
                                                                               unsigned int attributeMatrixCategory)
{
  typedef QVector<size_t> QVectorOfSizeType;
  MultiDataArraySelectionFilterParameter::DataStructureRequirements req;
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
  if(primitiveType.isEmpty() == false)
  {
    req.daTypes = QVector<QString>(1, primitiveType);
  }
  if(std::numeric_limits<unsigned int>::max() != allowedCompDim)
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
MultiDataArraySelectionFilterParameter::DataStructureRequirements MultiDataArraySelectionFilterParameter::CreateRequirement(const QString& primitiveType,
                                                                               size_t allowedCompDim,
                                                                               unsigned int attributeMatrixType,
                                                                               unsigned int geometryType)
{
  typedef QVector<size_t> QVectorOfSizeType;
  MultiDataArraySelectionFilterParameter::DataStructureRequirements req;
  QVector<unsigned int> amTypes;
  amTypes.push_back(attributeMatrixType);
  req.amTypes = amTypes;
  if(primitiveType.isEmpty() == false)
  {
    req.daTypes = QVector<QString>(1, primitiveType);
  }
  if(std::numeric_limits<unsigned int>::max() != allowedCompDim)
  {
    req.componentDimensions = QVector<QVectorOfSizeType>(1, QVectorOfSizeType(1, allowedCompDim));
  }
  if(DREAM3D::GeometryType::UnknownGeometry != geometryType)
  {
    req.dcGeometryTypes = QVector<unsigned int>(1, geometryType);
  }
  return req;
}
