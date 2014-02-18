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

#include "DREAM3DWidgetsLib/Widgets/PipelineFilterWidget.h"



#include "OrientationAnalysis/ui_ReadH5EbsdWidget.h"

class QualityMetricTableModel;
class ReadH5Ebsd;

class ReadH5EbsdWidget : public QWidget, private Ui::ReadH5EbsdWidget
{
    Q_OBJECT
  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    ReadH5EbsdWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);

    virtual ~ReadH5EbsdWidget();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

  public slots:
    //void widgetChanged(const QString& msg);
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);

  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged();

  protected slots:
    void on_m_InputFileBtn_clicked();
    void on_m_InputFile_textChanged(const QString & text);
    void on_m_CellList_itemClicked(QListWidgetItem * item);
    void on_m_DataArraysCheckBox_stateChanged(int state);

#if 0
    virtual AbstractFilter::Pointer getFilter(bool defaultValues);

    virtual void writeOptions(QSettings &prefs);
    virtual void readOptions(QSettings &prefs);

    PipelineFilterWidget* createDeepCopy();

    QString getFilterGroup();

    void setupGui();
    QString  getInputFile();



    Q_PROPERTY(int ZStartIndex READ getZStartIndex WRITE setZStartIndex)
    void setZStartIndex(int value) { this->m_ZStartIndex->setValue(value); emit parametersChanged();}
    int getZStartIndex() { return this->m_ZStartIndex->value(); }

    Q_PROPERTY(int ZEndIndex READ getZEndIndex WRITE setZEndIndex)
    void setZEndIndex(int value) { this->m_ZEndIndex->setValue(value); emit parametersChanged();}
    int getZEndIndex() { return this->m_ZEndIndex->value(); }

    Q_PROPERTY(QString RefFrameZDir READ getRefFrameZDir WRITE setRefFrameZDir)
    void setRefFrameZDir(QString value) { this->m_RefFrameZDir->setText(value); emit parametersChanged();}
    QString getRefFrameZDir() { return this->m_RefFrameZDir->text(); }

    virtual void openHtmlHelpFile();


  public slots:
   // void setInputFile(const QString &v);
    void arraySelectionWidgetChanged();

#endif

  protected:
    static void setOpenDialogLastDirectory(QString val) { m_OpenDialogLastDirectory = val; }
    static QString getOpenDialogLastDirectory() { return m_OpenDialogLastDirectory; }


    /**
     * @brief setInputFile
     * @param v
     */
    void setInputFile(const QString &v);

    /**
     * @brief setWidgetListEnabled
     */
    void setWidgetListEnabled(bool v);

    /**
     * @brief verifyPathExists
     * @param outFilePath
     * @param lineEdit
     * @return
     */
    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief resetGuiFileInfoWidgets
     */
    void resetGuiFileInfoWidgets();

    /**
     * @brief updateFileInfoWidgets
     */
    void updateFileInfoWidgets();


    /**
     * @brief getSelectedArrayNames
     * @return
     */
    QSet<QString> getSelectedArrayNames();

    /**
    * @brief getSelectedEnsembleNames
    * @return
    */
    QSet<QString> getSelectedEnsembleNames();

    void updateModelFromFilter(QSet<QString> &arrayNames, bool setChecked = false);


  private:
    ReadH5Ebsd*         m_Filter;
    FilterParameter*          m_FilterParameter;
    QList<QWidget*>             m_WidgetList;
    AxisAngleInput_t m_SampleTransformation;
    AxisAngleInput_t m_EulerTransformation;
    bool m_Version4Warning;
    static QString								m_OpenDialogLastDirectory;
    bool m_DidCausePreflight;

    ReadH5EbsdWidget(const ReadH5EbsdWidget&); // Copy Constructor Not Implemented
    void operator=(const ReadH5EbsdWidget&); // Operator '=' Not Implemented
};


#endif /* QReadH5Ebsd_H_ */
