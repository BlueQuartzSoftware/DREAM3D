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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::GrainGenerator( QObject* parent) :
QThread(parent),
m_InputDirectory("."),
m_OutputDirectory("."),
m_ErrorCondition(0),
m_Cancel(false)
{

}
#else
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::GrainGenerator() :
m_InputDirectory("."),
m_OutputDirectory("."),
m_ErrorCondition(0)
{
}
#endif


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
  std::string  EulerAnglesFile = m_InputDirectory + MXAFileSystemPath::Separator + AIM::Representation::EulerAnglesFile;
  std::string  MisorientationBinsFile = m_InputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MisorientationBinsFile;
  std::string  MicroBinsFile = m_InputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MicroBinsFile;

  std::string  CubeFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::CubeFile;
  std::string  GrainsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::GrainsFile;
  std::string  VolumeFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::VolumeFile;

   m = GrainGeneratorFunc::New();
   m->initialize(m_NumGrains, m_ShapeClass,
              m_XResolution, m_YResolution, m_ZResolution, m_OverlapAllowed,
              m_OverlapAssignment, m_CrystalStructure);

   m->numgrains = m_NumGrains;
   m->resx = m_XResolution;
   m->resy = m_YResolution;
   m->resz = m_ZResolution;

   m->resx1 = 4 * m->resx;
   m->resy1 = 4 * m->resy;
   m->resz1 = 4 * m->resz;


   m->nummisobins = 10;
   m->nummicrobins = 10;
   m->misoiter = 0;
   m->nummisomoves = 250;
   m->nummicromoves = 25;


   ifstream inputFile1;
   inputFile1.open(AxisOrientationsFile.c_str());
   inputFile1 >> m->numorients;
   inputFile1.close();
   ifstream inputFile2;
   inputFile2.open(EulerAnglesFile.c_str());
   inputFile2 >> m->numeulers;
   inputFile2.close();
//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Loading Stats File"), 5 );
   m->loadStatsData(StatsFile);

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Loading Orient File"), 10 );
   m->loadorientData(AxisOrientationsFile,m->numorients);

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Loading Euler File"), 15 );
   m->loadeulerData(EulerAnglesFile,m->numeulers);

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Generating Grains"), 20 );
   m->generate_grains(m->numgrains);

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Initializing"), 25 );
   m->initialize2(m->xpoints,m->ypoints,m->zpoints);

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Assigning Eulers"), 30 );
   m->assign_eulers(m->numgrains);

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Packing Grains"), 35 );
   m->make_points(m->numgrains);

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Filling Gaps"), 40 );
   m->fill_gaps(m->numgrains);

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Finding Neighbors"), 45 );
   m->find_neighbors();

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Loading Misorientations"), 50 );
   m->loadMisoData(MisorientationBinsFile);

//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Loading Microtexture"), 55 );
   m->loadMicroData(MicroBinsFile);

   for(int iter = 0; iter < m->misoiter; iter++)
   {
//     CHECK_FOR_CANCELED(ReconstructionFunc)
//     progressMessage(AIM_STRING("Matching Misorientations"), 65 );
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
//     CHECK_FOR_CANCELED(ReconstructionFunc)
//     progressMessage(AIM_STRING("Matching Microtexture"), 75 );
     m->measure_misorientations();
     m->count_misorientations(m->numgrains);
     m->nummicros = m->rank_microbins(m->numgrains);
     m->rank_grains2(m->numgrains);
     m->identify_grains2(m->numgrains, m->nummicromoves);
     m->move_grains2(m->numgrains);
   }
   for(int iter3 = 0; iter3 < m->misoiter; iter3++)
   {
//     CHECK_FOR_CANCELED(ReconstructionFunc)
//     progressMessage(AIM_STRING("Rematching Misorientations"), 85 );
     m->measure_misorientations();
     m->rank_misobins(m->numgrains);
     m->count_misorientations(m->numgrains);
     m->freeze_grains(m->numgrains);
     m->rank_grains1(m->numgrains);
     m->identify_grains1(m->numgrains, m->nummisomoves);
     m->move_grains1(m->numgrains);
   }
//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("writing Cube"), 90 );
   m->writeCube(CubeFile, m->numgrains);
   
//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Writing Grains"), 95 );
   m->write_grains(GrainsFile,m->numgrains);
   
//   CHECK_FOR_CANCELED(ReconstructionFunc)
//   progressMessage(AIM_STRING("Writing Volume"), 100 );
   m->write_volume(VolumeFile);

//   progressMessage(AIM_STRING("Generation Completed"), 100 );
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
     std::cout << "GrainGenerator::cancelWorker()" << std::endl;
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



