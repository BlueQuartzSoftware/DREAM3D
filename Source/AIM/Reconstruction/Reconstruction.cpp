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

#include "MXA/MXATypes.h"
#include "MXA/Utilities/MXADir.h"

#include "AIM/Common/Constants.h"
#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/ANG/AngDirectoryPatterns.h"
#include "AIM/ANG/AngReader.h"
#include "AIM/ANG/AbstractAngDataLoader.h"
#include "AIM/ANG/AngDataLoader.h"
#include "AIM/Common/OIMColoring.hpp"


#include "AIM/ANG/H5AngDataLoader.h"

#include "AIM/Common/HDF5/H5ReconStatsWriter.h"



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
#if AIM_USE_QT
Reconstruction::Reconstruction(QObject* parent) :
QObject(parent),
#else
Reconstruction::Reconstruction() :
#endif
m_H5AngFile(""),
m_OutputDirectory("."),
m_MergeTwins(false),
m_MergeColonies(false),
m_MinAllowedGrainSize(0),
m_MinSeedConfidence(0.0),
m_MinSeedImageQuality(0.0),
m_MisorientationTolerance(0.0),
m_CrystalStructure(AIM::Reconstruction::Cubic),
m_AlreadyFormed(false),
m_WriteVisualizationFile(false),
m_WriteIPFFile(false),
m_WriteDisorientationFile(false),
m_WriteImageQualityFile(false),
m_WriteSchmidFactorFile(false),
m_WriteDownSampledFile(false),
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
Reconstruction::~Reconstruction()
{
  std::cout << "~Reconstruction()" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::compute()
{
  //std::cout << "Reconstruction::compute Start" << std::endl;
  int err = -1;

  AbstractAngDataLoader::Pointer oimDataLoader = H5AngDataLoader::New();
  H5AngDataLoader* ptr = dynamic_cast<H5AngDataLoader*>(oimDataLoader.get());
  ptr->setFilename(m_H5AngFile);
  ptr->setZStartIndex(m_ZStartIndex);
  ptr->setZEndIndex(m_ZEndIndex);


  m = ReconstructionFunc::New();
  progressMessage(AIM_STRING("Gathering Size and Resolution Information from OIM Data"), 1);
  err = oimDataLoader->getSizeAndResolution(m->xpoints, m->ypoints, m->zpoints, m->resx, m->resy, m->resz);
  if (err < 0)
  {
     std::cout << "ReconstructionFunc Error: Problem loading data size and resolutions" << std::endl;
     return;
  }
  m->initialize(m->xpoints, m->ypoints, m->zpoints,
                m->resx, m->resy, m->resz,
                m_MergeTwins, m_MergeColonies, m_MinAllowedGrainSize,
	              m_MinSeedConfidence, m_DownSampleFactor, m_MinSeedImageQuality,
	              m_MisorientationTolerance, m_CrystalStructure, m_AlignmentMethod,
	              m_AlreadyFormed);
  m_OutputDirectory = MXADir::toNativeSeparators(m_OutputDirectory);

  // Create a new HDF5 Results file by overwriting any HDF5 file that may be in the way
  std::string hdf5ResultsFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::H5StatisticsFile;
  H5ReconStatsWriter::Pointer h5io = H5ReconStatsWriter::New(hdf5ResultsFile);
  std::string graindataFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::GrainDataFile;
  std::string alignmentFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::AlignmentFile;

  std::string reconVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::VisualizationVizFile;
  std::string reconIPFVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::IPFVizFile;
  std::string reconDisVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::DisorientationVizFile;
  std::string reconIQVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::ImageQualityVizFile;
  std::string reconSFVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::SchmidFactorVizFile;
  std::string reconDSVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::DownSampledVizFile;

  std::string hdf5GrainFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::HDF5GrainFile;

  if (m_AlreadyFormed == true)
  {
    // Sanity Check the the Reconstruction File does exist in the output directory
    if (MXADir::exists(reconVisFile) == true)
    {
      CHECK_FOR_CANCELED(ReconstructionFunc)
      progressMessage(AIM_STRING("Loading Existing Data"), 31);
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
    progressMessage(AIM_STRING("Loading Slices"), 3);
    oimDataLoader->loadData(m->voxels, m->xpoints, m->ypoints, m->zpoints);


    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Finding Border"), 6);
    m->find_border();

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Cleaning Data"), 9);
    m->cleanup_data();

    if (m_AlignmentMethod == AIM::Reconstruction::MutualInformation)
    {
      CHECK_FOR_CANCELED(ReconstructionFunc)
      progressMessage(AIM_STRING("Identifying Grains on Sections"), 11);
      m->form_grains_sections();
    }

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Aligning Slices"), 14);
    m->align_sections(alignmentFile);

    if (m_AlignmentMethod == AIM::Reconstruction::MutualInformation)
    {
      CHECK_FOR_CANCELED(ReconstructionFunc)
      progressMessage(AIM_STRING("Redefining Border"), 16);
      m->find_border();
    }

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Forming Macro-Grains"), 19);
    m->numgrains = m->form_grains();

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Assigning Bad Points"), 28);
    m->assign_badpoints();
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Neighbors"), 31);
  m->find_neighbors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Merging Contained Grains"), 34);
  m->merge_containedgrains();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Renumbering Grains"), 37);
  m->numgrains = m->renumber_grains();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Updating Reference Orientations For Grains"), 40);
  m->find_kernels();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Updating Voxel Lists For Grains"), 43);
  m->numgrains = m->reburn_grains();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Average Orientations For Grains"), 49);
  m->homogenize_grains();

  if (m_MergeTwins == true)
  {
    m->merge_twins();
    progressMessage(AIM_STRING("Merging Twins"), 51);
    m->characterize_twins();
	  CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Renumbering Grains"), 53);
    m->numgrains = m->renumber_grains3();
  }

  if (m_MergeColonies == true)
  {
    progressMessage(AIM_STRING("Merging Colonies"), 51);
    m->merge_colonies();
    progressMessage(AIM_STRING("Renumbering Grains"), 53);
    m->characterize_colonies();
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Centroids"), 55);
  if(m_ZEndIndex-m_ZStartIndex > 1) m->find_centroids();
  if(m_ZEndIndex-m_ZStartIndex == 1)m->find_centroids2D();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Moments"), 57);
  if(m_ZEndIndex-m_ZStartIndex > 1) m->find_moments();
  if(m_ZEndIndex-m_ZStartIndex == 1) m->find_moments2D();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Principal Axes Lengths"), 59);
  if(m_ZEndIndex-m_ZStartIndex > 1) m->find_axes();
  if(m_ZEndIndex-m_ZStartIndex == 1) m->find_axes2D();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Pricipal Axes Vectors"), 61);
  if(m_ZEndIndex-m_ZStartIndex > 1) m->find_vectors(h5io);
  if(m_ZEndIndex-m_ZStartIndex == 1) m->find_vectors2D(h5io);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Refinding Neighbors"), 63);
  m->find_neighbors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Euclidean Distance Maps"), 65);
  m->find_euclidean_map();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Euler ODF"), 66);
  m->find_eulerodf(h5io);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Measuring Misorientations"), 67);
  m->measure_misorientations(h5io);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain IPF Colors"), 69);
  m->find_colors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Convexities"), 70);
  m->find_convexities();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Schmid Factors"), 71);
  m->find_schmids();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Statistics"), 80);
  if(m_ZEndIndex-m_ZStartIndex > 1) m->volume_stats(h5io);
  if(m_ZEndIndex-m_ZStartIndex == 1) m->volume_stats2D(h5io);


  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Grain Data"), 82);
  m->write_graindata(graindataFile);

  /** ********** This section writes the ASCII based vtk files for visualization *** */

  progressMessage(AIM_STRING("Writing VTK Visualization File"), 88);
  if (m_WriteVisualizationFile) {m->writeVisualizationFile(reconVisFile);}

  progressMessage(AIM_STRING("Writing VTK Inverse Pole Figure File"), 89);
  if (m_WriteIPFFile) {m->writeIPFVizFile(reconIPFVisFile);}

  progressMessage(AIM_STRING("Writing VTK Disorientation File"), 90);
  if (m_WriteDisorientationFile) {m->writeDisorientationVizFile(reconDisVisFile);}

  progressMessage(AIM_STRING("Writing VTK Image Quality File"), 91);
  if (m_WriteImageQualityFile) {m->writeImageQualityVizFile(reconIQVisFile);}

  progressMessage(AIM_STRING("Writing VTK Schmid Factor File"), 92);
  if (m_WriteSchmidFactorFile) {m->writeSchmidFactorVizFile(reconSFVisFile);}

  progressMessage(AIM_STRING("Writing VTK Down Sampled File"), 93);
  if (m_WriteDownSampledFile) {m->writeDownSampledVizFile(reconDSVisFile);}

  /** ******* End VTK Visualization File Writing Section ****** */

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Out HDF5 Grain File. This may take a few minutes to complete."), 95);
  if (m_WriteHDF5GrainFile) { m->writeHDF5GrainsFile(hdf5GrainFile); }

  progressMessage(AIM_STRING("Reconstruction Complete"), 100);

  // Clean up all the memory by forcibly setting a NULL pointer to the Shared
  // pointer object.
  m = ReconstructionFunc::NullPointer();  // Clean up the memory
  //std::cout << "Reconstruction::compute Complete" << std::endl;
#if AIM_USE_QT
  emit finished();
#endif
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
  std::cout << progress << "% " << message << std::endl;
#endif
}

#ifdef AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::on_CancelWorker()
{
//  std::cout << "Reconstruction::cancelWorker()" << std::endl;
  this->m_Cancel = true;
}
#endif

