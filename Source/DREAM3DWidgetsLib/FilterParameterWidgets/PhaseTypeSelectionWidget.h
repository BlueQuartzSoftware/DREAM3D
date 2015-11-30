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

#ifndef _PhaseTypeSelectionWidget_H_
#define _PhaseTypeSelectionWidget_H_

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>


#include "QtSupportLib/FaderWidget.h"


#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/PhaseTypeSelectionFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
#include "DREAM3DWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"


#include "DREAM3DWidgetsLib/ui_PhaseTypeSelectionWidget.h"

class QComboBox;
class QLabel;

/**
* @brief
* @author
* @version
*/
class DREAM3DWidgetsLib_EXPORT PhaseTypeSelectionWidget : public FilterParameterWidget, private Ui::PhaseTypeSelectionWidget
{
    Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    PhaseTypeSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);

    PhaseTypeSelectionWidget(QWidget* parent = NULL);

    virtual ~PhaseTypeSelectionWidget();

    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

    /**
     * @brief checkStringValues
     * @param current
     * @param filt
     * @return
     */
    QString checkStringValues(QString current, QString filtDcName);

  public slots:
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);

    void phaseTypeComboBoxChanged(int index);


    void on_dataContainerCombo_currentIndexChanged(int index);

    void on_attributeMatrixCombo_currentIndexChanged(int index);

  protected:
    void updatePhaseComboBoxes();

    void populateComboBoxes();
    void populateAttributeMatrixList();
    DataContainerArrayProxy generateDCAProxy();
    void setSelectedPath(QString dcName, QString attrMatName, QString attrArrName);
    void selectDefaultPath();
    void resetPhaseComboBoxes();


  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged();

  private:
    bool              m_DidCausePreflight;

//    QList<QLabel*>              m_PhaseTypeLabels;
//    QList<QComboBox*>           m_PhaseTypeCombos;

    DataContainerArrayProxy m_DcaProxy;

    PhaseTypeSelectionFilterParameter* m_FilterParameter;

    PhaseTypeSelectionWidget(const PhaseTypeSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const PhaseTypeSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _PhaseTypeSelectionWidget_H_ */


