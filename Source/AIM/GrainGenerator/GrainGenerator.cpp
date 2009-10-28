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
  m_InputDirectory("."), m_OutputDirectory("."),
   m_ErrorCondition(0),m_Cancel(false)
{

}
#else
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::GrainGenerator()
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

void GrainGenerator::run()
{
  jackson_compute();
  m = MicroGen3D::NullPointer();  // Clean up the memory
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::jackson_compute()
{

  m_InputDirectory = MXAFileSystemPath::toNativeSeparators(m_InputDirectory);
  m_OutputDirectory = MXAFileSystemPath::toNativeSeparators(m_OutputDirectory);

  int err = SVG_3D_Main(m_InputDirectory, m_OutputDirectory, m_NumGrains, m_ShapeClass,
              m_XResolution, m_YResolution, m_ZResolution, m_OverlapAllowed,
              m_OverlapAssignment, m_CrystalStructure);
  setErrorCondition(err);

}


#define CREATE_INPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXAFileSystemPath::Separator + n;\
    f = MXAFileSystemPath::toNativeSeparators(f);

#define CREATE_OUTPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXAFileSystemPath::Separator + n;\
    f = MXAFileSystemPath::toNativeSeparators(f);

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

   m = MicroGen3D::New();
   m->numgrains = m_NumGrains;
   m->resx = m_XResolution;
   m->resy = m_YResolution;
   m->resz = m_ZResolution;

   m->resx1 = 4 * m->resx;
   m->resy1 = 4 * m->resy;
   m->resz1 = 4 * m->resz;


   m->numsizebins = 11;
   m->numneighbins = 100;
   m->numdiambins = 1000;
   m->numshapebins = 1000;
   m->numseNbins = 1001;
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
   m->gsizes = new int[m->numgrains];
   m->gremovals = new int[m->numgrains];
#if 0
   m->grains = new Grain[m->numgrains];
   m->tempgrain =  new Grain[m->numgrains];
   m->diambin =  new Bin[m->numdiambins];
   m->orient = new Orient[m->numorients];
   m->boverabin =  new Bin[m->numsizebins*m->numshapebins];
   m->coverabin = new Bin[m->numsizebins*m->numshapebins];
   m->coverbbin = new Bin[m->numsizebins*m->numshapebins];
   m->seNbin = new Bin[m->numsizebins*m->numseNbins];
   //m->eulercount = new int **[18];
   m->eulerbin = new Bin[18*18*18*100];
   for(int i = 0; i < 18; i++)
   {
      m->eulercount[i] = new int *[18];
     for(int j = 0; j < 18; j++)
     {
       m->eulercount[i][j] = new int [18];
       for(int k = 0; k < 18; k++)
       {
         m->eulercount[i][j][k] = 0;
       }
     }
   }
   m->loadVolData(readname1,m->numdiambins);
   m->loadboveraData(readname2,m->numshapebins);
   m->loadcoveraData(readname3,m->numshapebins);
   m->loadcoverbData(readname4,m->numshapebins);
   m->loadNData(readname5,m->numshapebins);
   m->loadorientData(readname6,m->numorients);
   m->loadeulerData(readname7,m->numeulers);
   int* takencheck = new int[m->numgrains];
   m->generate_grains(m->numgrains);
   m->assign_eulers(m->numgrains);
//   m->svn = new double *[m->numsizebins];
//   m->svs = new double *[m->numsizebins];
//   double** nsdist = new double *[m->numgrains];
//   double** bcent = new double *[100000];
   for(int temp = 0; temp < m->numsizebins; temp++)
   {
     m->svn[temp] = new double [m->numneighbins];
     m->svs[temp] = new double [m->numsizebins];
   }
   for(int temp1 = 0; temp1 < 100000; temp1++)
   {
     bcent[temp1] = new double [5];
   }
   for(int temp2 = 0; temp2 < m->numgrains; temp2++)
   {
     nsdist[temp2] = new double [m->numsizebins];
   }
   for(int temp3 = 0; temp3 < m->numgrains; temp3++)
   {
     for(int temp4 = 0; temp4 < m->numsizebins; temp4++)
     {
       nsdist[temp3][temp4] = 0;
     }
   }
   cout << "grains made" << endl;
   gridfine = (struct gridpoints *)malloc(((sizex/resx))*((sizey/resy))*((sizez/resz))*sizeof(struct gridpoints));
   cout << "fine points made" << endl;
   gridcourse = (struct gridpoints *)malloc(((sizex/resx1))*((sizey/resy1))*((sizez/resz1))*sizeof(struct gridpoints));
   cout << "coarse points made" << endl;
   packedgrain = (struct packedgrains *)malloc((numgrains)*sizeof(struct packedgrains));
   voxelvector.resize(m->numgrains);
   neighborvector.resize(m->numgrains);
   int bigerror = 10;
   for(int temp5 = 0; temp5 < m->numgrains; temp5++)
   {
     packedgrain[temp5].nserror = bigerror;
   }
   m->loadSVNData(readname8);
   m->loadSVSData(readname9);
   m->make_points(m->numgrains);
   m->fill_gaps(m->numgrains);
   m->find_neighbors();
   m->find_centroids();
   m->find_moments();
   m->find_axis();
   m->find_colors();
   m->actualmisobin = new Bin[m->nummisobins];
   m->simmisobin = new Bin[m->nummisobins];
   m->actualmicrobin = new Bin[m->nummicrobins];
   m->simmicrobin = new Bin[m->nummicrobins];
   m->loadMisoData(readname10);
   m->loadMicroData(readname11);
   for(int iter = 0; iter < m->misoiter; iter++)
   {
     m->measure_misorientations(m->numgrains);
     m->rank_misobins(m->numgrains);
     m->count_misorientations(m->numgrains);
     m->freeze_grains(m->numgrains);
     m->rank_grains1(m->numgrains);
     m->identify_grains1(m->numgrains, m->nummisomoves);
     m->move_grains1(m->numgrains);
   }
   while(m->nummicros != 1)
   {
     m->measure_misorientations(m->numgrains);
     m->count_misorientations(m->numgrains);
     nummicros = m->rank_microbins(m->numgrains);
     m->rank_grains2(m->numgrains);
     m->identify_grains2(m->numgrains, m->nummicromoves);
     m->move_grains2(m->numgrains);
   }
   for(int iter3 = 0; iter3 < m->misoiter; iter3++)
   {
     m->measure_misorientations(m->numgrains);
     m->rank_misobins(m->numgrains);
     m->count_misorientations(m->numgrains);
     m->freeze_grains(m->numgrains);
     m->rank_grains1(m->numgrains);
     m->identify_grains1(m->numgrains, m->nummisomoves);
     m->move_grains1(m->numgrains);
   }
   m->volume_stats(m->numgrains, writename2);
   m->writeCube(writename1, m->numgrains);
   m->write_grains(writename4,m->numgrains);
   m->write_volume(writename5);
   m->find_boundarycenters(writename3, m->numgrains);


  delete [] grain;
  delete [] gridfine;
  delete [] gridcourse;
  delete [] packedgrain;
  delete [] orient;
  delete [] diambin;
  delete [] boverabin;
  delete [] coverabin;
  delete [] coverbbin;
  delete [] seNbin;
  delete [] actualmisobin;
  delete [] simmisobin;
  delete [] actualmicrobin;
  delete [] simmicrobin;

#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::progressMessage(AIM_STRING message, int progress)
{
#ifdef AIM_USE_QT
   //   emit updateMessage(QString(message));
   //   emit updateProgress(progress);
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



