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

#ifndef _precipitatephasewidget_h_
#define _precipitatephasewidget_h_

#include <QtCore/QList>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"

#include "StatsGenerator/Widgets/StatsGenWidget.h"
#include "StatsGenerator/Widgets/PrimaryPhaseWidget.h"
#include "StatsGenerator/Widgets/Presets/AbstractMicrostructurePreset.h"
//#include "StatsGenerator/ui_PrecipitatePhaseWidget.h"


#ifndef QwtArray
#define QwtArray QVector
#endif


class StatsGenRDFWidget;

/*
 *
 */
class PrecipitatePhaseWidget : public PrimaryPhaseWidget
    //, private Ui::PrimaryPhaseWidget
{

    Q_OBJECT

  public:
    PrecipitatePhaseWidget(QWidget* parent = 0);
    virtual ~PrecipitatePhaseWidget();

    void updatePlots();

    void setCrystalStructure(unsigned int xtal) override;

    void setPhaseIndex(int index) override;

    SIMPL_INSTANCE_PROPERTY(float, PptFraction)

    void extractStatsData(AttributeMatrix::Pointer attrMat, int index) override;

    int gatherStatsData(AttributeMatrix::Pointer attrMat, bool preflight = false) override;

    virtual QIcon getPhaseIcon() override;

    virtual void generateDefaultData() override;


  protected slots:

    virtual void on_m_GenerateDefaultData_clicked() override;

  signals:

  void dataChanged();

  void progressText(const QString &text);

  protected:

    void setupGui() override;


  private:
    QList<QWidget*> m_WidgetList;

    StatsGenRDFWidget* m_RdfPlot = nullptr;

    PrecipitatePhaseWidget(const PrecipitatePhaseWidget&); // Copy Constructor Not Implemented
    void operator=(const PrecipitatePhaseWidget&); // Operator '=' Not Implemented
};

#endif /* PrecipitatePhaseWidget_H_ */

