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

#include "GrainGenerator.h"
#include "AIM/ANG/AngDirectoryPatterns.h"
#include "AIM/ANG/AngReader.h"
#include "AIM/ANG/AngFileHelper.h"
#include "AIM/Reconstruction/H5ReconStatsReader.h"

#include <MXA/Utilities/MXADir.h>


#define CREATE_INPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);

#define CREATE_OUTPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);


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
GrainGenerator::Pointer GrainGenerator::New( QObject* parent)
{
  Pointer sharedPtr(new GrainGenerator(parent));
  return sharedPtr;
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::GrainGenerator(
#if AIM_USE_QT
QObject* parent
#endif
) :
#if AIM_USE_QT
QThread(parent),
#endif
m_InputDirectory("."),
m_OutputDirectory("."),
m_NumGrains(0),
m_ShapeClass(0),
m_XResolution(0.0),
m_YResolution(0.0),
m_ZResolution(0.0),
m_OverlapAllowed(0),
m_AlreadyFormed(false),
m_Precipitates(0),
m_OverlapAssignment(0),
m_CrystalStructure(AIM::Reconstruction::Cubic),
m_ErrorCondition(0)
#if AIM_USE_QT
  ,m_Cancel(false)
#endif
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::~GrainGenerator()
{
}

#if AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::run()
{
  compute();
  m = GrainGeneratorFunc::NullPointer();  // Clean up the memory
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::compute()
{
  int err = 0;
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

#if AIM_HDF5_SUPPORT
    H5ReconStatsReader::Pointer h5reader = H5ReconStatsReader::New(m_InputDirectory + MXADir::Separator + AIM::Reconstruction::HDF5ResultsFile);
    if (h5reader.get() == NULL)
    {
      progressMessage(AIM_STRING("Error Opening HDF5 Stats File. Nothing generated"), 100 );
      return;
    }
#else
  std::string  StatsFile = m_InputDirectory + MXADir::Separator + AIM::Reconstruction::StatsFile;
  std::string  AxisOrientationsFile = m_InputDirectory + MXADir::Separator + AIM::Reconstruction::AxisOrientationsFile;
  std::string  EulerAnglesFile = m_InputDirectory + MXADir::Separator + AIM::Reconstruction::ODFFile;
  std::string  MisorientationBinsFile = m_InputDirectory + MXADir::Separator + AIM::Reconstruction::MisorientationBinsFile;
  std::string  MicroBinsFile = m_InputDirectory + MXADir::Separator + AIM::Reconstruction::MicroTextureFile;
#endif
  std::string  CubeFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::CubeFile;
  std::string  EulerFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::EulerFile;
  std::string AnalysisFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::AnalysisFile;
  std::string MoDFFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::MoDFFile;
  std::string CrystallographicErrorFile = m_OutputDirectory + MXADir::Separator + AIM::SyntheticBuilder::CrystallographicErrorFile;
  std::string graindataFile = m_OutputDirectory + MXADir::Separator + AIM::Reconstruction::GrainDataFile;



   m = GrainGeneratorFunc::New();
   m->initialize(m_NumGrains, m_ShapeClass,
              m_XResolution, m_YResolution, m_ZResolution, m_OverlapAllowed,
              m_OverlapAssignment, m_Precipitates, m_CrystalStructure, m_FractionPrecipitates);


   if(m_AlreadyFormed == false)
   {
	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Loading Stats File"), 5 );
#if AIM_HDF5_SUPPORT
	   m->readReconStatsData(h5reader);
#else
	   m->readReconStatsData(StatsFile);
#endif
	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Loading Orient File"), 10 );
#if AIM_HDF5_SUPPORT
	   m->readAxisOrientationData(h5reader);
#else
	   m->readAxisOrientationData(AxisOrientationsFile);
#endif

	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Packing Grains"), 25 );
	   m->numgrains = m->pack_grains(m->numgrains);

	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Assigning Voxels"), 30 );
	   m->numgrains = m->assign_voxels(m->numgrains);

	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Filling Gaps"), 40 );
	   m->fill_gaps(m->numgrains);

	   if(m_Precipitates > 1)
	   {
		   CHECK_FOR_CANCELED(GrainGeneratorFunc)
		   progressMessage(AIM_STRING("Generating Precipitates"), 40 );
		   m->numprecipitates = m->create_precipitates();

		   CHECK_FOR_CANCELED(GrainGeneratorFunc)
		   progressMessage(AIM_STRING("Inserting Precipitates"), 40 );
		   m->insert_precipitates(m->numprecipitates);
	   }
   }

   if(m_AlreadyFormed == true)
   {
	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Reading Structure"), 40 );
	   m->read_structure(CubeFile);
   }

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Finding Neighbors"), 45 );
   m->find_neighbors();

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Loading Euler File"), 15 );
#if AIM_HDF5_SUPPORT
   m->readODFData(h5reader);
#else
   m->readODFData(EulerAnglesFile);
#endif

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Loading Misorientations"), 50 );
#if AIM_HDF5_SUPPORT
   m->readMisorientationData(h5reader);
   #else
   m->readMisorientationData(MisorientationBinsFile);
#endif

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Loading Microtexture"), 55 );
#if AIM_HDF5_SUPPORT
   m->readMicroTextureData(h5reader);
#else
   m->readMicroTextureData(MicroBinsFile);
#endif

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Assigning Eulers"), 60 );
   m->assign_eulers(m->numgrains);

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Measuring Misorientations"), 65 );
   m->measure_misorientations(quat_symmcubic, quat_symmhex);

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Matching Crystallography"), 65 );
   m->matchCrystallography(quat_symmcubic, quat_symmhex, CrystallographicErrorFile);

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("writing Cube"), 90 );
   m->writeCube(CubeFile, m->numgrains);

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Writing Grain Data"), 94 );
   m->write_graindata(graindataFile, MoDFFile);

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Writing Euler Angles"), 98 );
   m->write_eulerangles(EulerFile);

   progressMessage(AIM_STRING("Generation Completed"), 100 );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::progressMessage(AIM_STRING message, int progress)
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
void GrainGenerator::on_CancelWorker()
{
 //    std::cout << "GrainGenerator::cancelWorker()" << std::endl;
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



