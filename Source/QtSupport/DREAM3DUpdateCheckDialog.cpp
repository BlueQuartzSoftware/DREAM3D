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

#include <QtGui/QDesktopServices>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>


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
  currentVersion->setText(m_CurrentVersion);
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

        int serverMajor = serverVersionParts.at(0).toInt(&ok);
        int appMajor = appVersionParts.at(0).toInt(&ok);

        int serverMinor = serverVersionParts.at(1).toInt(&ok);
        int appMinor = appVersionParts.at(1).toInt(&ok);

        int serverPatch = serverVersionParts.at(2).toInt(&ok);
        int appPatch = appVersionParts.at(2).toInt(&ok);

        if (serverMajor > appMajor )
        {
          message.append("<qt>There is a DREAM3D Update available. Please go to the <a href=\"http://dream3d.bluequartz.net/downloads\">website</a> to download the latest version</qt>");
        }
        else if (serverMinor > appMinor)
        {
          message.append("<qt>There is a DREAM3D Update available. Please go to the <a href=\"http://dream3d.bluequartz.net/downloads\">website</a> to download the latest version</qt>");
        }
        else if (serverPatch > appPatch)
        {
          message.append("<qt>There is a DREAM3D Update available. Please go to the <a href=\"http://dream3d.bluequartz.net/downloads\">website</a> to download the latest version</qt>");
        }
        else
        {
          message.append("DREAM3D is up to date.");
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
