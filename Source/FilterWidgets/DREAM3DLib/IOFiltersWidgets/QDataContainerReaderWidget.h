/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _QDataContainerReaderWidget_H_
#define _QDataContainerReaderWidget_H_


#include <string>
#include <set>

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include "PipelineBuilder/QFilterWidget.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "DREAM3DLib/IOFilters/DataContainerReader.h"

#include "FilterWidgets/ui_QDataContainerReaderWidget.h"

class QListWidget;

class QDataContainerReaderWidget : public QFilterWidget, private Ui::QDataContainerReaderWidget
{
    Q_OBJECT
  public:
    QDataContainerReaderWidget(QWidget* parent = NULL);
    virtual ~QDataContainerReaderWidget();

    virtual AbstractFilter::Pointer getFilter(bool defaultValues);

    void writeOptions(QSettings &prefs);
    void readOptions(QSettings &prefs);

    QFilterWidget* createDeepCopy();

    QString getFilterGroup();

    void setupGui();
    QString  getInputFile();

    Q_PROPERTY(bool ReadVolumeData READ getReadVolumeData WRITE setReadVolumeData)
    void setReadVolumeData(bool value) { this->ReadVolumeData->setChecked(value); emit parametersChanged();}
    bool getReadVolumeData() { return this->ReadVolumeData->isChecked(); }

    Q_PROPERTY(bool ReadSurfaceData READ getReadSurfaceData WRITE setReadSurfaceData)
    void setReadSurfaceData(bool value) { this->ReadSurfaceData->setChecked(value); emit parametersChanged();}
    bool getReadSurfaceData() { return this->ReadSurfaceData->isChecked(); }

    Q_PROPERTY(bool ReadVertexData READ getReadVertexData WRITE setReadVertexData)
    void setReadVertexData(bool value) { this->ReadVertexData->setChecked(value); emit parametersChanged();}
    bool getReadVertexData() { return this->ReadVertexData->isChecked(); }

    Q_PROPERTY(bool ReadVertexData READ getReadEdgeData WRITE setReadEdgeData)
    void setReadEdgeData(bool value) { this->ReadEdgeData->setChecked(value); emit parametersChanged();}
    bool getReadEdgeData() { return this->ReadEdgeData->isChecked(); }

    virtual void preflightAboutToExecute(VolumeDataContainer::Pointer vldc, SurfaceDataContainer::Pointer sdc, EdgeDataContainer::Pointer edc, VertexDataContainer::Pointer vdc);
    virtual void preflightDoneExecuting(VolumeDataContainer::Pointer vldc, SurfaceDataContainer::Pointer sdc, EdgeDataContainer::Pointer edc, VertexDataContainer::Pointer vdc);

    virtual void openHtmlHelpFile();

    virtual void getGuiParametersFromFilter(AbstractFilter* filt);

  public slots:
    void setInputFile(const QString &v);
    void arraySelectionWidgetChanged();


  protected slots:
    void on_InputFileBtn_clicked();
    void on_InputFile_textChanged(const QString & text);
    void on_ReadVolumeData_stateChanged(int state);
    void on_ReadSurfaceData_stateChanged(int state);
    void on_ReadVertexData_stateChanged(int state);
    void on_ReadEdgeData_stateChanged(int state);

  protected:
    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);


  private:
    QString m_FilterGroup;

    QDataContainerReaderWidget(const QDataContainerReaderWidget&);
    void operator=(const QDataContainerReaderWidget&);
};


#endif /* _QDataContainerReaderWidget_H_ */
