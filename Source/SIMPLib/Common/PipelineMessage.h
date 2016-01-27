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

#ifndef _PIPELINEMESSAGE_H_
#define _PIPELINEMESSAGE_H_

#include <QtCore/QString>
#include <QtCore/QMetaType>


#include "SIMPLib/Common/SIMPLibSetGetMacros.h"


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
      m_Prefix = rhs.m_Prefix;
      m_Text = rhs.m_Text;
      m_Code = rhs.m_Code;
      m_Type = rhs.m_Type;
      m_ProgressValue = rhs.m_ProgressValue;
    }


    PipelineMessage(const QString& className,
                    const char* msg,
                    int code,
                    MessageType msgType = UnknownMessageType,
                    int progress = -1) :
      m_FilterClassName(className),
      m_Text(msg),
      m_Code(code),
      m_Type(msgType),
      m_ProgressValue(progress)
    {}

    PipelineMessage(const QString& className,
                    const QString& msg,
                    int code,
                    MessageType msgType = UnknownMessageType,
                    int progress = -1) :
      m_FilterClassName(className),
      m_FilterHumanLabel(""),
      m_Text(msg),
      m_Code(code),
      m_Type(msgType),
      m_ProgressValue(progress)
    {

    }

    PipelineMessage(const QString& className,
                    const QString& humanLabel,
                    const QString& msg,
                    int code,
                    MessageType msgType = UnknownMessageType,
                    int progress = -1) :
      m_FilterClassName(className),
      m_FilterHumanLabel(humanLabel),
      m_Text(msg),
      m_Code(code),
      m_Type(msgType),
      m_ProgressValue(progress)
    {

    }


    static PipelineMessage CreateErrorMessage(const QString className, const QString humanLabel, const QString msg, int code)
    {
      PipelineMessage em(className, humanLabel, msg, code, Error, -1);
      return em;
    }

    static PipelineMessage CreateStatusMessage(const QString className, const QString humanLabel, const QString msg)
    {
      PipelineMessage em(className, humanLabel, msg, 0, StatusMessage, -1);
      return em;
    }

    static PipelineMessage CreateWarningMessage(const QString className, const QString humanLabel, const QString msg, int code)
    {
      PipelineMessage em(className, humanLabel,  msg, code, Warning, -1);
      return em;
    }


    SIMPL_TYPE_MACRO(PipelineMessage)

    virtual ~PipelineMessage() {}

    bool operator==(const PipelineMessage& rhs)
    {
      return (m_FilterClassName == rhs.m_FilterClassName &&
              m_Prefix == rhs.m_Prefix &&
              m_FilterHumanLabel == rhs.m_FilterHumanLabel &&
              m_Text == rhs.m_Text &&
              m_Code == rhs.m_Code &&
              m_Type == rhs.m_Type &&
              m_ProgressValue == rhs.m_ProgressValue);
    }

    void operator=(const PipelineMessage& rhs)
    {
      m_FilterClassName = rhs.m_FilterClassName;
      m_Prefix = rhs.m_Prefix;
      m_FilterHumanLabel = rhs.m_FilterHumanLabel;
      m_Text = rhs.m_Text;
      m_Code = rhs.m_Code;
      m_Type = rhs.m_Type;
      m_ProgressValue = rhs.m_ProgressValue;
    }

    SIMPL_INSTANCE_STRING_PROPERTY(FilterClassName)

    SIMPL_INSTANCE_STRING_PROPERTY(FilterHumanLabel)

    SIMPL_INSTANCE_STRING_PROPERTY(Prefix)

    SIMPL_INSTANCE_STRING_PROPERTY(Text)

    SIMPL_INSTANCE_PROPERTY(int, Code)

    SIMPL_INSTANCE_PROPERTY(MessageType, Type)

    SIMPL_INSTANCE_PROPERTY(int, ProgressValue)

//    void setFilterHumanLabel(const QString& s)
//    {
//      m_FilterHumanLabel = s;
//    }
//    QString getFilterHumanLabel() const
//    {
//      return m_FilterHumanLabel;
//    }


//    /**
//     * @brief This function is the member m_Text's accessor.
//     */
//    QString getMessageText() const { return m_Text; }

//    /**
//     * @brief This function is the member m_Text's mutator.
//     * @param val Variable whose value is assigned to m_Text.
//     */
//    void setMessageText(const QString& val) { m_Text = val; }

//    /**
//     * @brief This function is the member m_Code's accessor.
//     */
//    int getMessageCode() { return m_Code; }

//    /**
//     * @brief This function is the member m_Code's mutator.
//     * @param val Variable whose value is assigned to m_Code.
//     */
//    void setMessageCode(int val) { m_Code = val; }

//    /**
//     * @brief This function is the member m_Type's accessor.
//     */
//    PipelineMessage::MessageType getMessageType() { return m_Type; }

//    /**
//     * @brief This function is the member m_Type's mutator.
//     * @param val Variable whose value is assigned to m_Type.
//     */
//    void setMessageType(PipelineMessage::MessageType val) { m_Type = val; }

//    /**
//     * @brief This function is the member m_status's accessor.
//     */
//    int getProgressValue() const { return m_ProgressValue; }

//    /**
//     * @brief This function is the member m_status's mutator.
//     * @param val Variable whose value is assigned to m_ProgressValue.
//     */
//    void setProgressValue(int val) { m_ProgressValue = val; }

    /**
     * @brief This function creates and returns a string for error messages
     */
    QString generateErrorString()
    {
      QString ss = QObject::tr("Error (%1): %2: %3").arg(m_Code).arg(m_Prefix).arg(m_Text);
      return ss;
    }

    /**
     * @brief This function creates and returns a string for warning messages
     */
    QString generateWarningString()
    {
      QString ss = QObject::tr("Warning (%1): %2: %3").arg(m_Code).arg(m_Prefix).arg(m_Text);
      return ss;
    }

    /**
     * @brief This function creates and returns a string for status messages
     */
    QString generateStatusString()
    {
      QString ss = QObject::tr("%1: %2").arg(m_Prefix).arg(m_Text);
      return ss;
    }


  private:
//   QString m_FilterHumanLabel;
//   QString m_Text;          // Message Text
//   int m_Code;                 // Error/Warning Code
//    MessageType m_Type;      // Type of Message (see enumeration "MessageType")
//   int m_ProgressValue;             // Progress integer

};


Q_DECLARE_METATYPE(PipelineMessage)

#endif /* _PipelineMessage_H */

