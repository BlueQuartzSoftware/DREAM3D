/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "UpdateCheck.h"

#include <QtCore/QSysInfo>
#include <QtCore/QDate>
#include <QtCore/QSettings>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>


#include <QtWidgets/QMessageBox>
#include <QtWidgets/QWidget>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>


#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/AppVersion.h"
#include "SIMPLib/Common/Constants.h"

#include "DREAM3DWidgetsLib/Widgets/DREAM3DUpdateCheckDialog.h"


#include "UpdateCheckData.h"

#include "moc_UpdateCheck.cpp"

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


  QString header;
  QTextStream out(&header);
  out << "DREAM3D Version " << SIMPLib::Version::Complete() << " [" << DREAM3DProj_RELEASE_TYPE << "] ";
  out << "(" << QSysInfo::prettyProductName() << ")";
  request.setRawHeader("User-Agent", header.toLatin1());

  m_Nam->get(request);
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
    d->setCurrentVersion((SIMPLib::Version::Complete()));
    d->setApplicationName("DREAM3D");
    QString appName = d->getAppName();

    // read data from QNetworkReply here

    // Example 2: Reading bytes form the reply
    QString message;
    QTextStream outMsg(&message);


    QByteArray byteArray = reply->readAll();  // bytes
//   QString serverVersionStr(bytes); // string

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);

    QJsonObject root = doc.object();

    QJsonObject d3dJson = root["DREAM3D"].toObject();
    QString releaseDate = d3dJson["Release Date"].toString();
    QString releaseType = d3dJson["Release Type"].toString();

    QString serverMajor = d3dJson["Major"].toString();
    QString serverMinor = d3dJson["Minor"].toString();
    QString serverPatch = d3dJson["Patch"].toString();

    QString appMajor = SIMPLib::Version::Major();
    QString appMinor = SIMPLib::Version::Minor();
    QString appPatch = SIMPLib::Version::Patch();

    bool ok = false;
    AppVersion appVersion;
    appVersion.setMajorNum( appMajor.toInt(&ok) );
    appVersion.setMinorNum( appMinor.toInt(&ok) );
    appVersion.setPatchNum( appPatch.toInt(&ok) );

    AppVersion serverVersion;
    serverVersion.setMajorNum( serverMajor.toInt(&ok) );
    serverVersion.setMinorNum( serverMinor.toInt(&ok) );
    serverVersion.setPatchNum( serverPatch.toInt(&ok) );

    // If the server returned garbage values
    if ( serverVersion.getMajorNum() == 0 && serverVersion.getMinorNum() == 0 && serverVersion.getPatchNum() == 0 )
    {
      QString errorMessage = "Bad latest version values were read in from the DREAM3D database. ";
      errorMessage.append("Please contact the DREAM3D developers for more information.\n");
      dataObj->setMessageDescription(errorMessage);
      dataObj->setHasError(true);
      emit latestVersion(dataObj);
      return;
    }

    QString appVersionStr = appMajor + "." + appMinor + "." + appPatch;
    QString serverVersionStr = serverMajor + "." + serverMinor + "." + serverPatch;

    // If the server returned a legitimate version, compare it with the app version
    if (serverVersion > appVersion)
    {
      dataObj->setHasUpdate(true);
      outMsg << "<qt><b>There is an update available for " << appName;
      outMsg  << ". The update is a " << releaseType << " and was released on " << releaseDate;
      outMsg << ".</b><br /><br />  You are currently running version " << appVersionStr;
      outMsg << ". If you are ready to update you can go to the main download <a href=\"http://dream3d.bluequartz.net\">website</a>.</qt>";
    }
    else
    {
      dataObj->setHasUpdate(false);
      message.append("<qt><b>").append(appName).append(" is up to date.</b><br /><br /></qt>");
    }
    dataObj->setMessageDescription(message);
    dataObj->setAppString(appVersionStr);
    dataObj->setServerString(serverVersionStr);

    emit latestVersion(dataObj);
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
    emit latestVersion(dataObj);
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

  DREAM3DSettings updatePrefs;

  updatePrefs.beginGroup( DREAM3DUpdateCheckDialog::getUpdatePreferencesGroup() );
  updatePrefs.setValue (DREAM3DUpdateCheckDialog::getUpdateCheckKey(), currentDateToday.currentDate());
  updatePrefs.endGroup();
}
