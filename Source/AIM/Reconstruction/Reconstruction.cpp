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

  AngFileHelper::Pointer angFileHelper = AngFileHelper::New();
  angFileHelper->setZIndexStart(m_ZStartIndex);
  angFileHelper->setZIndexEnd(m_ZEndIndex);
  angFileHelper->setDirectoryPattern(p);

  m = ReconstructionFunc::New();
  m->m_angFileHelper = angFileHelper;
  m->setDirectoryPattern(p);
  m->initialize(m_ZStartIndex, m_ZEndIndex, m_ZResolution, m_MergeTwins, m_MergeColonies, m_MinAllowedGrainSize, 
	                   m_MinSeedConfidence, m_MinSeedImageQuality, m_MisorientationTolerance, m_CrystalStructure, m_AlignmentMethod, m_AlreadyFormed);

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
  std::string reconIPFVisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::ReconstructedIPFVisualizationFile;
  std::string reconDisVisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::ReconstructedDisVisualizationFile;
  std::string reconIQVisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::ReconstructedIQVisualizationFile;
  std::string reconSFVisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::ReconstructedSFVisualizationFile;
  std::string axisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::AxisOrientationsFile;
  std::string graindataFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::graindataFile;
  std::string eulerFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::EulerAnglesFile;

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Loading Slices"), 4 );
  m->loadSlices();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Border"), 8 );
  m->find_border();

  CHECK_FOR_CANCELED(ReconstructionFunc) 
  progressMessage(AIM_STRING("Cleaning Data"), 12 );
  m->cleanup_data();

  if(m_AlignmentMethod == 3)
  {
	  CHECK_FOR_CANCELED(ReconstructionFunc)
	  progressMessage(AIM_STRING("Identifying Grains on Sections"), 14 );
	  m->form_grains_sections(quat_symmcubic, quat_symmhex);
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Aligning Slices"), 16 );
  m->align_sections(quat_symmcubic, quat_symmhex);

  if(m_AlignmentMethod == 3)
  {
	  CHECK_FOR_CANCELED(ReconstructionFunc)
	  progressMessage(AIM_STRING("Redefining Border"), 18 );
	  m->find_border();
  }

  if (m_AlreadyFormed == true)
  {
    // Sanity Check the the Reconstruction File does exist in the output directory
    if (MXAFileSystemPath::exists(reconVisFile) == true)
    {
      CHECK_FOR_CANCELED(ReconstructionFunc)
      progressMessage(AIM_STRING("Loading Existing Data"), 28 );
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
    progressMessage(AIM_STRING("Forming Grains"), 20 );
    m->numgrains = m->form_grains(quat_symmcubic, quat_symmhex);

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Assigning Bad Points"), 24 );
    m->assign_badpoints();

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Writing Out Volume"), 28 );
    m->write_volume(reconVisFile, reconIPFVisFile, reconDisVisFile, reconIQVisFile, reconSFVisFile, false, false, false, false);
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Neighbors"), 32 );
  m->find_neighbors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Merging Contained Grains"), 36 );
  m->merge_containedgrains();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Renumbering Grains"), 40 );
  m->numgrains = m->renumber_grains2();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Refinding Neighbors"), 44 );
  m->find_neighbors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Redefining Grains"), 48 );
  m->reburn_grains();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Euclidean Distance Maps"), 52 );
  m->find_euclidean_map();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Centroids"), 56 );
  m->find_centroids();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Moments"), 60 );
  m->find_moments();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Principal Axes Lengths"), 64 );
  m->find_axes();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Pricipal Axes Vectors"), 68 );
  m->find_vectors();

  CHECK_FOR_CANCELED(ReconstructionFunc) 
  progressMessage(AIM_STRING("Finding Grain Reference Orientations"), 72 );
  m->find_kernels(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc) 
  progressMessage(AIM_STRING("Finding Grain Average Orientations"), 76 );
  m->homogenize_grains(quat_symmcubic, quat_symmhex);

  if (m_MergeTwins == true)
  {
    m->merge_twins(quat_symmcubic, quat_symmhex);
    m->characterize_twins();
	CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Renumbering Grains"), 76 );
    m->numgrains = m->renumber_grains3();
  }

  if (m_MergeColonies == true)
  {
    m->merge_colonies(quat_symmcubic, quat_symmhex);
    m->characterize_colonies();
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Euler ODF"), 80 );
  m->find_eulerodf();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Measuring Misorientations"), 84 );
  m->measure_misorientations(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain IPF Colors"), 88 );
  m->find_colors(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Convexities"), 92 );
  m->find_convexities();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Schmid Factors"), 94 );
  m->find_schmids();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Statistics"), 96 );
  m->volume_stats(statsFile,misorientationFile,microBinsFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Out Volume"), 98 );
  m->write_volume(reconVisFile, reconIPFVisFile, reconDisVisFile, reconIQVisFile, reconSFVisFile, m_IPFoutputoption, m_Disorientationoutputoption, m_ImageQualityoutputoption, m_SchmidFactoroutputoption);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Grain Data"), 100 );
  m->write_graindata(graindataFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Axis Orientation File"), 100 );
  m->write_axisodf(axisFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Euler Angle File"), 100 );
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

