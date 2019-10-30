/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 * SERVICES; LOSS OF USE, Data, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
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
#pragma once

#include <complex>
#include <vector>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Processing/ProcessingFilters/DetectEllipsoids.h"


class DetectEllipsoids;

using DE_ComplexDoubleVector = std::vector<std::complex<double>>;

/**
 * @brief The DetectEllipsoidsImpl class implements a threaded algorithm that detects ellipsoids in a FeatureIds array
 */
class DetectEllipsoidsImpl
{
public:
  DetectEllipsoidsImpl(int threadIndex, DetectEllipsoids* filter, int* cellFeatureIdsPtr, std::vector<size_t> cellFeatureIdsDims, UInt32ArrayType::Pointer corners, DE_ComplexDoubleVector convCoords_X,
                       DE_ComplexDoubleVector convCoords_Y, DE_ComplexDoubleVector convCoords_Z, std::vector<size_t> kernel_tDims, Int32ArrayType::Pointer convOffsetArray,
                       std::vector<double> smoothFil, Int32ArrayType::Pointer smoothOffsetArray, double axis_min, double axis_max, float tol_ellipse, float ba_min, DoubleArrayType::Pointer center,
                       DoubleArrayType::Pointer majaxis, DoubleArrayType::Pointer minaxis, DoubleArrayType::Pointer rotangle, AttributeMatrix::Pointer ellipseFeatureAM);

  virtual ~DetectEllipsoidsImpl();

  /**
   * @brief operator ()
   */
  void operator()() const;

  /**
   * @brief findEdges
   * @param imagePtr
   * @param image_tDims
   * @return
   */
  template <typename T>
  Int8ArrayType::Pointer findEdges(typename DataArray<T>::Pointer imagePtr, std::vector<size_t> image_tDims) const
  {
    Int8ArrayType::Pointer edgeArray = Int8ArrayType::CreateArray(image_tDims, std::vector<size_t>(1, 1), "Edge Array", true);
    edgeArray->initializeWithZeros();

    size_t xDim = image_tDims[0];
    size_t yDim = image_tDims[1];

    for(int y = 0; y < yDim; y++)
    {
      for(int x = 0; x < xDim; x++)
      {
        size_t index = (xDim * y) + x;
        T currentValue = imagePtr->getValue(index);
        if(currentValue != 0)
        {
          if((y - 1) >= 0)
          {
            int topIdx = (xDim * (y - 1)) + x;
            if(imagePtr->getValue(topIdx) == 0)
            {
              edgeArray->setValue(index, 1);
              continue;
            }
          }
          if((y + 1) < yDim)
          {
            int bottomIdx = (xDim * (y + 1)) + x;
            if(imagePtr->getValue(bottomIdx) == 0)
            {
              edgeArray->setValue(index, 1);
              continue;
            }
          }
          if((x - 1) >= 0)
          {
            int leftIdx = (xDim * y) + (x - 1);
            if(imagePtr->getValue(leftIdx) == 0)
            {
              edgeArray->setValue(index, 1);
              continue;
            }
          }
          if((x + 1) < xDim)
          {
            int rightIdx = (xDim * y) + (x + 1);
            if(imagePtr->getValue(rightIdx) == 0)
            {
              edgeArray->setValue(index, 1);
              continue;
            }
          }
        }
      }
    }

    return edgeArray;
  }

  /**
   * @brief convoluteImage
   * @param image
   * @param kernel
   * @param offsetArray
   * @param image_tDims
   * @return
   */
  template <typename T>
  std::vector<T> convoluteImage(DoubleArrayType::Pointer image, std::vector<T> kernel, Int32ArrayType::Pointer offsetArray, std::vector<size_t> image_tDims) const
  {
    std::vector<T> convArray;

    int* offsetArrayPtr = offsetArray->getPointer(0);
    double* imageArray = image->getPointer(0);
    int offsetArrayNumOfComps = offsetArray->getNumberOfComponents();

    T accumulator = 0;
    size_t xDim = image_tDims[0];
    size_t yDim = image_tDims[1];
    size_t zDim = 1; // 3DIM: This can be changed later to handle 3-dimensions
    int gradNumTuples = image->getNumberOfTuples();
    int reverseKernelCount = kernel.size();
    for(int i = 0; i < gradNumTuples; i++)
    {
      size_t imageCurrentX = 0, imageCurrentY = 0, imageCurrentZ = 0;
      m_Filter->ind2sub(image_tDims, i, imageCurrentX, imageCurrentY, imageCurrentZ);

      for(int j = 0; j < reverseKernelCount; j++)
      {
        int currCoord_X = imageCurrentX + offsetArrayPtr[j * offsetArrayNumOfComps];
        int currCoord_Y = imageCurrentY + offsetArrayPtr[(j * offsetArrayNumOfComps) + 1];
        int currCoord_Z = imageCurrentZ + offsetArrayPtr[(j * offsetArrayNumOfComps) + 2];

        if(currCoord_X >= 0)
        {
          if(currCoord_X < xDim)
          {
            if(currCoord_Y >= 0)
            {
              if(currCoord_Y < yDim)
              {
                if(currCoord_Z >= 0)
                {
                  if(currCoord_Z < zDim)
                  {
                    int gradIndex = (yDim * xDim * currCoord_Z) + (xDim * currCoord_Y) + currCoord_X;

                    T kernelVal = kernel[j];
                    double imageVal = imageArray[gradIndex];
                    T value = kernelVal * imageVal;
                    accumulator += value;
                  }
                }
              }
            }
          }
        }
      }

      convArray.push_back(accumulator);
      accumulator = 0;
    }

    return convArray;
  }

  /**
   * @brief findExtrema
   * @param thresholdArray
   * @param tDims
   * @return
   */
  QList<int> findExtrema(DoubleArrayType::Pointer thresholdArray, std::vector<size_t> tDims) const;

  /**
   * @brief plotlineEdgeInter
   * @param x0
   * @param y0
   * @param x1
   * @param y1
   * @param binImage
   * @param imageDims
   * @return
   */
  QPair<size_t, size_t> plotlineEdgeInter(int x0, int y0, int x1, int y1, DoubleArrayType::Pointer binImage, std::vector<size_t> imageDims) const;

  /**
   * @brief getIdOfMax
   * @param array
   * @return
   */
  template <typename T> int getIdOfMax(typename DataArray<T>::Pointer array) const
  {
    if(array.get() == nullptr)
    {
      return -1;
    }

    int arrayLength = array->getNumberOfTuples() * array->getNumberOfComponents();

    if(arrayLength <= 0)
    {
      return -1;
    }

    double maxId = 0;
    double maxValue = std::numeric_limits<T>::min();

    for(int i = 0; i < arrayLength; i++)
    {
      T value = array->getValue(i);
      if(value > maxValue)
      {
        maxValue = value;
        maxId = i;
      }
    }

    return maxId;
  }

  /**
   * @brief findNonZeroIndices
   * @param array
   * @param tDims
   * @return
   */
  template <typename T>
  SizeTArrayType::Pointer findNonZeroIndices(typename DataArray<T>::Pointer array, std::vector<size_t> tDims) const
  {
    int size = array->getNumberOfTuples();
    size_t xDim = tDims[0];
    size_t yDim = tDims[1];
    size_t zDim = 1; // 3DIM: This can be changed later to handle 3-dimensions
    typename DataArray<size_t>::Pointer indices = DataArray<size_t>::CreateArray(size, std::vector<size_t>(1, 2), "Non-Zero Indices", true);

    if(array.get() == nullptr)
    {
      return indices;
    }

    size_t count = 0;
    for(size_t x = 0; x < xDim; x++)
    {
      for(size_t y = 0; y < yDim; y++)
      {
        for(size_t z = 0; z < zDim; z++)
        {
          size_t index = (xDim * y) + x;
          T value = array->getValue(index);
          if(value != 0)
          {
            indices->setComponent(count, 0, x);
            indices->setComponent(count, 1, y);
            count++;
          }
        }
      }
    }

    indices->resizeTuples(count);
    return indices;
  }

  /**
   * @brief bitwiseMatrixCombination
   * @param matrix1
   * @param matrix2
   * @return
   */
  SizeTArrayType::Pointer bitwiseMatrixCombination(SizeTArrayType::Pointer matrix1, Int8ArrayType::Pointer matrix2) const;

  /**
   * @brief analyzeEdgePair
   * @param obj_edge_pair_a1
   * @param obj_edge_pair_b1
   * @param index
   * @param obj_tDims
   * @param obj_mask_edge
   * @param can_num
   * @param cenx_can
   * @param ceny_can
   * @param maj_can
   * @param min_can
   * @param rot_can
   * @param accum_can
   */
  void analyzeEdgePair(SizeTArrayType::Pointer obj_edge_pair_a1, SizeTArrayType::Pointer obj_edge_pair_b1, size_t index, std::vector<size_t> obj_tDims, Int8ArrayType::Pointer obj_mask_edge,
                       size_t& can_num, DoubleArrayType::Pointer cenx_can, DoubleArrayType::Pointer ceny_can, DoubleArrayType::Pointer maj_can, DoubleArrayType::Pointer min_can,
                       DoubleArrayType::Pointer rot_can, DoubleArrayType::Pointer accum_can) const;

private:
  DetectEllipsoids* m_Filter;
  int* m_CellFeatureIdsPtr;
  std::vector<size_t> m_CellFeatureIdsDims;
  UInt32ArrayType::Pointer m_Corners;
  DE_ComplexDoubleVector m_ConvCoords_X;
  DE_ComplexDoubleVector m_ConvCoords_Y;
  DE_ComplexDoubleVector m_ConvCoords_Z;
  std::vector<size_t> m_ConvKernel_tDims;
  Int32ArrayType::Pointer m_ConvOffsetArray;
  std::vector<double> m_SmoothKernel;
  Int32ArrayType::Pointer m_SmoothOffsetArray;
  double m_Axis_Min;
  double m_Axis_Max;
  float m_TolEllipse;
  float m_Ba_Min;
  DoubleArrayType::Pointer m_Center;
  DoubleArrayType::Pointer m_Majaxis;
  DoubleArrayType::Pointer m_Minaxis;
  DoubleArrayType::Pointer m_Rotangle;
  AttributeMatrix::Pointer m_EllipseFeatureAM;
  int m_ThreadIndex = 0;
};

