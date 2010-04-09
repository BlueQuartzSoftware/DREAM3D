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
              emit updateMessage(msg);\
              emit updateProgress(0);\
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
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::Reconstruction( 
#if AIM_USE_QT
QObject* parent
#endif
) :
#if AIM_USE_QT
QThread(parent),
#endif
m_InputDirectory("."),
m_OutputDirectory("."),
m_AngFilePrefix("Slice_"), 
m_AngSeriesMaxSlice(3),
m_ZStartIndex(0),
m_ZEndIndex(0), 
m_ZResolution(0.25),
m_MergeTwins(false),
m_MergeColonies(false),
m_MinAllowedGrainSize(0),
m_MinSeedConfidence(0.0),
m_MinSeedImageQuality(0.0),
m_MisorientationTolerance(0.0),
m_CrystalStructure(AIM::Representation::Cubic),
m_AlreadyFormed(false), 
m_ErrorCondition(0)
#if AIM_USE_QT
  ,m_Cancel(false)
#endif
{

}


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
  m->m_angFileHelper = angFileHelper;
  m->find_cutout(p->generateFullPathAngFileName(m_ZStartIndex), reader->getNumEvenCols(), reader->getNumRows(), m_MinSeedConfidence, m_MinSeedImageQuality);
  m->initialize(reader->getXStep(), reader->getYStep(), m_ZResolution,
                       reader->getNumEvenCols(), reader->getNumRows(), (m_ZEndIndex - m_ZStartIndex),
                       m_MergeTwins, m_MergeColonies, m_MinAllowedGrainSize, m_MinSeedConfidence, m_MinSeedImageQuality,
                       m_MisorientationTolerance, m_CrystalStructure, m_AlreadyFormed);
  reader = AngFileReader::NullPointer(); // Remove this object as it is no longer needed.

  int32 mindiameter = 100000;
  int32 maxdiameter = 0;
  double quat_symmcubic[24][5] = {
	  {0.000000000, 0.000000000, 0.000000000, 0.000000000, 1.000000000},
	  {0.000000000, 1.000000000, 0.000000000, 0.000000000, 0.000000000},
	  {0.000000000, 0.000000000, 1.000000000, 0.000000000, 0.000000000},
	  {0.000000000, 0.000000000, 0.000000000, 1.000000000, 0.000000000},
	  {0.000000000, 0.707106781, 0.000000000, 0.000000000, 0.707106781},
	  {0.000000000, 0.000000000, 0.707106781, 0.000000000, 0.707106781},
	  {0.000000000, 0.000000000, 0.000000000, 0.707106781, 0.707106781},
	  {0.000000000, -0.707106781, 0.000000000, 0.000000000, 0.707106781},
	  {0.000000000, 0.000000000, -0.707106781, 0.000000000, 0.707106781},
	  {0.000000000, 0.000000000, 0.000000000, -0.707106781, 0.707106781},
	  {0.000000000, 0.707106781, 0.707106781, 0.000000000, 0.000000000},
	  {0.000000000, -0.707106781, 0.707106781, 0.000000000, 0.000000000},
	  {0.000000000, 0.000000000, 0.707106781, 0.707106781, 0.000000000},
	  {0.000000000, 0.000000000, -0.707106781, 0.707106781, 0.000000000},
	  {0.000000000, 0.707106781, 0.000000000, 0.707106781, 0.000000000},
	  {0.000000000, -0.707106781, 0.000000000, 0.707106781, 0.000000000},
	  {0.000000000, 0.500000000, 0.500000000, 0.500000000, 0.500000000},
	  {0.000000000, -0.500000000, -0.500000000, -0.500000000, 0.500000000},
	  {0.000000000, 0.500000000, -0.500000000, 0.500000000, 0.500000000},
	  {0.000000000, -0.500000000, 0.500000000, -0.500000000, 0.500000000},
	  {0.000000000, -0.500000000, 0.500000000, 0.500000000, 0.500000000},
	  {0.000000000, 0.500000000, -0.500000000, -0.500000000, 0.500000000},
	  {0.000000000, -0.500000000, -0.500000000, 0.500000000, 0.500000000},
	  {0.000000000, 0.500000000, 0.500000000, -0.500000000, 0.500000000}};

  double quat_symmhex[12][5] = {
	  {0.000000000, 0.000000000, 0.000000000, 0.000000000, 1.000000000},
	  {0.000000000, 0.000000000, 0.000000000, 0.500000000, 0.866025400},
	  {0.000000000, 0.000000000, 0.000000000, 0.866025400, 0.500000000},
	  {0.000000000, 0.000000000, 0.000000000, 1.000000000, 0.000000000},
	  {0.000000000, 0.000000000, 0.000000000, 0.866025400, -0.50000000},
	  {0.000000000, 0.000000000, 0.000000000, 0.500000000, -0.86602540},
	  {0.000000000, 1.000000000, 0.000000000, 0.000000000, 0.000000000},
	  {0.000000000, 0.866025400, 0.500000000, 0.000000000, 0.000000000},
	  {0.000000000, 0.500000000, 0.866025400, 0.000000000, 0.000000000},
	  {0.000000000, 0.000000000, 1.000000000, 0.000000000, 0.000000000},
	  {0.000000000, -0.50000000, 0.866025400, 0.000000000, 0.000000000},
	  {0.000000000, -0.86602540, 0.500000000, 0.000000000, 0.000000000}};

  std::string  statsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::StatsFile;
  std::string  misorientationFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MisorientationBinsFile;
  std::string  microBinsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MicroBinsFile;
  std::string reconVisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::ReconstructedVisualizationFile;
  std::string axisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::AxisOrientationsFile;
  std::string graindataFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::graindataFile;
  std::string eulerFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::EulerAnglesFile;
  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Loading Slices"), 3 );
  m->loadSlices(quat_symmcubic, quat_symmhex);

  if (m_AlreadyFormed == true)
  {
    // Sanity Check the the Reconstruction File does exist in the output directory
    if (MXAFileSystemPath::exists(reconVisFile) == true)
    {
      CHECK_FOR_CANCELED(ReconstructionFunc)
      progressMessage(AIM_STRING("Loading Existing Data"), 8 );
      m->numgrains = m->load_data(reconVisFile);
    }
    else
    {
      std::cout << "******************************************************************************************" << std::endl;
      std::cout << "* Input file needed by using the --alreadyFormed option was not found. The following file " << std::endl;
      std::cout << "* should exist and be readable." << std::endl;
      std::cout << "* " << reconVisFile << std::endl;
      std::cout << "******************************************************************************************" << std::endl;
      m_ErrorCondition = -1;
      return;

    }
  }
  if (m_AlreadyFormed == false)
  {
    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Forming Grains"), 8 );
    m->numgrains = m->form_grains(quat_symmcubic, quat_symmhex);

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Removing Small Grains"), 10 );
    m->remove_smallgrains();

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Renumbering Small Grains"), 12 );
    m->numgrains = m->renumber_grains1();

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("assign_badpoints"), 21 );
    m->assign_badpoints();

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("write_volume"), 22 );
    m->write_volume(reconVisFile);
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_neighbors"), 24 );
  m->find_neighbors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("merge_containedgrains"), 27 );
  m->merge_containedgrains();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("renumber_grains2"), 30 );
  m->numgrains = m->renumber_grains2();

  if (m_MergeTwins == true)
  {
    m->merge_twins(quat_symmcubic, quat_symmhex);
    m->characterize_twins();
  }

  if (m_MergeColonies == true)
  {
    m->merge_colonies(quat_symmcubic, quat_symmhex);
    m->characterize_colonies();
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("renumber_grains3"), 39 );
  m->numgrains = m->renumber_grains3();
  
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
  progressMessage(AIM_STRING("homogenize_grains"), 33 );
  m->homogenize_grains(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_eulerodf"), 57 );
  m->find_eulerodf();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_axisodf"), 57 );
  m->find_axisodf();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("measure_misorientations"), 60 );
  m->measure_misorientations(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_colors"), 63 );
  m->find_colors(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("find_convexities"), 66 );
  m->find_convexities();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Statistics"), 69 );
  m->volume_stats(statsFile,misorientationFile,microBinsFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("write_volume"), 72 );
  m->write_volume(reconVisFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Grain Data"), 81 );
  m->write_graindata(graindataFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Axis Orientation File"), 81 );
  m->write_axisodf(axisFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Euler Angle File"), 84 );
  m->write_eulerodf(eulerFile);

  progressMessage(AIM_STRING("Reconstruction Complete"), 100 );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::progressMessage(AIM_STRING message, int progress)
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

