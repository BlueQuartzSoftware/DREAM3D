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

#include <QtWidgets/QComboBox>
#include <QtWidgets/QWidget>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/Presets/AbstractMicrostructurePreset.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/Presets/MicrostructurePresetManager.h"

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/*
 *
 */
class StatsGenWidget : public QWidget
{

  Q_OBJECT

public:
  StatsGenWidget(QWidget* parent = nullptr);
  virtual ~StatsGenWidget();

  /**
   * @brief Setter property for PhaseType
   */
  virtual void setPhaseType(PhaseType::Type value);
  /**
   * @brief Getter property for PhaseType
   * @return Value of PhaseType
   */
  virtual PhaseType::Type getPhaseType() const;

  /**
   * @brief Setter property for CrystalStructure
   */
  virtual void setCrystalStructure(unsigned int value);
  /**
   * @brief Getter property for CrystalStructure
   * @return Value of CrystalStructure
   */
  virtual unsigned int getCrystalStructure() const;

  /**
   * @brief Setter property for PhaseIndex
   */
  virtual void setPhaseIndex(int value);
  /**
   * @brief Getter property for PhaseIndex
   * @return Value of PhaseIndex
   */
  virtual int getPhaseIndex() const;

  /**
   * @brief Setter property for PhaseFraction
   */
  virtual void setPhaseFraction(float value);
  /**
   * @brief Getter property for PhaseFraction
   * @return Value of PhaseFraction
   */
  virtual float getPhaseFraction() const;

  /**
   * @brief Setter property for TotalPhaseFraction
   */
  virtual void setTotalPhaseFraction(float value);
  /**
   * @brief Getter property for TotalPhaseFraction
   * @return Value of TotalPhaseFraction
   */
  virtual float getTotalPhaseFraction() const;

  /**
   * @brief Setter property for DataHasBeenGenerated
   */
  virtual void setDataHasBeenGenerated(bool value);
  /**
   * @brief Getter property for DataHasBeenGenerated
   * @return Value of DataHasBeenGenerated
   */
  virtual bool getDataHasBeenGenerated() const;

  /**
   * @brief Setter property for BulkLoadFailure
   */
  virtual void setBulkLoadFailure(bool value);
  /**
   * @brief Getter property for BulkLoadFailure
   * @return Value of BulkLoadFailure
   */
  virtual bool getBulkLoadFailure() const;

  /**
   * @brief Setter property for TabTitle
   */
  virtual void setTabTitle(const QString& value);
  /**
   * @brief Getter property for TabTitle
   * @return Value of TabTitle
   */
  virtual QString getTabTitle() const;

  /**
   * @brief Setter property for PhaseName
   */
  virtual void setPhaseName(const QString& value);
  /**
   * @brief Getter property for PhaseName
   * @return Value of PhaseName
   */
  virtual QString getPhaseName() const;

  /**
   * @brief Setter property for MicroPreset
   */
  void setMicroPreset(const AbstractMicrostructurePreset::Pointer& value);
  /**
   * @brief Getter property for MicroPreset
   * @return Value of MicroPreset
   */
  AbstractMicrostructurePreset::Pointer getMicroPreset() const;

  virtual void extractStatsData(AttributeMatrix::Pointer attrMat, int index);
  virtual int gatherStatsData(AttributeMatrix::Pointer attrMat, bool preflight = false);

  virtual QString getComboString();

  /**
   * @brief RegisterPresetFactory
   * @param microstructurePresetCombo
   * @return
   */
  template <typename T> AbstractMicrostructurePresetFactory::Pointer RegisterPresetFactory(QComboBox* microstructurePresetCombo)
  {
    AbstractMicrostructurePresetFactory::Pointer presetFactory = T::New();
    MicrostructurePresetManager::registerFactory(presetFactory);
    QString displayString = (presetFactory->displayName());
    microstructurePresetCombo->addItem(displayString);
    return presetFactory;
  }

  virtual void generateDefaultData();

  //  public slots:
  //    virtual void on_m_GenerateDefaultData_clicked();

  //  protected slots:

  //    virtual void dataWasEdited();

  //  signals:
  //    void phaseParametersChanged();

public:
  StatsGenWidget(const StatsGenWidget&) = delete; // Copy Constructor Not Implemented
  StatsGenWidget(StatsGenWidget&&) = delete;      // Move Constructor Not Implemented
  StatsGenWidget& operator=(const StatsGenWidget&) = delete; // Copy Assignment Not Implemented
  StatsGenWidget& operator=(StatsGenWidget&&) = delete;      // Move Assignment Not Implemented

private:
  PhaseType::Type m_PhaseType = {};
  unsigned int m_CrystalStructure = {};
  int m_PhaseIndex = {};
  float m_PhaseFraction = {};
  float m_TotalPhaseFraction = {};
  bool m_DataHasBeenGenerated = {};
  bool m_BulkLoadFailure = {};
  QString m_TabTitle = {};
  QString m_PhaseName = {};
  AbstractMicrostructurePreset::Pointer m_MicroPreset = {};
};

