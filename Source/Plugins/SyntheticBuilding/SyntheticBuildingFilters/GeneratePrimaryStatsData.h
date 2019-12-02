/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

class StatsDataArray;
class StringDataArray;
class PrimaryStatsData;

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The GeneratePrimaryStatsData class. See [Filter documentation](@ref generateprimarystatsdata) for details.
 */
class SyntheticBuilding_EXPORT GeneratePrimaryStatsData : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(GeneratePrimaryStatsData SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(GeneratePrimaryStatsData)
  PYB11_FILTER_NEW_MACRO(GeneratePrimaryStatsData)
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
#endif

public:
  using Self = GeneratePrimaryStatsData;
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
   * @brief Returns the name of the class for GeneratePrimaryStatsData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GeneratePrimaryStatsData
   */
  static QString ClassName();

  ~GeneratePrimaryStatsData() override;

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
  GeneratePrimaryStatsData();

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

  StatsDataArray* m_StatsDataArray;
  PrimaryStatsData* m_PrimaryStatsData;
  UInt32ArrayType* m_CrystalStructures;
  UInt32ArrayType* m_PhaseTypes;
  StringDataArray* m_PhaseNames;

  QString m_NumberOfBins;
  QString m_FeatureESD;

public:
  GeneratePrimaryStatsData(const GeneratePrimaryStatsData&) = delete;            // Copy Constructor Not Implemented
  GeneratePrimaryStatsData(GeneratePrimaryStatsData&&) = delete;                 // Move Constructor Not Implemented
  GeneratePrimaryStatsData& operator=(const GeneratePrimaryStatsData&) = delete; // Copy Assignment Not Implemented
  GeneratePrimaryStatsData& operator=(GeneratePrimaryStatsData&&) = delete;      // Move Assignment Not Implemented
};

