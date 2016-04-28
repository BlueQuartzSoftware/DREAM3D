/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#ifndef _ReadEdaxH5DataWidget_H_
#define _ReadEdaxH5DataWidget_H_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SIMPLViewWidgetsLib/SIMPLViewWidgetsLib.h"
#include "SIMPLViewWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SIMPLViewWidgetsLib/Widgets/PipelineFilterWidget.h"

#include "OrientationAnalysis/ui_ReadEdaxH5DataWidget.h"


class QualityMetricTableModel;
class ReadEdaxH5Data;
class ReadEdaxH5DataFilterParameter;

/**
 * @brief The ReadEdaxH5DataWidget class
 */
class ReadEdaxH5DataWidget : public FilterParameterWidget, private Ui::ReadEdaxH5DataWidget
{
    Q_OBJECT
  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    ReadEdaxH5DataWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);

    virtual ~ReadEdaxH5DataWidget();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    void updateList();

    void setFilter(AbstractFilter* value);
    AbstractFilter* getFilter() const;

    void setFilterParameter(FilterParameter* value);
    FilterParameter* getFilterParameter() const;

  public slots:
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);

    void on_addScanName_pressed();
    void on_removeScanName_pressed();
    void on_stackLowToHighBtn_toggled(bool checked);

  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged();

  private:
    ReadEdaxH5Data*                         m_Filter;
    ReadEdaxH5DataFilterParameter*          m_FilterParameter;

    void sortList(DREAM3DListWidget* listWidget, Qt::SortOrder order);

    ReadEdaxH5DataWidget(const ReadEdaxH5DataWidget&); // Copy Constructor Not Implemented
    void operator=(const ReadEdaxH5DataWidget&); // Operator '=' Not Implemented
};


#endif /* ReadEdaxH5DataWidget_H_ */
