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

#ifndef _IOSupport_H_
#define _IOSupport_H_

#include <hdf5.h>

#include <QtCore/QString>
#include <QtCore/QVector>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Common/PipelineMessage.h"

/**
 * @brief The IOSupport class
 */
class  IOSupport : public Observable
{

  public:
    SIMPL_SHARED_POINTERS(IOSupport)
    SIMPL_STATIC_NEW_MACRO(IOSupport)
    SIMPL_TYPE_MACRO(IOSupport)

    virtual ~IOSupport();


    SIMPL_INSTANCE_PROPERTY(int, ErrorCondition)
    SIMPL_VIRTUAL_INSTANCE_PROPERTY(bool, Cancel)
    SIMPL_INSTANCE_PROPERTY(DataContainer*, DataContainer)
    SIMPL_INSTANCE_PROPERTY(unsigned int, DataContainerType)
    SIMPL_INSTANCE_PROPERTY(hid_t, HdfFileId)
    SIMPL_INSTANCE_PROPERTY(hid_t, HdfGroupId)


    SIMPL_INSTANCE_PROPERTY(QVector<PipelineMessage>, PipelineMessages)

    void addErrorMessage(PipelineMessage& msg);
    void addErrorMessage(const QString& filterName, const QString& errorDescription, int errorCode);
    void addErrorMessages(QVector<PipelineMessage> msgVector);

    void addWarningMessage(PipelineMessage& msg);
    void addWarningMessage(const QString& filterName, const QString& warnDescription, int warnCode);
    void addWarningMessages(QVector<PipelineMessage> msgVector);

    void removeErrorMessage(PipelineMessage msg);
    void removeErrorMessage(int index);
    void removeErrorMessages(int start, int end);

    void clearErrorMessages();

    QString getHumanLabel();


  protected:
    IOSupport();

  private:
    IOSupport(const IOSupport&); // Copy Constructor Not Implemented
    void operator=(const IOSupport&); // Operator '=' Not Implemented


};


#endif /* _IOSupport_H_ */

