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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "HierarchicalSmooth.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

struct HierarchicalSmooth::Impl
{
  UInt64ArrayType::ConstWeakPointer m_TriList;
  FloatArrayType::ConstWeakPointer m_VertexList;
  Int32ArrayType::ConstWeakPointer m_FaceLabelsList;
  Int32ArrayType::ConstWeakPointer m_NodeTypesList;

  FloatArrayType::WeakPointer m_SmoothedVertexList;

  Impl() = default;

  ~Impl() = default;

  Impl(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl& operator=(Impl&&) = delete;

  void resetDataArrays()
  {
    m_TriList.reset();
    m_VertexList.reset();
    m_FaceLabelsList.reset();
    m_NodeTypesList.reset();
    m_SmoothedVertexList.reset();
  }
};

// -----------------------------------------------------------------------------
HierarchicalSmooth::HierarchicalSmooth()
: p_Impl(std::make_unique<Impl>())
{
  initialize();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
HierarchicalSmooth::~HierarchicalSmooth() = default;

// -----------------------------------------------------------------------------
void HierarchicalSmooth::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt64, 3, AttributeMatrix::Type::Any, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Triangle List", TriListPath, FilterParameter::Category::RequiredArray, HierarchicalSmooth, req));
  }

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::Any, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Vertex List", VertexListPath, FilterParameter::Category::RequiredArray, HierarchicalSmooth, req));
  }

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", FaceLabelsPath, FilterParameter::Category::RequiredArray, HierarchicalSmooth, req));
  }

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Vertex, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Node Types", NodeTypesPath, FilterParameter::Category::RequiredArray, HierarchicalSmooth, req));
  }

  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Any, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Smoothed Vertex List", SmoothedVertexListPath, FilterParameter::Category::CreatedArray, HierarchicalSmooth, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  p_Impl->resetDataArrays();
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getCancel())
  {
    return;
  }
}

// -----------------------------------------------------------------------------
AbstractFilter::Pointer HierarchicalSmooth::newFilterInstance(bool copyFilterParameters) const
{
  HierarchicalSmooth::Pointer filter = HierarchicalSmooth::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
QString HierarchicalSmooth::getCompiledLibraryName() const
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
QString HierarchicalSmooth::getBrandingString() const
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
QString HierarchicalSmooth::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
QString HierarchicalSmooth::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
QString HierarchicalSmooth::getSubGroupName() const
{
  return SurfaceMeshingConstants::SurfaceMeshingPluginDisplayName;
}

// -----------------------------------------------------------------------------
QString HierarchicalSmooth::getHumanLabel() const
{
  return "Hierarchical Smooth";
}

// -----------------------------------------------------------------------------
QUuid HierarchicalSmooth::getUuid() const
{
  return QUuid("{c9aec6ea-46a2-51c8-9e64-1f65dc3ce3e1}");
}

// -----------------------------------------------------------------------------
HierarchicalSmooth::Pointer HierarchicalSmooth::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
HierarchicalSmooth::Pointer HierarchicalSmooth::New()
{
  struct make_shared_enabler : public HierarchicalSmooth
  {
  };
  return std::make_shared<make_shared_enabler>();
}

// -----------------------------------------------------------------------------
QString HierarchicalSmooth::getNameOfClass() const
{
  return ClassName();
}

// -----------------------------------------------------------------------------
QString HierarchicalSmooth::ClassName()
{
  return QString("HierarchicalSmooth");
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::setTriListPath(const DataArrayPath& value)
{
  m_TriListPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath HierarchicalSmooth::getTriListPath() const
{
  return m_TriListPath;
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::setVertexListPath(const DataArrayPath& value)
{
  m_VertexListPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath HierarchicalSmooth::getVertexListPath() const
{
  return m_VertexListPath;
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::setFaceLabelsPath(const DataArrayPath& value)
{
  m_FaceLabelsPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath HierarchicalSmooth::getFaceLabelsPath() const
{
  return m_FaceLabelsPath;
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::setNodeTypesPath(const DataArrayPath& value)
{
  m_NodeTypesPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath HierarchicalSmooth::getNodeTypesPath() const
{
  return m_NodeTypesPath;
}

// -----------------------------------------------------------------------------
void HierarchicalSmooth::setSmoothedVertexListPath(const DataArrayPath& value)
{
  m_SmoothedVertexListPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath HierarchicalSmooth::getSmoothedVertexListPath() const
{
  return m_SmoothedVertexListPath;
}
