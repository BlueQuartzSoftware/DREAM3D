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
#include <AIM/SurfaceMesh/Surface_Mesh_General_MCA_Layer.h>
#include <AIM/SurfaceMesh/Update_Edge_Tri.h>
#include <AIM/SurfaceMesh/smooth-grain3d.h>

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
m_DxFile(""),
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
m_DxFile(""),
m_OutputDirectory(""),
m_SmoothMesh(false),
m_SmoothIterations(0),
m_SmoothFileOutputIncrement(0),
m_SmoothLockQuadPoints(false),
m_ErrorCondition(0)
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
  CHECK_FOR_CANCELED(Surface Meshing)
  progressMessage(AIM_STRING("Running Surface Meshing"), 0 );

  m_ErrorCondition = SurfaceMesh_MCALayer(m_XDim, m_YDim, m_ZDim,
                                 m_OutputDirectory.c_str(),
                                 m_DxFile.c_str(),
                                 AIM::Representation::MeshStatFile.c_str(),
                                 AIM::Representation::NodesRawFile.c_str());


  CHECK_FOR_CANCELED(Surface Meshing)
  progressMessage(AIM_STRING("Converting Slice Files to Single Nodes/Triangles Pair"), 40 );
  // Convert the output edge and triangle files into a single node/triangle file
  m_ErrorCondition = Update_Edge_Tri(AIM::Representation::MeshStatFile.c_str(),
                                     m_OutputDirectory.c_str(),
                                     AIM::Representation::EdgesFile.c_str(),
                                     AIM::Representation::TrianglesFile.c_str(),
                                     AIM::Representation::NodesFile.c_str(),
                                     AIM::Representation::NodesRawFile.c_str() );


  if (m_SmoothMesh == true)
  {
    //TODO: Run the smoothing algorithm
    CHECK_FOR_CANCELED(Surface Meshing)
    progressMessage(AIM_STRING("Smoothing Surface Meshing"), 70 );
    int lockquads = 0;
    if (m_SmoothLockQuadPoints) { lockquads = 1; }
    m_ErrorCondition = SmoothGrain3D(AIM::Representation::NodesFile,
                                     AIM::Representation::TrianglesFile,
                                      m_OutputDirectory,
                                      m_SmoothIterations,
                                      m_SmoothFileOutputIncrement,
                                      lockquads);

  }

  progressMessage(AIM_STRING("Surface Meshing Complete"), 100 );
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

