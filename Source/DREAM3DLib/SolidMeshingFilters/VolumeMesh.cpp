/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force, 
 * BlueQuartz Software nor the names of its contributors may be used to endorse 
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VolumeMesh.h"

#include <MXA/Utilities/MXADir.h>
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/VolumeMesh/MeshGenerator.h"


#ifdef DREAM3D_USE_QT
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

#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    QString outpath = m_OutputDirectory + QDir::Separator + m_OutputFilePrefix + filename;


#if DREAM3D_USE_QT

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeMesh::Pointer VolumeMesh::New( QObject* parent)
{
  Pointer sharedPtr(new VolumeMesh(parent));
  return sharedPtr;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeMesh::VolumeMesh(
#if DREAM3D_USE_QT
QObject* parent
#endif
) :
#if DREAM3D_USE_QT
QObject(parent),
#endif
m_NodesFile(""),
m_TrianglesFile(""),
m_OutputDirectory(""),
m_OutputFilePrefix("VolumeMesh_"),
m_XDim(0.0),
m_YDim(0.0),
m_ZDim(0.0),
m_XRes(0.0),
m_YRes(0.0),
m_ZRes(0.0),
m_NumGrains(0),
m_ErrorCondition(0)
#if DREAM3D_USE_QT
,
m_Cancel(false)
#endif
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeMesh::~VolumeMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMesh::compute()
{
  CHECK_FOR_CANCELED(Surface Meshing)
  progressMessage(AIM_STRING("Running Volume Meshing"), 0 );


  MAKE_OUTPUT_FILE_PATH ( meshFile , DREAM3D::VolumeMeshing::MeshFile)
  MAKE_OUTPUT_FILE_PATH ( meshFile2 , DREAM3D::VolumeMeshing::MeshFile2)
  MAKE_OUTPUT_FILE_PATH ( qualFile , DREAM3D::VolumeMeshing::ElementQualityFile)
  MAKE_OUTPUT_FILE_PATH ( voxelsFile , DREAM3D::VolumeMeshing::VoxelsFile)


  m_ErrorCondition = MeshGenerator_Main(m_NodesFile,
                                     m_TrianglesFile,
                                     meshFile, // Vtk file
                                     meshFile2, // Vtk File
                                     qualFile, //"element_quality_measures_v5.txt"
                                     voxelsFile, //"voxels_v5.txt"
                                     m_XDim, m_YDim, m_ZDim,
                                     m_XRes, m_YRes, m_ZRes, m_NumGrains);

  progressMessage(AIM_STRING("Surface Volume Complete"), 100 );
#if DREAM3D_USE_QT
  emit finished();
#endif

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMesh::progressMessage(AIM_STRING message, int progress)
{
#ifdef DREAM3D_USE_QT
      emit updateMessage(QString(message));
      emit updateProgress(progress);
     // qDebug() << message.toStdString() ;
#else
  qDebug() << message ;
#endif
}

#ifdef DREAM3D_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeMesh::on_CancelWorker()
{
  // qDebug() << "VolumeMesh::cancelWorker()" ;
  this->m_Cancel = true;
}
#endif

