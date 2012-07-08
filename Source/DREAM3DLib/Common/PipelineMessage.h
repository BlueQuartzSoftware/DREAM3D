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

#include <string>



#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#if defined(QT_CORE_LIB)
#include <QtCore/QMetaType>
#endif

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
                        StatusValue,
                        StatusMessageAndValue,
                        UnknownMessageType};

    PipelineMessage() {}

    PipelineMessage(const PipelineMessage& rhs)
    {
      m_filterName = rhs.m_filterName;
      m_Msg = rhs.m_Msg;
      m_Code = rhs.m_Code;
      m_msgType = rhs.m_msgType;
      m_Progress = rhs.m_Progress;
    }


    PipelineMessage(const std::string &filterName, const char* msg, int code,
                   MessageType msgType = UnknownMessageType, int status = -1) :
        m_filterName(filterName),
        m_Msg(msg),
        m_Code(code),
        m_msgType(msgType),
        m_Progress(status)
    {}

    PipelineMessage(const std::string &filterName, const std::string &msg, int code,
      MessageType msgType = UnknownMessageType, int status = -1) :
        m_filterName(filterName),
        m_Msg(msg),
        m_Code(code),
        m_msgType(msgType),
        m_Progress(status)
    {}

    DREAM3D_TYPE_MACRO(PipelineMessage)

    virtual ~PipelineMessage() {}

    bool operator==(const PipelineMessage& rhs)
    {
      return (m_filterName == rhs.m_filterName &&
          m_Msg == rhs.m_Msg &&
            m_Code == rhs.m_Code &&
              m_msgType == rhs.m_msgType &&
                m_Progress == rhs.m_Progress);
    }

    void operator=(const PipelineMessage& rhs)
    {
      m_filterName = rhs.m_filterName;
      m_Msg = rhs.m_Msg;
      m_Code = rhs.m_Code;
      m_msgType = rhs.m_msgType;
      m_Progress = rhs.m_Progress;
    }

    /**
     * @brief This function is the member m_filterName's accessor.
     */
    std::string getFilterName() { return m_filterName; }

    /**
     * @brief This function is the member m_filterName's mutator.
     * @param val Variable whose value is assigned to m_filterName.
     */
    void setFilterName(const std::string &val) { m_filterName = val; }

    /**
     * @brief This function is the member m_Msg's accessor.
     */
    std::string getMessageText() { return m_Msg; }

    /**
     * @brief This function is the member m_Msg's mutator.
     * @param val Variable whose value is assigned to m_Msg.
     */
    void setMessageText(const std::string &val) { m_Msg = val; }

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
    std::string generateErrorString()
    {
      std::stringstream ss;
      ss << "Error(" << m_Code << "):" << m_filterName << " :" << m_Msg;
      return ss.str();
    }

    /**
     * @brief This function creates and returns a string for warning messages
     */
    std::string generateWarningString()
    {
      std::stringstream ss;
      ss << "Warning(" << m_Code << "):" << m_filterName << " :" << m_Msg;
      return ss.str();
    }

    /**
     * @brief This function creates and returns a string for status messages
     */
    std::string generateStatusString()
     {
       std::stringstream ss;
       ss << m_filterName << ":" << m_Msg;
       return ss.str();
     }

  private:
    std::string m_filterName;   // Name of the Filter
    std::string m_Msg;          // Message Text
    int m_Code;                 // Error/Warning Code
    MessageType m_msgType;      // Type of Message (see enumeration "MessageType")
    int m_Progress;             // Progress integer

};

#if defined(QT_CORE_LIB)
  Q_DECLARE_METATYPE(PipelineMessage);
#endif

#endif /* _PipelineMessage_H */
