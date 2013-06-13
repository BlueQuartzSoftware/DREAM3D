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

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UpdateCheck::UpdateCheck(QWidget* parent) :
QWidget(parent)
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

		QStringList serverVersionParts = serverVersion.split(QString("."));

		bool ok = false;
		int serverMajor = serverVersionParts.at(0).toInt(&ok);
		int serverMinor = serverVersionParts.at(1).toInt(&ok);
		int serverPatch = serverVersionParts.at(2).toInt(&ok);

		emit LatestVersion(serverMajor, serverMinor, serverPatch);
	}
}