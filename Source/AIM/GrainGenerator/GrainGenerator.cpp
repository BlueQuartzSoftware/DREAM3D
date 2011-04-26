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

#include "GrainGenerator.h"
#include "AIM/Common/HDF5/H5ReconStatsReader.h"
#include "MXA/Utilities/MXADir.h"


#define CREATE_INPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);

#define CREATE_OUTPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);


#ifdef AIM_USE_QT
#define CHECK_FOR_CANCELED(AClass)\
  if (this->m_Cancel) { \
  QString msg = #AClass; \
  msg += " was Canceled"; \
  emit updateMessage(msg);\
  emit updateProgress(0);\
  return;}

#define CHECK_ERROR(name)\
    if(err < 0) {\
      setErrorCondition(err);\
      QString msg = #name;\
      msg += " Returned an error condition. Grain Generator has stopped.";\
      emit updateMessage(msg);\
      emit updateProgress(0);\
      emit finished();\
      return;   }

#else
#define CHECK_FOR_CANCELED(AClass)\
  ;
#define CHECK_ERROR(name)\
    ;
#endif


#if AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::Pointer GrainGenerator::New( QObject* parent)
{
  Pointer sharedPtr(new GrainGenerator(parent));
  return sharedPtr;
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::GrainGenerator(
#if AIM_USE_QT
QObject* parent
#endif
) :
#if AIM_USE_QT
QObject(parent),
#endif
m_H5StatsFile(""),
m_OutputDirectory("."),
m_NumGrains(0),
m_ShapeClass(0),
m_XResolution(0.0),
m_YResolution(0.0),
m_ZResolution(0.0),
m_FillingErrorWeight(0.0),
m_NeighborhoodErrorWeight(0.0),
m_SizeDistErrorWeight(0.0),
m_AlreadyFormed(false),
m_Precipitates(0),
m_ErrorCondition(0)
#if AIM_USE_QT
  ,m_Cancel(false)
#endif
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::~GrainGenerator()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::compute()
{
  int err = 0;

  H5ReconStatsReader::Pointer h5reader = H5ReconStatsReader::New(m_H5StatsFile);
  if (h5reader.get() == NULL)
  {
    progressMessage(AIM_STRING("Error Opening HDF5 Stats File. Nothing generated"), 100);
    return;
  }
  std::string crystallographicErrorFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::CrystallographicErrorFile;
  std::string eulerFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::EulerFile;
  std::string graindataFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::GrainDataFile;
  std::string hdf5ResultsFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::H5StatisticsFile;
  std::string visFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::VisualizationFile;
  std::string packGrainsFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::PackGrainsFile;

  H5ReconStatsWriter::Pointer h5io = H5ReconStatsWriter::New(hdf5ResultsFile);
  m = GrainGeneratorFunc::New();



  if (m_AlreadyFormed == false)
  {
    m->numgrains = m_NumGrains;
    m->shapeclass = m_ShapeClass;
    m->resx = m_XResolution;
    m->resy = m_YResolution;
    m->resz = m_ZResolution;
    m->fillingerrorweight = m_FillingErrorWeight;
    m->neighborhooderrorweight = m_NeighborhoodErrorWeight;
    m->sizedisterrorweight = m_SizeDistErrorWeight;
#ifndef _WIN32
#warning Precipitates are not dealt with currently
#endif

    CHECK_FOR_CANCELED(GrainGeneratorFunc)
    progressMessage(AIM_STRING("Loading Stats Data"), 5);
    err = m->readReconStatsData(h5reader);
    CHECK_ERROR(readReconStatsData)

    CHECK_FOR_CANCELED(GrainGeneratorFunc)
    progressMessage(AIM_STRING("Loading Axis Orientation Data"), 10);
    err = m->readAxisOrientationData(h5reader);
    CHECK_ERROR(readAxisOrientationData);


    CHECK_FOR_CANCELED(GrainGeneratorFunc)
    progressMessage(AIM_STRING("Packing Grains"), 25);
    m->numgrains = m->pack_grains(packGrainsFile, m->numgrains);

    CHECK_FOR_CANCELED(GrainGeneratorFunc)
    progressMessage(AIM_STRING("Assigning Voxels"), 30);
    m->numgrains = m->assign_voxels(m->numgrains);

    CHECK_FOR_CANCELED(GrainGeneratorFunc)
    progressMessage(AIM_STRING("Filling Gaps"), 40);
    m->fill_gaps(m->numgrains);

    CHECK_FOR_CANCELED(GrainGeneratorFunc)
    progressMessage(AIM_STRING("Adjusting Boundaries"), 40);
    m->numgrains = m->adjust_boundaries(m->numgrains);

  }

#if 0
  if (m_AlreadyFormed == true)
  {
    m->initialize(m_NumGrains, m_ShapeClass, m_XResolution, m_YResolution, m_ZResolution,
                  m_FillingErrorWeight, m_NeighborhoodErrorWeight, m_SizeDistErrorWeight,
                  m_Precipitates, m_FractionPrecipitates, xtals);

    CHECK_FOR_CANCELED(GrainGeneratorFunc)
    progressMessage(AIM_STRING("Reading Structure"), 40);
    m->read_structure(visFile);
  }
#endif
  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Finding Neighbors"), 45);
  m->find_neighbors();

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Loading ODF Data"), 48);
  err = m->readODFData(h5reader);
  CHECK_ERROR(readODFData)

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Loading Misorientation Data"), 50);
  err = m->readMisorientationData(h5reader);
  CHECK_ERROR(readMisorientationData)

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Loading Microtexture Data "), 55);
  err = m->readMicroTextureData(h5reader);
  CHECK_ERROR(readMicroTextureData)


  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Assigning Eulers"), 60);
  m->assign_eulers(m->numgrains);

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Measuring Misorientations"), 65);
  m->measure_misorientations();

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Matching Crystallography"), 65);
  m->matchCrystallography(crystallographicErrorFile, h5io);

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Finding Grain Centroids"), 68);
  m->find_centroids();

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Finding Grain Moments"), 71);
  m->find_moments();

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Finding Grain Principal Axis Lengths"), 74);
  m->find_axes();

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Finding Grain Principal Axis Directions"), 77);
  m->find_vectors(h5io);

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Writing Stats"), 87);
  m->volume_stats(h5io);

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("writing Cube"), 90);
  m->writeCube(visFile, m->numgrains);

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Writing Grain Data"), 94);
  m->write_graindata(graindataFile);

  CHECK_FOR_CANCELED(GrainGeneratorFunc)
  progressMessage(AIM_STRING("Writing Euler Angles"), 98);
  m->write_eulerangles(eulerFile);

  progressMessage(AIM_STRING("Generation Completed"), 100);

  // Clean up all the memory
  m = GrainGeneratorFunc::NullPointer();
#if AIM_USE_QT
  emit finished();
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::progressMessage(AIM_STRING message, int progress)
{
#ifdef AIM_USE_QT
      emit updateMessage(QString(message));
      emit updateProgress(progress);
    //  std::cout << message.toStdString() << std::endl;
#else
  std::cout << message << std::endl;
#endif
}

#ifdef AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::on_CancelWorker()
{
 //    std::cout << "GrainGenerator::cancelWorker()" << std::endl;
     this->m_Cancel = true;
}
#endif



