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


#ifndef _ebsdtoh5ebsdwidget_h_
#define _ebsdtoh5ebsdwidget_h_


#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtCore/QUrl>

#include "EbsdLib/EbsdTransform.h"
#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SVWidgetsLib/QtSupport/QtSPluginFrame.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterWidget.h"


#include "ui_EbsdToH5EbsdWidget.h"




class EbsdToH5Ebsd;

/**
 * @class EbsdToH5EbsdWidget EbsdToH5EbsdWidget.h Plugins/EbsdImport/UI/EbsdToH5EbsdWidget.h
 * @brief This class represents the User Interface for the EBSD file import module
 * of the DREAM3D program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of EBSD data files into an HDF5 file
 * for better data management and archiving.
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class EbsdToH5EbsdWidget : public FilterParameterWidget, private Ui::EbsdToH5EbsdWidget
{
    Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    EbsdToH5EbsdWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

    virtual ~EbsdToH5EbsdWidget();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    void setFilter(AbstractFilter* value);
    AbstractFilter* getFilter() const;

  public slots:
    void widgetChanged(const QString& msg);
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);

    /**
     * @brief showFileInFileSystem
     */
    virtual void showFileInFileSystem();

  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged();

  protected slots:
    /* OIM Data Import Slots */
    void on_m_InputDirBtn_clicked();
    void on_m_OutputFileBtn_clicked();
    void on_m_RefFrameOptionsBtn_clicked();

    void on_m_FilePrefix_textChanged(const QString& string);
    void on_m_FileSuffix_textChanged(const QString& string);
    void on_m_FileExt_textChanged(const QString& string);
    void on_m_TotalDigits_valueChanged(int value);
    void on_m_ZStartIndex_valueChanged(int value);
    void on_m_ZEndIndex_valueChanged(int value);
    void on_m_zSpacing_textChanged(const QString& string);

    // slots to catch signals emitted by the various QLineEdit widgets
    void on_m_LineEdit_textChanged(const QString& text);
    void on_m_OutputFile_textChanged(const QString& text);

    void stackingOrderChanged(bool checked);

  protected:
    void setInputDirectory(QString val);
    QString getInputDirectory();

    void setOutputPath(QString val);
    QString getOutputPath();

    /**
     * @brief checkIOFiles
     */
    virtual void checkIOFiles();

    /**
     * @brief validateInputFile
     */
    void validateInputFile();

    /**
     * @brief setWidgetListEnabled
     */
    void setWidgetListEnabled(bool v);

    /**
     * @brief Method to attempt the extraction of the .ang max slice value and prefix
     */
    void findEbsdMaxSliceAndPrefix();

    /**
     * @brief m_generateExampleEbsdInputFile
     */
    void generateExampleEbsdInputFile();

    /**
     * @brief getRefFrameZDir
     * @return
     */
    uint32_t getRefFrameZDir();

    /**
     * @brief setRefFrameZDir
     * @param ref
     */
    void setRefFrameZDir(uint32_t ref);

    /**
     * @brief getGuiParametersFromFilter
     */
    void getGuiParametersFromFilter();

    /**
     * @brief IdentifyRefFrame
     */
    void identifyRefFrame();
    
    /**
    * @brief
    * @param event
    */
    void keyPressEvent(QKeyEvent* event);

    /**
     * @brief setupMenuField
     */
    void setupMenuField();

  private:
    EbsdToH5Ebsd*               m_Filter = nullptr;
    QList<QWidget*>             m_WidgetList;
    QButtonGroup*               m_StackingGroup = nullptr;
    QButtonGroup*               m_OriginGroup = nullptr;
    QButtonGroup*               m_zSpacingGroup = nullptr;

    bool m_TSLchecked;
    bool m_HKLchecked;
    bool m_HEDMchecked;
    bool m_NoTranschecked;
    AxisAngleInput_t m_SampleTransformation;
    AxisAngleInput_t m_EulerTransformation;
    QAction* m_ShowFileAction = nullptr;
    QString  m_CurrentlyValidPath = "";
    QString  m_CurrentText = "";
    bool     m_DidCausePreflight = false;


    EbsdToH5EbsdWidget(const EbsdToH5EbsdWidget&); // Copy Constructor Not Implemented
    void operator=(const EbsdToH5EbsdWidget&); // Operator '=' Not Implemented
};

#endif /* EbsdImportWIDGET_H_ */
