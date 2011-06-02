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

#include "ANG/AngDirectoryPatterns.h"
#include "ANG/AngReader.h"

#include "DREAM3D/ANGSupport/AbstractAngDataLoader.h"
#include "DREAM3D/ANGSupport/AngDataLoader.h"
#include "DREAM3D/ANGSupport/H5AngDataLoader.h"

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/OIMColoring.hpp"
#include "DREAM3D/Reconstruction/ReconstructionVTKWriter.h"
#include "DREAM3D/HDF5/H5ReconStatsWriter.h"
#include "DREAM3D/HDF5/H5GrainWriter.h"


#define AIM_RECONSTRUCTION_BENCHMARKS 1

#if AIM_RECONSTRUCTION_BENCHMARKS
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
      if(AIM_RECONSTRUCTION_BENCHMARKS) {\
    std::cout << #name << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;\
    millis = MXA::getMilliSeconds(); }


#else

#define CHECK_FOR_CANCELED(AClass, name)\
  m = AClass::NullPointer();\
  if(AIM_RECONSTRUCTION_BENCHMARKS) {\
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
  MAKE_OUTPUT_FILE_PATH ( graindataFile, AIM::Reconstruction::GrainDataFile);
  MAKE_OUTPUT_FILE_PATH ( reconDeformStatsFile, AIM::Reconstruction::DeformationStatsFile);
  MAKE_OUTPUT_FILE_PATH ( reconDeformIPFFile, AIM::Reconstruction::IPFDeformVTKFile);

  MAKE_OUTPUT_FILE_PATH ( hdf5ResultsFile, AIM::Reconstruction::H5StatisticsFile)
  H5ReconStatsWriter::Pointer h5io = H5ReconStatsWriter::New(hdf5ResultsFile);

  m = MicrostructureStatisticsFunc::New();

  START_CLOCK()

  progressMessage(AIM_STRING("Finding Reference Orientations For Grains"), 32);
  m->find_grain_and_kernel_misorientations();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, find_grain_and_kernel_misorientations)

  progressMessage(AIM_STRING("Finding Grain Schmid Factors"), 84);
  m->find_schmids();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, find_schmids)

  progressMessage(AIM_STRING("Finding Grain Centroids"), 48);
  if(m->zpoints > 1) m->find_centroids();
  if(m->zpoints == 1)m->find_centroids2D();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, find_centroids2D)

  progressMessage(AIM_STRING("Finding Grain Moments"), 52);
  if(m->zpoints > 1) m->find_moments();
  if(m->zpoints == 1) m->find_moments2D();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, find_moments2D)

  progressMessage(AIM_STRING("Finding Grain Principal Axes Lengths"), 56);
  if(m->zpoints > 1) m->find_axes();
  if(m->zpoints == 1) m->find_axes2D();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, find_axes2D)

  progressMessage(AIM_STRING("Finding Grain Pricipal Axes Vectors"), 60);
  if(m->zpoints > 1) m->find_vectors(h5io);
  if(m->zpoints == 1) m->find_vectors2D(h5io);
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, find_vectors2D)

  progressMessage(AIM_STRING("Defining Neighborhoods"), 64);
  m->define_neighborhood();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, define_neighborhood)

  progressMessage(AIM_STRING("Finding Euclidean Distance Maps"), 68);
  m->find_euclidean_map();
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, find_euclidean_map)

  progressMessage(AIM_STRING("Finding Euler ODF"), 72);
  m->find_eulerodf(h5io);
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, find_eulerodf)

  progressMessage(AIM_STRING("Measuring Misorientations"), 76);
  m->measure_misorientations(h5io);
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, measure_misorientations)

  progressMessage(AIM_STRING("Writing Statistics"), 88);
  if(m->zpoints > 1) { m->volume_stats(h5io); }
  if(m->zpoints == 1) { m->volume_stats2D(h5io); }

  progressMessage(AIM_STRING("Writing Deformation Statistics"), 88);
  m->deformation_stats(reconDeformStatsFile, reconDeformIPFFile);
  CHECK_FOR_CANCELED(MicrostructureStatisticsFunc, volume_stats)

  progressMessage(AIM_STRING("Writing Grain Data"), 92);
  m->write_graindata(graindataFile);

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
