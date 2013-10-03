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
#include "Observer.h"

#include <QtCore/QtDebug>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Observer::Observer()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Observer::~Observer()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observer::updatePipelineProgressAndMessage(const char* msg, int progress)
{
  qDebug() << progress << "% " << msg ;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observer::updatePipelineProgressAndMessage(const QString& msg, int progress)
{
  qDebug() << progress << "% " << msg ;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observer::updatePipelineProgress(int progress)
{
  qDebug() << progress << "%" ;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observer::updatePipelineMessage(const QString& msg)
{
  qDebug() << msg ;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observer::updatePipelineMessage(const char* msg)
{
  qDebug() << msg ;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observer::sendPipelineMessage(PipelineMessage& msg)
{
  QString ss;
  if(msg.getMessageType() == PipelineMessage::Error)
  {
    ss.append(msg.generateErrorString());
  }
  else if(msg.getMessageType() == PipelineMessage::Warning)
  {
    ss.append(msg.generateWarningString());
  }
  else if(msg.getMessageType() == PipelineMessage::StatusMessage)
  {
    ss.append(msg.generateStatusString());
  }
  else if(msg.getMessageType() == PipelineMessage::StatusValue)
  {
    ss.append(msg.getProgressValue()).append("%");
  }
  else if(msg.getMessageType() == PipelineMessage::StatusMessageAndValue)
  {
    ss = QObject::tr("%1 %  %2").arg(msg.getProgressValue()).arg(msg.generateStatusString());
  }
  qDebug() << ss;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observer::sendPipelineMessages(QVector<PipelineMessage> msgs)
{
  for (QVector<PipelineMessage>::iterator iter = msgs.begin(); iter != msgs.end(); ++iter )
  {
    sendPipelineMessage(*iter);
  }
}


