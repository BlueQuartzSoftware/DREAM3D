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

#include "AIM/ANG/AngDirectoryPatterns.h"
#include "AIM/ANG/AngReader.h"
#include "AIM/ANG/AngFileHelper.h"

#if MXA_HDF5_SUPPORT
#include "AIM/HDF5/AIMH5DataWriter.h"
#endif



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
  int32_t sliceCount = 1;
  int32_t width = 0;
  int32_t totalSlices = m_AngSeriesMaxSlice;
  while (sliceCount < totalSlices)
  {
    ++width;
    sliceCount *= 10;
  }

  m_InputDirectory = MXADir::toNativeSeparators(m_InputDirectory);
  m_OutputDirectory = MXADir::toNativeSeparators(m_OutputDirectory);
  AngDirectoryPatterns::Pointer p = AngDirectoryPatterns::New(m_InputDirectory, m_AngFilePrefix, width);

  AngFileHelper::Pointer angFileHelper = AngFileHelper::New();
  angFileHelper->setZIndexStart(m_ZStartIndex);
  angFileHelper->setZIndexEnd(m_ZEndIndex);
  angFileHelper->setDirectoryPattern(p);

  m = ReconstructionFunc::New();
  m->m_angFileHelper = angFileHelper;
  m->setDirectoryPattern(p);
  m->initialize(m_ZStartIndex, m_ZEndIndex, m_ZResolution, m_MergeTwins, m_MergeColonies, m_MinAllowedGrainSize,
	                   m_MinSeedConfidence, m_DownSampleFactor, m_MinSeedImageQuality, m_MisorientationTolerance, m_CrystalStructure, m_AlignmentMethod, m_AlreadyFormed);

//  int32_t mindiameter = 100000;
//  int32_t maxdiameter = 0;
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

  std::string  statsFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::StatsFile;
  std::string  misorientationFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::MisorientationBinsFile;
  std::string  microBinsFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::MicroBinsFile;
  std::string reconVisFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::ReconstructedVisualizationFile;
  std::string reconIPFVisFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::ReconstructedIPFVisualizationFile;
  std::string reconDisVisFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::ReconstructedDisVisualizationFile;
  std::string reconIQVisFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::ReconstructedIQVisualizationFile;
  std::string reconSFVisFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::ReconstructedSFVisualizationFile;
  std::string reconDSVisFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::ReconstructedDSVisualizationFile;
  std::string axisFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::AxisOrientationsFile;
  std::string graindataFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::graindataFile;
  std::string eulerFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::EulerAnglesFile;
  std::string hdf5GrainFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::HDF5GrainFile;
  std::string alignmentFile = m_OutputDirectory + MXADir::Separator + AIM::Representation::AlignmentFile;

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
	progressMessage(AIM_STRING("Loading Slices"), 3 );
	m->loadSlices();

	CHECK_FOR_CANCELED(ReconstructionFunc)
	progressMessage(AIM_STRING("Finding Border"), 6 );
	m->find_border();

	CHECK_FOR_CANCELED(ReconstructionFunc)
	progressMessage(AIM_STRING("Cleaning Data"), 9 );
	m->cleanup_data();

	if(m_AlignmentMethod == 3)
	{
	  CHECK_FOR_CANCELED(ReconstructionFunc)
	  progressMessage(AIM_STRING("Identifying Grains on Sections"), 11 );
	  m->form_grains_sections(quat_symmcubic, quat_symmhex);
	}

	CHECK_FOR_CANCELED(ReconstructionFunc)
	progressMessage(AIM_STRING("Aligning Slices"), 14 );
	m->align_sections(alignmentFile, quat_symmcubic, quat_symmhex);

  if (m_AlignmentMethod == 3)
  {
    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Redefining Border"), 16);
    m->find_border();
  }

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Forming Macro-Grains"), 19);
    m->numgrains = m->form_grains(quat_symmcubic, quat_symmhex);

	CHECK_FOR_CANCELED(ReconstructionFunc)
	progressMessage(AIM_STRING("Finding Grain Reference Orientations"), 22);
	m->find_kernels(quat_symmcubic, quat_symmhex);

	CHECK_FOR_CANCELED(ReconstructionFunc)
	progressMessage(AIM_STRING("Defining Sub-Grains"), 25);
	m->numgrains = m->define_subgrains(quat_symmcubic, quat_symmhex);

    CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Assigning Bad Points"), 28);
    m->assign_badpoints(quat_symmcubic, quat_symmhex);
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
  progressMessage(AIM_STRING("Identifying Grains"), 40);
  m->numgrains = m->reburn_grains(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Reference Orientations"), 43);
  m->find_kernels(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Average Orientations"), 49);
  m->homogenize_grains(quat_symmcubic, quat_symmhex);

  if (m_MergeTwins == true)
  {
    m->merge_twins(quat_symmcubic, quat_symmhex);
    progressMessage(AIM_STRING("Merging Twins"), 51);
    m->characterize_twins();
	CHECK_FOR_CANCELED(ReconstructionFunc)
    progressMessage(AIM_STRING("Renumbering Grains"), 53);
    m->numgrains = m->renumber_grains3();
  }

  if (m_MergeColonies == true)
  {
    progressMessage(AIM_STRING("Merging Colonies"), 51);
    m->merge_colonies(quat_symmcubic, quat_symmhex);
    progressMessage(AIM_STRING("Renumbering Grains"), 53);
    m->characterize_colonies();
  }

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Centroids"), 56);
  m->find_centroids();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Moments"), 59);
  m->find_moments();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Principal Axes Lengths"), 62);
  m->find_axes();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Pricipal Axes Vectors"), 65);
  m->find_vectors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Refinding Neighbors"), 68);
  m->find_neighbors();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Euclidean Distance Maps"), 71);
  m->find_euclidean_map();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Euler ODF"), 74);
  m->find_eulerodf();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Measuring Misorientations"), 77);
  m->measure_misorientations(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain IPF Colors"), 80);
  m->find_colors(quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Convexities"), 83);
  m->find_convexities();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Finding Grain Schmid Factors"), 86);
  m->find_schmids();

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Statistics"), 89);
  m->volume_stats(statsFile,misorientationFile,microBinsFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Out Volume"), 92);
  m->write_volume(reconVisFile, reconIPFVisFile, reconDisVisFile, reconIQVisFile, reconSFVisFile, reconDSVisFile, m_IPFoutputoption, m_Disorientationoutputoption, m_ImageQualityoutputoption, m_SchmidFactoroutputoption, quat_symmcubic, quat_symmhex);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Out Grains"), 94);
  m->write_grains(m_OutputDirectory /* quat_symmcubic, quat_symmhex */);
  writeHDF5GrainsFile(hdf5GrainFile, m);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Grain Data"), 96);
  m->write_graindata(graindataFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Axis Orientation File"), 98);
  m->write_axisodf(axisFile);

  CHECK_FOR_CANCELED(ReconstructionFunc)
  progressMessage(AIM_STRING("Writing Euler Angle File"), 100);
  m->write_eulerodf(eulerFile);

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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Reconstruction::writeHDF5GrainsFile(const std::string &hdfFile,
                                          ReconstructionFunc::Pointer r)
{
  int err = -1;
#if MXA_HDF5_SUPPORT
  AIMH5DataWriter::Pointer h5writer = AIMH5DataWriter::New();
  h5writer->setFileName(hdfFile);
  err = h5writer->openFile(false); // Open a new file over writing any other file

  std::stringstream ss;
  std::string hdfPath;
  for (int i = 1; i < r->numgrains; i++)
  {
    vector<int>* vlist = r->grains[i].voxellist;
    int vid = vlist->at(0);
    ss.str("");
    ss << "/" << i;
    hdfPath = ss.str();


    vector<int> plist(((r->xpoints + 1) * (r->ypoints + 1) * (r->zpoints + 1)), 0);
    int pcount = 0;
    int ocol, orow, oplane;
    int col, row, plane;
    int pid;
    int err = 0;
    // outFile << "POINTS " << pcount << " float" << endl;
    std::vector<float> points;
    std::vector<int32_t> cells;
    std::vector<int32_t> cell_types(vlist->size(), VTK_CELLTYPE_VOXEL);

    std::vector<float> kernelAvgDisorientation(vlist->size());
    std::vector<float> grainAvgDisorientation(vlist->size());
    std::vector<float> imageQuality(vlist->size());
    std::vector<float> schmidFactor(1);

    std::vector<int32_t> grainName(1);


    pcount = 0;
    plist.clear();
    plist.resize(((r->xpoints + 1) * (r->ypoints + 1) * (r->zpoints + 1)), 0);
    for (std::vector<int>::size_type j = 0; j < vlist->size(); j++)
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
          points.push_back( (row * r->resy));
          points.push_back((plane * r->resz));
        }
        // Add onto our cells vector
        cells.push_back(plist[pid]);
      }
      // Append a grainId to the grainIds vector
      kernelAvgDisorientation[j] = r->voxels[vid].kernelmisorientation;
      grainAvgDisorientation[j] = r->voxels[vid].misorientation;
      imageQuality[j] = r->voxels[vid].imagequality;
      grainName[0] = r->voxels[vid].grainname;
    }


    err = h5writer->writeUnstructuredGrid(hdfPath, points, cells, cell_types);
    points.resize(0);
    cells.resize(0);
    cell_types.resize(0);

    //Write the Field Data
    err = h5writer->writeFieldData<int>( hdfPath, grainName, "Grain_ID", 1);

    schmidFactor[0] = r->grains[i].schmidfactor;
    err = h5writer->writeFieldData<float>( hdfPath, schmidFactor, "SchmidFactor", 1);

    // Write CELL_DATA
    err = h5writer->writeCellData<float>(hdfPath, kernelAvgDisorientation, "KernelAvgDisorientation", 1);
    err = h5writer->writeCellData<float>(hdfPath, grainAvgDisorientation, "GrainAvgDisorientation", 1);
    err = h5writer->writeCellData<float>(hdfPath, imageQuality, "ImageQuality", 1);
  }

  err = h5writer->closeFile();
#else
  std::cout << "HDF5 support was not enabled in this build. Please reconfigure and recompile the sources." << std::endl;
#endif
  return err;
}

