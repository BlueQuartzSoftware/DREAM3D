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
#ifndef _AbstractFilter_H_
#define _AbstractFilter_H_

#include <QtCore/QSet>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/DataContainers/DataContainerArray.h"
#include "DREAM3DLib/FilterParameters/FilterParameter.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/Common/PipelineMessage.h"
#include "DREAM3DLib/Common/CreatedArrayHelpIndexEntry.h"


/**
 * @class AbstractFilter AbstractFilter.h DREAM3DLib/Common/AbstractFilter.h
 * @brief This class is the basic class to subclass when creating a new Filter for
 * DREAM.3D. The subclass must implement at least the  execute method. If an
 * error occurs during the execution of the filter set the errorCondition to
 * a non zero value and optionally use the setErrorMessage() method to explain what the
 * error was. This class also inherits from Observable so that the filter can
 * provide updates to the user interface during execution.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Nov 28, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT AbstractFilter : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(AbstractFilter)
    DREAM3D_STATIC_NEW_MACRO(AbstractFilter)
    DREAM3D_TYPE_MACRO_SUPER(AbstractFilter, Observable)

    virtual ~AbstractFilter();

    // These should be implemented by the subclass
    virtual const QString getGroupName() { return "YOUR CLASS SHOULD IMPLEMENT THIS";}
    virtual const QString getSubGroupName() { return "YOUR CLASS SHOULD IMPLEMENT THIS";}
    virtual const QString getHumanLabel() { return "YOUR CLASS SHOULD IMPLEMENT THIS";}

    virtual const QString getFilterVersion() { return DREAM3DLib::Version::Complete(); }

    DREAM3D_INSTANCE_PROPERTY(DataContainerArray::Pointer, DataContainerArray)

    DREAM3D_INSTANCE_PROPERTY(QVector<PipelineMessage>, PipelineMessages)

    void addErrorMessage(PipelineMessage &msg);
    void addErrorMessage(const QString &filterName, const QString &errorDescription, int errorCode);
    void addErrorMessages(QVector<PipelineMessage> msgVector);

    void addWarningMessage(PipelineMessage &msg);
    void addWarningMessage(const QString &filterName, const QString &warnDescription, int warnCode);
    void addWarningMessages(QVector<PipelineMessage> msgVector);

    void removeErrorMessage(PipelineMessage msg);
    void removeErrorMessage(int index);
    void removeErrorMessages(int start, int end);

    void clearErrorMessages();

    /**
     * @brief tbbTaskProgress If your filter spawns threads for parallelization you can use this callback to try and
     * keep progress for your parallel threads.
     */
    virtual void tbbTaskProgress();

    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition)


    DREAM3D_INSTANCE_PROPERTY(int, PipelineIndex)
    DREAM3D_INSTANCE_PROPERTY(AbstractFilter::Pointer, PreviousFilter)
    DREAM3D_INSTANCE_PROPERTY(AbstractFilter::Pointer, NextFilter)

   /**
     * @brief Cancel the operation
     */
    DREAM3D_VIRTUAL_INSTANCE_PROPERTY(bool, Cancel)


    DREAM3D_INSTANCE_PROPERTY(QVector<FilterParameter::Pointer>, FilterParameters)

    DREAM3D_INSTANCE_PROPERTY(QVector<CreatedArrayHelpIndexEntry::Pointer>, CreatedArrayHelpIndexEntries)
    void addCreatedArrayHelpIndexEntry(CreatedArrayHelpIndexEntry::Pointer entry);

    virtual void printValues(std::ostream &out){}


    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    * @param index The index the filter is in the pipeline that is being written
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);


    /**
     * @brief This method should be fully implemented in subclasses.
     */
    virtual void execute();
    virtual void preflight();

    virtual bool doesPipelineContainFilterBeforeThis(const QString &name);
    virtual bool doesPipelineContainFilterAfterThis(const QString &name);




  protected:
    AbstractFilter();



  private:


    AbstractFilter(const AbstractFilter&); // Copy Constructor Not Implemented
    void operator=(const AbstractFilter&); // Operator '=' Not Implemented
};




#endif /* _AbstractFilter_H_  */
