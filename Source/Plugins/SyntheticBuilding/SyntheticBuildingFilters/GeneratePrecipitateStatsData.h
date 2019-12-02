/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/FilterParameters/FloatVec2FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

class StatsDataArray;
class StringDataArray;
class PrecipitateStatsData;

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The GeneratePrecipitateStatsData class. See [Filter documentation](@ref GeneratePrecipitateStatsData) for details.
 */
class SyntheticBuilding_EXPORT GeneratePrecipitateStatsData : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(GeneratePrecipitateStatsData SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(GeneratePrecipitateStatsData)
  PYB11_FILTER_NEW_MACRO(GeneratePrecipitateStatsData)
  PYB11_FILTER_PARAMETER(QString, PhaseName)
  PYB11_FILTER_PARAMETER(int, PhaseIndex)
  PYB11_FILTER_PARAMETER(int, CrystalSymmetry)
  PYB11_FILTER_PARAMETER(int, MicroPresetModel)
  PYB11_FILTER_PARAMETER(double, PhaseFraction)
  PYB11_FILTER_PARAMETER(double, Mu)
  PYB11_FILTER_PARAMETER(double, Sigma)
  PYB11_FILTER_PARAMETER(double, MinCutOff)
  PYB11_FILTER_PARAMETER(double, MaxCutOff)
  PYB11_FILTER_PARAMETER(double, BinStepSize)
  PYB11_FILTER_PARAMETER(bool, CreateEnsembleAttributeMatrix)
  PYB11_FILTER_PARAMETER(DataArrayPath, DataContainerName)
  PYB11_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
  PYB11_FILTER_PARAMETER(bool, AppendToExistingAttributeMatrix)
  PYB11_FILTER_PARAMETER(DataArrayPath, SelectedEnsembleAttributeMatrix)
  PYB11_FILTER_PARAMETER(DynamicTableData, OdfData)
  PYB11_FILTER_PARAMETER(DynamicTableData, MdfData)
  PYB11_FILTER_PARAMETER(DynamicTableData, AxisOdfData)
  PYB11_FILTER_PARAMETER(FloatVec2Type, RdfMinMaxDistance)
  PYB11_FILTER_PARAMETER(int, RdfNumBins)
  PYB11_FILTER_PARAMETER(FloatVec3Type, RdfBoxSize)
  PYB11_PROPERTY(QString PhaseName READ getPhaseName WRITE setPhaseName)
  PYB11_PROPERTY(int PhaseIndex READ getPhaseIndex WRITE setPhaseIndex)
  PYB11_PROPERTY(int CrystalSymmetry READ getCrystalSymmetry WRITE setCrystalSymmetry)
  PYB11_PROPERTY(int MicroPresetModel READ getMicroPresetModel WRITE setMicroPresetModel)
  PYB11_PROPERTY(double PhaseFraction READ getPhaseFraction WRITE setPhaseFraction)
  PYB11_PROPERTY(double Mu READ getMu WRITE setMu)
  PYB11_PROPERTY(double Sigma READ getSigma WRITE setSigma)
  PYB11_PROPERTY(double MinCutOff READ getMinCutOff WRITE setMinCutOff)
  PYB11_PROPERTY(double MaxCutOff READ getMaxCutOff WRITE setMaxCutOff)
  PYB11_PROPERTY(double BinStepSize READ getBinStepSize WRITE setBinStepSize)
  PYB11_PROPERTY(bool CreateEnsembleAttributeMatrix READ getCreateEnsembleAttributeMatrix WRITE setCreateEnsembleAttributeMatrix)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
  PYB11_PROPERTY(bool AppendToExistingAttributeMatrix READ getAppendToExistingAttributeMatrix WRITE setAppendToExistingAttributeMatrix)
  PYB11_PROPERTY(DataArrayPath SelectedEnsembleAttributeMatrix READ getSelectedEnsembleAttributeMatrix WRITE setSelectedEnsembleAttributeMatrix)
  PYB11_PROPERTY(DynamicTableData OdfData READ getOdfData WRITE setOdfData)
  PYB11_PROPERTY(DynamicTableData MdfData READ getMdfData WRITE setMdfData)
  PYB11_PROPERTY(DynamicTableData AxisOdfData READ getAxisOdfData WRITE setAxisOdfData)
  PYB11_PROPERTY(FloatVec2Type RdfMinMaxDistance READ getRdfMinMaxDistance WRITE setRdfMinMaxDistance)
  PYB11_PROPERTY(int RdfNumBins READ getRdfNumBins WRITE setRdfNumBins)
  PYB11_PROPERTY(FloatVec3Type RdfBoxSize READ getRdfBoxSize WRITE setRdfBoxSize)
#endif

public:
  using Self = GeneratePrecipitateStatsData;
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
   * @brief Returns the name of the class for GeneratePrecipitateStatsData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GeneratePrecipitateStatsData
   */
  static QString ClassName();

  ~GeneratePrecipitateStatsData() override;

  /**
   * @brief Setter property for PhaseName
   */
  void setPhaseName(const QString& value);
  /**
   * @brief Getter property for PhaseName
   * @return Value of PhaseName
   */
  QString getPhaseName() const;

  Q_PROPERTY(QString PhaseName READ getPhaseName WRITE setPhaseName)

  /**
   * @brief Setter property for PhaseIndex
   */
  void setPhaseIndex(int value);
  /**
   * @brief Getter property for PhaseIndex
   * @return Value of PhaseIndex
   */
  int getPhaseIndex() const;

  Q_PROPERTY(int PhaseIndex READ getPhaseIndex WRITE setPhaseIndex)

  /**
   * @brief Setter property for CrystalSymmetry
   */
  void setCrystalSymmetry(int value);
  /**
   * @brief Getter property for CrystalSymmetry
   * @return Value of CrystalSymmetry
   */
  int getCrystalSymmetry() const;

  Q_PROPERTY(int CrystalSymmetry READ getCrystalSymmetry WRITE setCrystalSymmetry)

  /**
   * @brief Setter property for MicroPresetModel
   */
  void setMicroPresetModel(int value);
  /**
   * @brief Getter property for MicroPresetModel
   * @return Value of MicroPresetModel
   */
  int getMicroPresetModel() const;

  Q_PROPERTY(int MicroPresetModel READ getMicroPresetModel WRITE setMicroPresetModel)

  /**
   * @brief Setter property for PhaseFraction
   */
  void setPhaseFraction(double value);
  /**
   * @brief Getter property for PhaseFraction
   * @return Value of PhaseFraction
   */
  double getPhaseFraction() const;

  Q_PROPERTY(double PhaseFraction READ getPhaseFraction WRITE setPhaseFraction)

  /**
   * @brief Setter property for Mu
   */
  void setMu(double value);
  /**
   * @brief Getter property for Mu
   * @return Value of Mu
   */
  double getMu() const;

  Q_PROPERTY(double Mu READ getMu WRITE setMu)

  /**
   * @brief Setter property for Sigma
   */
  void setSigma(double value);
  /**
   * @brief Getter property for Sigma
   * @return Value of Sigma
   */
  double getSigma() const;

  Q_PROPERTY(double Sigma READ getSigma WRITE setSigma)

  /**
   * @brief Setter property for MinCutOff
   */
  void setMinCutOff(double value);
  /**
   * @brief Getter property for MinCutOff
   * @return Value of MinCutOff
   */
  double getMinCutOff() const;

  Q_PROPERTY(double MinCutOff READ getMinCutOff WRITE setMinCutOff)

  /**
   * @brief Setter property for MaxCutOff
   */
  void setMaxCutOff(double value);
  /**
   * @brief Getter property for MaxCutOff
   * @return Value of MaxCutOff
   */
  double getMaxCutOff() const;

  Q_PROPERTY(double MaxCutOff READ getMaxCutOff WRITE setMaxCutOff)

  /**
   * @brief Setter property for BinStepSize
   */
  void setBinStepSize(double value);
  /**
   * @brief Getter property for BinStepSize
   * @return Value of BinStepSize
   */
  double getBinStepSize() const;

  Q_PROPERTY(double BinStepSize READ getBinStepSize WRITE setBinStepSize)

  /**
   * @brief Setter property for CreateEnsembleAttributeMatrix
   */
  void setCreateEnsembleAttributeMatrix(bool value);
  /**
   * @brief Getter property for CreateEnsembleAttributeMatrix
   * @return Value of CreateEnsembleAttributeMatrix
   */
  bool getCreateEnsembleAttributeMatrix() const;

  Q_PROPERTY(bool CreateEnsembleAttributeMatrix READ getCreateEnsembleAttributeMatrix WRITE setCreateEnsembleAttributeMatrix)

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  DataArrayPath getDataContainerName() const;

  Q_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)

  /**
   * @brief Setter property for CellEnsembleAttributeMatrixName
   */
  void setCellEnsembleAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellEnsembleAttributeMatrixName
   * @return Value of CellEnsembleAttributeMatrixName
   */
  QString getCellEnsembleAttributeMatrixName() const;

  Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

  /**
   * @brief Setter property for AppendToExistingAttributeMatrix
   */
  void setAppendToExistingAttributeMatrix(bool value);
  /**
   * @brief Getter property for AppendToExistingAttributeMatrix
   * @return Value of AppendToExistingAttributeMatrix
   */
  bool getAppendToExistingAttributeMatrix() const;

  Q_PROPERTY(bool AppendToExistingAttributeMatrix READ getAppendToExistingAttributeMatrix WRITE setAppendToExistingAttributeMatrix)

  /**
   * @brief Setter property for SelectedEnsembleAttributeMatrix
   */
  void setSelectedEnsembleAttributeMatrix(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedEnsembleAttributeMatrix
   * @return Value of SelectedEnsembleAttributeMatrix
   */
  DataArrayPath getSelectedEnsembleAttributeMatrix() const;

  Q_PROPERTY(DataArrayPath SelectedEnsembleAttributeMatrix READ getSelectedEnsembleAttributeMatrix WRITE setSelectedEnsembleAttributeMatrix)

  QString getNumberOfBins();
  Q_PROPERTY(QString NumberOfBins READ getNumberOfBins)

  QString getFeatureESD();
  Q_PROPERTY(QString FeatureESD READ getFeatureESD)

  /**
   * @brief Setter property for OdfData
   */
  void setOdfData(const DynamicTableData& value);
  /**
   * @brief Getter property for OdfData
   * @return Value of OdfData
   */
  DynamicTableData getOdfData() const;

  Q_PROPERTY(DynamicTableData OdfData READ getOdfData WRITE setOdfData)

  /**
   * @brief Setter property for MdfData
   */
  void setMdfData(const DynamicTableData& value);
  /**
   * @brief Getter property for MdfData
   * @return Value of MdfData
   */
  DynamicTableData getMdfData() const;

  Q_PROPERTY(DynamicTableData MdfData READ getMdfData WRITE setMdfData)

  /**
   * @brief Setter property for AxisOdfData
   */
  void setAxisOdfData(const DynamicTableData& value);
  /**
   * @brief Getter property for AxisOdfData
   * @return Value of AxisOdfData
   */
  DynamicTableData getAxisOdfData() const;

  Q_PROPERTY(DynamicTableData AxisOdfData READ getAxisOdfData WRITE setAxisOdfData)

  /**
   * @brief Setter property for RdfMinMaxDistance
   */
  void setRdfMinMaxDistance(const FloatVec2Type& value);
  /**
   * @brief Getter property for RdfMinMaxDistance
   * @return Value of RdfMinMaxDistance
   */
  FloatVec2Type getRdfMinMaxDistance() const;

  Q_PROPERTY(FloatVec2Type RdfMinMaxDistance READ getRdfMinMaxDistance WRITE setRdfMinMaxDistance)

  /**
   * @brief Setter property for RdfNumBins
   */
  void setRdfNumBins(int value);
  /**
   * @brief Getter property for RdfNumBins
   * @return Value of RdfNumBins
   */
  int getRdfNumBins() const;

  Q_PROPERTY(int RdfNumBins READ getRdfNumBins WRITE setRdfNumBins)

  /**
   * @brief Setter property for RdfBoxSize
   */
  void setRdfBoxSize(const FloatVec3Type& value);
  /**
   * @brief Getter property for RdfBoxSize
   * @return Value of RdfBoxSize
   */
  FloatVec3Type getRdfBoxSize() const;

  Q_PROPERTY(FloatVec3Type RdfBoxSize READ getRdfBoxSize WRITE setRdfBoxSize)

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
  GeneratePrecipitateStatsData();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  void normalizePhaseFractions(StatsDataArray* statsDataArray);

private:
  QString m_PhaseName = {};
  int m_PhaseIndex = {};
  int m_CrystalSymmetry = {};
  int m_MicroPresetModel = {};
  double m_PhaseFraction = {};
  double m_Mu = {};
  double m_Sigma = {};
  double m_MinCutOff = {};
  double m_MaxCutOff = {};
  double m_BinStepSize = {};
  bool m_CreateEnsembleAttributeMatrix = {};
  DataArrayPath m_DataContainerName = {};
  QString m_CellEnsembleAttributeMatrixName = {};
  bool m_AppendToExistingAttributeMatrix = {};
  DataArrayPath m_SelectedEnsembleAttributeMatrix = {};
  DynamicTableData m_OdfData = {};
  DynamicTableData m_MdfData = {};
  DynamicTableData m_AxisOdfData = {};
  FloatVec2Type m_RdfMinMaxDistance = {};
  int m_RdfNumBins = {};
  FloatVec3Type m_RdfBoxSize = {};

  StatsDataArray* m_StatsDataArray;
  PrecipitateStatsData* m_PrecipitateStatsData;
  UInt32ArrayType* m_CrystalStructures;
  UInt32ArrayType* m_PhaseTypes;
  StringDataArray* m_PhaseNames;

  QString m_NumberOfBins;
  QString m_FeatureESD;

public:
  GeneratePrecipitateStatsData(const GeneratePrecipitateStatsData&) = delete;            // Copy Constructor Not Implemented
  GeneratePrecipitateStatsData(GeneratePrecipitateStatsData&&) = delete;                 // Move Constructor Not Implemented
  GeneratePrecipitateStatsData& operator=(const GeneratePrecipitateStatsData&) = delete; // Copy Assignment Not Implemented
  GeneratePrecipitateStatsData& operator=(GeneratePrecipitateStatsData&&) = delete;      // Move Assignment Not Implemented
};

