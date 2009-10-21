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
#include <AIM/Common/Constants.h>
#include <MXA/Utilities/MXAFileSystemPath.h>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::Reconstruction()
{
  // TODO Auto-generated constructor stub

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
  AngDirectoryPatterns::Pointer p = AngDirectoryPatterns::New();
  p->setParentDirectory(m_InputDirectory);
  p->parseDocumentName(m_AngFilePrefix, "Slice_050.ang");

  std::stringstream ss;
  ss << m_InputDirectory << MXAFileSystemPath::Separator << p->generateAngFileName(m_ZIndexStart, 3);

  AngFileReader::Pointer reader = AngFileReader::New();
  reader->readFile(ss.str());

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
                       reader->getNumEvenCols() + 3, reader->getNumRows() + 3, zPoints + 3,
                       m_MergeTwins, m_MinAllowedGrainSize, m_MinSeedConfidence,
                       m_MisorientationTolerance, m_CrystalStructure, m_AlreadyFormed);
  microgen->_angFileHelper = angFileHelper;

  int32 numgrains = 0;
  if(m_AlreadyFormed == false)
  {
    microgen->loadSlices();
    numgrains = microgen->form_grains();
    microgen->remove_smallgrains();
    numgrains = microgen->renumber_grains1();
    microgen->write_volume1(AIM::Reconstruction::ReconstructedDataFile);
  }
#if 0
    if(alreadyformed == true)
    {
      microgen->load_data(AIM::Reconstruction::InputFile);
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
    if(mergetwinsoption == 1)
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
#endif
}
