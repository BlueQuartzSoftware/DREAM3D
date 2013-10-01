/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

/**
 * @file H5VoxelToVtk.cpp
 * @brief This program is mainly a debugging program to ensure the .h5voxel file
 * has the correct data since at the current time there is no way to visualize
 * the .h5voxel in ParaView or anything else. This will convert the .h5voxel to a
 * VTK structured points data file with Grain IDs and IPS Colors.
 */

#include <iostream>
#include <limits>
#include <QString>
#include <QVector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/VTKUtils/VTKWriterMacros.h"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"

#include "DREAM3DLib/IOFilters/DataContainerReader.h"


typedef struct {
    int xpoints;
    int ypoints;
    int zpoints;
    float resx; float resy; float resz;
    int* grain_indicies;
    int* phases;
    float* euler1s;
    float* euler2s;
    float* euler3s;
    float* quats;
    unsigned int* crystruct;
} Test;



void updateProgressAndMessage(const QString &msg, int prog)
{
  qDebug() << prog << "% - " << msg ;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void pipelineProgress(int value)
{
  qDebug() << value << "%" ;
}

void pipelineProgressMessage(const QString &msg)
{
  qDebug() << msg ;
}

void pipelineErrorMessage(const char* message)
{
  qDebug() << "Error Message: " << message ;
}

void setErrorCondition(int err) {
  qDebug() << "Error Condition: " << err ;
}

void pipelineFinished()
{
  qDebug() << "Pipeline Complete." ;
}

void setErrorMessage(const QString &msg)
{
  qDebug() << msg ;
}

QString getNameOfClass()
{
  return "H5VoxelToVtk";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  qDebug() << "Starting Conversion of H5Voxel to VTK with Grain ID and IPF Colors" ;
  if (argc < 3)
  {
    qDebug() << "This program takes 2 arguments: Input .h5voxel file and output vtk file." ;
    return EXIT_FAILURE;
  }


  QString iFile = argv[1];

  int err = 0;

  VolumeDataContainer::Pointer m = VolumeDataContainer::New();

  DataContainerReader::Pointer h5Reader = DataContainerReader::New();
  h5Reader->setInputFile(iFile);
  h5Reader->setVolumeDataContainer(m.get());
  size_t dcDims[3];
  float spacing[3];
  float origin[3];
  h5Reader->execute();
  err = h5Reader->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(err);
 //   addErrorMessages(h5Reader->getErrorMessages());
    return EXIT_FAILURE;
  }
  m->getDimensions(dcDims);
  m->getResolution(spacing);
  m->getOrigin(origin);

  int64_t dims[3] = {dcDims[0], dcDims[1], dcDims[2]};

  /* Sanity check what we are trying to load to make sure it can fit in our address space.
   * Note that this does not guarantee the user has enough left, just that the
   * size of the volume can fit in the address space of the program
   */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
  if (dims[0] * dims[1] * dims[2] > max )
  {
    err = -1;
    QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version.")
    .arg(dims[0] * dims[1] * dims[2]);
    setErrorCondition(err);
    setErrorMessage(ss);
    return EXIT_FAILURE;
  }

  if (dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version."
    " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[2]);
    setErrorCondition(err);
    setErrorMessage(ss);
    return EXIT_FAILURE;
  }
  /* ************ End Sanity Check *************************** */

  qDebug() << "Writing VTK file" ;

  FILE* f = fopen(argv[2], "wb");

  WRITE_STRUCTURED_POINTS_HEADER("ASCII", m)


//  VoxelIPFColorScalarWriter<VolumeDataContainer> ipfWriter(m.get());
//  ipfWriter.m_WriteBinaryFiles = false;
//  ipfWriter.writeScalars(f);


  int64_t totalPoints = m->getTotalPoints();
  int32_t* m_GrainIds = NULL;
  m_GrainIds = m->getCellDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::CellData::GrainIds, totalPoints, 1, NULL);
    if (0 == m_GrainIds ) {
      QString ss = QObject::tr("Filter %1 requires the data array 'DREAM3D::CellData::GrainIds' to already be created prior to execution.")
      .arg(getNameOfClass());
      setErrorCondition(-300);
      setErrorMessage(ss);
    }

  WRITE_VTK_GRAIN_IDS_ASCII(m, DREAM3D::CellData::GrainIds, m_GrainIds)

  fclose(f);


  qDebug() << "Done Converting" ;
  return EXIT_SUCCESS;
}
