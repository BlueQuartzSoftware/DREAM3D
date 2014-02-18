/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
#ifndef _ComparisonSelectionWidget_H_
#define _ComparisonSelectionWidget_H_


#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtGui/QWidget>




#include "DREAM3DLib/Common/AbstractFilter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"


#include "ComparisonSelectionTableModel.h"

#include "ui_ComparisonSelectionWidget.h"

/**
 * @class ComparisonSelectionWidget ComparisonSelectionWidget.h PipelineBuilder/UI/ComparisonSelectionWidget.h
 * @brief This class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class ComparisonSelectionWidget : public QWidget, private Ui::ComparisonSelectionWidget
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

    DREAM3D_INSTANCE_PROPERTY(ArrayListType, ArrayListType)
    DREAM3D_INSTANCE_PROPERTY(bool, ShowOperators)

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
    QVector<ComparisonInput_t> getComparisonInputs();


  protected slots:

    void on_addComparison_clicked();
    void on_removeComparison_clicked();


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;
    DataContainerArrayProxy m_DcaProxy;
    bool m_DidCausePreflight;
    ComparisonSelectionTableModel*    m_ComparisonSelectionTableModel;

    ComparisonSelectionWidget(const ComparisonSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const ComparisonSelectionWidget&); // Operator '=' Not Implemented

};




#endif /* _ComparisonSelectionWidget_H_ */

