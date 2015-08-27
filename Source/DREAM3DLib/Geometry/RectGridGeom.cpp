/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ============================================================================
 * RectGridGeom uses code adapated from the following vtk modules:
 *
 * * vtkLine.cxx
 *   - adapted vtkVoxel::InterpolationDerivs to RectGridGeom::getShapeFunctions
 * * vtkGradientFilter.cxx
 *   - adapted vtkGradientFilter template function ComputeGradientsSG to
 *     RectGridGeom::findDerivatives
 *
 * The vtk license is reproduced below.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ============================================================================
 * Copyright (c) 1993-2008 Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names of
 * any contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "DREAM3DLib/Geometry/RectGridGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RectGridGeom::RectGridGeom()
{
  m_GeometryTypeName = DREAM3D::Geometry::RectGridGeometry;
  m_GeometryType = DREAM3D::GeometryType::RectGridGeometry;
  m_XdmfGridType = DREAM3D::XdmfGridType::RectilinearGrid;
  m_UnitDimensionality = 3;
  m_SpatialDimensionality = 3;
  m_Dimensions[0] = 0;
  m_Dimensions[1] = 0;
  m_Dimensions[2] = 0;
  m_xBounds = FloatArrayType::NullPointer();
  m_yBounds = FloatArrayType::NullPointer();
  m_zBounds = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RectGridGeom::~RectGridGeom()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RectGridGeom::Pointer RectGridGeom::CreateGeometry(const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  RectGridGeom* d = new RectGridGeom();
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setXBounds(FloatArrayType::Pointer xBnds)
{
  m_xBounds = xBnds;
  m_xBounds->setName(DREAM3D::Geometry::xBoundsList);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setYBounds(FloatArrayType::Pointer yBnds)
{
  m_yBounds = yBnds;
  m_yBounds->setName(DREAM3D::Geometry::yBoundsList);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setZBounds(FloatArrayType::Pointer zBnds)
{
  m_zBounds = zBnds;
  m_zBounds->setName(DREAM3D::Geometry::zBoundsList);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t idx[3], float coords[3])
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = 0.5f * (xBnds[idx[0]] + xBnds[idx[0] + 1]);
  coords[1] = 0.5f * (yBnds[idx[1]] + yBnds[idx[1] + 1]);
  coords[2] = 0.5f * (zBnds[idx[2]] + zBnds[idx[2] + 1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t& x, size_t& y, size_t& z, float coords[3])
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = 0.5f * (xBnds[x] + xBnds[x + 1]);
  coords[1] = 0.5f * (yBnds[y] + yBnds[y + 1]);
  coords[2] = 0.5f * (zBnds[z] + zBnds[z + 1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t idx[3], double coords[3])
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = static_cast<double>(0.5 * (xBnds[idx[0]] + xBnds[idx[0] + 1]));
  coords[1] = static_cast<double>(0.5 * (yBnds[idx[1]] + yBnds[idx[1] + 1]));
  coords[2] = static_cast<double>(0.5 * (zBnds[idx[2]] + zBnds[idx[2] + 1]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getCoords(size_t& x, size_t& y, size_t& z, double coords[3])
{
  float* xBnds = m_xBounds->getPointer(0);
  float* yBnds = m_yBounds->getPointer(0);
  float* zBnds = m_zBounds->getPointer(0);

  coords[0] = static_cast<double>(0.5 * (xBnds[x] + xBnds[x + 1]));
  coords[1] = static_cast<double>(0.5 * (yBnds[y] + yBnds[y + 1]));
  coords[2] = static_cast<double>(0.5 * (zBnds[z] + zBnds[z + 1]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::initializeWithZeros()
{
  for (size_t i = 0; i < 3; i++)
  {
    m_Dimensions[i] = 0;
  }
  m_xBounds = FloatArrayType::NullPointer();
  m_yBounds = FloatArrayType::NullPointer();
  m_zBounds = FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if (data->getType() != 3)
  {
    // RectGridGeom can only accept cell Attribute Matrices
    return;
  }
  if (data->getNumTuples() != getNumberOfElements())
  {
    return;
  }
  if (data->getName().compare(name) != 0)
  {
    data->setName(name);
  }
  m_AttributeMatrices[name] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t RectGridGeom::getNumberOfElements()
{
  return (m_Dimensions[0] * m_Dimensions[1] * m_Dimensions[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::findElementsContainingVert()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer RectGridGeom::getElementsContainingVert()
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setElementsContainingVert(ElementDynamicList::Pointer DREAM3D_NOT_USED(elementsContainingVert))
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::deleteElementsContainingVert()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::findElementNeighbors()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ElementDynamicList::Pointer RectGridGeom::getElementNeighbors()
{
  return ElementDynamicList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setElementNeighbors(ElementDynamicList::Pointer DREAM3D_NOT_USED(elementNeighbors))
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::deleteElementNeighbors()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::findElementCentroids()
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatArrayType::Pointer RectGridGeom::getElementCentroids()
{
  return FloatArrayType::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::setElementCentroids(FloatArrayType::Pointer DREAM3D_NOT_USED(elementsCentroids))
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::deleteElementCentroids()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getParametricCenter(double pCoords[3])
{
  pCoords[0] = 0.5;
  pCoords[1] = 0.5;
  pCoords[2] = 0.5;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::getShapeFunctions(double pCoords[3], double* shape)
{
  double rm, sm, tm;

  rm = 1.0 - pCoords[0];
  sm = 1.0 - pCoords[1];
  tm = 1.0 - pCoords[2];

  // r derivatives
  shape[0] = -sm * tm;
  shape[1] = sm * tm;
  shape[2] = -pCoords[1] * tm;
  shape[3] = pCoords[1] * tm;
  shape[4] = -sm * pCoords[2];
  shape[5] = sm * pCoords[2];
  shape[6] = -pCoords[1] * pCoords[2];
  shape[7] = pCoords[1] * pCoords[2];

  // s derivatives
  shape[8] = -rm * tm;
  shape[9] = -pCoords[0] * tm;
  shape[10] = rm * tm;
  shape[11] = pCoords[0] * tm;
  shape[12] = -rm * pCoords[2];
  shape[13] = -pCoords[0] * pCoords[2];
  shape[14] = rm * pCoords[2];
  shape[15] = pCoords[0] * pCoords[2];

  // t derivatives
  shape[16] = -rm * sm;
  shape[17] = -pCoords[0] * sm;
  shape[18] = -rm * pCoords[1];
  shape[19] = -pCoords[0] * pCoords[1];
  shape[20] = rm * sm;
  shape[21] = pCoords[0] * sm;
  shape[22] = rm * pCoords[1];
  shape[23] = pCoords[0] * pCoords[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RectGridGeom::findDerivatives(DoubleArrayType::Pointer field, DoubleArrayType::Pointer derivatives)
{
  int inputComponent;
  double xp[3], xm[3], factor;
  xp[0] = xp[1] = xp[2] = xm[0] = xm[1] = xm[2] = factor = 0;
  double xxi, yxi, zxi, xeta, yeta, zeta, xzeta, yzeta, zzeta;
  xxi = yxi = zxi = xeta = yeta = zeta = xzeta = yzeta = zzeta = 0;
  double aj, xix, xiy, xiz, etax, etay, etaz, zetax, zetay, zetaz;
  aj = xix = xiy = xiz = etax = etay = etaz = zetax = zetay = zetaz = 0;
  size_t idx, idx2, tmpIdx, tmpIdx2;
  int numberOfInputComponents = field->getNumberOfComponents();
  double* fieldPtr = field->getPointer(0);
  double* derivsPtr = derivatives->getPointer(0);
  // for finite differencing -- the values on the "plus" side and
  // "minus" side of the point to be computed at
  std::vector<double> plusvalues(numberOfInputComponents);
  std::vector<double> minusvalues(numberOfInputComponents);

  std::vector<double> dValuesdXi(numberOfInputComponents);
  std::vector<double> dValuesdEta(numberOfInputComponents);
  std::vector<double> dValuesdZeta(numberOfInputComponents);

  size_t dims[3];
  getDimensions(dims);

  size_t xysize = dims[0] * dims[1];

  for (size_t z = 0; z < dims[2]; z++)
  {
    for (size_t y = 0; y < dims[1]; y++)
    {
      for (size_t x = 0; x < dims[0]; x++)
      {
        //  Xi derivatives.
        if ( dims[0] == 1 ) // 2D in this direction
        {
          factor = 1.0;
          for (size_t ii = 0; ii < 3; ii++)
          {
            xp[ii] = xm[ii] = 0.0;
          }
          xp[0] = 1.0;
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = minusvalues[inputComponent] = 0;
          }
        }
        else if ( x == 0 )
        {
          factor = 1.0;
          idx = (x + 1) + y * dims[0] + z * xysize;
          idx2 = x + y * dims[0] + z * xysize;
          tmpIdx = x + 1;
          getCoords(tmpIdx, y, z, xp);
          getCoords(x, y, z, xm);
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = fieldPtr[idx * numberOfInputComponents + inputComponent];
            minusvalues[inputComponent] = fieldPtr[idx2 * numberOfInputComponents + inputComponent];
          }
        }
        else if ( x == (dims[0] - 1) )
        {
          factor = 1.0;
          idx = x + y * dims[0] + z * xysize;
          idx2 = x - 1 + y * dims[0] + z * xysize;
          tmpIdx = x - 1;
          getCoords(x, y, z, xp);
          getCoords(tmpIdx, y, z, xm);
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = fieldPtr[idx * numberOfInputComponents + inputComponent];
            minusvalues[inputComponent] = fieldPtr[idx2 * numberOfInputComponents + inputComponent];
          }
        }
        else
        {
          factor = 0.5;
          idx = (x + 1) + y * dims[0] + z * xysize;
          idx2 = (x - 1) + y * dims[0] + z * xysize;
          tmpIdx = x + 1;
          tmpIdx2 = x - 1;
          getCoords(tmpIdx, y, z, xp);
          getCoords(tmpIdx2, y, z, xm);
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = fieldPtr[idx * numberOfInputComponents + inputComponent];
            minusvalues[inputComponent] = fieldPtr[idx2 * numberOfInputComponents + inputComponent];
          }
        }

        xxi = factor * (xp[0] - xm[0]);
        yxi = factor * (xp[1] - xm[1]);
        zxi = factor * (xp[2] - xm[2]);
        for(inputComponent = 0; inputComponent < numberOfInputComponents; inputComponent++)
        {
          dValuesdXi[inputComponent] = factor *
                                       (plusvalues[inputComponent] - minusvalues[inputComponent]);
        }

        //  Eta derivatives.
        if ( dims[1] == 1 ) // 2D in this direction
        {
          factor = 1.0;
          for (size_t ii = 0; ii < 3; ii++)
          {
            xp[ii] = xm[ii] = 0.0;
          }
          xp[1] = 1.0;
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = minusvalues[inputComponent] = 0;
          }
        }
        else if ( y == 0 )
        {
          factor = 1.0;
          idx = x + (y + 1) * dims[0] + z * xysize;
          idx2 = x + y * dims[0] + z * xysize;
          tmpIdx = y + 1;
          getCoords(x, tmpIdx, z, xp);
          getCoords(x, y, z, xm);
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = fieldPtr[idx * numberOfInputComponents + inputComponent];
            minusvalues[inputComponent] = fieldPtr[idx2 * numberOfInputComponents + inputComponent];
          }
        }
        else if ( y == (dims[1] - 1) )
        {
          factor = 1.0;
          idx = x + y * dims[0] + z * xysize;
          idx2 = x + (y - 1) * dims[0] + z * xysize;
          tmpIdx = y - 1;
          getCoords(x, y, z, xp);
          getCoords(x, tmpIdx, z, xm);
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = fieldPtr[idx * numberOfInputComponents + inputComponent];
            minusvalues[inputComponent] = fieldPtr[idx2 * numberOfInputComponents + inputComponent];
          }
        }
        else
        {
          factor = 0.5;
          idx = x + (y + 1) * dims[0] + z * xysize;
          idx2 = x + (y - 1) * dims[0] + z * xysize;
          tmpIdx = y + 1;
          tmpIdx2 = y - 1;
          getCoords(x, tmpIdx, z, xp);
          getCoords(x, tmpIdx2, z, xm);
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = fieldPtr[idx * numberOfInputComponents + inputComponent];
            minusvalues[inputComponent] = fieldPtr[idx2 * numberOfInputComponents + inputComponent];
          }
        }

        xeta = factor * (xp[0] - xm[0]);
        yeta = factor * (xp[1] - xm[1]);
        zeta = factor * (xp[2] - xm[2]);
        for(inputComponent = 0; inputComponent < numberOfInputComponents; inputComponent++)
        {
          dValuesdEta[inputComponent] = factor *
                                        (plusvalues[inputComponent] - minusvalues[inputComponent]);
        }

        //  Zeta derivatives.
        if ( dims[2] == 1 ) // 2D in this direction
        {
          factor = 1.0;
          for (size_t ii = 0; ii < 3; ii++)
          {
            xp[ii] = xm[ii] = 0.0;
          }
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = minusvalues[inputComponent] = 0;
          }
          xp[2] = 1.0;
        }
        else if ( z == 0 )
        {
          factor = 1.0;
          idx = x + y * dims[0] + (z + 1) * xysize;
          idx2 = x + y * dims[0] + z * xysize;
          tmpIdx = z + 1;
          getCoords(x, y, tmpIdx, xp);
          getCoords(x, y, z, xm);
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = fieldPtr[idx * numberOfInputComponents + inputComponent];
            minusvalues[inputComponent] = fieldPtr[idx2 * numberOfInputComponents + inputComponent];
          }
        }
        else if ( z == (dims[2] - 1) )
        {
          factor = 1.0;
          idx = x + y * dims[0] + z * xysize;
          idx2 = x + y * dims[0] + (z - 1) * xysize;
          tmpIdx = z - 1;
          getCoords(x, y, z, xp);
          getCoords(x, y, tmpIdx, xm);
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = fieldPtr[idx * numberOfInputComponents + inputComponent];
            minusvalues[inputComponent] = fieldPtr[idx2 * numberOfInputComponents + inputComponent];
          }
        }
        else
        {
          factor = 0.5;
          idx = x + y * dims[0] + (z + 1) * xysize;
          idx2 = x + y * dims[0] + (z - 1) * xysize;
          tmpIdx = z + 1;
          tmpIdx2 = z - 1;
          getCoords(x, y, tmpIdx, xp);
          getCoords(x, y, tmpIdx2, xm);
          for(inputComponent = 0; inputComponent < numberOfInputComponents;
              inputComponent++)
          {
            plusvalues[inputComponent] = fieldPtr[idx * numberOfInputComponents + inputComponent];
            minusvalues[inputComponent] = fieldPtr[idx2 * numberOfInputComponents + inputComponent];
          }
        }

        xzeta = factor * (xp[0] - xm[0]);
        yzeta = factor * (xp[1] - xm[1]);
        zzeta = factor * (xp[2] - xm[2]);
        for(inputComponent = 0; inputComponent < numberOfInputComponents; inputComponent++)
        {
          dValuesdZeta[inputComponent] = factor *
                                         (plusvalues[inputComponent] - minusvalues[inputComponent]);
        }

        // Now calculate the Jacobian.  Grids occasionally have
        // singularities, or points where the Jacobian is infinite (the
        // inverse is zero).  For these cases, we'll set the Jacobian to
        // zero, which will result in a zero derivative.
        //
        aj =  xxi * yeta * zzeta + yxi * zeta * xzeta + zxi * xeta * yzeta
              - zxi * yeta * xzeta - yxi * xeta * zzeta - xxi * zeta * yzeta;
        if (aj != 0.0)
        {
          aj = 1. / aj;
        }

        //  Xi metrics.
        xix  =  aj * (yeta * zzeta - zeta * yzeta);
        xiy  = -aj * (xeta * zzeta - zeta * xzeta);
        xiz  =  aj * (xeta * yzeta - yeta * xzeta);

        //  Eta metrics.
        etax = -aj * (yxi * zzeta - zxi * yzeta);
        etay =  aj * (xxi * zzeta - zxi * xzeta);
        etaz = -aj * (xxi * yzeta - yxi * xzeta);

        //  Zeta metrics.
        zetax =  aj * (yxi * zeta - zxi * yeta);
        zetay = -aj * (xxi * zeta - zxi * xeta);
        zetaz =  aj * (xxi * yeta - yxi * xeta);

        // Finally compute the actual derivatives
        idx = x + y * dims[0] + z * xysize;
        for(inputComponent = 0; inputComponent < numberOfInputComponents; inputComponent++)
        {
          derivsPtr[idx * numberOfInputComponents * 3 + inputComponent * 3] =
            xix * dValuesdXi[inputComponent] + etax * dValuesdEta[inputComponent] +
            zetax * dValuesdZeta[inputComponent];

          derivsPtr[idx * numberOfInputComponents * 3 + inputComponent * 3 + 1] =
            xiy * dValuesdXi[inputComponent] + etay * dValuesdEta[inputComponent] +
            zetay * dValuesdZeta[inputComponent];

          derivsPtr[idx * numberOfInputComponents * 3 + inputComponent * 3 + 2] =
            xiz * dValuesdXi[inputComponent] + etaz * dValuesdEta[inputComponent] +
            zetaz * dValuesdZeta[inputComponent];
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::writeGeometryToHDF5(hid_t parentId, bool DREAM3D_NOT_USED(writeXdmf))
{
  herr_t err = 0;
  int64_t volDims[3] =
  { static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints()) };

  int32_t rank = 1;
  hsize_t dims[1] = {3};

  err = H5Lite::writePointerDataset(parentId, H5_DIMENSIONS, rank, dims, volDims);
  if (err < 0)
  {
    return err;
  }
  if (m_xBounds.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_xBounds);
    if (err < 0)
    {
      return err;
    }
  }
  if (m_yBounds.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_yBounds);
    if (err < 0)
    {
      return err;
    }
  }
  if (m_zBounds.get() != NULL)
  {
    err = GeometryHelpers::GeomIO::WriteListToHDF5(parentId, m_zBounds);
    if (err < 0)
    {
      return err;
    }
  }

  err |= H5Gclose(parentId);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName)
{
  herr_t err = 0;

  int64_t volDims[3] =
  { static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints()) };

  out << "  <!-- *************** START OF " << dcName << " *************** -->" << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">" << "\n";
  out << "    <Topology TopologyType=\"3DRectMesh\" Dimensions=\"" << volDims[2] + 1 << " " << volDims[1] + 1 << " " << volDims[0] + 1 << " \"></Topology>" << "\n";
  out << "    <Geometry Type=\"VxVyVz\">" << "\n";
  out << "    <DataItem Format=\"HDF\" Dimensions=\"" << m_xBounds->getNumberOfTuples() << "\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  out << "      " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::xBoundsList << "\n";
  out << "    </DataItem>" << "\n";
  out << "    <DataItem Format=\"HDF\" Dimensions=\"" << m_yBounds->getNumberOfTuples() << "\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  out << "      " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::yBoundsList << "\n";
  out << "    </DataItem>" << "\n";
  out << "    <DataItem Format=\"HDF\" Dimensions=\"" << m_zBounds->getNumberOfTuples() << "\" NumberType=\"Float\" Precision=\"4\">" << "\n";
  out << "      " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::zBoundsList << "\n";
  out << "    </DataItem>" << "\n";
  out << "    </Geometry>" << "\n";

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RectGridGeom::getInfoString(DREAM3D::InfoStringFormat format)
{
  QString info;
  QTextStream ss (&info);

  int64_t volDims[3] =
  { static_cast<int64_t>(getXPoints()), static_cast<int64_t>(getYPoints()), static_cast<int64_t>(getZPoints()) };

  if(format == DREAM3D::HtmlFormat)
  {
    ss << "<tr bgcolor=\"#D3D8E0\"><th colspan=2>RectGrid Geometry Info</th></tr>";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Dimensions:</th><td>" << volDims[0] << " x " << volDims[1] << " x " << volDims[2] << "</td></tr>";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Resolution:</th><td>" << "Variable" << "</td></tr>";
  }
  else
  {

  }
  return info;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  int err = 0;
  size_t volDims[3] =
  { 0, 0, 0 };
  err = gatherMetaData(parentId, volDims, preflight);
  if (err < 0)
  {
    return err;
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer RectGridGeom::deepCopy()
{
  RectGridGeom::Pointer RectGridCopy = RectGridGeom::CreateGeometry(getName());

  size_t volDims[3] =
  { 0, 0, 0 };
  getDimensions(volDims);
  FloatArrayType::Pointer xBnds = getXBounds();
  FloatArrayType::Pointer yBnds = getYBounds();
  FloatArrayType::Pointer zBnds = getZBounds();
  RectGridCopy->setDimensions(volDims);
  RectGridCopy->setXBounds(xBnds);
  RectGridCopy->setYBounds(yBnds);
  RectGridCopy->setZBounds(zBnds);
  RectGridCopy->setSpatialDimensionality(getSpatialDimensionality());

  return RectGridCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RectGridGeom::gatherMetaData(hid_t parentId, size_t volDims[3], bool preflight)
{
  int err = QH5Lite::readPointerDataset(parentId, H5_DIMENSIONS, volDims);
  if(err < 0)
  {
    return err;
  }
  FloatArrayType::Pointer xBnds = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(DREAM3D::Geometry::xBoundsList, parentId, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer yBnds = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(DREAM3D::Geometry::yBoundsList, parentId, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  FloatArrayType::Pointer zBnds = GeometryHelpers::GeomIO::ReadListFromHDF5<FloatArrayType>(DREAM3D::Geometry::zBoundsList, parentId, preflight, err);
  if (err < 0 && err != -2)
  {
    return -1;
  }
  setDimensions(volDims[0], volDims[1], volDims[2]);
  setXBounds(xBnds);
  setYBounds(yBnds);
  setZBounds(zBnds);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME RectGridGeom
#include "DREAM3DLib/Geometry/SharedGeometryOps.cpp"
