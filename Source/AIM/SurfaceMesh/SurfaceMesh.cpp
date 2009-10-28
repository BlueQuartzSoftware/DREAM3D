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

#include "SurfaceMesh.h"

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
SurfaceMesh::Pointer SurfaceMesh::New( QObject* parent)
{
  Pointer sharedPtr(new SurfaceMesh(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh( QObject* parent) :
QThread(parent),
m_InputFile(""),
m_OutputDirectory(""),
m_SmoothMesh(false),
m_SmoothIterations(0),
m_SmoothFileOutputIncrement(0),
m_SmoothLockQuadPoints(false),
m_ErrorCondition(0),
m_Cancel(false)
{

}
#else
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh() :
m_InputFile(""),
m_OutputDirectory(""),
m_SmoothMesh(false),
m_SmoothIterations(0),
m_SmoothFileOutputIncrement(0),
m_SmoothLockQuadPoints(false),
m_ErrorCondition(0),
m_Cancel(false)
{

}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMesh::~SurfaceMesh()
{
}

#if AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::run()
{
  compute();
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::compute()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMesh::progressMessage(AIM_STRING message, int progress)
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
void SurfaceMesh::on_CancelWorker()
{
  // std::cout << "SurfaceMesh::cancelWorker()" << std::endl;
  this->m_Cancel = true;
}
#endif

