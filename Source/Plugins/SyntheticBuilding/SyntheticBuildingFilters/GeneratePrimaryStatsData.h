/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

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
  PYB11_CREATE_BINDINGS(GeneratePrimaryStatsData SUPERCLASS AbstractFilter)
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
  PYB11_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
  PYB11_PROPERTY(bool AppendToExistingAttributeMatrix READ getAppendToExistingAttributeMatrix WRITE setAppendToExistingAttributeMatrix)
  PYB11_PROPERTY(DataArrayPath SelectedEnsembleAttributeMatrix READ getSelectedEnsembleAttributeMatrix WRITE setSelectedEnsembleAttributeMatrix)
  PYB11_PROPERTY(DynamicTableData OdfData READ getOdfData WRITE setOdfData)
  PYB11_PROPERTY(DynamicTableData MdfData READ getMdfData WRITE setMdfData)
  PYB11_PROPERTY(DynamicTableData AxisOdfData READ getAxisOdfData WRITE setAxisOdfData)

public:
  SIMPL_SHARED_POINTERS(GeneratePrimaryStatsData)
  SIMPL_FILTER_NEW_MACRO(GeneratePrimaryStatsData)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(GeneratePrimaryStatsData, AbstractFilter)

  ~GeneratePrimaryStatsData() override;

  SIMPL_FILTER_PARAMETER(QString, PhaseName)
  Q_PROPERTY(QString PhaseName READ getPhaseName WRITE setPhaseName)

  SIMPL_FILTER_PARAMETER(int, PhaseIndex)
  Q_PROPERTY(int PhaseIndex READ getPhaseIndex WRITE setPhaseIndex)

  SIMPL_FILTER_PARAMETER(int, CrystalSymmetry)
  Q_PROPERTY(int CrystalSymmetry READ getCrystalSymmetry WRITE setCrystalSymmetry)

  SIMPL_FILTER_PARAMETER(int, MicroPresetModel)
  Q_PROPERTY(int MicroPresetModel READ getMicroPresetModel WRITE setMicroPresetModel)

  SIMPL_FILTER_PARAMETER(double, PhaseFraction)
  Q_PROPERTY(double PhaseFraction READ getPhaseFraction WRITE setPhaseFraction)

  SIMPL_FILTER_PARAMETER(double, Mu)
  Q_PROPERTY(double Mu READ getMu WRITE setMu)

  SIMPL_FILTER_PARAMETER(double, Sigma)
  Q_PROPERTY(double Sigma READ getSigma WRITE setSigma)

  SIMPL_FILTER_PARAMETER(double, MinCutOff)
  Q_PROPERTY(double MinCutOff READ getMinCutOff WRITE setMinCutOff)

  SIMPL_FILTER_PARAMETER(double, MaxCutOff)
  Q_PROPERTY(double MaxCutOff READ getMaxCutOff WRITE setMaxCutOff)

  SIMPL_FILTER_PARAMETER(double, BinStepSize)
  Q_PROPERTY(double BinStepSize READ getBinStepSize WRITE setBinStepSize)

  SIMPL_FILTER_PARAMETER(bool, CreateEnsembleAttributeMatrix)
  Q_PROPERTY(bool CreateEnsembleAttributeMatrix READ getCreateEnsembleAttributeMatrix WRITE setCreateEnsembleAttributeMatrix)

  SIMPL_FILTER_PARAMETER(QString, DataContainerName)
  Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
  Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(bool, AppendToExistingAttributeMatrix)
  Q_PROPERTY(bool AppendToExistingAttributeMatrix READ getAppendToExistingAttributeMatrix WRITE setAppendToExistingAttributeMatrix)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SelectedEnsembleAttributeMatrix)
  Q_PROPERTY(DataArrayPath SelectedEnsembleAttributeMatrix READ getSelectedEnsembleAttributeMatrix WRITE setSelectedEnsembleAttributeMatrix)

  QString getNumberOfBins();
  Q_PROPERTY(QString NumberOfBins READ getNumberOfBins)

  QString getFeatureESD();
  Q_PROPERTY(QString FeatureESD READ getFeatureESD)

  SIMPL_FILTER_PARAMETER(DynamicTableData, OdfData)
  Q_PROPERTY(DynamicTableData OdfData READ getOdfData WRITE setOdfData)

  SIMPL_FILTER_PARAMETER(DynamicTableData, MdfData)
  Q_PROPERTY(DynamicTableData MdfData READ getMdfData WRITE setMdfData)

  SIMPL_FILTER_PARAMETER(DynamicTableData, AxisOdfData)
  Q_PROPERTY(DynamicTableData AxisOdfData READ getAxisOdfData WRITE setAxisOdfData)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

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

