///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#include "Reconstruction.h"
#include <AIM/Common/MicroGen3D.h>
#include <AIM/ANG/AngDirectoryPatterns.h>
#include <AIM/ANG/AngFileReader.h>
#include <AIM/ANG/AngFileHelper.h>
#include <MXA/Utilities/MXAFileSystemPath.h>

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
QObject(parent),
m_Cancel(false),
  m_InputDirectory("."), m_OutputDirectory("."), m_AngFilePrefix("Slice_"), m_AngSeriesMaxSlice(3), m_ZIndexStart(0), m_ZIndexEnd(0), m_ZResolution(0.25),
  m_MergeTwins(false), m_MinAllowedGrainSize(0.0), m_MinSeedConfidence(0.0), m_MisorientationTolerance(0.0), m_CrystalStructure(AIM::Reconstruction::HCP),
  m_AlreadyFormed(false), m_ErrorCondition(0)
{

}



#else
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::Reconstruction() :
  m_InputDirectory("."), m_OutputDirectory("."), m_AngFilePrefix("Slice_"), m_AngSeriesMaxSlice(3), m_ZIndexStart(0), m_ZIndexEnd(0), m_ZResolution(0.25),
      m_MergeTwins(false), m_MinAllowedGrainSize(0.0), m_MinSeedConfidence(0.0), m_MisorientationTolerance(0.0), m_CrystalStructure(AIM::Reconstruction::HCP),
      m_AlreadyFormed(false)
{

}

#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::~Reconstruction()
{
  // TODO Auto-generated destructor stub
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::parseAngFile()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Reconstruction::compute()
{
  int32 err = 0;
  int32 sliceCount = 1;
  int32 width = 0;
  int32 totalSlices = m_AngSeriesMaxSlice;
  while (sliceCount < totalSlices)
  {
    ++width;
    sliceCount *= 10;
  }

  AngDirectoryPatterns::Pointer p = AngDirectoryPatterns::New(m_InputDirectory, m_AngFilePrefix, width);

  AngFileReader::Pointer reader = AngFileReader::New();
  reader->readFile(p->generateFullPathAngFileName(m_ZIndexStart));

  double xSize = reader->getXStep() * reader->getNumEvenCols();
  std::cout << "X Size: " << xSize << std::endl;

  double ySize = reader->getYStep() * reader->getNumRows();
  std::cout << "Y Size: " << ySize << std::endl;

  size_t zPoints = m_ZIndexEnd - m_ZIndexStart;
  double zSize = zPoints * m_ZResolution;

  std::cout << "Z Size: " << zSize << std::endl;
  std::cout << "X Res: " << reader->getXStep() << std::endl;
  std::cout << "Y Res: " << reader->getYStep() << std::endl;
  std::cout << "Z Res: " << m_ZResolution << std::endl;
  std::cout << "X Points: " << reader->getNumEvenCols() + 3 << std::endl;
  std::cout << "Y Points: " << reader->getNumRows() + 3 << std::endl;
  std::cout << "Z Points: " << zPoints + 3 << std::endl;

  AngFileHelper::Pointer angFileHelper = AngFileHelper::New();
  angFileHelper->setZIndexStart(m_ZIndexStart);
  angFileHelper->setZIndexEnd(m_ZIndexEnd);
  angFileHelper->setDirectoryPattern(p);

  MicroGen3D::Pointer microgen = MicroGen3D::New();
  microgen->initialize(xSize, ySize, zSize,
                       reader->getXStep(), reader->getYStep(), m_ZResolution,
                       reader->getNumEvenCols() + 2, reader->getNumRows() + 2, zPoints + 2,
                       m_MergeTwins, m_MinAllowedGrainSize, m_MinSeedConfidence,
                       m_MisorientationTolerance, m_CrystalStructure, m_AlreadyFormed);
  microgen->m_angFileHelper = angFileHelper;

  int32 numgrains = 0;

  std::string reconFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::ReconstructedDataFile;
  if (m_AlreadyFormed == false)
  {
    microgen->loadSlices();
    numgrains = microgen->form_grains();
    microgen->remove_smallgrains();
    numgrains = microgen->renumber_grains1();
    microgen->write_volume1(reconFile);
  }

  if (m_AlreadyFormed == true)
  {
    // Sanity Check the the Reconstruction File does exist in the output directory
    if (MXAFileSystemPath::exists(reconFile) == true)
    {
      microgen->load_data(reconFile);
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

  for (int iter1 = 0; iter1 < 180; iter1++)
  {
    for (int iter2 = 0; iter2 < 180; iter2++)
    {
      for (int iter3 = 0; iter3 < 180; iter3++)
      {
        microgen->eulerrank[iter1][iter2][iter3] = 0;
      }
    }
  }
  microgen->homogenize_grains();
  microgen->assign_badpoints();
  microgen->find_neighbors();
  microgen->merge_containedgrains();
  numgrains = microgen->renumber_grains2();
  microgen->write_volume1(reconFile);
  if (m_MergeTwins == 1)
  {
    microgen->merge_twins();
    microgen->characterize_twins();
  }
  numgrains = microgen->renumber_grains3();
  microgen->write_volume2(reconFile);
  microgen->find_goodneighbors();
  microgen->find_centroids();
  microgen->find_moments();
  microgen->find_axes();
  microgen->find_vectors();
  microgen->measure_misorientations();
  microgen->find_colors();
  microgen->find_convexities();

  std::string  statsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::StatsFile;
  std::string  volBinFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::VolBinFile;
  std::string  bOverABinsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::BOverABinsFile;
  std::string  cOverABinsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::COverABinsFile;
  std::string  cOverBBinsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::COverBBinsFile;
  std::string  svnFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::SVNFile;
  std::string  svsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::SVSFile;
  std::string  misorientationFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::MisorientationBinsFile;
  std::string  microBinsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::MicroBinsFile;


  microgen->volume_stats(statsFile,
                         volBinFile,
                         bOverABinsFile,
                         cOverABinsFile,
                         cOverBBinsFile,
                         svnFile,
                         svsFile,
                         misorientationFile,
                         microBinsFile);
  microgen->write_volume2(reconFile);

  std::string reconVisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::ReconstructedVisualizationFile;
  std::string grainsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::GrainsFile;

  std::string axisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::AxisOrientationsFile;
  std::string eulerFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::EulerAnglesFile;
  std::string boundaryFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::BoundaryCentersFile;


  microgen->create_visualization(reconVisFile);
  microgen->write_grains(grainsFile);
  microgen->write_axisorientations(axisFile);
  microgen->write_eulerangles(eulerFile);
  microgen->find_boundarycenters(boundaryFile);
#ifdef AIM_USE_QT
  QString msg = QString("Reconstruction Ending");
  emit workerHasMessage(msg);
  emit workerComplete();
#endif
}
