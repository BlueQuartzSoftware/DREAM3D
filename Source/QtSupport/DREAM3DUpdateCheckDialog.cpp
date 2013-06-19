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


#include "DREAM3DUpdateCheckDialog.h"

#include <iostream>

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDate>

#include <QtGui/QDesktopServices>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>


namespace Detail
{
	const QString UpdatePreferencesGroup("UpdatePreferences");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DUpdateCheckDialog::DREAM3DUpdateCheckDialog(QWidget* parent) :
  QDialog(parent),
  m_UpdateCheckThread(NULL),
  m_DialogState(DefaultDialog),
  m_UpdateCheck(NULL)
{

  setupUi(this);

  setupGui();

  // If the UpdatePreferences.ini file exists, read the values in
  m_UpdatePreferencesPath = createUpdatePreferencesPath();
  QSettings updatePrefs(m_UpdatePreferencesPath, QSettings::IniFormat);
  QDir prefPathDir(m_UpdatePreferencesPath);
  if ( prefPathDir.exists(m_UpdatePreferencesPath) )
  {
    readUpdatePreferences(updatePrefs);
  }

  // If file doesn't exist yet, write the default preferences for the first time
  else
  {
    m_WhenToCheck = UpdateCheckManual;
      writeUpdatePreferences(updatePrefs);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DUpdateCheckDialog::~DREAM3DUpdateCheckDialog()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DREAM3DUpdateCheckDialog::getUpdatePreferencesGroup()
{
	return Detail::UpdatePreferencesGroup;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRadioButton* DREAM3DUpdateCheckDialog::getAutomaticallyBtn()
{
	return automatically;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRadioButton* DREAM3DUpdateCheckDialog::getManuallyBtn()
{
	return manually;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QComboBox* DREAM3DUpdateCheckDialog::getHowOftenComboBox()
{
	return howOften;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPushButton* DREAM3DUpdateCheckDialog::getCheckNowBtn()
{
	return checkNowBtn;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DREAM3DUpdateCheckDialog::getCurrentVersion()
{
	return m_CurrentVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* DREAM3DUpdateCheckDialog::getCurrentVersionLabel()
{
	return currentVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* DREAM3DUpdateCheckDialog::getLatestVersionLabel()
{
	return latestVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DREAM3DUpdateCheckDialog::getAppName()
{
	return m_AppName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLabel* DREAM3DUpdateCheckDialog::getFeedbackTextLabel()
{
	return feedbackText;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DREAM3DUpdateCheckDialog::createUpdatePreferencesPath()
{
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
  QString extension = ".ini";
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
  QString extension = ".ini";
#endif

  QString prefFile = prefs.fileName();
  QFileInfo prefFileInfo = QFileInfo(prefFile);
  QString parentPath = prefFileInfo.path();
  QString newParentPrefPath = parentPath + "/DREAM3D_Update_Preferences";
  QDir parentPathDir = QDir(newParentPrefPath);

  if(parentPathDir.mkpath(newParentPrefPath))
  {
    QString newPrefPath = newParentPrefPath + "/UpdatePreferences" + extension;

    newPrefPath = QDir::toNativeSeparators(newPrefPath);
    return newPrefPath;
  }
  else
  {
    // This lets us know if there was an error creating the parent path
    return "";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DREAM3DUpdateCheckDialog::getUpdatePreferencesPath()
{
  return m_UpdatePreferencesPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::setCurrentVersion(QString version)
{
  m_CurrentVersion = version;
  QStringList appVersionParts = m_CurrentVersion.split(QString("."));

  {
    QString vStr(appVersionParts.at(0));
    vStr.append(".").append(appVersionParts.at(1)).append(".").append(appVersionParts.at(2));
    currentVersion->setText(vStr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::setLastCheckDateTime(QDateTime lastDateTime)
{
  m_LastCheckDateTime = lastDateTime;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::setApplicationName(QString name)
{
  m_AppName = name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DREAM3DUpdateCheckDialog::getWhenToCheck()
{
  return m_WhenToCheck;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::setWhenToCheck(int whenToCheck)

{
  m_WhenToCheck = whenToCheck;
  if (m_WhenToCheck == DREAM3DUpdateCheckDialog::UpdateCheckManual)
  {
    manually->setChecked(true);
    howOften->setEnabled(false);
  }
  else
  {
    automatically->setChecked(true);
    howOften->setCurrentIndex(whenToCheck);
    howOften->setEnabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::setUpdateWebSite(QString url)
{
  m_UpdateWebSite = url;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::setupGui()
{
  latestVersion->setText("Not Checked");
  feedbackText->setText("");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::on_checkNowBtn_clicked()
{
	checkNowBtn->setEnabled(false);
	feedbackText->setText("Checking for Updates...");
	m_UpdateCheck = new UpdateCheck(this);

	connect( m_UpdateCheck, SIGNAL( LatestVersion(UpdateCheckData*) ), 
		this, SLOT( LatestVersionReplied(UpdateCheckData*) ) );

	m_UpdateCheck->checkVersion(m_UpdateWebSite);
	checkNowBtn->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::on_websiteBtn_clicked()
{
  QUrl url("http://dream3d.bluequartz.net/downloads");
  QDesktopServices::openUrl(url);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::readUpdatePreferences(QSettings &prefs)
{  // Read in value from preferences file
  prefs.beginGroup(Detail::UpdatePreferencesGroup);
  bool ok = false;
  m_WhenToCheck = static_cast<UpdateType>( prefs.value("Frequency").toInt(&ok) );
  prefs.endGroup();

  if (m_WhenToCheck == UpdateCheckManual)
  {
    manually->blockSignals(true);
    manually->setChecked(true);
    howOften->setEnabled(false);
    manually->blockSignals(false);
  }
  else
  {
    automatically->blockSignals(true);
    howOften->blockSignals(true);
    automatically->setChecked(true);
    if (m_WhenToCheck == UpdateCheckDaily)
    {
      howOften->setCurrentIndex(UpdateCheckDaily);
    }
    else if (m_WhenToCheck == UpdateCheckWeekly)
    {
      howOften->setCurrentIndex(UpdateCheckWeekly);
    }
    else
    {
      howOften->setCurrentIndex(UpdateCheckMonthly);
    }
    automatically->blockSignals(false);
    howOften->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::writeUpdatePreferences(QSettings &prefs)
{
  prefs.beginGroup(Detail::UpdatePreferencesGroup);
  prefs.setValue( "Frequency", m_WhenToCheck );
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::LatestVersionReplied(UpdateCheckData* dataObj)
{
	QString message = dataObj->getMessageDescription();
	feedbackText->setText(message);
	currentVersion->setText( dataObj->getAppString() );
	setCurrentVersion( dataObj->getAppString() );
	latestVersion->setText( dataObj->getServerString() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::toSimpleUpdateCheckDialog()
{
	// Exit immediately if the dialog is already in simple state
	if (m_DialogState == SimpleDialog)
	{
		return;
	}

	checkNowBtn->setVisible(false);
	automatically->setVisible(false);
	manually->setVisible(false);
	howOften->setVisible(false);
	messageLabel->setText("Update Available!");

	// Update Dialog State
	m_DialogState = SimpleDialog;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::toDefaultUpdateCheckDialog()
{
	// Exit immediately if the dialog is already in default state
	if (m_DialogState == DefaultDialog)
	{
		return;
	}

	checkNowBtn->setVisible(true);
	automatically->setVisible(true);
	manually->setVisible(true);
	howOften->setVisible(true);
	messageLabel->setText("How would you like to check for updates?");

	// Update Dialog State
	m_DialogState = DefaultDialog;
}