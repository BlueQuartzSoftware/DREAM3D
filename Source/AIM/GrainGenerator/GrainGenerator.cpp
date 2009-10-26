///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#include "GrainGenerator.h"
#include <AIM/ANG/AngDirectoryPatterns.h>
#include <AIM/ANG/AngFileReader.h>
#include <AIM/ANG/AngFileHelper.h>
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
Reconstruction::Pointer Reconstruction::New( QObject* parent)
{
  Pointer sharedPtr(new Reconstruction(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Reconstruction::Reconstruction( QObject* parent) :
QThread(parent),
  m_InputDirectory("."), m_OutputDirectory("."),
  m_AngFilePrefix("Slice_"), m_AngSeriesMaxSlice(3),
  m_ZStartIndex(0), m_ZEndIndex(0), m_ZResolution(0.25),
  m_MergeTwins(false), m_MinAllowedGrainSize(0.0),
  m_MinSeedConfidence(0.0), m_MisorientationTolerance(0.0),
  m_CrystalStructure(AIM::Reconstruction::Hexagonal),
  m_AlreadyFormed(false), m_ErrorCondition(0),m_Cancel(false)
{

}
#else
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::GrainGenerator()
{
  // TODO Auto-generated constructor stub

}

#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGenerator::~GrainGenerator()
{
  // TODO Auto-generated destructor stub
}

#if AIM_USE_QT
void Reconstruction::run()
{
  compute();
  m_microgen = MicroGen3D::NullPointer();  // Clean up the memory
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGenerator::compute()
{


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
     if (m_microgen.get() != NULL)
     {
       if (m_microgen->m_angFileHelper.get() != NULL)
       {
         m_microgen->m_angFileHelper->setCancel(true);
       }
     }

}
#endif



