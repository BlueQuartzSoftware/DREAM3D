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


#ifndef _FILTERPIPELINE_H_
#define _FILTERPIPELINE_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QTextStream>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Common/AbstractFilter.h"

/**
 * @class FilterPipeline FilterPipeline.h DREAM3DLib/Common/FilterPipeline.h
 * @brief  This class holds the list of filters that will be run. This should be
 * the class (or a subclass) that you use to build up a pipeline in order to run
 * it.
 *
 * @date Sep 28, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT FilterPipeline : public QObject
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(FilterPipeline)
    SIMPL_TYPE_MACRO(FilterPipeline)
    SIMPL_STATIC_NEW_MACRO(FilterPipeline)

    virtual ~FilterPipeline();

    typedef QList<AbstractFilter::Pointer>  FilterContainerType;

    SIMPL_INSTANCE_PROPERTY(int, ErrorCondition)
    SIMPL_INSTANCE_PROPERTY(AbstractFilter::Pointer, CurrentFilter)

    /**
     * @brief Cancel the operation
     */
    virtual void setCancel(bool value);
    virtual bool getCancel();


    /**
     * @brief A pure virtual function that gets called from the "run()" method. Subclasses
     * are expected to create a concrete implementation of this method.
     */
    virtual void execute();

    /**
     * @brief This will preflight the pipeline and report any errors that would occur during
     * execution of the pipeline
     */
    virtual int preflightPipeline();


    /**
     * @brief
     */
    virtual void pushFront(AbstractFilter::Pointer f);
    virtual void popFront();
    virtual void pushBack(AbstractFilter::Pointer f);
    virtual void popBack();
    virtual void insert(size_t index, AbstractFilter::Pointer f);
    virtual void erase(size_t index);
    virtual void clear();
    virtual size_t size();
    virtual bool empty();

    virtual FilterContainerType& getFilterContainer();

    /**
     * @brief
     */
    virtual AbstractFilter::Pointer removeFirstFilterByName(const QString& name);


    virtual void printFilterNames(QTextStream& out);

    /**
     * @brief This method adds a QObject based class that is capable of being connected with the following signals from
     * AbstractFilter:
     * @li processPipelineMessage(PipelineMessage&)
     * @param obj Class that implements needed processPipelineMessage(PipelineMessage&) method
     */
    void addMessageReceiver(QObject* obj);

    void connectFilterNotifications(QObject* filter);
    void disconnectFilterNotifications(QObject* filter);

  public slots:

    /**
     * @brief This method is called to start the pipeline for a plugin
     */
    virtual void run();

    /**
     * @brief cancelPipeline
     */
    virtual void cancelPipeline();

  protected:
    FilterPipeline();


    void updatePrevNextFilters();

  signals:
    void pipelineGeneratedMessage(const PipelineMessage& message);

    /**
    * @brief This method is called from the run() method just before exiting and
    * signals the end of the pipeline execution
    */
    void pipelineFinished();

  private:
    bool m_Cancel;
    FilterContainerType  m_Pipeline;

    QVector<QObject*> m_MessageReceivers;


    FilterPipeline(const FilterPipeline&); // Copy Constructor Not Implemented
    void operator=(const FilterPipeline&); // Operator '=' Not Implemented
};

#endif /* FILTERPIPELINE_H_ */

