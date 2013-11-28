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
#include "UpdateCheck.h"

#include <QtCore/QDate>
#include <QtCore/QSettings>

#include <QtGui/QMessageBox>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>


#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/AppVersion.h"

#include "QtSupport/DREAM3DUpdateCheckDialog.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCheck::UpdateCheck(QObject* parent) :
  QObject(parent),
  m_Nam(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCheck::~UpdateCheck()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheck::checkVersion(QUrl website)
{
  writeUpdateCheckDate();

  if (m_Nam != NULL)
  {
    m_Nam->deleteLater();
  }
  m_Nam = new QNetworkAccessManager(this);

  QObject::connect(m_Nam, SIGNAL(finished(QNetworkReply*)),
                   this, SLOT(networkReplied(QNetworkReply*)));

  QNetworkRequest request;
  request.setUrl(website);
  request.setRawHeader("User-Agent", "DREAM3D");

  QNetworkReply* reply = m_Nam->get(request);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheck::networkReplied(QNetworkReply* reply)
{

  UpdateCheckData* dataObj = new UpdateCheckData(this);
  // Reading attributes of the reply
  // e.g. the HTTP status code
  QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  // Or the target URL if it was a redirect:
  QVariant redirectionTargetUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
  // see CS001432 on how to handle this

  // no error received?
  if (reply->error() == QNetworkReply::NoError)
  {
    DREAM3DUpdateCheckDialog* d = new DREAM3DUpdateCheckDialog(NULL);
    d->setCurrentVersion((DREAM3DLib::Version::Complete()));
    d->setApplicationName("DREAM3D");
    QString appName = d->getAppName();

    // read data from QNetworkReply here

    // Example 2: Reading bytes form the reply
    QString message;


    QByteArray bytes = reply->readAll();  // bytes
    QString serverVersionStr(bytes); // string
    serverVersionStr = serverVersionStr.trimmed();

    QString appVersionStr = (DREAM3DLib::Version::Complete());

    QStringList serverVersionParts = serverVersionStr.split(QString("."));
    QStringList appVersionParts = appVersionStr.split(QString("."));

    appVersionStr = appVersionParts.at(0);
    appVersionStr.append(".").append(appVersionParts.at(1)).append(".").append(appVersionParts.at(2));
    serverVersionStr = serverVersionParts.at(0);
    serverVersionStr.append(".").append(serverVersionParts.at(1)).append(".").append(serverVersionParts.at(2));

    bool ok = false;
    AppVersion appVersion;
    appVersion.setMajorNum( appVersionParts.at(0).toInt(&ok) );
    appVersion.setMinorNum( appVersionParts.at(1).toInt(&ok) );
    appVersion.setPatchNum( appVersionParts.at(2).toInt(&ok) );

    AppVersion serverVersion;
    serverVersion.setMajorNum( serverVersionParts.at(0).toInt(&ok) );
    serverVersion.setMinorNum( serverVersionParts.at(1).toInt(&ok) );
    serverVersion.setPatchNum( serverVersionParts.at(2).toInt(&ok) );

    // If the server returned garbage values
    if ( serverVersion.getMajorNum() == 0 && serverVersion.getMinorNum() == 0 && serverVersion.getPatchNum() == 0 )
    {
      QString errorMessage = "Bad latest version values were read in from the DREAM3D database. ";
      errorMessage.append("Please contact the DREAM3D developers for more information.\n");
      dataObj->setMessageDescription(errorMessage);
      dataObj->setHasError(true);
      emit LatestVersion(dataObj);
      return;
    }

    // If the server returned a legitimate version, compare it with the app version
    if (serverVersion > appVersion)
    {
      dataObj->setHasUpdate(true);
      message.append("<qt><b>There is an update available for ").append(appName).append(".</b><br /><br />  You are currently running version ").append(appVersionStr).append(". If you are ready to update you can go to the main download <a href=\"http://dream3d.bluequartz.net/downloads\">website</a>.</qt>");
    }
    else
    {
      dataObj->setHasUpdate(false);
      message.append("<qt><b>").append(appName).append(" is up to date.</b><br /><br /></qt>");
    }
    dataObj->setMessageDescription(message);
    dataObj->setAppString(appVersionStr);
    dataObj->setServerString(serverVersionStr);

    emit LatestVersion(dataObj);
  }
  // The URL does not exist on the server
  else
  {
    QString errorMessage = "There was an error while reading information from the DREAM3D server. ";
    errorMessage.append("Please contact the DREAM3D developers for more information.\n\n");
    errorMessage.append("Error Message: ");
    errorMessage.append( reply->errorString() );
    dataObj->setMessageDescription(errorMessage);
    dataObj->setHasError(true);
    emit LatestVersion(dataObj);
    reply->abort();
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateCheck::writeUpdateCheckDate()
{
  QDate systemDate;
  QDate currentDateToday = systemDate.currentDate();

#if defined (Q_OS_MAC)
  QSettings updatePrefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings updatePrefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

  updatePrefs.beginGroup( DREAM3DUpdateCheckDialog::getUpdatePreferencesGroup() );
  updatePrefs.setValue (DREAM3DUpdateCheckDialog::getUpdateCheckKey(), currentDateToday.currentDate());
  updatePrefs.endGroup();
}
