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

#ifndef _ComparisonSelectionWidget_H_
#define _ComparisonSelectionWidget_H_


#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>


#include "QtSupportLib/FaderWidget.h"


#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/ComparisonSelectionFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
#include "DREAM3DWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"



#include "ComparisonSelectionTableModel.h"

#include "ui_ComparisonSelectionWidget.h"

/**
 * @class ComparisonSelectionWidget ComparisonSelectionWidget.h PipelineBuilder/UI/ComparisonSelectionWidget.h
 * @brief This class
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class ComparisonSelectionWidget : public FilterParameterWidget, private Ui::ComparisonSelectionWidget
{

    Q_OBJECT
  public:
    ComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    virtual ~ComparisonSelectionWidget();



    enum ArrayListType
    {
      CellListType,
      FeatureListType,
      EnsembleListType,
      VertexListType,
      EdgeListType,
      FaceListType,
    };

    SIMPL_INSTANCE_PROPERTY(ArrayListType, ArrayListType)
    SIMPL_INSTANCE_PROPERTY(bool, ShowOperators)

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

  public slots:
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);


  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged();

  protected:
    /**
     * @brief setComparisons
     * @param comparisons
     */
    virtual void setComparisons(QVector<ComparisonInput_t> comparisons);

    /**
     * @brief getComparisonInputs
     * @return
     */
    ComparisonInputs getComparisonInputs();

    void populateComboBoxes();
    void populateAttributeMatrixList();
    QStringList generateAttributeArrayList();
    QString checkStringValues(QString curDcName, QString filtDcName);

  protected slots:

    void on_addComparison_clicked();
    void on_removeComparison_clicked();

    void on_dataContainerCombo_currentIndexChanged(int index);

    void on_attributeMatrixCombo_currentIndexChanged(int index);

    void tableDataWasChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void widgetChanged(const QString& text);

  private:

    DataContainerArrayProxy m_DcaProxy;
    bool m_DidCausePreflight;


    ComparisonSelectionTableModel*    m_ComparisonSelectionTableModel;

    ComparisonSelectionFilterParameter* m_FilterParameter;

    ComparisonSelectionWidget(const ComparisonSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const ComparisonSelectionWidget&); // Operator '=' Not Implemented

};




#endif /* _ComparisonSelectionWidget_H_ */

