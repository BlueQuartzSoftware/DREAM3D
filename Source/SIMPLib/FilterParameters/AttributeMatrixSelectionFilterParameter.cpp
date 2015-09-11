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

#include "AttributeMatrixSelectionFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionFilterParameter::AttributeMatrixSelectionFilterParameter() :
  FilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionFilterParameter::~AttributeMatrixSelectionFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionFilterParameter::Pointer AttributeMatrixSelectionFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const DataArrayPath& defaultValue, Category category, const RequirementType req, int groupIndex)
{

  AttributeMatrixSelectionFilterParameter::Pointer ptr = AttributeMatrixSelectionFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setDefaultGeometryTypes(req.dcGeometryTypes);
  ptr->setDefaultAttributeMatrixTypes(req.amTypes);
  ptr->setGroupIndex(groupIndex);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrixSelectionFilterParameter::getWidgetType()
{
  return QString("AttributeMatrixSelectionWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionFilterParameter::RequirementType AttributeMatrixSelectionFilterParameter::CreateRequirement(uint32_t attributeMatrixObjectType)
{
  typedef QVector<size_t> QVectorOfSizeType;
  AttributeMatrixSelectionFilterParameter::RequirementType req;
  QVector<unsigned int> amTypes;
  if(attributeMatrixObjectType == DREAM3D::AttributeMatrixObjectType::Element)
  {
    amTypes.push_back(DREAM3D::AttributeMatrixType::Cell);
    amTypes.push_back(DREAM3D::AttributeMatrixType::Face);
    amTypes.push_back(DREAM3D::AttributeMatrixType::Edge);
    amTypes.push_back(DREAM3D::AttributeMatrixType::Vertex);
  }
  else if(attributeMatrixObjectType == DREAM3D::AttributeMatrixObjectType::Feature)
  {
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexFeature);
  }
  else if(attributeMatrixObjectType == DREAM3D::AttributeMatrixObjectType::Ensemble)
  {
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexEnsemble);
  }
  req.amTypes = amTypes;

  return req;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionFilterParameter::RequirementType AttributeMatrixSelectionFilterParameter::CreateRequirement(uint32_t attributeMatrixType,
    uint32_t geometryType)
{
  typedef QVector<size_t> QVectorOfSizeType;
  AttributeMatrixSelectionFilterParameter::RequirementType req;
  if(DREAM3D::Defaults::AnyAttributeMatrix != attributeMatrixType)
  {
    req.amTypes = QVector<unsigned int>(1, attributeMatrixType);;
  }
  if(DREAM3D::Defaults::AnyGeometry != geometryType)
  {
    req.dcGeometryTypes = QVector<uint32_t>(1, geometryType);
  }
  return req;
}
