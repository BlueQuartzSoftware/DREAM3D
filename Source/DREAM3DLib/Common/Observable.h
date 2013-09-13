/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef OBSERVABLE_H_
#define OBSERVABLE_H_

#include <QtCore/QString>
#include <QtCore/QVector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/PipelineMessage.h"


class Observer;


/**
 * @class Observable Observable.h DREAM3D/Common/Observable.h
 * @brief This class implements the "Subject" class of the "Observer" design pattern
 * from the "Gang of Four" book. The basic operations of adding and removing an
 * <b>Observer</b> oject from this class. Multiple Observers can be stored in this
 * class if needed. The order in which they are added is the order in which they
 * are notified. There are several types of notification aspects that can be used
 * based on the enumeration that is used. From just updating a message to sending
 * a message that should be considered a warning or error.
 *
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 22, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT Observable
{
  public:
    Observable();
    virtual ~Observable();

    DREAM3D_TYPE_MACRO(Observable)

    /**
     * @brief Adds an observer to notify when changes occur
     * @param observer The observer to notify
     */
    virtual void addObserver(Observer* observer);

    /**
     * @brief Removes a specific observer from the list of observers
     * @param observer The Observer to remove
     */
    virtual void removeObserver(Observer* observer);

    virtual void notifyMessage(PipelineMessage &msg);

    void notifyErrorMessage(QString errDesc, int errCode);

    void notifyWarningMessage(QString warnDesc, int warnCode);

    void notifyStatusMessage(QString statusDesc);

    void notifyProgressValue(int status);

    void notifyStatusAndProgress(QString statusDesc, int statusVal);

    virtual QVector<Observer*> getObservers();

    virtual void setObservers(QVector<Observer*> obs);

    virtual void setMessagePrefix(const QString &str);
    virtual QString getMessagePrefix();

  private:
    QVector<Observer*> m_Observers;
    QString m_Prefix;

    Observable(const Observable&); // Copy Constructor Not Implemented
    void operator=(const Observable&); // Operator '=' Not Implemented
};

//EXPIMP_TEMPLATE template class DREAM3DLib_EXPORT QVector<Observer*>;


#endif /* OBSERVABLE_H_ */
