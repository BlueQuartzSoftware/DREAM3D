/*
 * Your License or Copyright can go here
 */

#ifndef _generateprimarystatsdata_h_
#define _generateprimarystatsdata_h_

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/SIMPLib.h"

class StatsDataArray;
class StringDataArray;
class PrimaryStatsData;

/**
 * @brief The GeneratePrimaryStatsData class. See [Filter documentation](@ref generateprimarystatsdata) for details.
 */
class GeneratePrimaryStatsData : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(GeneratePrimaryStatsData)
  SIMPL_STATIC_NEW_MACRO(GeneratePrimaryStatsData)
  SIMPL_TYPE_MACRO_SUPER(GeneratePrimaryStatsData, AbstractFilter)

  virtual ~GeneratePrimaryStatsData();

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
  virtual const QString getCompiledLibraryName();

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  virtual const QString getBrandingString();

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  virtual const QString getFilterVersion();

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getGroupName();

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getSubGroupName();

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  virtual const QString getHumanLabel();

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void setupFilterParameters();

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  virtual void execute();

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  virtual void preflight();

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

  GeneratePrimaryStatsData(const GeneratePrimaryStatsData&); // Copy Constructor Not Implemented
  void operator=(const GeneratePrimaryStatsData&);           // Operator '=' Not Implemented
};

#endif /* _GeneratePrimaryStatsData_H_ */
