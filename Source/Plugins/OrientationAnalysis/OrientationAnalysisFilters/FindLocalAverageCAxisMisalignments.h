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

#include "OrientationLib/LaueOps/LaueOps.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @class FindLocalAverageCAxisMisalignments FindLocalAverageCAxisMisalignments.h Plugins/Statistics/StatisticsFilters/FindLocalAverageCAxisMisalignments.h
 * @brief
 * @author Joseph C. Tucker
 * @date Jan 30, 2014
 * @version 5.0
 */
class OrientationAnalysis_EXPORT FindLocalAverageCAxisMisalignments : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindLocalAverageCAxisMisalignments SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindLocalAverageCAxisMisalignments)
  PYB11_FILTER_NEW_MACRO(FindLocalAverageCAxisMisalignments)
  PYB11_FILTER_PARAMETER(DataArrayPath, NewCellFeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(bool, CalcUnbiasedAvg)
  PYB11_FILTER_PARAMETER(bool, CalcBiasedAvg)
  PYB11_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CAxisMisalignmentListArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellParentIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgCAxisMisalignmentsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureParentIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(QString, UnbiasedLocalCAxisMisalignmentsArrayName)
  PYB11_FILTER_PARAMETER(QString, LocalCAxisMisalignmentsArrayName)
  PYB11_FILTER_PARAMETER(QString, NumFeaturesPerParentArrayName)
  PYB11_PROPERTY(DataArrayPath NewCellFeatureAttributeMatrixName READ getNewCellFeatureAttributeMatrixName WRITE setNewCellFeatureAttributeMatrixName)
  PYB11_PROPERTY(bool CalcUnbiasedAvg READ getCalcUnbiasedAvg WRITE setCalcUnbiasedAvg)
  PYB11_PROPERTY(bool CalcBiasedAvg READ getCalcBiasedAvg WRITE setCalcBiasedAvg)
  PYB11_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)
  PYB11_PROPERTY(DataArrayPath CAxisMisalignmentListArrayPath READ getCAxisMisalignmentListArrayPath WRITE setCAxisMisalignmentListArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellParentIdsArrayPath READ getCellParentIdsArrayPath WRITE setCellParentIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath AvgCAxisMisalignmentsArrayPath READ getAvgCAxisMisalignmentsArrayPath WRITE setAvgCAxisMisalignmentsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureParentIdsArrayPath READ getFeatureParentIdsArrayPath WRITE setFeatureParentIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(QString UnbiasedLocalCAxisMisalignmentsArrayName READ getUnbiasedLocalCAxisMisalignmentsArrayName WRITE setUnbiasedLocalCAxisMisalignmentsArrayName)
  PYB11_PROPERTY(QString LocalCAxisMisalignmentsArrayName READ getLocalCAxisMisalignmentsArrayName WRITE setLocalCAxisMisalignmentsArrayName)
  PYB11_PROPERTY(QString NumFeaturesPerParentArrayName READ getNumFeaturesPerParentArrayName WRITE setNumFeaturesPerParentArrayName)
#endif

public:
  using Self = FindLocalAverageCAxisMisalignments;
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
   * @brief Returns the name of the class for FindLocalAverageCAxisMisalignments
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindLocalAverageCAxisMisalignments
   */
  static QString ClassName();

  ~FindLocalAverageCAxisMisalignments() override;

  /**
   * @brief Setter property for NewCellFeatureAttributeMatrixName
   */
  void setNewCellFeatureAttributeMatrixName(const DataArrayPath& value);
  /**
   * @brief Getter property for NewCellFeatureAttributeMatrixName
   * @return Value of NewCellFeatureAttributeMatrixName
   */
  DataArrayPath getNewCellFeatureAttributeMatrixName() const;

  Q_PROPERTY(DataArrayPath NewCellFeatureAttributeMatrixName READ getNewCellFeatureAttributeMatrixName WRITE setNewCellFeatureAttributeMatrixName)

  /**
   * @brief Setter property for CalcUnbiasedAvg
   */
  void setCalcUnbiasedAvg(bool value);
  /**
   * @brief Getter property for CalcUnbiasedAvg
   * @return Value of CalcUnbiasedAvg
   */
  bool getCalcUnbiasedAvg() const;

  Q_PROPERTY(bool CalcUnbiasedAvg READ getCalcUnbiasedAvg WRITE setCalcUnbiasedAvg)

  /**
   * @brief Setter property for CalcBiasedAvg
   */
  void setCalcBiasedAvg(bool value);
  /**
   * @brief Getter property for CalcBiasedAvg
   * @return Value of CalcBiasedAvg
   */
  bool getCalcBiasedAvg() const;

  Q_PROPERTY(bool CalcBiasedAvg READ getCalcBiasedAvg WRITE setCalcBiasedAvg)

  /**
   * @brief Setter property for NeighborListArrayPath
   */
  void setNeighborListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NeighborListArrayPath
   * @return Value of NeighborListArrayPath
   */
  DataArrayPath getNeighborListArrayPath() const;

  Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

  /**
   * @brief Setter property for CAxisMisalignmentListArrayPath
   */
  void setCAxisMisalignmentListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CAxisMisalignmentListArrayPath
   * @return Value of CAxisMisalignmentListArrayPath
   */
  DataArrayPath getCAxisMisalignmentListArrayPath() const;

  Q_PROPERTY(DataArrayPath CAxisMisalignmentListArrayPath READ getCAxisMisalignmentListArrayPath WRITE setCAxisMisalignmentListArrayPath)

  /**
   * @brief Setter property for FeatureIdsArrayPath
   */
  void setFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsArrayPath
   * @return Value of FeatureIdsArrayPath
   */
  DataArrayPath getFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  /**
   * @brief Setter property for CellParentIdsArrayPath
   */
  void setCellParentIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellParentIdsArrayPath
   * @return Value of CellParentIdsArrayPath
   */
  DataArrayPath getCellParentIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath CellParentIdsArrayPath READ getCellParentIdsArrayPath WRITE setCellParentIdsArrayPath)

  /**
   * @brief Setter property for AvgCAxisMisalignmentsArrayPath
   */
  void setAvgCAxisMisalignmentsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AvgCAxisMisalignmentsArrayPath
   * @return Value of AvgCAxisMisalignmentsArrayPath
   */
  DataArrayPath getAvgCAxisMisalignmentsArrayPath() const;

  Q_PROPERTY(DataArrayPath AvgCAxisMisalignmentsArrayPath READ getAvgCAxisMisalignmentsArrayPath WRITE setAvgCAxisMisalignmentsArrayPath)

  /**
   * @brief Setter property for FeatureParentIdsArrayPath
   */
  void setFeatureParentIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureParentIdsArrayPath
   * @return Value of FeatureParentIdsArrayPath
   */
  DataArrayPath getFeatureParentIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureParentIdsArrayPath READ getFeatureParentIdsArrayPath WRITE setFeatureParentIdsArrayPath)

  /**
   * @brief Setter property for CrystalStructuresArrayPath
   */
  void setCrystalStructuresArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CrystalStructuresArrayPath
   * @return Value of CrystalStructuresArrayPath
   */
  DataArrayPath getCrystalStructuresArrayPath() const;

  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  /**
   * @brief Setter property for UnbiasedLocalCAxisMisalignmentsArrayName
   */
  void setUnbiasedLocalCAxisMisalignmentsArrayName(const QString& value);
  /**
   * @brief Getter property for UnbiasedLocalCAxisMisalignmentsArrayName
   * @return Value of UnbiasedLocalCAxisMisalignmentsArrayName
   */
  QString getUnbiasedLocalCAxisMisalignmentsArrayName() const;

  Q_PROPERTY(QString UnbiasedLocalCAxisMisalignmentsArrayName READ getUnbiasedLocalCAxisMisalignmentsArrayName WRITE setUnbiasedLocalCAxisMisalignmentsArrayName)

  /**
   * @brief Setter property for LocalCAxisMisalignmentsArrayName
   */
  void setLocalCAxisMisalignmentsArrayName(const QString& value);
  /**
   * @brief Getter property for LocalCAxisMisalignmentsArrayName
   * @return Value of LocalCAxisMisalignmentsArrayName
   */
  QString getLocalCAxisMisalignmentsArrayName() const;

  Q_PROPERTY(QString LocalCAxisMisalignmentsArrayName READ getLocalCAxisMisalignmentsArrayName WRITE setLocalCAxisMisalignmentsArrayName)

  /**
   * @brief Setter property for NumFeaturesPerParentArrayName
   */
  void setNumFeaturesPerParentArrayName(const QString& value);
  /**
   * @brief Getter property for NumFeaturesPerParentArrayName
   * @return Value of NumFeaturesPerParentArrayName
   */
  QString getNumFeaturesPerParentArrayName() const;

  Q_PROPERTY(QString NumFeaturesPerParentArrayName READ getNumFeaturesPerParentArrayName WRITE setNumFeaturesPerParentArrayName)

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
  FindLocalAverageCAxisMisalignments();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellParentIdsPtr;
  int32_t* m_CellParentIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeatureParentIdsPtr;
  int32_t* m_FeatureParentIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NumFeaturesPerParentPtr;
  int32_t* m_NumFeaturesPerParent = nullptr;
  std::weak_ptr<DataArray<float>> m_AvgCAxisMisalignmentsPtr;
  float* m_AvgCAxisMisalignments = nullptr;
  std::weak_ptr<DataArray<float>> m_LocalCAxisMisalignmentsPtr;
  float* m_LocalCAxisMisalignments = nullptr;
  std::weak_ptr<DataArray<float>> m_UnbiasedLocalCAxisMisalignmentsPtr;
  float* m_UnbiasedLocalCAxisMisalignments = nullptr;
  std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures = nullptr;

  DataArrayPath m_NewCellFeatureAttributeMatrixName = {};
  bool m_CalcUnbiasedAvg = {};
  bool m_CalcBiasedAvg = {};
  DataArrayPath m_NeighborListArrayPath = {};
  DataArrayPath m_CAxisMisalignmentListArrayPath = {};
  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_CellParentIdsArrayPath = {};
  DataArrayPath m_AvgCAxisMisalignmentsArrayPath = {};
  DataArrayPath m_FeatureParentIdsArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  QString m_UnbiasedLocalCAxisMisalignmentsArrayName = {};
  QString m_LocalCAxisMisalignmentsArrayName = {};
  QString m_NumFeaturesPerParentArrayName = {};

  QVector<LaueOps::Pointer> m_OrientationOps;

  NeighborList<int>::WeakPointer m_NeighborList;
  NeighborList<float>::WeakPointer m_CAxisMisalignmentList;

public:
  FindLocalAverageCAxisMisalignments(const FindLocalAverageCAxisMisalignments&) = delete; // Copy Constructor Not Implemented
  FindLocalAverageCAxisMisalignments(FindLocalAverageCAxisMisalignments&&) = delete;      // Move Constructor Not Implemented
  FindLocalAverageCAxisMisalignments& operator=(const FindLocalAverageCAxisMisalignments&) = delete; // Copy Assignment Not Implemented
  FindLocalAverageCAxisMisalignments& operator=(FindLocalAverageCAxisMisalignments&&) = delete;      // Move Assignment Not Implemented
};

