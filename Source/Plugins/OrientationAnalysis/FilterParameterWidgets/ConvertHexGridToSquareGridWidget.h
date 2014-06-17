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

#ifndef _ConvertHexGridToSquareGridWidget_H_
#define _ConvertHexGridToSquareGridWidget_H_


#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtCore/QUrl>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "QtSupport/DREAM3DPluginFrame.h"

#include "DREAM3DWidgetsLib/Widgets/PipelineFilterWidget.h"


#include "ui_ConvertHexGridToSquareGridWidget.h"


class ConvertHexGridToSquareGrid;

/**
 * @class ConvertHexGridToSquareGridWidget ConvertHexGridToSquareGridWidget.h Plugins/EbsdImport/UI/ConvertHexGridToSquareGridWidget.h
 * @brief This class represents the User Interface for the EBSD file import module
 * of the DREAM3D program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of EBSD data files into an HDF5 file
 * for better data management and archiving.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class ConvertHexGridToSquareGridWidget : public QWidget, private Ui::ConvertHexGridToSquareGridWidget
{
    Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    ConvertHexGridToSquareGridWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);

    virtual ~ConvertHexGridToSquareGridWidget();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

  public slots:
    void widgetChanged(const QString& msg);
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);


  signals:
    void errorSettingFilterParameter(const QString& msg);
    void parametersChanged();

  protected slots:

    void on_m_InputDirBtn_clicked();
    void on_m_InputDir_textChanged(const QString& text);

    void on_m_OutputDirBtn_clicked();
    void on_m_OutputDir_textChanged(const QString& text);

    void on_m_OutputPrefix_textChanged(const QString& text);

    void on_m_FilePrefix_textChanged(const QString& string);
    void on_m_FileSuffix_textChanged(const QString& string);
    void on_m_FileExt_textChanged(const QString& string);
    void on_m_TotalDigits_valueChanged(int value);
    void on_m_ZStartIndex_valueChanged(int value);
    void on_m_ZEndIndex_valueChanged(int value);

    void resolutionChanged(const QString& string);

  protected:
    static void setOpenDialogLastDirectory(QString val) { m_OpenDialogLastDirectory = val; }
    static QString getOpenDialogLastDirectory() { return m_OpenDialogLastDirectory; }

    /**
     * @brief checkIOFiles
     */
    virtual void checkIOFiles();

    /**
     * @brief verifyPathExists
     * @param outFilePath
     * @param lineEdit
     * @return
     */
    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief setWidgetListEnabled
     */
    void setWidgetListEnabled(bool v);

    /**
     * @brief Method to attempt the extraction of the .ang max slice value and prefix
     */
    void findMaxSliceAndPrefix();

    /**
     * @brief generateExampleInputFile
     */
    void generateExampleInputFile();

    /**
     * @brief getGuiParametersFromFilter
     */
    void getGuiParametersFromFilter();


  private:
    ConvertHexGridToSquareGrid*               m_Filter;
    FilterParameter*            m_FilterParameter;
    QList<QWidget*>             m_WidgetList;
    QButtonGroup*               m_StackingGroup;
    QButtonGroup*               m_OriginGroup;
    QButtonGroup*               m_zSpacingGroup;

    static QString                m_OpenDialogLastDirectory;
    bool m_DidCausePreflight;


    ConvertHexGridToSquareGridWidget(const ConvertHexGridToSquareGridWidget&); // Copy Constructor Not Implemented
    void operator=(const ConvertHexGridToSquareGridWidget&); // Operator '=' Not Implemented
};

#endif /* EbsdImportWIDGET_H_ */
