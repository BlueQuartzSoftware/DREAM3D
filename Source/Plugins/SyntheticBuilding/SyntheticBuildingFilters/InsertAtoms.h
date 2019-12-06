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

#pragma once


#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The InsertAtoms class. See [Filter documentation](@ref insertatoms) for details.
 */
class SyntheticBuilding_EXPORT InsertAtoms : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(InsertAtoms SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(InsertAtoms)
  PYB11_FILTER_NEW_MACRO(InsertAtoms)
  PYB11_FILTER_PARAMETER(DataArrayPath, VertexDataContainerName)
  PYB11_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
  PYB11_FILTER_PARAMETER(FloatVec3Type, LatticeConstants)
  PYB11_FILTER_PARAMETER(int, Basis)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  PYB11_FILTER_PARAMETER(QString, AtomFeatureLabelsArrayName)
  PYB11_PROPERTY(DataArrayPath VertexDataContainerName READ getVertexDataContainerName WRITE setVertexDataContainerName)
  PYB11_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  PYB11_PROPERTY(FloatVec3Type LatticeConstants READ getLatticeConstants WRITE setLatticeConstants)
  PYB11_PROPERTY(int Basis READ getBasis WRITE setBasis)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
  PYB11_PROPERTY(QString AtomFeatureLabelsArrayName READ getAtomFeatureLabelsArrayName WRITE setAtomFeatureLabelsArrayName)
#endif

public:
  using Self = InsertAtoms;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for InsertAtoms
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for InsertAtoms
   */
  static QString ClassName();

  ~InsertAtoms() override;

  /**
   * @brief Setter property for VertexDataContainerName
   */
  void setVertexDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for VertexDataContainerName
   * @return Value of VertexDataContainerName
   */
  DataArrayPath getVertexDataContainerName() const;

  Q_PROPERTY(DataArrayPath VertexDataContainerName READ getVertexDataContainerName WRITE setVertexDataContainerName)

  /**
   * @brief Setter property for VertexAttributeMatrixName
   */
  void setVertexAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName
   * @return Value of VertexAttributeMatrixName
   */
  QString getVertexAttributeMatrixName() const;

  Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

  /**
   * @brief Setter property for LatticeConstants
   */
  void setLatticeConstants(const FloatVec3Type& value);
  /**
   * @brief Getter property for LatticeConstants
   * @return Value of LatticeConstants
   */
  FloatVec3Type getLatticeConstants() const;

  Q_PROPERTY(FloatVec3Type LatticeConstants READ getLatticeConstants WRITE setLatticeConstants)

  /**
   * @brief Setter property for Basis
   */
  void setBasis(int value);
  /**
   * @brief Getter property for Basis
   * @return Value of Basis
   */
  int getBasis() const;

  Q_PROPERTY(int Basis READ getBasis WRITE setBasis)

  /**
   * @brief Setter property for SurfaceMeshFaceLabelsArrayPath
   */
  void setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshFaceLabelsArrayPath
   * @return Value of SurfaceMeshFaceLabelsArrayPath
   */
  DataArrayPath getSurfaceMeshFaceLabelsArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

  /**
   * @brief Setter property for AvgQuatsArrayPath
   */
  void setAvgQuatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AvgQuatsArrayPath
   * @return Value of AvgQuatsArrayPath
   */
  DataArrayPath getAvgQuatsArrayPath() const;

  Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

  /**
   * @brief Setter property for AtomFeatureLabelsArrayName
   */
  void setAtomFeatureLabelsArrayName(const QString& value);
  /**
   * @brief Getter property for AtomFeatureLabelsArrayName
   * @return Value of AtomFeatureLabelsArrayName
   */
  QString getAtomFeatureLabelsArrayName() const;

  Q_PROPERTY(QString AtomFeatureLabelsArrayName READ getAtomFeatureLabelsArrayName WRITE setAtomFeatureLabelsArrayName)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  InsertAtoms();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief assign_points Assigns Feature Ids to the generated 'atoms'
   * @param points VertexGeom object of 'atom' points
   * @param inFeature Boolean array to determine inside/outside relationships for points
   */
  virtual void assign_points(QVector<VertexGeom::Pointer> points, QVector<BoolArrayType::Pointer> inFeature);

  /**
   * @brief updateVertexInstancePointers updates raw Vertex pointers
   */
  virtual void updateVertexInstancePointers();

private:
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
  int32_t* m_SurfaceMeshFaceLabels = nullptr;
  std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
  float* m_AvgQuats = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_AtomFeatureLabelsPtr;
  int32_t* m_AtomFeatureLabels = nullptr;

  DataArrayPath m_VertexDataContainerName = {};
  QString m_VertexAttributeMatrixName = {};
  FloatVec3Type m_LatticeConstants = {};
  int m_Basis = {};
  DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {};
  DataArrayPath m_AvgQuatsArrayPath = {};
  QString m_AtomFeatureLabelsArrayName = {};

public:
  InsertAtoms(const InsertAtoms&) = delete;            // Copy Constructor Not Implemented
  InsertAtoms(InsertAtoms&&) = delete;                 // Move Constructor Not Implemented
  InsertAtoms& operator=(const InsertAtoms&) = delete; // Copy Assignment Not Implemented
  InsertAtoms& operator=(InsertAtoms&&) = delete;      // Move Assignment Not Implemented
};

