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
#ifndef _QGenerateEnsembleStatisticsWidget_H_
#define _QGenerateEnsembleStatisticsWidget_H_


#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QWidget>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/StatisticsFilters/GenerateEnsembleStatistics.h"

#include "PipelineBuilder/QFilterWidget.h"

#include "FilterWidgets/ui_QGenerateEnsembleStatisticsWidget.h"

class DistributionTypeWidget;

/**
 * @brief The QGenerateEnsembleStatisticsWidget class
 */
class QGenerateEnsembleStatisticsWidget : public QFilterWidget, private Ui::QGenerateEnsembleStatisticsWidget
{
    Q_OBJECT
  public:
    QGenerateEnsembleStatisticsWidget(QWidget* parent = NULL);
    ~QGenerateEnsembleStatisticsWidget();

    virtual AbstractFilter::Pointer getFilter(bool defaultValues);
    void writeOptions(QSettings &prefs);
    void readOptions(QSettings &prefs);

    QFilterWidget* createDeepCopy();

    QString getFilterGroup();


    virtual void preflightAboutToExecute(VolumeDataContainer::Pointer vldc, SurfaceDataContainer::Pointer sdc, EdgeDataContainer::Pointer edc, VertexDataContainer::Pointer vdc);
    virtual void preflightDoneExecuting(VolumeDataContainer::Pointer vldc, SurfaceDataContainer::Pointer sdc, EdgeDataContainer::Pointer edc, VertexDataContainer::Pointer vdc);

    virtual void openHtmlHelpFile();
    virtual void getGuiParametersFromFilter(AbstractFilter* filt);

  protected:
    void setupGui();


  private slots:
    void on_addPhaseType_clicked();
    void on_removePhaseType_clicked();
    void inputChanged();


  private:
    QString m_FilterGroup;


    QGenerateEnsembleStatisticsWidget(const QGenerateEnsembleStatisticsWidget&); // Copy Constructor Not Implemented
    void operator=(const QGenerateEnsembleStatisticsWidget&); // Operator '=' Not Implemented


};





#endif /* _QGenerateEnsembleStatisticsWidget_H_ */
