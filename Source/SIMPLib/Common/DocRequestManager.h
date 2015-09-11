/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _DocRequestManger_H_
#define _DocRequestManger_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>

#include "SIMPLib/SIMPLib.h"


/**
 * @brief The DocRequestManager class is the moderator between low level libraries
 * that request the documentation for a filter be shown to the user in some capacity.
 * The expected design is for the Higher level Application to use Qt's signals
 * and slots to "connect" one of their own slots to the signals emitted from
 * this class. Lower level classes should get the instance of this class (which
 * is a singleton) and call one of the slots that will in turn emit a signal.
 */
class SIMPLib_EXPORT DocRequestManager : public QObject
{

    Q_OBJECT

  public:
    DocRequestManager();
    virtual ~DocRequestManager();

    static DocRequestManager* Instance();

  public Q_SLOTS:
    void requestFilterDocs(const QString &filter);

    void requestFilterDocUrl(const QUrl &filter);

  Q_SIGNALS:

    void showFilterDocs(const QString &filter);

    void showFilterDocUrl(const QUrl &url);

  private:

     static DocRequestManager *self;

     DocRequestManager(const DocRequestManager&); // Copy Constructor Not Implemented
     void operator=(const DocRequestManager&); // Operator '=' Not Implemented
};

#endif /* _DocRequestManger_H_ */
