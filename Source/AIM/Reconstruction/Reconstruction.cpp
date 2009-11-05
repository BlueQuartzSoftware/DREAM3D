///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include "Reconstruction.h"

#include <AIM/ANG/AngDirectoryPatterns.h>
#include <AIM/ANG/AngFileReader.h>
#include <AIM/ANG/AngFileHelper.h>
#include <MXA/Utilities/MXAFileSystemPath.h>


#ifdef AIM_USE_QT


#define CHECK_FOR_CANCELED(AClass)\
    if (this->m_Cancel) { \
      QString msg = #AClass; \
              msg += " was Canceled"; \
      return;}

#else

#define CHECK_FOR_CANCELED(AClass)\
    ;

#endif


#if AIM_USE_QT

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::Pointer Reconstruction::New( QObject* parent)
{
  Pointer sharedPtr(new Reconstruction(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::Reconstruction( QObject* parent) :
QThread(parent),
  m_InputDirectory("."), m_OutputDirectory("."),
  m_AngFilePrefix("Slice_"), m_AngSeriesMaxSlice(3),
  m_ZStartIndex(0), m_ZEndIndex(0), m_ZResolution(0.25),
  m_MergeTwins(false), m_MinAllowedGrainSize(0.0),
  m_MinSeedConfidence(0.0), m_MisorientationTolerance(0.0),
  m_CrystalStructure(AIM::Representation::Hexagonal),
  m_AlreadyFormed(false), m_ErrorCondition(0),m_Cancel(false)
{

}
#else
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::Reconstruction() :
m_InputDirectory("."), m_OutputDirectory("."),
m_AngFilePrefix("Slice_"), m_AngSeriesMaxSlice(3),
m_ZStartIndex(0), m_ZEndIndex(0),
m_ZResolution(0.25),
m_MergeTwins(false), m_MinAllowedGrainSize(0.0),
m_MinSeedConfidence(0.0), m_MisorientationTolerance(0.0),
m_CrystalStructure(AIM::Representation::Hexagonal),
m_AlreadyFormed(false)
{

}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::~Reconstruction()
{
  std::cout << "~Reconstruction()" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::parseAngFile()
{

}

#if AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::run()
{
  compute();
  m = ReconstructionFunc::NullPointer();  // Clean up the memory
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::compute()
{
//  std::cout << "Reconstruction::compute" << std::endl;
  int32 sliceCount = 1;
  int32 width = 0;
  int32 totalSlices = m_AngSeriesMaxSlice;
  while (sliceCount < totalSlices)
  {
    ++width;
    sliceCount *= 10;
  }

  m_InputDirectory = MXAFileSystemPath::toNativeSeparators(m_InputDirectory);
  m_OutputDirectory = MXAFileSystemPath::toNativeSeparators(m_OutputDirectory);
  AngDirectoryPatterns::Pointer p = AngDirectoryPatterns::New(m_InputDirectory, m_AngFilePrefix, width);

  AngFileReader::Pointer reader = AngFileReader::New();
  reader->readFile(p->generateFullPathAngFileName(m_ZStartIndex));

  AngFileHelper::Pointer angFileHelper = AngFileHelper::New();
  angFileHelper->setZIndexStart(m_ZStartIndex);
  angFileHelper->setZIndexEnd(m_ZEndIndex);
  angFileHelper->setDirectoryPattern(p);

  m = ReconstructionFunc::New();
  m->initialize(  reader->getXStep(), reader->getYStep(), m_ZResolution,
                       reader->getNumEvenCols(), reader->getNumRows(), (m_ZEndIndex - m_ZStartIndex),
                       m_MergeTwins, m_MinAllowedGrainSize, m_MinSeedConfidence,
                       m_MisorientationTolerance, m_CrystalStructure, m_AlreadyFormed);
  m->m_angFileHelper = angFileHelper;
  reader = AngFileReader::NullPointer(); // Remove this object as it is no longer needed.

  int32 numgrains = 0;
  int32 mindiameter = 100000;
  int32 maxdiameter = 0;

  std::string reconFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::ReconstructedDataFile;
  reconFile = MXAFileSystemPath::toNativeSeparators(reconFile);
  if (m_AlreadyFormed == false)
  {
    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Loading Slices"), 3 );
    m->loadSlices();

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Forming Grains"), 6 );
    numgrains = m->form_grains();
    m->numgrains = numgrains;

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Removing Small Grains"), 9 );
    m->remove_smallgrains();

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Renumbering Small Grains"), 12 );
    numgrains = m->renumber_grains1();

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("write_volume"), 15 );
    m->write_volume(reconFile);
  }

  if (m_AlreadyFormed == true)
  {
    // Sanity Check the the Reconstruction File does exist in the output directory
    if (MXAFileSystemPath::exists(reconFile) == true)
    {
      CHECK_FOR_CANCELED(ReconstructionFunc)
      progressMessage(AIM_STRING("Loading Existing Data"), 18 );
      m->load_data(reconFile);
    }
    else
    {
      std::cout << "******************************************************************************************" << std::endl;
      std::cout << "* Input file needed by using the --alreadyFormed option was not found. The following file " << std::endl;
      std::cout << "* should exist and be readable." << std::endl;
      std::cout << "* " << reconFile << std::endl;
      std::cout << "******************************************************************************************" << std::endl;
      m_ErrorCondition = -1;
      return;

    }
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("homogenize_grains"), 21 );
  m->homogenize_grains();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("assign_badpoints"), 24 );
  m->assign_badpoints();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_neighbors"), 27 );
  m->find_neighbors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("merge_containedgrains"), 30 );
  m->merge_containedgrains();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("renumber_grains2"), 33 );
  numgrains = m->renumber_grains2();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("write_volume"), 36 );
  m->write_volume(reconFile);
  if (m_MergeTwins == 1)
  {
    m->merge_twins();
    m->characterize_twins();
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("renumber_grains3"), 39 );
  numgrains = m->renumber_grains3();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("write_volume"), 42 );
  m->write_volume(reconFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_goodneighbors"), 45 );
  m->find_goodneighbors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_centroids"), 48 );
  m->find_centroids();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_moments"), 51 );
  m->find_moments();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_axes"), 54 );
  m->find_axes();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_vectors"), 57 );
  m->find_vectors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("measure_misorientations"), 60 );
  m->measure_misorientations();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_colors"), 63 );
  m->find_colors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_convexities"), 66 );
  m->find_convexities();

  std::string  statsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::StatsFile;
//  std::string  volBinFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::VolBinFile;
//  std::string  bOverABinsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::BOverABinsFile;
//  std::string  cOverABinsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::COverABinsFile;
//  std::string  cOverBBinsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::COverBBinsFile;
//  std::string  svnFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::SVNFile;
//  std::string  svsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::SVSFile;
  std::string  misorientationFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MisorientationBinsFile;
  std::string  microBinsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MicroBinsFile;

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Statistics"), 69 );
  m->volume_stats(statsFile,misorientationFile,microBinsFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("write_volume"), 72 );
  m->write_volume(reconFile);

  std::string reconVisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::ReconstructedVisualizationFile;
  std::string grainsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::GrainsFile;

  std::string axisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::AxisOrientationsFile;
  std::string eulerFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::EulerAnglesFile;
  std::string boundaryFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::BoundaryCentersFile;

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Reconstruction Visualization File"), 75 );
  m->create_visualization(reconVisFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Grains File"), 78 );
  m->write_grains(grainsFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Axis Orientation File"), 81 );
  m->write_axisorientations(axisFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Euler Angle File"), 84 );
  m->write_eulerangles(eulerFile);

  progressMessage(AIM_STRING("Reconstruction Complete"), 100 );

 // std::cout << "Reconstruction::compute is Complete" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::progressMessage(AIM_STRING message, int progress)
{
#ifdef AIM_USE_QT
      emit updateMessage(QString(message));
      emit updateProgress(progress);
      std::cout << message.toStdString() << std::endl;
#else

  std::cout << message << std::endl;

#endif
}

#ifdef AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::on_CancelWorker()
{
  // std::cout << "Reconstruction::cancelWorker()" << std::endl;
  this->m_Cancel = true;
  if (m.get() != NULL)
  {
   if (m->m_angFileHelper.get() != NULL)
   {
     m->m_angFileHelper->setCancel(true);
   }
  }
}
#endif

