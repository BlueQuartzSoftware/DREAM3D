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
#include <AIM/ANG/AngDirectoryPatterns.h>
#include <AIM/ANG/AngFileReader.h>
#include <AIM/ANG/AngFileHelper.h>

#include <MXA/Utilities/MXAFileSystemPath.h>


#define CREATE_INPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXAFileSystemPath::Separator + n;\
    f = MXAFileSystemPath::toNativeSeparators(f);

#define CREATE_OUTPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXAFileSystemPath::Separator + n;\
    f = MXAFileSystemPath::toNativeSeparators(f);


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
m_CrystalStructure(AIM::Representation::Cubic),
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
  std::string  StatsFile = m_InputDirectory + MXAFileSystemPath::Separator + AIM::Representation::StatsFile;
  std::string  AxisOrientationsFile = m_InputDirectory + MXAFileSystemPath::Separator + AIM::Representation::AxisOrientationsFile;
  std::string  StructureFile = m_InputDirectory + MXAFileSystemPath::Separator + AIM::Representation::StructureFile;
  std::string  EulerAnglesFile = m_InputDirectory + MXAFileSystemPath::Separator + AIM::Representation::EulerAnglesFile;
  std::string  MisorientationBinsFile = m_InputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MisorientationBinsFile;
  std::string  MicroBinsFile = m_InputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MicroBinsFile;

  std::string  CubeFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::CubeFile;
  std::string  EulerFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::EulerFile;
  std::string cubedxFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::CubeDxFile;
  std::string AnalysisFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::AnalysisFile;

   m = GrainGeneratorFunc::New();
   m->initialize(m_NumGrains, m_ShapeClass,
              m_XResolution, m_YResolution, m_ZResolution, m_OverlapAllowed,
              m_OverlapAssignment, m_Precipitates, m_CrystalStructure, m_FractionPrecipitates);

   m->nummisobins = 10;
   m->nummicrobins = 10;
   m->misoiter = 0;
   m->nummisomoves = 250;
   m->nummicromoves = 25;

   if(m_AlreadyFormed == false)
   {
	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Loading Stats File"), 5 );
	   m->loadStatsData(StatsFile);

	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Loading Orient File"), 10 );
	   m->loadorientData(AxisOrientationsFile);

	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Generating Grains"), 20 );
	   m->generate_grains(m->numgrains);

	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Initializing"), 25 );
	   m->initialize2();

	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Packing Grains"), 35 );
	   m->pack_grains(m->numgrains);

	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Filling Gaps"), 40 );
	   m->fill_gaps(m->numgrains);

	   CHECK_FOR_CANCELED(GrainGeneratorFunc)
	   progressMessage(AIM_STRING("Finding Surface Voxels"), 45 );
	   m->find_surfacevoxels();

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
	   m->read_structure(StructureFile);
   }

/*   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Finding Neighbors"), 45 );
   m->find_neighbors();

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Loading Euler File"), 15 );
   m->loadeulerData(EulerAnglesFile);

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Loading Misorientations"), 50 );
   m->loadMisoData(MisorientationBinsFile);

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Loading Microtexture"), 55 );
   m->loadMicroData(MicroBinsFile);

   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Assigning Eulers"), 30 );
   m->assign_eulers(m->numgrains);

   for(int iter = 0; iter < m->misoiter; iter++)
   {
     CHECK_FOR_CANCELED(GrainGeneratorFunc)
     progressMessage(AIM_STRING("Matching Misorientations"), 65 );
     m->measure_misorientations();
     m->rank_misobins(m->numgrains);
     m->count_misorientations(m->numgrains);
     m->freeze_grains(m->numgrains);
     m->rank_grains1(m->numgrains);
     m->identify_grains1(m->numgrains, m->nummisomoves);
     m->move_grains1(m->numgrains);
   }
   while(m->nummicros != 1)
   {
     CHECK_FOR_CANCELED(GrainGeneratorFunc)
     progressMessage(AIM_STRING("Matching Microtexture"), 75 );
     m->measure_misorientations();
     m->count_misorientations(m->numgrains);
     m->nummicros = m->rank_microbins(m->numgrains);
     m->rank_grains2(m->numgrains);
     m->identify_grains2(m->numgrains, m->nummicromoves);
     m->move_grains2(m->numgrains);
   }
   for(int iter3 = 0; iter3 < m->misoiter; iter3++)
   {
     CHECK_FOR_CANCELED(GrainGeneratorFunc)
     progressMessage(AIM_STRING("Rematching Misorientations"), 85 );
     m->measure_misorientations();
     m->rank_misobins(m->numgrains);
     m->count_misorientations(m->numgrains);
     m->freeze_grains(m->numgrains);
     m->rank_grains1(m->numgrains);
     m->identify_grains1(m->numgrains, m->nummisomoves);
     m->move_grains1(m->numgrains);
   }


*/   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("writing Cube"), 90 );
   m->writeCube(CubeFile, m->numgrains, AnalysisFile);
   
   CHECK_FOR_CANCELED(GrainGeneratorFunc)
   progressMessage(AIM_STRING("Writing DX File"), 94);
   m->create_dxfile(cubedxFile);

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



