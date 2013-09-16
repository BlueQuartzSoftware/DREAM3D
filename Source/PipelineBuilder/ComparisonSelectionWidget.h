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
#include <set>

#include <QtCore/QSettings>
#include <QtGui/QWidget>


#include "ui_ComparisonSelectionWidget.h"

#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataContainers/SurfaceDataContainer.h"
#include "DREAM3DLib/DataContainers/EdgeDataContainer.h"
#include "DREAM3DLib/DataContainers/VertexDataContainer.h"
#include "DREAM3DLib/FilterParameters/FilterParameter.h"
#include "PipelineBuilder/PipelineBuilderDLLExport.h"


#include "ComparisonSelectionTableModel.h"


/**
 * @class ComparisonSelectionWidget ComparisonSelectionWidget.h PipelineBuilder/UI/ComparisonSelectionWidget.h
 * @brief This class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class PipelineBuilderLib_EXPORT ComparisonSelectionWidget : public QWidget, private Ui::ComparisonSelectionWidget
{

    Q_OBJECT
  public:
    ComparisonSelectionWidget(bool showOperators, QWidget* parent = 0);
    virtual ~ComparisonSelectionWidget();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    virtual void setHumanLabel(QString title);

    enum ArrayListType
    {
      CellListType,
      FieldListType,
      EnsembleListType,
      VertexListType,
      EdgeListType,
      FaceListType,
    };

    DREAM3D_INSTANCE_PROPERTY(ArrayListType, ArrayListType)
    DREAM3D_INSTANCE_PROPERTY(bool, ShowOperators)

    /**
     * @brief populateArrayNames
     * @param vdc
     * @param smdc
     * @param sdc
     */
    virtual void populateArrayNames(VolumeDataContainer::Pointer vldc,
                                    SurfaceDataContainer::Pointer sdc,
                                    EdgeDataContainer::Pointer edc,
                                    VertexDataContainer::Pointer vdc);


    /**
     * @brief setComparisons
     * @param comparisons
     */
    virtual void setComparisons(QVector<ComparisonInput_t> comparisons);

    /**
     * @brief writeOptions
     * @param prefs
     */
    virtual void readOptions(QSettings &prefs, QString name);

        /**
     * @brief writeOptions
     * @param prefs
     */
    virtual void writeOptions(QSettings &prefs, QString name);

    /**
     * @brief getComparisonInputs
     * @return
     */
    QVector<ComparisonInput_t> getComparisonInputs();

  signals:
    void parametersChanged();

  protected slots:

    void on_addComparison_clicked();
    void on_removeComparison_clicked();

  protected:



    virtual void populateVolumeArrayNames(VolumeDataContainer::Pointer vldc);
    virtual void populateSurfaceArrayNames(SurfaceDataContainer::Pointer sdc);
    virtual void populateEdgeArrayNames(EdgeDataContainer::Pointer edc);
    virtual void populateVertexArrayNames(VertexDataContainer::Pointer vdc);


  private:
    ComparisonSelectionTableModel*    m_ComparisonSelectionTableModel;

    ComparisonSelectionWidget(const ComparisonSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const ComparisonSelectionWidget&); // Operator '=' Not Implemented

};




#endif /* _ComparisonSelectionWidget_H_ */
