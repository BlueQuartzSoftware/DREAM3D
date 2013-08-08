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

#ifndef _QINITIALIZESYNTHETICVOLUMEWIDGET_H_
#define _QINITIALIZESYNTHETICVOLUMEWIDGET_H_


#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QFileDialog>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/SyntheticBuildingFilters/InitializeSyntheticVolume.h"

#include "PipelineBuilder/QFilterWidget.h"

#include "FilterWidgets/ui_QInitializeSyntheticVolumeWidget.h"

class QComboBox;

/*
 *
 */
class QInitializeSyntheticVolumeWidget : public QFilterWidget, private Ui::QInitializeSyntheticVolumeWidget
{
  Q_OBJECT

  public:
    QInitializeSyntheticVolumeWidget(QWidget* parent = NULL);
    virtual ~QInitializeSyntheticVolumeWidget();

    virtual AbstractFilter::Pointer getFilter(bool defaultValues);

    virtual void writeOptions(QSettings &prefs);
    virtual void readOptions(QSettings &prefs);

    int estimate_numgrains(int xpoints, int ypoints, int zpoints,
                           float xres, float yres, float zres);
    void estimateNumGrainsSetup();

    QFilterWidget* createDeepCopy();

    void setShapeTypes(DataArray<unsigned int>::Pointer array);

    virtual QString getFilterGroup();

    virtual void openHtmlHelpFile();
    virtual void getGuiParametersFromFilter(AbstractFilter* filt);

  protected slots:
    // Auto Hookup Slots
    void on_m_InputFileBtn_clicked();
    void on_m_InputFile_textChanged(const QString &text);
    void on_m_XPoints_valueChanged(int v);
    void on_m_YPoints_valueChanged(int v);
    void on_m_ZPoints_valueChanged(int v);
    void on_m_XResolution_valueChanged(double v);
    void on_m_YResolution_valueChanged(double v);
    void on_m_ZResolution_valueChanged(double v);

  private:
  //  InitializeSyntheticVolume::Pointer m_Filter;
    void setupGui();
    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    QString                     m_OpenDialogLastDirectory;
    VoxelDataContainer::Pointer      m_DataContainer;
    QList<QLabel*>              m_ShapeTypeLabels;
    QList<QComboBox*>           m_ShapeTypeCombos;


    QInitializeSyntheticVolumeWidget(const QInitializeSyntheticVolumeWidget&); // Copy Constructor Not Implemented
    void operator=(const QInitializeSyntheticVolumeWidget&); // Operator '=' Not Implemented

};

#endif /* _QINITIALIZESYNTHETICVOLUMEWIDGET_H_ */
