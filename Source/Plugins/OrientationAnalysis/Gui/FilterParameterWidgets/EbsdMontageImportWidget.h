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

#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVector>

#include <QtWidgets/QButtonGroup>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/QtSupport/QtSPluginFrame.h"

#include "OrientationAnalysis/FilterParameters/EbsdMontageImportFilterParameter.h"

namespace Ui
{
class EbsdMontageImportWidget;
}

/**
 * @class EbsdMontageImportWidget EbsdMontageImportWidget.h Plugins/EbsdImport/UI/EbsdMontageImportWidget.h
 * @brief This class represents the User Interface for the EBSD file import module
 * of the SIMPLView program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of EBSD data files into an HDF5 file
 * for better data management and archiving.
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class EbsdMontageImportWidget : public FilterParameterWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  EbsdMontageImportWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

  ~EbsdMontageImportWidget() override;

  Q_PROPERTY(QPixmap Icon READ getIcon WRITE setIcon)
  void setIcon(const QPixmap& path);
  QPixmap getIcon();

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void setupGui() override;

public slots:
  void widgetChanged(const QString& msg);
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);
  void spinnerChanged(int value);

protected slots:

  // Slots to catch signals emitted by the various ui widgets
  void inputDirBtn_clicked();
  void inputDir_textChanged(const QString& text);

protected:
  void setInputDirectory(const QString& val);
  QString getInputDirectory();

  static void SetOpenDialogLastFilePath(const QString& val);

  static QString GetOpenDialogLastFilePath();

  /**
   * @brief setWidgetListEnabled
   */
  void setWidgetListEnabled(bool v);

  /**
   * @brief generateExampleInputFile
   */
  void updateFileListView();

  /**
   * @brief getGuiParametersFromFilter
   */
  void getGuiParametersFromFilter();

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
  QSharedPointer<Ui::EbsdMontageImportWidget> m_Ui;

  QList<QWidget*> m_WidgetList;
  static QString m_OpenDialogLastFilePath;

  QAction* m_ShowFileAction = nullptr;
  QString m_CurrentlyValidPath = "";
  QString m_CurrentText = "";
  bool m_DidCausePreflight = false;
  QPixmap m_Icon = QPixmap(QLatin1String(":/SIMPL/icons/images/caret-bottom.png"));

  /**
   * @brief connectSignalsSlots
   */
  void connectSignalsSlots();

public:
  EbsdMontageImportWidget(const EbsdMontageImportWidget&) = delete;            // Copy Constructor Not Implemented
  EbsdMontageImportWidget(EbsdMontageImportWidget&&) = delete;                 // Move Constructor Not Implemented
  EbsdMontageImportWidget& operator=(const EbsdMontageImportWidget&) = delete; // Copy Assignment Not Implemented
  EbsdMontageImportWidget& operator=(EbsdMontageImportWidget&&) = delete;      // Move Assignment Not Implemented
};
