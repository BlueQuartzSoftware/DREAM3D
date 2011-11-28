/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef ABSTRACTPIPELINE_H_
#define ABSTRACTPIPELINE_H_


#include "MXA/Common/LogTime.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

/**
 * @class AbstractPipeline AbstractPipeline.h DREAM3D/Common/AbstractPipeline.h
 * @brief All DREAM.3D classes that perform a set of actions that result in some
 * outputs should inherit from this simple class. It defines some basic methods
 * and a virtual method that needs to be implemented by the plugin developer. The
 * plugin developer should implement the "execute()" method with their pipeline.
 * This implementation simply prints any update messages to the console. Subclasses
 * can over ride this behavior to send signals to the Qt Gui (or another class)
 * with the update methods.
 *
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jun 1, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT AbstractPipeline
{
  public:
    AbstractPipeline();
    virtual ~AbstractPipeline();

    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition);

    /**
     * @brief Cancel the operation
     */
    DREAM3D_INSTANCE_PROPERTY(bool, Cancel);

    /**
     * @brief This method is called to start the pipeline for a plugin
     */
    virtual void run();

    /**
     * @brief A pure virtual function that gets called from the "run()" method. Subclasses
     * are expected to create a concrete implementation of this method.
     */
    virtual void execute() = 0;

    /**
     * @brief This method is called from the run() method just before exiting and
     * signals the end of the pipeline execution
     */
    virtual void pipelineFinished();

  private:
    AbstractPipeline(const AbstractPipeline&); // Copy Constructor Not Implemented
    void operator=(const AbstractPipeline&); // Operator '=' Not Implemented
};

#define CREATE_INPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);

#define CREATE_OUTPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);

#define AIM_RECONSTRUCTION_BENCHMARKS 0

#if AIM_RECONSTRUCTION_BENCHMARKS
#define START_CLOCK()\
  unsigned long long int millis;\
  millis = MXA::getMilliSeconds();
#else
#define START_CLOCK() unsigned long long int millis = 0;\
  millis = 0;
#endif


#define CHECK_FOR_CANCELED(FuncClass, Message, name)\
    if (this->getCancel() ) { \
              pipelineProgressMessage(#Message);\
              pipelineProgress(0);\
              pipelineFinished();\
              m = FuncClass::NullPointer();\
      return;}\
      if(AIM_RECONSTRUCTION_BENCHMARKS) {\
    std::cout << #name << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;\
    millis = MXA::getMilliSeconds(); }

#define CHECK_FOR_ERROR(FuncClass, Message, err)\
    if(err < 0) {\
      setErrorCondition(err);\
      std::string msg = std::string(Message);\
      pipelineErrorMessage(msg.c_str());\
      pipelineProgress(0);\
      pipelineFinished();\
      m = FuncClass::NullPointer();\
      return;   }


#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    std::string outpath = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + filename;



#endif /* ABSTRACTPIPELINE_H_ */
