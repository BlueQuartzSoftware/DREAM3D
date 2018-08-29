/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ComputeStereographicProjection.h"

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "OrientationLib/Utilities/ModifiedLambertProjection.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeStereographicProjection::ComputeStereographicProjection(FloatArrayType* xyzCoords, PoleFigureConfiguration_t* config, DoubleArrayType* intensity)
: m_XYZCoords(xyzCoords)
, m_Config(config)
, m_Intensity(intensity)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeStereographicProjection::~ComputeStereographicProjection() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeStereographicProjection::operator()() const
{
  m_Intensity->resize(static_cast<size_t>(m_Config->imageDim * m_Config->imageDim));
  m_Intensity->initializeWithZeros();

  if(m_Config->discrete)
  {
    int halfDim = m_Config->imageDim / 2;
    double* intensity = m_Intensity->getPointer(0);
    size_t numCoords = m_XYZCoords->getNumberOfTuples();
    float* xyzPtr = m_XYZCoords->getPointer(0);
    for(size_t i = 0; i < numCoords; i++)
    {
      if(xyzPtr[i * 3 + 2] < 0.0f)
      {
        xyzPtr[i * 3 + 0] *= -1.0f;
        xyzPtr[i * 3 + 1] *= -1.0f;
        xyzPtr[i * 3 + 2] *= -1.0f;
      }
      float x = xyzPtr[i * 3] / (1 + xyzPtr[i * 3 + 2]);
      float y = xyzPtr[i * 3 + 1] / (1 + xyzPtr[i * 3 + 2]);

      int xCoord = static_cast<int>(x * (halfDim - 1)) + halfDim;
      int yCoord = static_cast<int>(y * (halfDim - 1)) + halfDim;

      size_t index = static_cast<size_t>((yCoord * m_Config->imageDim) + xCoord);

      intensity[index]++;
    }
#if 0
    // This chunk is here for some debugging....
    int dim = m_Config->imageDim;
    QString filename = QString("/tmp/Discrete-%1-%2.h5").arg(m_Intensity->getName()).arg(dim);
    hid_t file_id = H5Utilities::createFile(filename.toStdString());
    hsize_t dims[2];
    dims[0] = dim;
    dims[1] = dim;
    H5Lite::writePointerDataset(file_id, "Discrete", 2, dims, intensity);
    H5Fclose(file_id);
#endif
  }
  else
  {
    ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::LambertBallToSquare(m_XYZCoords, m_Config->lambertDim, m_Config->sphereRadius);
    lambert->normalizeSquaresToMRD();
#if 0
    int dim = lambert->getDimension();
    QString filename = QString("/tmp/Lambert-%1.h5").arg(dim).arg(m_Config->);
    hid_t file_id = H5Utilities::createFile(filename.toStdString());
    lambert->writeHDF5Data(file_id);
    H5Fclose(file_id);
#endif
    lambert->createStereographicProjection(m_Config->imageDim, m_Intensity);
  }
}
