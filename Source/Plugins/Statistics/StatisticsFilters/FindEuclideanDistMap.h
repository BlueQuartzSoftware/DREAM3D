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
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Statistics/StatisticsDLLExport.h"

/**
 * @brief The FindEuclideanDistMap class. See [Filter documentation](@ref findeuclideandistmap) for details.
 */
class Statistics_EXPORT FindEuclideanDistMap : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindEuclideanDistMap SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindEuclideanDistMap)
  PYB11_FILTER_NEW_MACRO(FindEuclideanDistMap)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(QString, GBDistancesArrayName)
  PYB11_FILTER_PARAMETER(QString, TJDistancesArrayName)
  PYB11_FILTER_PARAMETER(QString, QPDistancesArrayName)
  PYB11_FILTER_PARAMETER(QString, NearestNeighborsArrayName)
  PYB11_FILTER_PARAMETER(bool, DoBoundaries)
  PYB11_FILTER_PARAMETER(bool, DoTripleLines)
  PYB11_FILTER_PARAMETER(bool, DoQuadPoints)
  PYB11_FILTER_PARAMETER(bool, SaveNearestNeighbors)
  PYB11_FILTER_PARAMETER(bool, CalcManhattanDist)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(QString GBDistancesArrayName READ getGBDistancesArrayName WRITE setGBDistancesArrayName)
  PYB11_PROPERTY(QString TJDistancesArrayName READ getTJDistancesArrayName WRITE setTJDistancesArrayName)
  PYB11_PROPERTY(QString QPDistancesArrayName READ getQPDistancesArrayName WRITE setQPDistancesArrayName)
  PYB11_PROPERTY(QString NearestNeighborsArrayName READ getNearestNeighborsArrayName WRITE setNearestNeighborsArrayName)
  PYB11_PROPERTY(bool DoBoundaries READ getDoBoundaries WRITE setDoBoundaries)
  PYB11_PROPERTY(bool DoTripleLines READ getDoTripleLines WRITE setDoTripleLines)
  PYB11_PROPERTY(bool DoQuadPoints READ getDoQuadPoints WRITE setDoQuadPoints)
  PYB11_PROPERTY(bool SaveNearestNeighbors READ getSaveNearestNeighbors WRITE setSaveNearestNeighbors)
  PYB11_PROPERTY(bool CalcManhattanDist READ getCalcManhattanDist WRITE setCalcManhattanDist)
#endif

public:
  using Self = FindEuclideanDistMap;
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
   * @brief Returns the name of the class for FindEuclideanDistMap
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindEuclideanDistMap
   */
  static QString ClassName();

  ~FindEuclideanDistMap() override;

  using EnumType = uint32_t;

  enum class MapType : EnumType
  {
    FeatureBoundary = 0, //!<
    TripleJunction = 1,  //!<
    QuadPoint = 2,       //!<
  };

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
   * @brief Setter property for GBDistancesArrayName
   */
  void setGBDistancesArrayName(const QString& value);
  /**
   * @brief Getter property for GBDistancesArrayName
   * @return Value of GBDistancesArrayName
   */
  QString getGBDistancesArrayName() const;

  Q_PROPERTY(QString GBDistancesArrayName READ getGBDistancesArrayName WRITE setGBDistancesArrayName)

  /**
   * @brief Setter property for TJDistancesArrayName
   */
  void setTJDistancesArrayName(const QString& value);
  /**
   * @brief Getter property for TJDistancesArrayName
   * @return Value of TJDistancesArrayName
   */
  QString getTJDistancesArrayName() const;

  Q_PROPERTY(QString TJDistancesArrayName READ getTJDistancesArrayName WRITE setTJDistancesArrayName)

  /**
   * @brief Setter property for QPDistancesArrayName
   */
  void setQPDistancesArrayName(const QString& value);
  /**
   * @brief Getter property for QPDistancesArrayName
   * @return Value of QPDistancesArrayName
   */
  QString getQPDistancesArrayName() const;

  Q_PROPERTY(QString QPDistancesArrayName READ getQPDistancesArrayName WRITE setQPDistancesArrayName)

  /**
   * @brief Setter property for NearestNeighborsArrayName
   */
  void setNearestNeighborsArrayName(const QString& value);
  /**
   * @brief Getter property for NearestNeighborsArrayName
   * @return Value of NearestNeighborsArrayName
   */
  QString getNearestNeighborsArrayName() const;

  Q_PROPERTY(QString NearestNeighborsArrayName READ getNearestNeighborsArrayName WRITE setNearestNeighborsArrayName)

  /**
   * @brief Setter property for DoBoundaries
   */
  void setDoBoundaries(bool value);
  /**
   * @brief Getter property for DoBoundaries
   * @return Value of DoBoundaries
   */
  bool getDoBoundaries() const;

  Q_PROPERTY(bool DoBoundaries READ getDoBoundaries WRITE setDoBoundaries)

  /**
   * @brief Setter property for DoTripleLines
   */
  void setDoTripleLines(bool value);
  /**
   * @brief Getter property for DoTripleLines
   * @return Value of DoTripleLines
   */
  bool getDoTripleLines() const;

  Q_PROPERTY(bool DoTripleLines READ getDoTripleLines WRITE setDoTripleLines)

  /**
   * @brief Setter property for DoQuadPoints
   */
  void setDoQuadPoints(bool value);
  /**
   * @brief Getter property for DoQuadPoints
   * @return Value of DoQuadPoints
   */
  bool getDoQuadPoints() const;

  Q_PROPERTY(bool DoQuadPoints READ getDoQuadPoints WRITE setDoQuadPoints)

  /**
   * @brief Setter property for SaveNearestNeighbors
   */
  void setSaveNearestNeighbors(bool value);
  /**
   * @brief Getter property for SaveNearestNeighbors
   * @return Value of SaveNearestNeighbors
   */
  bool getSaveNearestNeighbors() const;

  Q_PROPERTY(bool SaveNearestNeighbors READ getSaveNearestNeighbors WRITE setSaveNearestNeighbors)

  /**
   * @brief Setter property for CalcManhattanDist
   */
  void setCalcManhattanDist(bool value);
  /**
   * @brief Getter property for CalcManhattanDist
   * @return Value of CalcManhattanDist
   */
  bool getCalcManhattanDist() const;

  Q_PROPERTY(bool CalcManhattanDist READ getCalcManhattanDist WRITE setCalcManhattanDist)

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
  FindEuclideanDistMap();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief find_euclideandistmap Provides setup for Euclidean distance map caluclation; note that the
   * actual algorithm is performed in a threaded implementation
   */
  void findDistanceMap();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NearestNeighborsPtr;
  int32_t* m_NearestNeighbors = nullptr;
  std::weak_ptr<DataArray<float>> m_GBEuclideanDistancesPtr;
  float* m_GBEuclideanDistances = nullptr;
  std::weak_ptr<DataArray<float>> m_TJEuclideanDistancesPtr;
  float* m_TJEuclideanDistances = nullptr;
  std::weak_ptr<DataArray<float>> m_QPEuclideanDistancesPtr;
  float* m_QPEuclideanDistances = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_GBManhattanDistancesPtr;
  int32_t* m_GBManhattanDistances = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_TJManhattanDistancesPtr;
  int32_t* m_TJManhattanDistances = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_QPManhattanDistancesPtr;
  int32_t* m_QPManhattanDistances = nullptr;

  DataArrayPath m_FeatureIdsArrayPath = {};
  QString m_GBDistancesArrayName = {};
  QString m_TJDistancesArrayName = {};
  QString m_QPDistancesArrayName = {};
  QString m_NearestNeighborsArrayName = {};
  bool m_DoBoundaries = {};
  bool m_DoTripleLines = {};
  bool m_DoQuadPoints = {};
  bool m_SaveNearestNeighbors = {};
  bool m_CalcManhattanDist = {};

  // Full Euclidean Distance Arrays

  //  Distance Arrays

public:
  FindEuclideanDistMap(const FindEuclideanDistMap&) = delete; // Copy Constructor Not Implemented
  FindEuclideanDistMap(FindEuclideanDistMap&&) = delete;      // Move Constructor Not Implemented
  FindEuclideanDistMap& operator=(const FindEuclideanDistMap&) = delete; // Copy Assignment Not Implemented
  FindEuclideanDistMap& operator=(FindEuclideanDistMap&&) = delete;      // Move Assignment Not Implemented
};

