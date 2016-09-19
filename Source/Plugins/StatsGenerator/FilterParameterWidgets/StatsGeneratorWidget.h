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

#ifndef _statsGeneratorWidget_h_
#define _statsGeneratorWidget_h_


#include <QtCore/QObject>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterWidget.h"

#include "StatsGenerator/Widgets/SGWidget.h"
#include "StatsGenerator/Widgets/PrimaryPhaseWidget.h"
#include "StatsGenerator/Widgets/PrecipitatePhaseWidget.h"
#include "StatsGenerator/Widgets/MatrixPhaseWidget.h"
#include "StatsGenerator/Widgets/BoundaryPhaseWidget.h"
#include "StatsGenerator/Widgets/TransformationPhaseWidget.h"

#include <ui_StatsGeneratorWidget.h>

class StatsGeneratorFilter;
class StatsGeneratorFilterParameter;

/**
 * @class StatsGeneratorWidget StatsGeneratorWidget.h StatsGenerator/StatsGeneratorWidget.h
 * @brief
 *
 * @date Jan 4, 2011
 * @version 1.0
 */
class StatsGeneratorWidget : public FilterParameterWidget, private Ui::StatsGeneratorWidget
{
    Q_OBJECT

  public:
    StatsGeneratorWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);
    virtual ~StatsGeneratorWidget();

    void adjustWindowTitle();
    void displayDialogBox(QString title, QString text, QMessageBox::Icon icon);

  protected slots:

    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);
    void on_updatePipelineBtn_clicked();

   // void on_m_GenerateDefaultData_clicked();
    void on_saveH5Btn_clicked();
    void on_actionSaveAs_triggered();
    void on_addPhase_clicked();
    void on_deletePhase_clicked();
    void on_editPhase_clicked();
    void on_saveJsonBtn_clicked();
    void on_openStatsFile_clicked();
    void on_phaseTabs_tabCloseRequested ( int index );

  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged(); // This function MUST be named this way

  protected:
    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    void setupGui();


    /**
     * @brief Verifies that a path exists on the file system.
     * @param outFilePath The file path to check
     * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
     */
    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief Verifies that a parent path exists on the file system.
     * @param outFilePath The parent file path to check
     * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
     */
    bool verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief generateDataContainerArray
     * @return
     */
    DataContainerArray::Pointer generateDataContainerArray();

  private:
    StatsGeneratorFilterParameter* m_FilterParameter;
    StatsGeneratorFilter*          m_Filter;

    QString m_OpenDialogLastDirectory; // Must be last in the list
    StatsGeneratorWidget(const StatsGeneratorWidget&); // Copy Constructor Not Implemented
    void operator=(const StatsGeneratorWidget&); // Operator '=' Not Implemented
};

#endif /* _statsGeneratorWidget_h_ */

