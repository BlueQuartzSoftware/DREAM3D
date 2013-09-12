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

#ifndef FILTERPIPELINE_H_
#define FILTERPIPELINE_H_


#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QTextStream>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

/**
 * @class FilterPipeline FilterPipeline.h DREAM3DLib/Common/FilterPipeline.h
 * @brief  This class holds the list of filters that will be run. This should be
 * the class (or a subclass) that you use to build up a pipeline in order to run
 * it.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 28, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT FilterPipeline : public Observer
{
  public:
    DREAM3D_SHARED_POINTERS(FilterPipeline)
    DREAM3D_TYPE_MACRO_SUPER(FilterPipeline, Observer)
    DREAM3D_STATIC_NEW_MACRO(FilterPipeline)

    virtual ~FilterPipeline();

    typedef QList<AbstractFilter::Pointer>  FilterContainerType;

    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition)
    DREAM3D_INSTANCE_PROPERTY(AbstractFilter::Pointer, CurrentFilter)

    /**
     * @brief Cancel the operation
     */
    virtual void setCancel(bool value);
    virtual bool getCancel();

    /**
     * @brief This method is called to start the pipeline for a plugin
     */
    virtual void run();

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
     * @brief This method is called from the run() method just before exiting and
     * signals the end of the pipeline execution
     */
    virtual void pipelineFinished();


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
    virtual AbstractFilter::Pointer removeFirstFilterByName(const QString &name);


    virtual void printFilterNames(QTextStream &out);



  protected:
    FilterPipeline();
    void updatePrevNextFilters();


  private:
    bool m_Cancel;
    FilterContainerType  m_Pipeline;

    FilterPipeline(const FilterPipeline&); // Copy Constructor Not Implemented
    void operator=(const FilterPipeline&); // Operator '=' Not Implemented
};

#endif /* FILTERPIPELINE_H_ */
