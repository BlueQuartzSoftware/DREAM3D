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

#ifndef ABSTRACTPIPELINE_H_
#define ABSTRACTPIPELINE_H_

#include "MXA/Common/MXASetGetMacros.h"
#include "DREAM3D/DREAM3DConfiguration.h"

/**
 * @class AbstractPipeline AbstractPipeline.h DREAM3D/Common/AbstractPipeline.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jun 1, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT AbstractPipeline
{
  public:
    AbstractPipeline();
    virtual ~AbstractPipeline();

    MXA_INSTANCE_PROPERTY(int, ErrorCondition);

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY(bool, Cancel);

    /**
     *
     */
    virtual void run();

    virtual void execute() = 0;

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the GrainGenerator normalized to a value between 0 and 100
     */
    virtual void updateProgressAndMessage(const char* message, int progress);

    /**
     *
     */
    virtual void pipelineProgress(int value);

    /**
     *
     */
    virtual void pipelineMessage(const char* message);

    /**
     *
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


#define AIM_RECONSTRUCTION_BENCHMARKS 1

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
              pipelineMessage(#Message);\
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
      pipelineMessage(msg.c_str());\
      pipelineProgress(0);\
      pipelineFinished();\
      m = FuncClass::NullPointer();\
      return;   }


#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    std::string outpath = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + filename;



#endif /* ABSTRACTPIPELINE_H_ */
