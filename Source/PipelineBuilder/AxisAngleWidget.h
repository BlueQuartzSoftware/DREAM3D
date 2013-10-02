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
#ifndef _AxisAngleWidget_H_
#define _AxisAngleWidget_H_

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtGui/QWidget>

#include "ui_AxisAngleWidget.h"

#include "DREAM3DLib/FilterParameters/FilterParameter.h"

#include "PipelineBuilder/PipelineBuilderDLLExport.h"
#include "AxisAngleTableModel.h"



/**
 * @class AxisAngleWidget AxisAngleWidget.h PipelineBuilder/UI/AxisAngleWidget.h
 * @brief This class presents the use with a Table where they can add pairs of HKL axis and angle in degrees that define
 * a rotation.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class PipelineBuilderLib_EXPORT AxisAngleWidget : public QWidget, private Ui::AxisAngleWidget
{

    Q_OBJECT
  public:
    AxisAngleWidget(QWidget* parent = 0);
    virtual ~AxisAngleWidget();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

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
     * @brief getAxisAngleRotations
     * @return
     */
    virtual std::vector<AxisAngleInput_t> getAxisAngleRotations();

    virtual AxisAngleTableModel* getTableModel();

  signals:
    void AxisAnglesChanged();

  protected slots:

    void on_addRow_clicked();
    void on_deleteRow_clicked();

  protected:


  private:
    AxisAngleTableModel* m_TableModel;

    QString m_OpenDialogLastDirectory; // Must be last in the list

    AxisAngleWidget(const AxisAngleWidget&); // Copy Constructor Not Implemented
    void operator=(const AxisAngleWidget&); // Operator '=' Not Implemented

};




#endif /* _AxisAngleWidget_H_ */



