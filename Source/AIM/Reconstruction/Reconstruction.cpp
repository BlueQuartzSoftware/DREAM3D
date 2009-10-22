/*
 * Reconstruction.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#include "Reconstruction.h"
#include <AIM/Common/MicroGen3D.h>
#include <AIM/ANG/AngDirectoryPatterns.h>
#include <AIM/ANG/AngFileReader.h>
#include <AIM/ANG/AngFileHelper.h>
#include <MXA/Utilities/MXAFileSystemPath.h>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::Reconstruction() :
m_InputDirectory("."),
m_OutputDirectory("."),
m_AngFilePrefix("Slice_"),
m_AngSeriesMaxSlice(3),
m_ZIndexStart(0),
m_ZIndexEnd(0),
m_ZResolution(0.25),
m_MergeTwins(false),
m_MinAllowedGrainSize(0.0),
m_MinSeedConfidence(0.0),
m_MisorientationTolerance(0.0),
m_CrystalStructure(AIM::Reconstruction::HCP),
m_AlreadyFormed(false)
{

}

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
int32 Reconstruction::compute()
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

  MicroGen3D* microgen = MicroGen3D::New();
  microgen->initialize(xSize, ySize, zSize,
                       reader->getXStep(), reader->getYStep(), m_ZResolution,
                       reader->getNumEvenCols() + 2, reader->getNumRows() + 2, zPoints + 2,
                       m_MergeTwins, m_MinAllowedGrainSize, m_MinSeedConfidence,
                       m_MisorientationTolerance, m_CrystalStructure, m_AlreadyFormed);
  microgen->m_angFileHelper = angFileHelper;

  int32 numgrains = 0;
  if(m_AlreadyFormed == false)
  {
    microgen->loadSlices();
    numgrains = microgen->form_grains();
    microgen->remove_smallgrains();
    numgrains = microgen->renumber_grains1();
    std::string outFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::ReconstructedDataFile;
    microgen->write_volume1(outFile);
  }

    if(m_AlreadyFormed == true)
    {
	sanity check that this has the same name as the above outfile exits
std::string infile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Reconstruction::ReconstructedDataFile;

      microgen->load_data(infile );
    }
    for(int iter1=0;iter1<180;iter1++)
    {
      for(int iter2=0;iter2<180;iter2++)
      {
        for(int iter3=0;iter3<180;iter3++)
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
    microgen->write_volume1(AIM::Reconstruction::ReconstructedDataFile);
    if(m_MergeTwins == 1)
    {
      microgen->merge_twins();
      microgen->characterize_twins();
    }
    numgrains = microgen->renumber_grains3();
    microgen->write_volume2(AIM::Reconstruction::ReconstructedDataFile);
    microgen->find_goodneighbors();
    microgen->find_centroids();
    microgen->find_moments();
    microgen->find_axes();
    microgen->find_vectors();
    microgen->measure_misorientations();
    microgen->find_colors();
    microgen->find_convexities();
    microgen->volume_stats(AIM::Reconstruction::StatsFile, AIM::Reconstruction::VolBinFile, AIM::Reconstruction::BOverABinsFile,
                          AIM::Reconstruction::COverABinsFile, AIM::Reconstruction::COverBBinsFile, AIM::Reconstruction::SVNFile, AIM::Reconstruction::SVSFile,
                          AIM::Reconstruction::MisorientationBinsFile, AIM::Reconstruction::MicroBinsFile);
    microgen->write_volume2(AIM::Reconstruction::ReconstructedDataFile);
    microgen->create_visualization(AIM::Reconstruction::ReconstructedVisualizationFile);
    microgen->write_grains(AIM::Reconstruction::GrainsFile);
    microgen->write_axisorientations(AIM::Reconstruction::AxisOrientationsFile);
    microgen->write_eulerangles(AIM::Reconstruction::EulerAnglesFile);
    microgen->find_boundarycenters(AIM::Reconstruction::BoundaryCentersFile);
    delete microgen;


    return err;
}
