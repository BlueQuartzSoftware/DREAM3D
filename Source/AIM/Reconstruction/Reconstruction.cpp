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
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "AIM/Common/Constants.h"
#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/OIMColoring.hpp"
#include "AIM/Common/ReconstructionVTKWriter.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"
#include "AIM/Common/HDF5/H5GrainWriter.h"
#include "AIM/ANG/AngDirectoryPatterns.h"
#include "AIM/ANG/AngReader.h"
#include "AIM/ANG/AbstractAngDataLoader.h"
#include "AIM/ANG/AngDataLoader.h"
#include "AIM/ANG/H5AngDataLoader.h"


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
      return;}\
      if(AIM_RECONSTRUCTION_BENCHMARKS) {\
    std::cout << #name << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;\
    millis = MXA::getMilliSeconds(); }


#else

#define CHECK_FOR_CANCELED(AClass, name)\
  if(AIM_RECONSTRUCTION_BENCHMARKS) {\
    std::cout << #name << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;\
    millis = MXA::getMilliSeconds(); }
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
m_FillinSample(false),
m_MinAllowedGrainSize(0),
m_MinSeedConfidence(0.0),
m_SizeBinStepSize(0.0),
m_MinSeedImageQuality(0.0),
m_MisorientationTolerance(0.0),
m_CrystalStructure(AIM::Reconstruction::Cubic),
m_AlreadyFormed(false),
m_Orientation(Ang::NoOrientation),
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
 // std::cout << "~Reconstruction()" << std::endl;
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
  ptr->setOrientation(m_Orientation);

  // Create our File Output Writer Object. This will handle all the File Output duties
  ReconstructionVTKWriter::Pointer outWriter = ReconstructionVTKWriter::New();
  H5GrainWriter::Pointer h5GrainWriter = H5GrainWriter::New();

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
                m_MisorientationTolerance, m_SizeBinStepSize, m_CrystalStructure, m_AlignmentMethod,
                m_AlreadyFormed);
  m_OutputDirectory = MXADir::toNativeSeparators(m_OutputDirectory);

  // Create a new HDF5 Results file by overwriting any HDF5 file that may be in the way
  std::string hdf5ResultsFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::H5StatisticsFile;
  H5ReconStatsWriter::Pointer h5io = H5ReconStatsWriter::New(hdf5ResultsFile);
  if (h5io.get() == NULL)
  {
    progressMessage("The HDF5 Statistics file could not be created. Does the path exist and do you have write access to the output directory.", 100);
    m = ReconstructionFunc::NullPointer();  // Clean up the memory
    //std::cout << "Reconstruction::compute Complete" << std::endl;
  #if AIM_USE_QT
    emit finished();
  #endif
    return;
  }

  std::string graindataFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::GrainDataFile;
  std::string alignmentFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::AlignmentFile;

  std::string reconVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::VisualizationVizFile;
  std::string reconIPFVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::IPFVizFile;
  std::string reconDisVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::DisorientationVizFile;
  std::string reconIQVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::ImageQualityVizFile;
  std::string reconSFVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::SchmidFactorVizFile;
  std::string reconDSVisFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::DownSampledVizFile;
  std::string reconDeformStatsFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::DeformationStatsFile;

  std::string hdf5GrainFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::HDF5GrainFile;

  START_CLOCK()

  if (m_AlreadyFormed == true)
  {
    // Sanity Check the the Reconstruction File does exist in the output directory
    if (MXADir::exists(reconVisFile) == true)
    {
      progressMessage(AIM_STRING("Loading Existing Data"), 31);
      int numgrains = m->load_data(reconVisFile);
      CHECK_FOR_CANCELED(ReconstructionFunc, load_data)
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
  else /* if (m_AlreadyFormed == false) */
  {

    progressMessage(AIM_STRING("Loading Slices"), 3);
    oimDataLoader->loadData(m->voxels, m->xpoints, m->ypoints, m->zpoints);
    CHECK_FOR_CANCELED(ReconstructionFunc, loadData)

    progressMessage(AIM_STRING("Finding Border"), 6);
    m->find_border();
    CHECK_FOR_CANCELED(ReconstructionFunc, find_border)

    if (m_AlignmentMethod == AIM::Reconstruction::MutualInformation)
    {
      progressMessage(AIM_STRING("Identifying Grains on Sections"), 11);
      m->form_grains_sections();
      CHECK_FOR_CANCELED(ReconstructionFunc, form_grains_sections)
    }

    progressMessage(AIM_STRING("Aligning Slices"), 9);
    m->align_sections(alignmentFile);
    CHECK_FOR_CANCELED(ReconstructionFunc, align_sections)

    progressMessage(AIM_STRING("Cleaning Data"), 12);
    m->cleanup_data();
    CHECK_FOR_CANCELED(ReconstructionFunc, cleanup_data)

    if (m_AlignmentMethod == AIM::Reconstruction::MutualInformation)
    {
      progressMessage(AIM_STRING("Redefining Border"), 16);
      m->find_border();
      CHECK_FOR_CANCELED(ReconstructionFunc, find_border)
    }

    progressMessage(AIM_STRING("Forming Macro-Grains"), 19);
    m->form_grains();
    CHECK_FOR_CANCELED(ReconstructionFunc, form_grains)

    progressMessage(AIM_STRING("Assigning Bad Points"), 28);
    m->assign_badpoints();
    CHECK_FOR_CANCELED(ReconstructionFunc, assign_badpoints)
  }


  progressMessage(AIM_STRING("Finding Neighbors"), 31);
  m->find_neighbors();
  CHECK_FOR_CANCELED(ReconstructionFunc, find_neighbors)

  progressMessage(AIM_STRING("Merging Contained Grains"), 34);
  m->merge_containedgrains();
  CHECK_FOR_CANCELED(ReconstructionFunc, merge_containedgrains)

  progressMessage(AIM_STRING("Reordering Grains"), 37);
  m->reorder_grains();
  CHECK_FOR_CANCELED(ReconstructionFunc, reorder_grains)


  progressMessage(AIM_STRING("Finding Reference Orientations For Grains"), 40);
  m->find_grain_and_kernel_misorientations();
  CHECK_FOR_CANCELED(ReconstructionFunc, find_grain_and_kernel_misorientations)

  if(m_FillinSample == true)
  {
    progressMessage(AIM_STRING("Creating Smooth Rectangular Sample"), 49);
    m->fillin_sample();
    CHECK_FOR_CANCELED(ReconstructionFunc, fillin_sample)
  }

  if (m_MergeTwins == true)
  {
    progressMessage(AIM_STRING("Merging Twins"), 51);
    m->merge_twins();
    CHECK_FOR_CANCELED(ReconstructionFunc, merge_twins)
    
    progressMessage(AIM_STRING("Characterizing Twins"), 52);
    m->characterize_twins();
    CHECK_FOR_CANCELED(ReconstructionFunc, characterize_twins)

    progressMessage(AIM_STRING("Renumbering Grains"), 53);
    m->renumber_grains3();
    CHECK_FOR_CANCELED(ReconstructionFunc, renumber_grains3)
  }

  if (m_MergeColonies == true)
  {
    progressMessage(AIM_STRING("Merging Colonies"), 51);
    m->merge_colonies();
    CHECK_FOR_CANCELED(ReconstructionFunc, merge_colonies)

    progressMessage(AIM_STRING("Renumbering Grains"), 53);
    m->characterize_colonies();
    CHECK_FOR_CANCELED(ReconstructionFunc, characterize_colonies)
  }

  progressMessage(AIM_STRING("Finding Grain Centroids"), 55);
  if(m_ZEndIndex-m_ZStartIndex > 1) m->find_centroids();
  if(m_ZEndIndex-m_ZStartIndex == 1)m->find_centroids2D();
  CHECK_FOR_CANCELED(ReconstructionFunc, find_centroids2D)

  progressMessage(AIM_STRING("Finding Grain Moments"), 57);
  if(m_ZEndIndex-m_ZStartIndex > 1) m->find_moments();
  if(m_ZEndIndex-m_ZStartIndex == 1) m->find_moments2D();
  CHECK_FOR_CANCELED(ReconstructionFunc, find_moments2D)

  progressMessage(AIM_STRING("Finding Grain Principal Axes Lengths"), 59);
  if(m_ZEndIndex-m_ZStartIndex > 1) m->find_axes();
  if(m_ZEndIndex-m_ZStartIndex == 1) m->find_axes2D();
  CHECK_FOR_CANCELED(ReconstructionFunc, find_axes2D)

  progressMessage(AIM_STRING("Finding Grain Pricipal Axes Vectors"), 61);
  if(m_ZEndIndex-m_ZStartIndex > 1) m->find_vectors(h5io);
  if(m_ZEndIndex-m_ZStartIndex == 1) m->find_vectors2D(h5io);
  CHECK_FOR_CANCELED(ReconstructionFunc, find_vectors2D)

  progressMessage(AIM_STRING("Defining Neighborhoods"), 63);
  m->define_neighborhood();
  CHECK_FOR_CANCELED(ReconstructionFunc, find_neighbors)


  progressMessage(AIM_STRING("Finding Euclidean Distance Maps"), 65);
  m->find_euclidean_map();
  CHECK_FOR_CANCELED(ReconstructionFunc, find_euclidean_map)

  progressMessage(AIM_STRING("Finding Euler ODF"), 66);
  m->find_eulerodf(h5io);
  CHECK_FOR_CANCELED(ReconstructionFunc, find_eulerodf)

  progressMessage(AIM_STRING("Measuring Misorientations"), 67);
  m->measure_misorientations(h5io);
  CHECK_FOR_CANCELED(ReconstructionFunc, measure_misorientations)

  progressMessage(AIM_STRING("Finding Grain IPF Colors"), 69);
  m->find_colors();
  CHECK_FOR_CANCELED(ReconstructionFunc, find_colors)

  progressMessage(AIM_STRING("Finding Grain Schmid Factors"), 71);
  m->find_schmids();
  CHECK_FOR_CANCELED(ReconstructionFunc, find_schmids)

  progressMessage(AIM_STRING("Writing Statistics"), 80);
  if(m_ZEndIndex-m_ZStartIndex > 1) { m->volume_stats(h5io); }
  if(m_ZEndIndex-m_ZStartIndex == 1) { m->volume_stats2D(h5io); }
  m->deformation_stats(reconDeformStatsFile);
  CHECK_FOR_CANCELED(ReconstructionFunc, volume_stats)

  progressMessage(AIM_STRING("Writing Grain Data"), 82);
  m->write_graindata(graindataFile);

  /** ********** This section writes the ASCII based vtk files for visualization *** */

  progressMessage(AIM_STRING("Writing VTK Visualization File"), 88);
  if (m_WriteVisualizationFile) {outWriter->writeVisualizationFile(m.get(), reconVisFile);}

  progressMessage(AIM_STRING("Writing VTK Inverse Pole Figure File"), 89);
  if (m_WriteIPFFile) {outWriter->writeIPFVizFile(m.get(), reconIPFVisFile);}

  progressMessage(AIM_STRING("Writing VTK Disorientation File"), 90);
  if (m_WriteDisorientationFile) {outWriter->writeDisorientationFile(m.get(), reconDisVisFile);}

  progressMessage(AIM_STRING("Writing VTK Image Quality File"), 91);
  if (m_WriteImageQualityFile) {outWriter->writeImageQualityVizFile(m.get(), reconIQVisFile);}

  progressMessage(AIM_STRING("Writing VTK Schmid Factor File"), 92);
  if (m_WriteSchmidFactorFile) {outWriter->writeSchmidFactorVizFile(m.get(), reconSFVisFile);}

  progressMessage(AIM_STRING("Writing VTK Down Sampled File"), 93);
  if (m_WriteDownSampledFile) {outWriter->writeDownSampledVizFile(m.get(), reconDSVisFile);}




  /** ******* End VTK Visualization File Writing Section ****** */
  CHECK_FOR_CANCELED(ReconstructionFunc, vtk_viz_files)
  progressMessage(AIM_STRING("Writing Out HDF5 Grain File. This may take a few minutes to complete."), 95);
  if (m_WriteHDF5GrainFile) { h5GrainWriter->writeHDF5GrainsFile(m.get(), hdf5GrainFile); }
  CHECK_FOR_CANCELED(ReconstructionFunc, writeHDF5GrainsFile)


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

#define PRINT_PROPERTY( out, var)\
  out << #var << ": " << m_##var << std::endl;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::printSettings(std::ostream &ostream)
{
  ostream << "Reconstruction Settings Being Used" << std::endl;
    PRINT_PROPERTY(ostream, H5AngFile)
    PRINT_PROPERTY(ostream, ZStartIndex)
    PRINT_PROPERTY(ostream, ZEndIndex)
    PRINT_PROPERTY(ostream, OutputDirectory)
    PRINT_PROPERTY(ostream, MergeTwins)
    PRINT_PROPERTY(ostream, MergeColonies)
    PRINT_PROPERTY(ostream, FillinSample)
    PRINT_PROPERTY(ostream, MinAllowedGrainSize)
    PRINT_PROPERTY(ostream, MinSeedConfidence)
    PRINT_PROPERTY(ostream, SizeBinStepSize)
    PRINT_PROPERTY(ostream, DownSampleFactor)
    PRINT_PROPERTY(ostream, MinSeedImageQuality)
    PRINT_PROPERTY(ostream, MisorientationTolerance)
    PRINT_PROPERTY(ostream, CrystalStructure)
    PRINT_PROPERTY(ostream, AlignmentMethod)
    PRINT_PROPERTY(ostream, AlreadyFormed)
    PRINT_PROPERTY(ostream, Orientation)

    PRINT_PROPERTY(ostream, WriteVisualizationFile)
    PRINT_PROPERTY(ostream, WriteIPFFile)
    PRINT_PROPERTY(ostream, WriteDisorientationFile)
    PRINT_PROPERTY(ostream, WriteImageQualityFile)
    PRINT_PROPERTY(ostream, WriteSchmidFactorFile)
    PRINT_PROPERTY(ostream, WriteDownSampledFile)
    PRINT_PROPERTY(ostream, WriteHDF5GrainFile)
}