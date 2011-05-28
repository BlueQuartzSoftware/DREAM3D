/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#include "MicrostructureStatistics.h"

#include "MXA/MXATypes.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"



#define DREAM3D_BENCHMARKS 1

#if DREAM3D_BENCHMARKS
#define START_CLOCK()\
  unsigned long long int millis;\
  millis = MXA::getMilliSeconds();
#else
#define START_CLOCK() unsigned long long int millis = 0;\
  millis = 0;
#endif

#ifdef DREAM3D_USE_QT
#define CHECK_FOR_CANCELED(AClass, name)\
    if (this->m_Cancel) { \
      QString msg = #AClass; \
              msg += " was Canceled"; \
              emit updateMessage(msg);\
              emit updateProgress(0);\
              emit finished();\
              m = AClass::NullPointer();\
      return;}\
      if(DREAM3D_BENCHMARKS) {\
    std::cout << #name << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;\
    millis = MXA::getMilliSeconds(); }


#else

#define CHECK_FOR_CANCELED(AClass, name)\
  m = AClass::NullPointer();\
  if(DREAM3D_BENCHMARKS) {\
    std::cout << #name << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;\
    millis = MXA::getMilliSeconds(); }
#endif





#if DREAM3D_USE_QT

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatistics::Pointer MicrostructureStatistics::New( QObject* parent)
{
  Pointer sharedPtr(new MicrostructureStatistics(parent));
  return sharedPtr;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#if DREAM3D_USE_QT
MicrostructureStatistics::MicrostructureStatistics(QObject* parent) :
QObject(parent),
#else
MicrostructureStatistics::MicrostructureStatistics() :
#endif

m_OutputDirectory("."),
m_OutputFilePrefix("MicrostructureStatistics_"),
m_ErrorCondition(0)
#if DREAM3D_USE_QT
  ,m_Cancel(false)
#endif
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatistics::~MicrostructureStatistics()
{
 // std::cout << "~MicrostructureStatistics()" << std::endl;
}

#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    std::string outpath = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + filename;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatistics::compute()
{
  //std::cout << "MicrostructureStatistics::compute Start" << std::endl;
  int err = -1;



  progressMessage(AIM_STRING("MicrostructureStatistics Complete"), 100);
  // Clean up all the memory by forcibly setting a NULL pointer to the Shared
  // pointer object.
  m = MicrostructureStatisticsFunc::NullPointer();  // Clean up the memory
  //std::cout << "MicrostructureStatistics::compute Complete" << std::endl;
#if DREAM3D_USE_QT
  emit finished();
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatistics::progressMessage(AIM_STRING message, int progress)
{
#ifdef DREAM3D_USE_QT
      emit updateMessage(QString(message));
      emit updateProgress(progress);
    //  std::cout << message.toStdString() << std::endl;
#else
  std::cout << progress << "% " << message << std::endl;
#endif
}

#ifdef DREAM3D_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatistics::on_CancelWorker()
{
//  std::cout << "MicrostructureStatistics::cancelWorker()" << std::endl;
  this->m_Cancel = true;
}
#endif

#define PRINT_PROPERTY( out, var)\
  out << #var << ": " << m_##var << std::endl;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatistics::printSettings(std::ostream &ostream)
{
  ostream << "MicrostructureStatistics Settings Being Used" << std::endl;

}
