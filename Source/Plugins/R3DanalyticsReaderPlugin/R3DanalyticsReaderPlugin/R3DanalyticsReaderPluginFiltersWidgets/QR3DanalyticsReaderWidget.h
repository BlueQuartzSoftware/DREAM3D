/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-13-M-5048
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef R3DAnalyticsReaderWidget_H_
#define R3DAnalyticsReaderWidget_H_

#include <string>
#include <vector>

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "QtSupport/DREAM3DPluginFrame.h"

#include "PipelineBuilder/QFilterWidget.h"

#include "ui_QR3DAnalyticsReaderWidget.h"


/**
 * @class QR3DAnalyticsReaderWidget QR3DAnalyticsReaderWidget.h Plugins/EbsdImport/UI/R3DAnalyticsReaderWidget.h
 * @brief This class represents the User Interface for the R3D Analytics module
 * of the DREAM3D program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of R3D Analytics output data files into DREAM3D data structure file
 * for better data management and archiving.
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Joseph C. Tucker for UES, Inc.
 * @date Feb 10, 2014
 * @version 1.0
 */
class QR3DAnalyticsReaderWidget : public QFilterWidget, private Ui::QR3DAnalyticsReaderWidget
{
  Q_OBJECT

  public:
    QR3DAnalyticsReaderWidget(QWidget *parent = 0);
    virtual ~QR3DAnalyticsReaderWidget();

    virtual AbstractFilter::Pointer getFilter();

    virtual void writeOptions(QSettings &prefs);
    virtual void readOptions(QSettings &prefs);

    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
     */
    virtual void setWidgetListEnabled(bool b);

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    QFilterWidget* createDeepCopy();

    virtual void checkIOFiles();

    virtual QString getFilterGroup();
    /**
     * @brief Method to attempt the extraction of the .ang max slice value and prefix
     */
    void m_findEbsdMaxSliceAndPrefix();
    void m_generateExampleEbsdInputFile();
    Ebsd::RefFrameZDir getRefFrameZDir();

    std::vector<std::string> generateFileList(int start, int end, bool &hasMissingFiles,
                                                   bool stackLowToHigh, QString filename);


    virtual QUrl htmlHelpIndexFile();

  protected slots:
    /* OIM Data Import Slots */
    void on_m_InputDirBtn_clicked();

    void on_m_FilePrefix_textChanged(const QString &string);
    void on_m_FileSuffix_textChanged(const QString &string);
    void on_m_FileExt_textChanged(const QString &string);
    void on_m_TotalDigits_valueChanged(int value);
    void on_m_ZStartIndex_valueChanged(int value);
    void on_m_ZEndIndex_valueChanged(int value);

    // slots to catch signals emittd by the various QLineEdit widgets
    void on_m_InputDir_textChanged(const QString & text);

  private:
    QList<QWidget*>             m_WidgetList;


    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    QR3DAnalyticsReaderWidget(const QR3DAnalyticsReaderWidget&); // Copy Constructor Not Implemented
    void operator=(const QR3DAnalyticsReaderWidget&); // Operator '=' Not Implemented
};

#endif /* R3DAnalyticsReaderWidget_H_ */
