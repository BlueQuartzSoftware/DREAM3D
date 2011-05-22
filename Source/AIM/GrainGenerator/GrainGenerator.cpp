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

#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"

#include "AIM/Common/HDF5/H5ReconStatsReader.h"
#include "AIM/Common/StructureReaders/AbstractStructureReader.h"
#include "AIM/Common/StructureReaders/VTKStructureReader.h"
#include "AIM/Common/GrainGeneratorVTKWriter.h"
#include "AIM/Common/HDF5/H5GrainWriter.h"


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

#ifdef AIM_USE_QT
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




#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    std::string outpath = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + filename;


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
m_OutputFilePrefix("GrainGenerator_"),
m_NumGrains(0),
m_ShapeClass(0),
m_XResolution(0.0),
m_YResolution(0.0),
m_ZResolution(0.0),
m_FillingErrorWeight(0.0),
m_NeighborhoodErrorWeight(0.0),
m_SizeDistErrorWeight(0.0),
m_PeriodicBoundary(true),
m_AlreadyFormed(false),
m_Precipitates(0),
m_WriteBinaryFiles(true),
m_WriteVisualizationFile(false),
m_WriteIPFFile(false),
m_WriteHDF5GrainFile(false),
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
  MAKE_OUTPUT_FILE_PATH ( crystallographicErrorFile , AIM::SyntheticBuilder::CrystallographicErrorFile)
  MAKE_OUTPUT_FILE_PATH ( eulerFile , AIM::SyntheticBuilder::GrainAnglesFile)
  MAKE_OUTPUT_FILE_PATH ( graindataFile , AIM::SyntheticBuilder::GrainDataFile)
  MAKE_OUTPUT_FILE_PATH ( packGrainsFile , AIM::SyntheticBuilder::PackGrainsFile)
  MAKE_OUTPUT_FILE_PATH ( hdf5ResultsFile , AIM::SyntheticBuilder::H5StatisticsFile)

  MAKE_OUTPUT_FILE_PATH ( reconVisFile, AIM::Reconstruction::VisualizationVizFile);
  MAKE_OUTPUT_FILE_PATH ( reconIPFVisFile, AIM::Reconstruction::IPFVizFile);
  MAKE_OUTPUT_FILE_PATH ( hdf5GrainFile, AIM::Reconstruction::HDF5GrainFile);

  START_CLOCK()

  H5ReconStatsWriter::Pointer h5io = H5ReconStatsWriter::New(hdf5ResultsFile);
  m = GrainGeneratorFunc::New();

  if (m_AlreadyFormed == false)
  {
    m->periodic_boundaries = m_PeriodicBoundary;
	m->numgrains = m_NumGrains;
    m->shapeclass = m_ShapeClass;
    m->resx = m_XResolution;
    m->resy = m_YResolution;
    m->resz = m_ZResolution;
    m->fillingerrorweight = m_FillingErrorWeight;
    m->neighborhooderrorweight = m_NeighborhoodErrorWeight;
    m->sizedisterrorweight = m_SizeDistErrorWeight;

    progressMessage(AIM_STRING("Loading Stats Data"), 5);
    err = m->readReconStatsData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, readReconStatsData)

    progressMessage(AIM_STRING("Loading Axis Orientation Data"), 10);
    err = m->readAxisOrientationData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, readAxisOrientationData);

    progressMessage(AIM_STRING("Packing Grains"), 25);
    m->numgrains = m->pack_grains(packGrainsFile, m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, pack_grains)

    progressMessage(AIM_STRING("Assigning Voxels"), 30);
    m->numgrains = m->assign_voxels(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, assign_voxels)

    progressMessage(AIM_STRING("Filling Gaps"), 40);
    m->fill_gaps(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, fill_gaps)

    progressMessage(AIM_STRING("Adjusting Boundaries"), 42);
//    m->numgrains = m->adjust_boundaries(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, adjust_boundaries)

  }
  else if (m_AlreadyFormed == true)
  {
    progressMessage(AIM_STRING("Loading Stats Data"), 10);
    err = m->readReconStatsData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, readReconStatsData)

    progressMessage(AIM_STRING("Loading Axis Orientation Data"), 25);
    err = m->readAxisOrientationData(h5reader);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, readAxisOrientationData);

    progressMessage(AIM_STRING("Reading Structure"), 40);
    std::string ext = MXAFileInfo::extension(m_StructureFile);
    if (ext.compare("vtk") == 0) {
      VTKStructureReader::Pointer reader = VTKStructureReader::New();
      reader->setInputFileName(m_StructureFile);
      reader->setGrainIdScalarName(AIM::Reconstruction::GrainIdScalarName);
      reader->setPhaseIdScalarName(AIM::Reconstruction::PhaseIdScalarName);
      err = reader->readStructure(m.get());
      if (err < 0) { m_Cancel = true; }
      CHECK_FOR_CANCELED(GrainGeneratorFunc, reading_structure)
    }
    else {
    //FIXME: Use another reader to get the structure.
    }
  }

  progressMessage(AIM_STRING("Finding Neighbors"), 44);
  m->find_neighbors();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, find_neighbors)

  if (m_AlreadyFormed == false)
  {
    progressMessage(AIM_STRING("Placing Precipitates"), 46);
    m->numgrains = m->place_precipitates(m->numgrains);
    CHECK_FOR_CANCELED(GrainGeneratorFunc, place_precipitates)
  }
  progressMessage(AIM_STRING("Loading ODF Data"), 48);
  err = m->readODFData(h5reader);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, readODFData)

  progressMessage(AIM_STRING("Loading Misorientation Data"), 50);
  err = m->readMisorientationData(h5reader);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, readMisorientationData)

  progressMessage(AIM_STRING("Loading Microtexture Data "), 55);
  err = m->readMicroTextureData(h5reader);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, readMicroTextureData)

  progressMessage(AIM_STRING("Assigning Eulers"), 60);
  m->assign_eulers(m->numgrains);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, assign_eulers)

  progressMessage(AIM_STRING("Measuring Misorientations"), 65);
  m->measure_misorientations();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, measure_misorientations)

  progressMessage(AIM_STRING("Matching Crystallography"), 65);
  m->matchCrystallography(crystallographicErrorFile, h5io);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, matchCrystallography)

  progressMessage(AIM_STRING("Finding Grain Centroids"), 68);
  m->find_centroids();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, find_centroids)

  progressMessage(AIM_STRING("Finding Grain Moments"), 71);
  m->find_moments();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, find_moments)

  progressMessage(AIM_STRING("Finding Grain Principal Axis Lengths"), 74);
  m->find_axes();
  CHECK_FOR_CANCELED(GrainGeneratorFunc, find_axes)


  progressMessage(AIM_STRING("Finding Grain Principal Axis Directions"), 77);
  m->find_vectors(h5io);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, find_vectors)

  progressMessage(AIM_STRING("Writing Stats"), 78);
  m->volume_stats(h5io);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, volume_stats)

  progressMessage(AIM_STRING("Writing Grain Data"), 80);
  m->write_graindata(graindataFile);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, write_graindata)

  progressMessage(AIM_STRING("Writing Euler Angles"), 81);
  m->write_eulerangles(eulerFile);
  CHECK_FOR_CANCELED(GrainGeneratorFunc, write_eulerangles)

  /** ********** This section writes the VTK files for visualization *** */
  GrainGeneratorVTKWriter::Pointer vtkWriter = GrainGeneratorVTKWriter::New();
  vtkWriter->setWriteBinaryFiles(m_WriteBinaryFiles);

  progressMessage(AIM_STRING("Writing VTK Visualization File"), 93);
  if (m_WriteVisualizationFile) {vtkWriter->writeVisualizationFile(m.get(), reconVisFile);}
  CHECK_FOR_CANCELED(GrainGeneratorFunc, writeVisualizationFile)

  progressMessage(AIM_STRING("Writing VTK Inverse Pole Figure File"), 94);
  if (m_WriteIPFFile) {vtkWriter->writeIPFVizFile(m.get(), reconIPFVisFile);}
  CHECK_FOR_CANCELED(GrainGeneratorFunc, writeIPFVizFile)
  /** ******* End VTK Visualization File Writing Section ****** */


  progressMessage(AIM_STRING("Writing Out HDF5 Grain File. This may take a few minutes to complete."), 99);
  H5GrainWriter::Pointer h5GrainWriter = H5GrainWriter::New();
  if (m_WriteHDF5GrainFile) { h5GrainWriter->writeHDF5GrainsFile(m.get(), hdf5GrainFile); }
  CHECK_FOR_CANCELED(GrainGeneratorFunc, writeHDF5GrainsFile)
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



