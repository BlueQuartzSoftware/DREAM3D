/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef EbsdImportWIDGET_H_
#define EbsdImportWIDGET_H_


#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtCore/QUrl>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/IOFilters/EbsdToH5Ebsd.h"

#include "QtSupport/DREAM3DPluginFrame.h"

#include "DREAM3DWidgetsLib/Widgets/PipelineFilterWidget.h"


#include "ui_EbsdToH5EbsdWidget.h"




/**
 * @class EbsdToH5EbsdWidget EbsdToH5EbsdWidget.h Plugins/EbsdImport/UI/EbsdToH5EbsdWidget.h
 * @brief This class represents the User Interface for the EBSD file import module
 * of the DREAM3D program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of EBSD data files into an HDF5 file
 * for better data management and archiving.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class DREAM3DWidgetsLib_EXPORT EbsdToH5EbsdWidget : public QWidget, private Ui::EbsdToH5EbsdWidget
{
    Q_OBJECT

  public:
      /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    EbsdToH5EbsdWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    virtual ~EbsdToH5EbsdWidget();

    static void setOpenDialogLastDirectory(QString val) { m_OpenDialogLastDirectory = val; }
    static QString getOpenDialogLastDirectory() { return m_OpenDialogLastDirectory; }



    virtual AbstractFilter::Pointer getFilter(bool defaultValues);

    virtual void writeOptions(QSettings &prefs);
    virtual void readOptions(QSettings &prefs);


    DREAM3D_INSTANCE_PROPERTY(float, SampleTransformationAngle)
    DREAM3D_INSTANCE_PROPERTY(float, EulerTransformationAngle)

    DREAM3D_INSTANCE_PROPERTY(QVector<QString>, EbsdFileList)
    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
     */
    virtual void setWidgetListEnabled(bool b);

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    PipelineFilterWidget* createDeepCopy();

    virtual void checkIOFiles();

    virtual QString getFilterGroup();
    /**
     * @brief Method to attempt the extraction of the .ang max slice value and prefix
     */
    void m_findEbsdMaxSliceAndPrefix();
    void m_generateExampleEbsdInputFile();
    uint32_t getRefFrameZDir();
    void setRefFrameZDir(uint32_t ref);

    QVector<QString> generateFileList(int start, int end, bool &hasMissingFiles,
                                      bool stackLowToHigh, QString filename);

    virtual void openHtmlHelpFile();
    virtual void getGuiParametersFromFilter(AbstractFilter* filt);

  protected:


  protected slots:
    /* OIM Data Import Slots */
    void on_m_InputDirBtn_clicked();
    void on_m_OutputFileBtn_clicked();
    void on_m_RefFrameOptionsBtn_clicked();

    void on_m_FilePrefix_textChanged(const QString &string);
    void on_m_FileSuffix_textChanged(const QString &string);
    void on_m_FileExt_textChanged(const QString &string);
    void on_m_TotalDigits_valueChanged(int value);
    void on_m_ZStartIndex_valueChanged(int value);
    void on_m_ZEndIndex_valueChanged(int value);

    // slots to catch signals emittd by the various QLineEdit widgets
    void on_m_InputDir_textChanged(const QString & text);
    void on_m_OutputFile_textChanged(const QString & text);

  private slots:
    void stackingOrderChanged(bool checked);


  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged();

  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;
    QList<QWidget*>             m_WidgetList;
    QButtonGroup*               m_StackingGroup;
    QButtonGroup*               m_OriginGroup;
    QButtonGroup*               m_zSpacingGroup;

    bool m_TSLchecked;
    bool m_HKLchecked;
    bool m_HEDMchecked;
    bool m_NoTranschecked;
    QVector<float> m_SampleTransformationAxis;
    QVector<float> m_EulerTransformationAxis;

    static QString								m_OpenDialogLastDirectory;

    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    EbsdToH5EbsdWidget(const EbsdToH5EbsdWidget&); // Copy Constructor Not Implemented
    void operator=(const EbsdToH5EbsdWidget&); // Operator '=' Not Implemented
};

#endif /* EbsdImportWIDGET_H_ */
