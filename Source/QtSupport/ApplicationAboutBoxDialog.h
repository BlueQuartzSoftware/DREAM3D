/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef R3DApplicationAboutBoxDialog_H_
#define R3DApplicationAboutBoxDialog_H_

#include <QtGui/QDialog>

//-- UIC generated Header
#include <ui_ApplicationAboutBoxDialog.h>


/**
 * @class ApplicationAboutBoxDialog ApplicationAboutBoxDialog.h R3D/Common/Qt/ApplicationAboutBoxDialog.h
 * @brief This will display a simple dialog box that acts like an "About Box" for
 * showing some basic information about the application being used.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sept 14, 2010
 * @version 1.0
 */
class ApplicationAboutBoxDialog: public QDialog, private Ui::ApplicationAboutBoxDialog
{
    Q_OBJECT

  public:
    ApplicationAboutBoxDialog(QStringList files, QWidget* parent = 0);
    virtual ~ApplicationAboutBoxDialog();

    /**
     * @brief Sets information about the application
     * @param appName
     * @param appVersion
     */
    void setApplicationInfo( QString appName, QString appVersion);

    /**
     * @brief This will set the list of licenses that can be viewed by the
     * about box. The list should contain values such as ":/SomeLicense.license" which
     * are defined in a Qt resource file (*.qrc).
     * @param files The QString list of files to allow the About Box to load.
     */
    void setLicenseFiles(QStringList files);


  protected slots:
    void on_licenseCombo_currentIndexChanged(int index);


  protected:

    /**
     * @brief Loads a file that is stored as a resource in the applications Qt resource
     * system.
     * @param qresourceFile The source file to load of the form ":/SomeFile.xxx"
     */
    void loadResourceFile(const QString qresourceFile);

  private:
    QStringList m_licenseFiles;

    ApplicationAboutBoxDialog(const ApplicationAboutBoxDialog&); // Copy Constructor Not Implemented
    void operator=(const ApplicationAboutBoxDialog&); // Operator '=' Not Implemented


};

#endif /* R3DApplicationAboutBoxDialog_H_ */
