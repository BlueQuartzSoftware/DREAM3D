/* ============================================================================
 * Copyright (c) 2012, Michael A. Jackson (BlueQuartz Software)
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
#ifndef _PIPELINEMESSAGE_H_
#define _PIPELINEMESSAGE_H_

#include <QtCore/QString>
#include <QtCore/QMetaType>


#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"


/**
 * @class PipelineMessage PipelineMessage.h DREAM3DLib/Common/PipelineMessage.h
 * @brief This class enables the creation of Error, Warning, and Status messages that
 * can be sent up from filters to the DREAM3D GUI.
 * @author Joey Kleingers (SOCHE)
 * @date Jun 21, 2012
 * @version 1.0
 */
class PipelineMessage
{
  public:

    enum MessageType {Error,
                      Warning,
                      StatusMessage,
                      ProgressValue,
                      StatusMessageAndProgressValue,
                      UnknownMessageType
                     };

    PipelineMessage() {}

    PipelineMessage(const PipelineMessage& rhs)
    {
      m_FilterClassName = rhs.m_FilterClassName;
      m_FilterHumanLabel = rhs.m_FilterHumanLabel;
      m_MessagePrefix = rhs.m_MessagePrefix;
      m_Msg = rhs.m_Msg;
      m_Code = rhs.m_Code;
      m_msgType = rhs.m_msgType;
      m_Progress = rhs.m_Progress;
    }


    PipelineMessage(const QString& className,
                    const char* msg,
                    int code,
                    MessageType msgType = UnknownMessageType,
                    int progress = -1) :
      m_FilterClassName(className),
      m_Msg(msg),
      m_Code(code),
      m_msgType(msgType),
      m_Progress(progress)
    {}

    PipelineMessage(const QString& className,
                    const QString& msg,
                    int code,
                    MessageType msgType = UnknownMessageType,
                    int progress = -1) :
      m_FilterClassName(className),
      m_FilterHumanLabel(""),
      m_Msg(msg),
      m_Code(code),
      m_msgType(msgType),
      m_Progress(progress)
    {

    }

    static PipelineMessage CreateErrorMessage(const QString className, const QString msg, int code)
    {
      PipelineMessage em(className, msg, code, Error, -1);
      return em;
    }

    static PipelineMessage CreateStatusMessage(const QString className, const QString msg)
    {
      PipelineMessage em(className, msg, 0, StatusMessage, -1);
      return em;
    }

    static PipelineMessage CreateWarningMessage(const QString className, const QString msg, int code)
    {
      PipelineMessage em(className, msg, code, Warning, -1);
      return em;
    }


    DREAM3D_TYPE_MACRO(PipelineMessage)

    virtual ~PipelineMessage() {}

    bool operator==(const PipelineMessage& rhs)
    {
      return (m_FilterClassName == rhs.m_FilterClassName &&
              m_MessagePrefix == rhs.m_MessagePrefix &&
              m_FilterHumanLabel == rhs.m_FilterHumanLabel &&
              m_Msg == rhs.m_Msg &&
              m_Code == rhs.m_Code &&
              m_msgType == rhs.m_msgType &&
              m_Progress == rhs.m_Progress);
    }

    void operator=(const PipelineMessage& rhs)
    {
      m_FilterClassName = rhs.m_FilterClassName;
      m_MessagePrefix = rhs.m_MessagePrefix;
      m_FilterHumanLabel = rhs.m_FilterHumanLabel;
      m_Msg = rhs.m_Msg;
      m_Code = rhs.m_Code;
      m_msgType = rhs.m_msgType;
      m_Progress = rhs.m_Progress;
    }



    DREAM3D_INSTANCE_STRING_PROPERTY(FilterClassName)

    private:
      QString m_FilterHumanLabel;

  public:
    void setFilterHumanLabel(const QString& s)
    {
      m_FilterHumanLabel = s;
    }
    QString getFilterHumanLabel()
    {
      return m_FilterHumanLabel;
    }

    DREAM3D_INSTANCE_STRING_PROPERTY(MessagePrefix)


    /**
     * @brief This function is the member m_Msg's accessor.
     */
    QString getMessageText() { return m_Msg; }

    /**
     * @brief This function is the member m_Msg's mutator.
     * @param val Variable whose value is assigned to m_Msg.
     */
    void setMessageText(const QString& val) { m_Msg = val; }

    /**
     * @brief This function is the member m_Code's accessor.
     */
    int getMessageCode() { return m_Code; }

    /**
     * @brief This function is the member m_Code's mutator.
     * @param val Variable whose value is assigned to m_Code.
     */
    void setMessageCode(int val) { m_Code = val; }

    /**
     * @brief This function is the member m_msgType's accessor.
     */
    PipelineMessage::MessageType getMessageType() { return m_msgType; }

    /**
     * @brief This function is the member m_msgType's mutator.
     * @param val Variable whose value is assigned to m_msgType.
     */
    void setMessageType(PipelineMessage::MessageType val) { m_msgType = val; }

    /**
     * @brief This function is the member m_status's accessor.
     */
    int getProgressValue() const { return m_Progress; }

    /**
     * @brief This function is the member m_status's mutator.
     * @param val Variable whose value is assigned to m_Progress.
     */
    void setProgressValue(int val) { m_Progress = val; }

    /**
     * @brief This function creates and returns a string for error messages
     */
    QString generateErrorString()
    {
      QString ss = QObject::tr("Error (%1): %2: %3").arg(m_Code).arg(m_MessagePrefix).arg(m_Msg);
      return ss;
    }

    /**
     * @brief This function creates and returns a string for warning messages
     */
    QString generateWarningString()
    {
      QString ss = QObject::tr("Warning (%1): %2: %3").arg(m_Code).arg(m_MessagePrefix).arg(m_Msg);
      return ss;
    }

    /**
     * @brief This function creates and returns a string for status messages
     */
    QString generateStatusString()
    {
      QString ss = QObject::tr("%1: %2").arg(m_MessagePrefix).arg(m_Msg);
      return ss;
    }

  private:
    QString m_Msg;          // Message Text
    int m_Code;                 // Error/Warning Code
    MessageType m_msgType;      // Type of Message (see enumeration "MessageType")
    int m_Progress;             // Progress integer

};


Q_DECLARE_METATYPE(PipelineMessage)

#endif /* _PipelineMessage_H */
