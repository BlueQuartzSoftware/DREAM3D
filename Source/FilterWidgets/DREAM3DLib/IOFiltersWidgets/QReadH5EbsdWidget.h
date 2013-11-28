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

#ifndef QReadH5Ebsd_H_
#define QReadH5Ebsd_H_

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <QtGui/QFileDialog>


#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/IOFilters/ReadH5Ebsd.h"
#include "PipelineBuilder/QFilterWidget.h"

#include "FilterWidgets/ui_QReadH5EbsdWidget.h"

class QualityMetricTableModel;


class QReadH5EbsdWidget : public QFilterWidget, private Ui::QReadH5EbsdWidget
{
    Q_OBJECT
  public:
    QReadH5EbsdWidget(QWidget* parent = NULL);
    virtual ~QReadH5EbsdWidget();

    virtual AbstractFilter::Pointer getFilter(bool defaultValues);

    virtual void writeOptions(QSettings& prefs);
    virtual void readOptions(QSettings& prefs);

    QFilterWidget* createDeepCopy();

    QString getFilterGroup();

    void setupGui();
    QString  getInputFile();

    void resetGuiFileInfoWidgets();
    void updateFileInfoWidgets();

    Q_PROPERTY(int ZStartIndex READ getZStartIndex WRITE setZStartIndex)
    void setZStartIndex(int value) { this->m_ZStartIndex->setValue(value); emit parametersChanged();}
    int getZStartIndex() { return this->m_ZStartIndex->value(); }

    Q_PROPERTY(int ZEndIndex READ getZEndIndex WRITE setZEndIndex)
    void setZEndIndex(int value) { this->m_ZEndIndex->setValue(value); emit parametersChanged();}
    int getZEndIndex() { return this->m_ZEndIndex->value(); }

    Q_PROPERTY(QString RefFrameZDir READ getRefFrameZDir WRITE setRefFrameZDir)
    void setRefFrameZDir(QString value) { this->m_RefFrameZDir->setText(value); emit parametersChanged();}
    QString getRefFrameZDir() { return this->m_RefFrameZDir->text(); }


    virtual void preflightAboutToExecute(DataContainerArray::Pointer dca);
    virtual void preflightDoneExecuting(DataContainerArray::Pointer dca);

    virtual void openHtmlHelpFile();
    virtual void getGuiParametersFromFilter(AbstractFilter* filt);

  public slots:
    void setInputFile(const QString& v);
    void arraySelectionWidgetChanged();

  protected slots:
    void on_m_InputFileBtn_clicked();
    void on_m_InputFile_textChanged(const QString& text);

  protected:
    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

  private:
    float                        sampleTransAngle;
    QVector<float>           sampleTransAxis;
    float                        eulerTransAngle;
    QVector<float>           eulerTransAxis;

    QString m_FilterGroup;
    bool m_Version4Warning;

    QReadH5EbsdWidget(const QReadH5EbsdWidget&); // Copy Constructor Not Implemented
    void operator=(const QReadH5EbsdWidget&); // Operator '=' Not Implemented
};


#endif /* QReadH5Ebsd_H_ */
