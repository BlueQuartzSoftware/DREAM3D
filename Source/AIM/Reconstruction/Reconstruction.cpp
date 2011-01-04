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
#include "AIM/HDF5/AIM_H5VtkDataWriter.h"
#include "AIM/Reconstruction/H5ReconStatsWriter.h"



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
QThread(parent),
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

    if (m_AlignmentMethod == 3)
    {
      CHECK_FOR_CANCELED(ReconstructionFunc)
      progressMessage(AIM_STRING("Identifying Grains on Sections"), 11);
      m->form_grains_sections();
    }

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Aligning Slices"), 14);
    m->align_sections(alignmentFile);

    if (m_AlignmentMethod == 3)
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
  if(m_ZEndIndex-m_ZStartIndex > 1) m->find_vectors();
  if(m_ZEndIndex-m_ZStartIndex == 1) m->find_vectors2D();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Refinding Neighbors"), 63);
  m->find_neighbors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Euclidean Distance Maps"), 65);
  m->find_euclidean_map();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Euler ODF"), 66);
  m->find_eulerodf();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Measuring Misorientations"), 67);
  m->measure_misorientations();

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

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Axis Orientation File"), 84);
  err = h5io->writeAxisOrientationData(m->axisodf, m->crystruct, m->totalaxes);


  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Euler Angle File"), 86);
  err = h5io->writeODFData(m->crystruct, m->eulerodf, m->totalvol);

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
  if (m_WriteHDF5GrainFile) { writeHDF5GrainsFile(hdf5GrainFile, m); }

  progressMessage(AIM_STRING("Reconstruction Complete"), 100);

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
 // std::cout << "Reconstruction::cancelWorker()" << std::endl;
  this->m_Cancel = true;
  if (m.get() != NULL)
  {

  }
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Reconstruction::writeHDF5GrainsFile(const std::string &hdfFile,
                                          ReconstructionFunc::Pointer r)
{
  int err = -1;
  AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();
  h5writer->setFileName(hdfFile);
  err = h5writer->openFile(false); // Open a new file over writing any other file

  std::stringstream ss;
  std::string hdfPath;
  std::vector<std::string > hdfPaths;
 // std::cout << "Writing out " << r->numgrains << " to an HDF5 Grain File..." << std::endl;
  for (int i = 1; i < r->numgrains; i++)
  {
 //   std::cout << " Grain: " << i << " Gathering Data" << std::endl;
    vector<int >* vlist = r->m_Grains[i].voxellist;
    int vid = vlist->at(0);
    ss.str("");
    ss << "/" << i;
    hdfPath = ss.str();
    hdfPaths.push_back(hdfPath);

    vector<int > plist(((r->xpoints + 1) * (r->ypoints + 1) * (r->zpoints + 1)), 0);
    int pcount = 0;
	  double q1[5];
	  unsigned char rgb[3] = {0, 0, 0};
	  double RefDirection[3] = {0.0, 0.0, 1.0};
    int ocol, orow, oplane;
    int col, row, plane;
    int pid;
    int err = 0;
    // outFile << "POINTS " << pcount << " float" << endl;
    std::vector<float > points;
    std::vector<int32_t > cells;
    std::vector<int32_t > cell_types(vlist->size(), VTK_CELLTYPE_VOXEL);

    std::vector<float > kernelAvgDisorientation(vlist->size());
    std::vector<float > grainAvgDisorientation(vlist->size());
    std::vector<float > imageQuality(vlist->size());
    std::vector<unsigned char > ipfColor(vlist->size() * 3);
    std::vector<float > schmidFactor(1);

    std::vector<int32_t > grainName(1);

    pcount = 0;
    plist.clear();
    plist.resize(((r->xpoints + 1) * (r->ypoints + 1) * (r->zpoints + 1)), 0);
    for (std::vector<int >::size_type j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      ocol = vid % r->xpoints;
      orow = (vid / r->xpoints) % r->ypoints;
      oplane = vid / (r->xpoints * r->ypoints);
      cells.push_back(8);
      for (int k = 0; k < 8; k++)
      {
        if (k == 0) col = ocol, row = orow, plane = oplane;
        if (k == 1) col = ocol + 1, row = orow, plane = oplane;
        if (k == 2) col = ocol, row = orow + 1, plane = oplane;
        if (k == 3) col = ocol + 1, row = orow + 1, plane = oplane;
        if (k == 4) col = ocol, row = orow, plane = oplane + 1;
        if (k == 5) col = ocol + 1, row = orow, plane = oplane + 1;
        if (k == 6) col = ocol, row = orow + 1, plane = oplane + 1;
        if (k == 7) col = ocol + 1, row = orow + 1, plane = oplane + 1;
        pid = (plane * (r->xpoints + 1) * (r->ypoints + 1)) + (row * (r->xpoints + 1)) + col;
        if (plist[pid] == 0)
        {
          plist[pid] = pcount;
          pcount++;
          //     outFile << (col * r->resx) << "  " << (row * r->resy) << "  " << (plane * r->resz) << endl;
          points.push_back((col * r->resx));
          points.push_back((row * r->resy));
          points.push_back((plane * r->resz));
        }
        // Add onto our cells vector
        cells.push_back(plist[pid]);
      }
      // Append a grainId to the grainIds vector
      kernelAvgDisorientation[j] = r->voxels[vid].kernelmisorientation;
      grainAvgDisorientation[j] = r->voxels[vid].misorientation;
      imageQuality[j] = r->voxels[vid].imagequality;
	  if(m_CrystalStructure == AIM::Reconstruction::Cubic)
      {
        OIMColoring::GenerateIPFColor(r->voxels[vid].euler1, r->voxels[vid].euler2, r->voxels[vid].euler3,
                                      RefDirection[0], RefDirection[1], RefDirection[2],
                                      rgb);
      }
      if(m_CrystalStructure == AIM::Reconstruction::Hexagonal)
      {
        q1[0]=r->voxels[i].quat[1];
        q1[1]=r->voxels[i].quat[2];
        q1[2]=r->voxels[i].quat[3];
        q1[3]=r->voxels[i].quat[4];
        OIMColoring::CalculateHexIPFColor(q1, rgb);
      }
      ipfColor[j * 3] = rgb[0];
      ipfColor[j * 3 + 1] = rgb[1];
      ipfColor[j * 3 + 2] = rgb[2];
      grainName[0] = r->voxels[vid].grainname;
    }
 //   std::cout << " Grain: " << i << " Writing HDF5 File" << std::endl;
    err = h5writer->writeUnstructuredGrid(hdfPath, points, cells, cell_types);
    points.resize(0);
    cells.resize(0);
    cell_types.resize(0);

    //Write the Field Data
    err = h5writer->writeFieldData<int > (hdfPath, grainName, AIM::Representation::Grain_ID.c_str(), 1);

    schmidFactor[0] = r->m_Grains[i].schmidfactor;
    err = h5writer->writeFieldData<float > (hdfPath, schmidFactor, AIM::Representation::SchmidFactor.c_str(), 1);

    // Write the Neighbor list
    err = h5writer->writeFieldData<int > (hdfPath, *(r->m_Grains[i].neighborlist), AIM::Representation::Neighbor_Grain_ID_List.c_str(), 1);

    // Write CELL_DATA
    err = h5writer->writeCellData<float > (hdfPath, kernelAvgDisorientation, AIM::Representation::KernelAvgDisorientation.c_str(), 1);
    err = h5writer->writeCellData<float > (hdfPath, grainAvgDisorientation, AIM::Representation::GrainAvgDisorientation.c_str(), 1);
    err = h5writer->writeCellData<float > (hdfPath, imageQuality, AIM::Representation::ImageQuality.c_str(), 1);
    err = h5writer->writeCellData<unsigned char > (hdfPath, ipfColor, AIM::Representation::IPFColor.c_str(), 3);

  }

  err = h5writer->writeObjectIndex(hdfPaths);
  err = h5writer->closeFile();
  return err;
}

