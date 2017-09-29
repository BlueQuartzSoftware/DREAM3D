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

#ifndef _emmpmwidget_h_
#define _emmpmwidget_h_

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/DataContainerReaderFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

#include "EMMPM/ui_EMMPMWidget.h"

class EMMPMFilterParameter;
class EMMPMFilter;
class DynamicTableData;

/**
* @brief
* @author
* @version
*/
class EMMPMWidget : public FilterParameterWidget, private Ui::EMMPMWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parameter The FilterParameter object that this widget represents
  * @param filter The instance of the filter that this parameter is a part of
  * @param parent The parent QWidget for this Widget
  */
  EMMPMWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

  /**
   * @brief EMMPMWidget
   * @param parent
   */
  EMMPMWidget(QWidget* parent = nullptr);

  virtual ~EMMPMWidget();

  void initialize(FilterParameter* parameter, AbstractFilter* filter = nullptr);

  /**
  * @brief This method does additional GUI widget connections
  */
  void setupGui();

  void setFilter(AbstractFilter* value);
  AbstractFilter* getFilter() const;

  void setFilterParameter(FilterParameter* value);
  FilterParameter* getFilterParameter() const;

public slots:
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);

  void on_tableWidget_cellChanged(int row, int column);
  void on_m_NumClasses_valueChanged(int i);
  void on_enableManualInit_toggled(bool checked);

signals:
  void errorSettingFilterParameter(const QString& msg);
  void parametersChanged();

private:
  EMMPMFilter* m_Filter;
  EMMPMFilterParameter* m_FilterParameter;
  bool m_DidCausePreflight;

  DynamicTableData getDynamicTableData(QTableWidget* tableWidget);

  EMMPMWidget(const EMMPMWidget&);    // Copy Constructor Not Implemented
  void operator=(const EMMPMWidget&); // Operator '=' Not Implemented
};

#endif /* _emmpmwidget_h_ */
