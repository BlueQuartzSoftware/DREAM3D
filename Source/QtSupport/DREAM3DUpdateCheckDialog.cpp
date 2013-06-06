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

#include <QtGui/QDesktopServices>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>


namespace Detail
{
	const QString UpdatePreferencesGroup("Update Preferences");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DUpdateCheckDialog::DREAM3DUpdateCheckDialog(QWidget* parent) :
  QDialog(parent),
  nam(NULL),
  m_UpdateCheckThread(NULL)
{

  setupUi(this);

  setupGui();

  m_UpdatePreferencesPath = createUpdatePreferencesPath();
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
	QDir parentPathDir = QDir(parentPath);

	if(parentPathDir.mkpath(parentPath))
	{
		QString newParentPrefPath = parentPath + "/DREAM3D_Update_Preferences";
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
DREAM3DUpdateCheckDialog::~DREAM3DUpdateCheckDialog()
{

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
void DREAM3DUpdateCheckDialog::setWhenToCheck(UpdateType whenToCheck)

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
  if (nam != NULL)
  {
    nam->deleteLater();
  }
  nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
                   this, SLOT(networkReplied(QNetworkReply*)));

  QNetworkRequest request;
  request.setUrl(QUrl(m_UpdateWebSite));
  request.setRawHeader("User-Agent", "DREAM3D");

  QNetworkReply* reply = nam->get(request);

  feedbackText->setText("Checking Website for latest version....");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::checkVersion()
{


#if 0
  m_UpdateCheckThread = new QThread(); // Create a new Thread Resource
  m_UpdateCheck = new DREAM3DUpdateCheck(NULL);
  m_UpdateCheck->moveToThread(m_UpdateCheckThread);
  // When the thread starts its event loop, start the PipelineBuilder going
  connect(m_UpdateCheckThread, SIGNAL(started()),
          m_UpdateCheck, SLOT(checkVersion()));

  // When the PipelineBuilder ends then tell the QThread to stop its event loop
  connect(m_UpdateCheck, SIGNAL(finished() ),
          m_UpdateCheckThread, SLOT(quit()) );

  connect(m_UpdateCheck, SIGNAL(hasMessage(const QString&)),
          this, SLOT(threadHasMessage(const QString)));
  m_UpdateCheckThread->start();
#endif



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::networkReplied(QNetworkReply* reply)
{
  // Reading attributes of the reply
  // e.g. the HTTP status code
  QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  // Or the target URL if it was a redirect:
  QVariant redirectionTargetUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
  // see CS001432 on how to handle this

  // no error received?
  if (reply->error() == QNetworkReply::NoError)
  {
    // read data from QNetworkReply here

    // Example 2: Reading bytes form the reply
    QString message;


    QByteArray bytes = reply->readAll();  // bytes
    QString serverVersion(bytes); // string
    serverVersion = serverVersion.trimmed();

    latestVersion->setText(serverVersion);

    QString appVersion = m_CurrentVersion;

    bool ok = false;
    QStringList serverVersionParts = serverVersion.split(QString("."));
    QStringList appVersionParts = appVersion.split(QString("."));

    {
      QString vStr(appVersionParts.at(0));
      vStr.append(".").append(appVersionParts.at(1)).append(".").append(appVersionParts.at(2));
      currentVersion->setText(vStr);
      m_CurrentVersion = vStr;
    }

    {
      QString vStr(serverVersionParts.at(0));
      vStr.append(".").append(serverVersionParts.at(1)).append(".").append(serverVersionParts.at(2));
      latestVersion->setText(vStr);
    }

    int serverMajor = serverVersionParts.at(0).toInt(&ok);
    int appMajor = appVersionParts.at(0).toInt(&ok);

    int serverMinor = serverVersionParts.at(1).toInt(&ok);
    int appMinor = appVersionParts.at(1).toInt(&ok);

    int serverPatch = serverVersionParts.at(2).toInt(&ok);
    int appPatch = appVersionParts.at(2).toInt(&ok);

    if (serverMajor > appMajor  || serverMinor > appMinor || serverPatch > appPatch)
    {
      message.append("<qt><b>There is an update available for ").append(m_AppName).append(".</b><br /><br />  You are currently running version ").append(m_CurrentVersion).append(". If you are ready to update you can go to the regular download <a href=\"http://dream3d.bluequartz.net/downloads\">website</a>.</qt>");
    }
    else
    {
      message.append("<qt><b>").append(m_AppName).append(" is up to date.</b><br /><br /></qt>");
    }
    feedbackText->setText(message);
  }
  // Some http error received
  else
  {
    // handle errors here
  }

  // We receive ownership of the reply object
  // and therefore need to handle deletion.
  reply->deleteLater();
  emit finished();

  std::cout << "DREAM3DUpdateCheckDialog::networkReplied  complete" << std::endl;
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
{
	prefs.beginGroup(Detail::UpdatePreferencesGroup);
	bool ok = false;
	m_WhenToCheck = prefs.value("Frequency").toInt(&ok);
	prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUpdateCheckDialog::writeUpdatePreferences(QSettings &prefs)
{
	prefs.beginGroup(Detail::UpdatePreferencesGroup);
	bool ok = false;
	prefs.setValue( "Frequency", m_WhenToCheck );
	prefs.endGroup();
}