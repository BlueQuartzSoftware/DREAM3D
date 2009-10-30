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

#include "VolumeMesh.h"

#include <MXA/Utilities/MXAFileSystemPath.h>
#include <AIM/Common/Constants.h>
#include <AIM/VolumeMesh/MeshGenerator.h>


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
VolumeMesh::Pointer VolumeMesh::New( QObject* parent)
{
  Pointer sharedPtr(new VolumeMesh(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeMesh::VolumeMesh( QObject* parent) :
QThread(parent),
m_NodesFile(""),
m_TrianglesFile(""),
m_OutputDirectory(""),
m_XDim(0.0), m_YDim(0.0), m_ZDim(0.0),
m_XRes(0.0), m_YRes(0.0), m_ZRes(0.0), m_NumGrains(0),
m_ErrorCondition(0),
m_Cancel(false)
{

}
#else
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeMesh::VolumeMesh() :
m_NodesFile(""),
m_TrianglesFile(""),
m_OutputDirectory(""),
m_XDim(0.0), m_YDim(0.0), m_ZDim(0.0),
m_XRes(0.0), m_YRes(0.0), m_ZRes(0.0), m_NumGrains(0),
m_ErrorCondition(0)
{

}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeMesh::~VolumeMesh()
{
}

#if AIM_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMesh::run()
{
  compute();
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMesh::compute()
{
  CHECK_FOR_CANCELED(Surface Meshing)
  progressMessage(AIM_STRING("Running Volume Meshing"), 0 );


  std::string meshFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MeshFile;
  meshFile = MXAFileSystemPath::toNativeSeparators(meshFile);

  std::string meshFile2 = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::MeshFile2;
  meshFile2 = MXAFileSystemPath::toNativeSeparators(meshFile2);

  std::string qualFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::ElementQualityFile;
  qualFile = MXAFileSystemPath::toNativeSeparators(qualFile);

  std::string voxelsFile = m_OutputDirectory + MXAFileSystemPath::Separator + AIM::Representation::VoxelsFile;
  voxelsFile = MXAFileSystemPath::toNativeSeparators(voxelsFile);


  m_ErrorCondition = MeshGenerator_Main(m_NodesFile,
                                     m_TrianglesFile,
                                     meshFile, // Vtk file
                                     meshFile2, // Vtk File
                                     qualFile, //"element_quality_measures_v5.txt"
                                     voxelsFile, //"voxels_v5.txt"
                                     m_XDim, m_YDim, m_ZDim,
                                     m_XRes, m_YRes, m_ZRes, m_NumGrains);

  progressMessage(AIM_STRING("Surface Volume Complete"), 100 );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMesh::progressMessage(AIM_STRING message, int progress)
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
void VolumeMesh::on_CancelWorker()
{
  // std::cout << "VolumeMesh::cancelWorker()" << std::endl;
  this->m_Cancel = true;
}
#endif

