/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include <stdlib.h>

#include <string>
#include <vector>

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/QualityMetricFilter.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/ReconstructionFilters/LoadSlices.h"
#include "DREAM3DLib/ReconstructionFilters/AlignSections.h"
#include "DREAM3DLib/ReconstructionFilters/SegmentGrains.h"
#include "DREAM3DLib/ReconstructionFilters/CleanupGrains.h"

//#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"


#define DREAM3D_BENCHMARKS 1

#if DREAM3D_BENCHMARKS
#define START_CLOCK()\
  unsigned long long int millis;\
  millis = MXA::getMilliSeconds();
#else
#define START_CLOCK() unsigned long long int millis = 0;\
  millis = 0;
#endif


const std::string EBSDFile("/Users/Shared/Data/Ang_Data/Small_IN100_Output/Small_IN100.h5ebsd");

void updateProgressAndMessage(const std::string &msg, int prog)
{
  std::cout << prog << "% - " << msg << std::endl;
}

std::string getH5AngFile()
{
  return EBSDFile;
}

int getZStartIndex() { return 1; }
int getZEndIndex() { return 117; }
std::vector<DREAM3D::Reconstruction::PhaseType> getPhaseTypes() {
  std::vector<DREAM3D::Reconstruction::PhaseType> phaseTypes(1, DREAM3D::Reconstruction::UnknownPhaseType);
  phaseTypes.push_back(DREAM3D::Reconstruction::PrimaryPhase);
  return phaseTypes;
}

std::vector<QualityMetricFilter::Pointer> getQualityMetricFilters()
{
  std::vector<QualityMetricFilter::Pointer> filters;
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(Ebsd::Ang::ImageQuality);
    filter->setFieldValue(120);
    filter->setFieldOperator(">");
    filters.push_back(filter);
  }
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(Ebsd::Ang::ConfidenceIndex);
    filter->setFieldValue(0.1);
    filter->setFieldOperator(">");
    filters.push_back(filter);
  }

  return filters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void pipelineProgress(int value)
{
  std::cout << value << "%" << std::endl;
}

void pipelineProgressMessage(const std::string &msg)
{
  std::cout << msg << std::endl;
}

void pipelineErrorMessage(const char* message)
{
  std::cout << "Error Message: " << message << std::endl;
}

void setErrorCondition(int err) {
  std::cout << "Error Condition: " << err << std::endl;
}

void pipelineFinished()
{
  std::cout << "Pipeline Complete." << std::endl;
}



int main(int argc, char **argv)
{


  float m_MisorientationTolerance = 0.5f;
  int m_MinAllowedGrainSize = 10;

  // Create the DataContainer object
  DataContainer::Pointer m = DataContainer::New();
//  m->addObserver(static_cast<Observer*>(this));

  std::string m_OutputDirectory = MXADir::toNativeSeparators(UnitTest::NeighborListTest::TestDir);

  // Create a Vector to hold all the filters. Later on we will execute all the filters
  std::vector<AbstractFilter::Pointer> pipeline;

 // updateProgressAndMessage(("Loading Slices"), 10);
  LoadSlices::Pointer load_slices = LoadSlices::New();
  load_slices->setH5EbsdFile(getH5AngFile());
  load_slices->setZStartIndex(getZStartIndex());
  load_slices->setZEndIndex(getZEndIndex());
  load_slices->setPhaseTypes(getPhaseTypes());
  load_slices->setQualityMetricFilters(getQualityMetricFilters());
  load_slices->setRefFrameZDir(Ebsd::LowtoHigh);
  load_slices->setMisorientationTolerance(m_MisorientationTolerance);
  pipeline.push_back(load_slices);

  AlignSections::Pointer align_sections = AlignSections::New();
  align_sections->setmisorientationtolerance(m_MisorientationTolerance);
  align_sections->setalignmeth(DREAM3D::Reconstruction::OuterBoundary);
  pipeline.push_back(align_sections);

  SegmentGrains::Pointer segment_grains = SegmentGrains::New();
  segment_grains->setMisorientationTolerance(m_MisorientationTolerance);
  pipeline.push_back(segment_grains);

  CleanupGrains::Pointer cleanup_grains = CleanupGrains::New();
  cleanup_grains->setminallowedgrainsize(m_MinAllowedGrainSize);
  cleanup_grains->setMisorientationTolerance(m_MisorientationTolerance);
  pipeline.push_back(cleanup_grains);

  Observer observer;
  int err = 0;
  // Start a Benchmark Clock so we can keep track of each filter's execution time
  START_CLOCK()
  // Start looping through the Pipeline
  float progress = 0.0f;
  std::stringstream ss;
  for (std::vector<AbstractFilter::Pointer>::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter)
  {

    progress = progress + 1.0f;
    pipelineProgress(progress / (pipeline.size() + 1) * 100.0f);
    ss.str("");
    ss << logTime() << " Executing Filter [" << progress << "/" << pipeline.size() << "] - " << (*filter)->getNameOfClass();
    pipelineProgressMessage(ss.str());
    (*filter)->addObserver(&observer);
    (*filter)->setDataContainer(m.get());
 //   setCurrentFilter(*filter);
    (*filter)->execute();
    (*filter)->removeObserver(&observer);
    err = (*filter)->getErrorCondition();
    if(err < 0)
    {
      setErrorCondition(err);
      pipelineErrorMessage((*filter)->getErrorMessage().c_str());
      pipelineProgress(100);
      pipelineFinished();
      return EXIT_FAILURE;
    }

    if(DREAM3D_BENCHMARKS)
    {
      std::cout << (*filter)->getNameOfClass() << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;
      millis = MXA::getMilliSeconds();
    }
  }

  updateProgressAndMessage("FindNeighborsTest Complete", 100);
  return EXIT_SUCCESS;
}

