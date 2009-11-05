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
#include <AIM/GrainGenerator/SVG-3D.h>

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
  jackson_compute();
  m = GrainGeneratorFunc::NullPointer();  // Clean up the memory
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::jackson_compute()
{
  CHECK_FOR_CANCELED(Synthetic Builder)
  progressMessage(AIM_STRING("Running Synthetic Builder"), 0 );
  m_InputDirectory = MXAFileSystemPath::toNativeSeparators(m_InputDirectory);
  m_OutputDirectory = MXAFileSystemPath::toNativeSeparators(m_OutputDirectory);

  int err = SVG_3D_Main(m_InputDirectory, m_OutputDirectory, m_NumGrains, m_ShapeClass,
              m_XResolution, m_YResolution, m_ZResolution, m_OverlapAllowed,
              m_OverlapAssignment, m_CrystalStructure);
  setErrorCondition(err);

  progressMessage(AIM_STRING("Synthetic Builder Complete"), 100 );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::compute()
{
  CREATE_INPUT_FILENAME(readname1, AIM::Representation::VolBinFile)
  CREATE_INPUT_FILENAME(readname2, AIM::Representation::BOverABinsFile)
  CREATE_INPUT_FILENAME(readname3, AIM::Representation::COverABinsFile)
  CREATE_INPUT_FILENAME(readname4, AIM::Representation::COverBBinsFile)
  CREATE_INPUT_FILENAME(readname5, AIM::Representation::SeNBinsFile)
  CREATE_INPUT_FILENAME(readname6, AIM::Representation::AxisOrientationsFile)
  CREATE_INPUT_FILENAME(readname7, AIM::Representation::EulerAnglesFile)
  CREATE_INPUT_FILENAME(readname8, AIM::Representation::SVNFile)
  CREATE_INPUT_FILENAME(readname9, AIM::Representation::SVSFile)
  CREATE_INPUT_FILENAME(readname10, AIM::Representation::MisorientationBinsFile)
  CREATE_INPUT_FILENAME(readname11, AIM::Representation::MicroBinsFile)

#if 0
  readname1 = "volbins.txt";
  readname2 = "boverabins.txt";
  readname3 = "coverabins.txt";
  readname4 = "coverbbins.txt";
  readname5 = "seNbins.txt";
  readname6 = "axisorientations.txt";
  readname7 = "eulerangles.txt";
  readname8 = "svn.txt";
  readname9 = "svs.txt";
  readname10 = "misobins.txt";
  readname11 = "microbins.txt";
#endif

  CREATE_OUTPUT_FILENAME(writename1, AIM::Representation::CubeFile)
  CREATE_OUTPUT_FILENAME(writename2, AIM::Representation::AnalysisFile)
  CREATE_OUTPUT_FILENAME(writename3, AIM::Representation::BoundaryCentersFile)
  CREATE_OUTPUT_FILENAME(writename4, AIM::Representation::GrainsFile)
  CREATE_OUTPUT_FILENAME(writename5, AIM::Representation::VolumeFile)

#if 0
  writename1 = "cube.vtk";
  writename2 = "analysis.txt";
  writename3 = "boundarycenters.txt";
  writename4 = "grains.txt";
  writename5 = "volume.txt";
#endif


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
   inputFile1.open(readname6.c_str());
   inputFile1 >> m->numorients;
   inputFile1.close();
   ifstream inputFile2;
   inputFile2.open(readname7.c_str());
   inputFile2 >> m->numeulers;
   inputFile2.close();
   m->loadStatsData(readname1);
   m->loadorientData(readname6,m->numorients);
   m->loadeulerData(readname7,m->numeulers);
   int* takencheck = new int[m->numgrains];
   m->generate_grains(m->numgrains);
   m->initialize2(m->xpoints,m->ypoints,m->zpoints);  
   m->assign_eulers(m->numgrains);
   m->make_points(m->numgrains);
   m->fill_gaps(m->numgrains);
   m->find_neighbors();
   m->loadMisoData(readname10);
   m->loadMicroData(readname11);
   for(int iter = 0; iter < m->misoiter; iter++)
   {
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
     m->measure_misorientations();
     m->count_misorientations(m->numgrains);
     m->nummicros = m->rank_microbins(m->numgrains);
     m->rank_grains2(m->numgrains);
     m->identify_grains2(m->numgrains, m->nummicromoves);
     m->move_grains2(m->numgrains);
   }
   for(int iter3 = 0; iter3 < m->misoiter; iter3++)
   {
     m->measure_misorientations();
     m->rank_misobins(m->numgrains);
     m->count_misorientations(m->numgrains);
     m->freeze_grains(m->numgrains);
     m->rank_grains1(m->numgrains);
     m->identify_grains1(m->numgrains, m->nummisomoves);
     m->move_grains1(m->numgrains);
   }
   m->writeCube(writename1, m->numgrains);
   m->write_grains(writename4,m->numgrains);
   m->write_volume(writename5);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::progressMessage(AIM_STRING message, int progress)
{
#ifdef AIM_USE_QT
      emit updateMessage(QString(message));
      emit updateProgress(progress);
      std::cout << message.toStdString() << std::endl;
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



