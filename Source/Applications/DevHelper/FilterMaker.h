/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
* Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
* All rights reserved.
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
* the US Air Force, BlueQuartz Software nor the names of its contributors may be
* used to endorse or promote products derived from this software without specific
* prior written permission.
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
*  This code was written under United States Air Force Contract number
*                           FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _FilterMaker_H_
#define _FilterMaker_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>

#include <QtWidgets/QWidget>

#include "DevHelper/AddFilterParameter.h"
#include "DevHelper/PMFileGenerator.h"

#include "ui_FilterMaker.h"

enum FPColumnIndex
{
  VAR_NAME,
  HUMAN_NAME,
  TYPE,
  INIT_VALUE
};

class FilterMaker : public QWidget, public Ui::FilterMaker
{
  Q_OBJECT

public:
  FilterMaker(QWidget* parent = 0);
  ~FilterMaker();

  QString getFilterName();
  QString getPluginDir();
  bool isPublic();

public slots:
  void addFilterParameterToTable(AddFilterParameter* widget);

protected:
  void setupGui();

  QString getDefaultSetupFPContents();
  QString getDefaultReadFPContents();
  QString getDefaultWriteFPContents();
  QString getDefaultDataCheckContents();
  QString getDefaultFPContents();
  QString getDefaultInitListContents();
  QString getDefaultFilterHIncludesContents();
  QString getDefaultFilterCPPIncludesContents();

protected slots:
  void on_selectBtn_clicked();
  void on_codeChooser_currentIndexChanged(int index);
  void on_pluginDir_textChanged(const QString& text);
  void on_filterName_textChanged(const QString& text);
  void on_filterParametersTable_itemChanged(QTableWidgetItem* item);
  void on_generateBtn_clicked();
  void on_cancelBtn_clicked();
  void on_addFilterParameterBtn_clicked();
  void on_removeFilterParameterBtn_clicked();
  void on_errorString_linkActivated(const QString &link);
  void generationError(const QString& test);

signals:
  void generateBtnPressed();
  void cancelBtnPressed();
  void updateStatusBar(QString message);

private:
  QString                     m_OpenDialogLastDirectory;
  PMFileGenerator*            cppGenerator;
  PMFileGenerator*            hGenerator;
  PMFileGenerator*            htmlGenerator;
  PMFileGenerator*            testGenerator;

  void updateFilterFileGenerators();
  void generateFilterFiles();
  void updateSourceList();
  void updateTestLocations();
  void updateTestList();
  QString createNamespaceString();
  bool validityCheck();

  QMap<QString, QString> getFunctionContents();

  FilterMaker(const FilterMaker&);    // Copy Constructor Not Implemented
  void operator=(const FilterMaker&);  // Operator '=' Not Implemented
};

#endif
