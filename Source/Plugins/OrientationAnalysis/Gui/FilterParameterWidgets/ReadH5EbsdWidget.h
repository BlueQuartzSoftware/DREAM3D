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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

// Needed for AxisAngle_t
#include "EbsdLib/Core/EbsdLibConstants.h"

#include "SIMPLib/FilterParameters/AxisAngleInput.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

#include "OrientationAnalysis/ui_ReadH5EbsdWidget.h"

class QualityMetricTableModel;
class ReadH5Ebsd;
class ReadH5EbsdFilterParameter;

/**
 * @brief The ReadH5EbsdWidget class
 */
class ReadH5EbsdWidget : public FilterParameterWidget, private Ui::ReadH5EbsdWidget
{
  Q_OBJECT
public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  ReadH5EbsdWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

  ~ReadH5EbsdWidget() override;

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void setupGui() override;

  void setFilter(AbstractFilter* value) override;
  AbstractFilter* getFilter() const override;

  void setFilterParameter(FilterParameter* value) override;
  FilterParameter* getFilterParameter() const override;

public Q_SLOTS:
  // void widgetChanged(const QString& msg);
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);

protected Q_SLOTS:
  void on_m_LineEditBtn_clicked();
  void on_m_LineEdit_textChanged(const QString& text);
  void on_m_CellList_itemClicked(QListWidgetItem* item);
  void on_m_DataArraysCheckBox_stateChanged(int state);
  void on_m_ZStartIndex_valueChanged(int value);
  void on_m_ZEndIndex_valueChanged(int value);
  void on_m_UseTransformations_stateChanged(int state);
  void on_m_AngleRepresentationCB_currentIndexChanged(int index);

protected:
  void setInputFilePath(QString val);
  QString getInputFilePath();

  /**
   * @brief validateInputFile
   */
  void validateInputFile();

  /**
   * @brief setWidgetListEnabled
   */
  void setWidgetListEnabled(bool v);

  /**
   * @brief resetGuiFileInfoWidgets
   */
  void resetGuiFileInfoWidgets();

  /**
   * @brief updateFileInfoWidgets
   */
  void updateFileInfoWidgets();

  /**
   * @brief getSelectedArrayNames
   * @return
   */
  QSet<QString> getSelectedArrayNames();

  /**
   * @brief getSelectedEnsembleNames
   * @return
   */
  QSet<QString> getSelectedEnsembleNames();

  void updateModelFromFilter(QSet<QString>& arrayNames, bool setChecked = false);

  /**
   * @brief
   * @param event
   */
  void keyPressEvent(QKeyEvent* event) override;

  /**
   * @brief setupMenuField
   */
  void setupMenuField();

private:
  ReadH5Ebsd* m_Filter = nullptr;
  ReadH5EbsdFilterParameter* m_FilterParameter = nullptr;
  QList<QWidget*> m_WidgetList;
  AxisAngleInput m_SampleTransformation = {0.0F, 0.0F, 1.0F, 0.0F};
  AxisAngleInput m_EulerTransformation = {0.0F, 0.0F, 1.0F, 0.0F};

  QAction* m_ShowFileAction = nullptr;
  QString m_CurrentText = "";
  bool m_DidCausePreflight = false;
  bool m_NewFileLoaded = false;
  bool m_Version4Warning = false;

public:
  ReadH5EbsdWidget(const ReadH5EbsdWidget&) = delete;            // Copy Constructor Not Implemented
  ReadH5EbsdWidget(ReadH5EbsdWidget&&) = delete;                 // Move Constructor Not Implemented
  ReadH5EbsdWidget& operator=(const ReadH5EbsdWidget&) = delete; // Copy Assignment Not Implemented
  ReadH5EbsdWidget& operator=(ReadH5EbsdWidget&&) = delete;      // Move Assignment Not Implemented
};
