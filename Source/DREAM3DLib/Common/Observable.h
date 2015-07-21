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


#ifndef _OBSERVABLE_H_
#define _OBSERVABLE_H_

#include <QtCore/QString>
#include <QtCore/QVector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/PipelineMessage.h"


/**
 * @class Observable Observable.h DREAM3D/Common/Observable.h
 * @brief
 *
 *
 * @date Sep 22, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT Observable : public QObject
{
    Q_OBJECT

  public:
    Observable();
    virtual ~Observable();

    DREAM3D_TYPE_MACRO(Observable)
    // ------------------------------
    // These are convenience methods that construct a @see PipelineMessage object and then 'emit' that object
    // ------------------------------
    virtual void notifyErrorMessage(const QString& humanLabel, const QString& ss, int code);

    virtual void notifyWarningMessage(const QString& humanLabel, const QString& ss, int code);

    virtual void notifyStatusMessage(const QString& humanLabel, const QString& ss);

    virtual void notifyStatusMessage(const QString& prefix, const QString& humanLabel, const QString& ss);

    virtual void notifyProgressMessage(const QString& prefix, const QString& humanLabel, const QString& str, int progress);

  public slots:

    /**
     * @brief This method will cause this object to 'emit' the filterGeneratedMessage() signal. This is useful if other
     * classes need the filter to emit an error or warning messge from a class that is not able to emit the proper signals
     * or the class is not connected to anything that would receive the signals
     * @param msg
     */
    void broadcastPipelineMessage(const PipelineMessage& msg);


  signals:

    /**
     * @brief filterGeneratedMessage This is a Qt Signal that is used when the filter generates Errors, Warnings, Status and Progress Messages
     * @param msg
     */
    void filterGeneratedMessage(const PipelineMessage& msg);

  private:

    Observable(const Observable&); // Copy Constructor Not Implemented
    void operator=(const Observable&); // Operator '=' Not Implemented
};

//EXPIMP_TEMPLATE template class DREAM3DLib_EXPORT QVector<Observer*>;


#endif /* OBSERVABLE_H_ */

