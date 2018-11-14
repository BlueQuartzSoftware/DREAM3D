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

#include "DetectEllipsoidsImpl.h"

#include "ProcessingFilters/HelperClasses/ComputeGradient.h"
#include "SIMPLib/Math/SIMPLibMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DetectEllipsoidsImpl::DetectEllipsoidsImpl(int threadIndex, DetectEllipsoids* filter, int* cellFeatureIdsPtr, QVector<size_t> cellFeatureIdsDims, UInt32ArrayType::Pointer corners,
                                           DE_ComplexDoubleVector convCoords_X, DE_ComplexDoubleVector convCoords_Y, DE_ComplexDoubleVector convCoords_Z, QVector<size_t> kernel_tDims,
                                           Int32ArrayType::Pointer convOffsetArray, std::vector<double> smoothFil, Int32ArrayType::Pointer smoothOffsetArray, double axis_min, double axis_max,
                                           float tol_ellipse, float ba_min, DoubleArrayType::Pointer center, DoubleArrayType::Pointer majaxis, DoubleArrayType::Pointer minaxis,
                                           DoubleArrayType::Pointer rotangle, AttributeMatrix::Pointer ellipseFeatureAM)
: m_Filter(filter)
, m_CellFeatureIdsPtr(cellFeatureIdsPtr)
, m_CellFeatureIdsDims(cellFeatureIdsDims)
, m_Corners(corners)
, m_ConvCoords_X(convCoords_X)
, m_ConvCoords_Y(convCoords_Y)
, m_ConvCoords_Z(convCoords_Z)
, m_ConvKernel_tDims(kernel_tDims)
, m_ConvOffsetArray(convOffsetArray)
, m_SmoothKernel(smoothFil)
, m_SmoothOffsetArray(smoothOffsetArray)
, m_Axis_Min(axis_min)
, m_Axis_Max(axis_max)
, m_TolEllipse(tol_ellipse)
, m_Ba_Min(ba_min)
, m_Center(center)
, m_Majaxis(majaxis)
, m_Minaxis(minaxis)
, m_Rotangle(rotangle)
, m_EllipseFeatureAM(ellipseFeatureAM)
, m_ThreadIndex(threadIndex)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DetectEllipsoidsImpl::~DetectEllipsoidsImpl() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetectEllipsoidsImpl::operator()() const
{
  // Initialize temporary arrays for candidate ellipse and accumulation counter
  DoubleArrayType::Pointer cenx_can = DoubleArrayType::CreateArray(10, QVector<size_t>(1, 1), "cenx_can"); // x-coordinate of ellipse
  for(int i = 0; i < cenx_can->getNumberOfTuples(); i++)
  {
    cenx_can->setComponent(i, 0, std::numeric_limits<double>::quiet_NaN());
  }

  DoubleArrayType::Pointer ceny_can = DoubleArrayType::CreateArray(10, QVector<size_t>(1, 1), "ceny_can"); // y-coordinate of ellipse
  for(int i = 0; i < ceny_can->getNumberOfTuples(); i++)
  {
    ceny_can->setComponent(i, 0, std::numeric_limits<double>::quiet_NaN());
  }

  DoubleArrayType::Pointer maj_can = DoubleArrayType::CreateArray(10, QVector<size_t>(1, 1), "maj_can"); // major semi-axis
  for(int i = 0; i < maj_can->getNumberOfTuples(); i++)
  {
    maj_can->setComponent(i, 0, std::numeric_limits<double>::quiet_NaN());
  }

  DoubleArrayType::Pointer min_can = DoubleArrayType::CreateArray(10, QVector<size_t>(1, 1), "min_can"); // minor semi-axis
  for(int i = 0; i < min_can->getNumberOfTuples(); i++)
  {
    min_can->setComponent(i, 0, std::numeric_limits<double>::quiet_NaN());
  }

  DoubleArrayType::Pointer rot_can = DoubleArrayType::CreateArray(10, QVector<size_t>(1, 1), "rot_can"); // Counter clockwise rotation from x-axis
  for(int i = 0; i < rot_can->getNumberOfTuples(); i++)
  {
    rot_can->setComponent(i, 0, std::numeric_limits<double>::quiet_NaN());
  }

  DoubleArrayType::Pointer accum_can = DoubleArrayType::CreateArray(10, QVector<size_t>(1, 1), "accum_can"); // Accumulation matrix
  for(int i = 0; i < accum_can->getNumberOfTuples(); i++)
  {
    accum_can->setComponent(i, 0, std::numeric_limits<double>::quiet_NaN());
  }

  // Run the ellipse detection algorithm on each object
  int32_t featureId = m_Filter->getNextFeatureId();
  while(featureId > 0)
  {
    size_t topL_X = m_Corners->getComponent(featureId, 0);
    size_t topL_Y = m_Corners->getComponent(featureId, 1);
    // size_t topL_Z = m_Corners->getComponent(featureId, 2);  // 3DIM: This can be changed later to handle 3-dimensions
    size_t bottomR_X = m_Corners->getComponent(featureId, 3);
    size_t bottomR_Y = m_Corners->getComponent(featureId, 4);
    // size_t bottomR_Z = m_Corners->getComponent(featureId, 5); // 3DIM: This can be changed later to handle 3-dimensions

    // Calculate the object's dimensions with a 1-pixel border around it
    size_t paddedObj_xDim = (bottomR_X + 1) - (topL_X - 1) + 1;
    size_t paddedObj_yDim = (bottomR_Y + 1) - (topL_Y - 1) + 1;
    // size_t paddedObj_zDim = (bottomR_Z+1) - (topL_Z-1) + 1; // 3DIM: This can be changed later to handle 3-dimensions

    QVector<size_t> paddedObj_tDims;
    paddedObj_tDims.push_back(paddedObj_xDim);
    paddedObj_tDims.push_back(paddedObj_yDim);
    // image_tDims.push_back(paddedObj_zDim);  // 3DIM: This can be changed later to handle 3-dimensions

    // Copy the feature id object into its own flattened 2D array called featureObjArray
    QVector<size_t> cDims(1, 1);
    DoubleArrayType::Pointer featureObjArray = DoubleArrayType::CreateArray(paddedObj_tDims, cDims, "featureObjArray");
    featureObjArray->initializeWithZeros();

    size_t z = 0; // 3DIM: This can be changed later to handle 3-dimensions
                  //    for (size_t z = topL_Z; z <= bottomR_Z; z++)  // 3DIM: This can be changed later to handle 3-dimensions
                  //    {
    for(size_t y = topL_Y; y <= bottomR_Y; y++)
    {
      for(size_t x = topL_X; x <= bottomR_X; x++)
      {
        size_t objX = x - topL_X;
        size_t objY = y - topL_Y;
        // size_t objZ = z - topL_Z; // 3DIM: This can be changed later to handle 3-dimensions
        // size_t objIndex = (paddedObj_yDim * paddedObj_xDim * objZ) + (paddedObj_xDim * (objY+1)) + (objX+1);  // 3DIM: This can be changed later to handle 3-dimensions
        size_t objIndex = m_Filter->sub2ind(paddedObj_tDims, objX + 1, objY + 1, z);
        // size_t imageIndex = (m_CellFeatureIdsDims[1] * m_CellFeatureIdsDims[0] * z) + (m_CellFeatureIdsDims[0] * y) + x;  // 3DIM: This can be changed later to handle 3-dimensions
        size_t imageIndex = m_Filter->sub2ind(m_CellFeatureIdsDims, x, y, z);
        double featureValue = m_CellFeatureIdsPtr[imageIndex];

        if(featureValue == featureId)
        {
          featureValue = 1.0;
        }
        else
        {
          featureValue = 0.0;
        }

        featureObjArray->setValue(objIndex, featureValue);
      }
    }
    //    }

    // Calculate the minimum pixel threshold
    double min_pix = std::round(SIMPLib::Constants::k_Pi * m_Axis_Min * m_Axis_Min / 2);

    // Find all indices of non-zero elements in the featureObjArray
    SizeTArrayType::Pointer objPixelsArray = findNonZeroIndices<double>(featureObjArray, paddedObj_tDims);

    size_t numberOfDetectedEllipses = 0; // Keep track of how many ellipses we've found in this object

    // Run this loop until the number of non-zero pixels in the object is less than the minimum pixel threshold
    while(objPixelsArray->getNumberOfTuples() > min_pix)
    {
      // Find the gradient matrix of the object
      ComputeGradient grad(featureObjArray, paddedObj_xDim, paddedObj_yDim);
      grad.compute();

      DoubleArrayType::Pointer gradX = grad.getGradX();
      DoubleArrayType::Pointer gradY = grad.getGradY();

      // Convolute Gradient of object with convolution kernel
      DE_ComplexDoubleVector gradX_conv = convoluteImage(gradX, m_ConvCoords_X, m_ConvOffsetArray, paddedObj_tDims);
      DE_ComplexDoubleVector gradY_conv = convoluteImage(gradY, m_ConvCoords_Y, m_ConvOffsetArray, paddedObj_tDims);

      // Calculate the magnitude matrix of the convolution.
      DoubleArrayType::Pointer obj_conv_mag = DoubleArrayType::CreateArray(gradX_conv.size(), QVector<size_t>(1, 1), "obj_conv_mag");
      for(int i = 0; i < gradX_conv.size(); i++)
      {
        std::complex<double> complexValue = gradX_conv[i] + gradY_conv[i];
        double value = std::abs(complexValue);
        obj_conv_mag->setValue(i, value);
      }

      // Smooth the magnitude matrix using a smoothing kernel.
      std::vector<double> obj_conv_mag_smooth = convoluteImage(obj_conv_mag, m_SmoothKernel, m_SmoothOffsetArray, paddedObj_tDims);
      double obj_conv_max = 0;
      for(int i = 0; i < obj_conv_mag_smooth.size(); i++)
      {
        // Find max peak to set threshold
        if(obj_conv_mag_smooth[i] > obj_conv_max)
        {
          obj_conv_max = obj_conv_mag_smooth[i];
        }
        obj_conv_mag->setValue(i, obj_conv_mag_smooth[i]);
      }

      // Create threshold matrix
      DoubleArrayType::Pointer obj_conv_thresh = DoubleArrayType::CreateArray(obj_conv_mag->getNumberOfTuples(), QVector<size_t>(1, 1), "obj_conv_thresh");
      obj_conv_thresh->initializeWithZeros();
      for(int i = 0; i < obj_conv_thresh->getNumberOfTuples(); i++)
      {
        // Copy values into threshold matrix that are greater than (0.7 * obj_conv_max) from magnitude matrix
        if(obj_conv_mag->getValue(i) > (0.7 * obj_conv_max))
        {
          double value = obj_conv_mag->getValue(i);
          obj_conv_thresh->setValue(i, value);
        }
      }

      // Find all local extrema coordinates in the threshold matrix
      QList<int> obj_ext_indices = findExtrema(obj_conv_thresh, paddedObj_tDims);
      int obj_ext_num = obj_ext_indices.size();

      // Only process a maximum of 3 sub-objects
      if(obj_ext_num > 3)
      {
        obj_ext_num = 3;
      }

      // Calculate mask radius
      size_t mask_rad = m_Axis_Max + m_Axis_Min;

      int detectedObjIdx = 0;
      // For each sub-object...
      for(; detectedObjIdx < obj_ext_num; detectedObjIdx++)
      {
        size_t obj_ext_index = obj_ext_indices[detectedObjIdx];

        // Get x,y coordinates of current extrema value
        size_t obj_ext_x = 0, obj_ext_y = 0, obj_ext_z = 0;
        m_Filter->ind2sub(paddedObj_tDims, obj_ext_index, obj_ext_y, obj_ext_x, obj_ext_z);

        // Calculate mask array of the sub-object by finding min and max x/y values
        int mask_min_x = obj_ext_x - mask_rad + 1;
        if(mask_min_x < 1)
        {
          mask_min_x = 1;
        }

        int mask_min_y = obj_ext_y - mask_rad + 1;
        if(mask_min_y < 1)
        {
          mask_min_y = 1;
        }

        int mask_max_x = obj_ext_x + mask_rad + 1;
        if(mask_max_x > paddedObj_tDims[1])
        {
          mask_max_x = paddedObj_tDims[1];
        }

        int mask_max_y = obj_ext_y + mask_rad + 1;
        if(mask_max_y > paddedObj_tDims[0])
        {
          mask_max_y = paddedObj_tDims[0];
        }

        // Create and populate mask array of the sub-object
        DoubleArrayType::Pointer obj_mask = DoubleArrayType::CreateArray(paddedObj_tDims, QVector<size_t>(1, 1), "obj_mask");
        obj_mask->initializeWithZeros();

        for(size_t y = mask_min_y - 1; y < mask_max_y; y++)
        {
          for(size_t x = mask_min_x - 1; x < mask_max_x; x++)
          {
            size_t index = m_Filter->sub2ind(paddedObj_tDims, y, x, z);
            obj_mask->setValue(index, featureObjArray->getValue(index));
          }
        }

        // Compute the edge matrix of the sub-object
        Int8ArrayType::Pointer edgeArray = findEdges<double>(obj_mask, paddedObj_tDims);

        SizeTArrayType::Pointer obj_edges = findNonZeroIndices<int8_t>(edgeArray, paddedObj_tDims);

        SizeTArrayType::Pointer obj_edge_pair_a = SizeTArrayType::CreateArray(obj_edges->getNumberOfTuples(), QVector<size_t>(1, 1), "obj_edge_pair_a");
        SizeTArrayType::Pointer obj_edge_pair_b = SizeTArrayType::CreateArray(obj_edges->getNumberOfTuples(), QVector<size_t>(1, 1), "obj_edge_pair_b");
        SizeTArrayType::Pointer obj_edge_pair_a1 = SizeTArrayType::CreateArray(obj_edges->getNumberOfTuples(), QVector<size_t>(1, 2), "obj_edge_pair_a1");
        SizeTArrayType::Pointer obj_edge_pair_b1 = SizeTArrayType::CreateArray(obj_edges->getNumberOfTuples(), QVector<size_t>(1, 2), "obj_edge_pair_b1");

        // Determine edge pairs that will be used to analyze if the sub-object is an ellipse
        int count = 0;
        for(int j = 0; j < obj_edges->getNumberOfTuples(); j++)
        {
          QPair<size_t, size_t> edgeIndices = plotlineEdgeInter(obj_ext_x, obj_ext_y, obj_edges->getComponent(j, 1), obj_edges->getComponent(j, 0), featureObjArray, paddedObj_tDims);

          if(edgeIndices.first != 0 && edgeIndices.second != 0)
          {
            obj_edge_pair_a->setValue(count, edgeIndices.first);
            obj_edge_pair_b->setValue(count, edgeIndices.second);

            size_t x1 = 0, y1 = 0, z1 = 0, x2 = 0, y2 = 0, z2 = 0;
            m_Filter->ind2sub(paddedObj_tDims, edgeIndices.first, x1, y1, z1);
            m_Filter->ind2sub(paddedObj_tDims, edgeIndices.second, x2, y2, z2);

            // Store the edge pair x,y coordinates
            obj_edge_pair_a1->setComponent(count, 0, x1);
            obj_edge_pair_a1->setComponent(count, 1, y1);
            obj_edge_pair_b1->setComponent(count, 0, x2);
            obj_edge_pair_b1->setComponent(count, 1, y2);
            count++;
          }
        }
        obj_edge_pair_a->resize(count);
        obj_edge_pair_b->resize(count);
        obj_edge_pair_a1->resize(count);
        obj_edge_pair_b1->resize(count);

        // Analyze each edge pair using an accumulation array to gain votes to help determine that the sub-object is an ellipse
        size_t can_num = 0;
        for(int k = 0; k < obj_edge_pair_a1->getNumberOfTuples(); k++)
        {
          analyzeEdgePair(obj_edge_pair_a1, obj_edge_pair_b1, k, paddedObj_tDims, edgeArray, can_num, cenx_can, ceny_can, maj_can, min_can, rot_can, accum_can);
        }

        // If the sub-object has enough votes, it is found to be an ellipse
        if(can_num > 0) // Assume best match is the ellipse
        {
          // Increment the ellipse counter
          m_Filter->setEllipse_Count(m_Filter->getEllipse_Count() + 1);

          // Get the index into the ellipse value arrays that has the most votes
          int accum_idx = getIdOfMax<double>(accum_can);

          /* If this is another ellipse in the same overall object,
           * create a new feature id and resize our output arrays */
          size_t objId = featureId;
          if(numberOfDetectedEllipses > 0)
          {
            objId = m_Filter->getUniqueFeatureId();
            m_EllipseFeatureAM->resizeAttributeArrays(QVector<size_t>(1, objId + 1));
          }

          double cenx_val = cenx_can->getValue(accum_idx);
          double ceny_val = ceny_can->getValue(accum_idx);
          double majaxis_val = maj_can->getValue(accum_idx);
          double minaxis_val = min_can->getValue(accum_idx);

          double rotangle_val = rot_can->getValue(accum_idx);

          // Convert rotational angle until it is within -pi/2 and pi/2
          while(rotangle_val > SIMPLib::Constants::k_PiOver2 || rotangle_val < -SIMPLib::Constants::k_PiOver2)
          {
            if(rotangle_val > SIMPLib::Constants::k_PiOver2)
            {
              rotangle_val = rotangle_val - SIMPLib::Constants::k_Pi;
            }
            else
            {
              rotangle_val = rotangle_val + SIMPLib::Constants::k_Pi;
            }
          }

          // Store ellipse parameters
          m_Center->setComponent(objId, 0, cenx_val);
          m_Center->setComponent(objId, 1, ceny_val);
          m_Majaxis->setValue(objId, majaxis_val);
          m_Minaxis->setValue(objId, minaxis_val);
          m_Rotangle->setValue(objId, rotangle_val);

          // Remove the sub-object from the feature id object's 2D array
          Int32ArrayType::Pointer featureObjOnesArray = Int32ArrayType::CreateArray(paddedObj_tDims, QVector<size_t>(1, 1), "featureObjOnesArray");
          featureObjOnesArray->initializeWithValue(1);

          Int32ArrayType::Pointer I_tmp = m_Filter->fillEllipse(featureObjOnesArray, paddedObj_tDims, cenx_val, ceny_val, majaxis_val + 1, minaxis_val + 1, rotangle_val, 0);

          for(int i = 0; i < I_tmp->getNumberOfTuples(); i++)
          {
            double value = featureObjArray->getValue(i) * I_tmp->getValue(i);
            featureObjArray->setValue(i, value);
          }

          // Translate center of ellipse into feature id array coordinates from object coordinates
          size_t obj_x_min = topL_Y;
          size_t obj_y_min = topL_X;

          m_Center->setComponent(objId, 0, m_Center->getComponent(objId, 0) + obj_x_min);
          m_Center->setComponent(objId, 1, m_Center->getComponent(objId, 1) + obj_y_min);

          // Clear Accumulator
          accum_can->initializeWithZeros();

          // Find all indices of non-zero elements in the featureObjArray
          objPixelsArray = findNonZeroIndices<double>(featureObjArray, paddedObj_tDims);

          // We have found an ellipse, so increment the counter
          numberOfDetectedEllipses++;
          break;
        }
      }

      if(detectedObjIdx == obj_ext_num)
      {
        break;
      }

      if(m_Filter->getCancel())
      {
        return;
      }
    }

    // Notify the user interface that the feature is completed
    m_Filter->notifyFeatureCompleted(featureId, m_ThreadIndex);

    featureId = m_Filter->getNextFeatureId();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<int> DetectEllipsoidsImpl::findExtrema(DoubleArrayType::Pointer thresholdArray, QVector<size_t> tDims) const
{
  QSet<int> extrema;
  size_t xDim = tDims[0];
  size_t yDim = tDims[1];

  QSet<int> extremaCols;
  QList<int> extremaCol_flatIndices;

  // Step 1: Find and store the row number of each column's peak

  size_t z = 0; // 3DIM: This can be changed later to handle 3-dimensions
  for(int x = 0; x < xDim; x++)
  {
    double value = 0;
    int extrema_Y = -1;
    int flat_index = -1;
    for(int y = 0; y < yDim; y++)
    {
      size_t index = m_Filter->sub2ind(tDims, x, y, z);
      double threshValue = thresholdArray->getValue(index);
      if(threshValue > value)
      {
        extrema_Y = y;
        value = threshValue;
        flat_index = index;
      }
    }
    if(extrema_Y >= 0)
    {
      extremaCols.insert(extrema_Y);
      extremaCol_flatIndices.push_back(flat_index);
    }
  }

  // Find and store peaks in rows that were found in Step 1
  QList<int> extremaRow_flatIndices;
  QList<int> extremaCols_list = extremaCols.toList();
  int extremaColSize = extremaCols_list.size();
  for(int i = 0; i < extremaColSize; i++)
  {
    int y = extremaCols_list[i];
    size_t extremaIndex = m_Filter->sub2ind(tDims, 0, y, z);
    for(int x = 1; x < xDim; x++)
    {
      size_t index = m_Filter->sub2ind(tDims, x, y, z);
      if(thresholdArray->getValue(index) > thresholdArray->getValue(extremaIndex))
      {
        extremaIndex = index;
      }
    }
    extremaRow_flatIndices.push_back(extremaIndex);
  }

  // Peaks in rows and in columns (intersection):
  QSet<int> rcIntersection;
  int extremaColFlatIndicesSize = extremaCol_flatIndices.size();
  for(int i = 0; i < extremaColFlatIndicesSize; i++)
  {
    if(extremaRow_flatIndices.contains(extremaCol_flatIndices[i]))
    {
      rcIntersection.insert(extremaCol_flatIndices[i]);
      extremaRow_flatIndices.removeAll(extremaCol_flatIndices[i]);
    }
  }

  int extremaRowFlatIndicesSize = extremaRow_flatIndices.size();
  for(int i = 0; i < extremaRowFlatIndicesSize; i++)
  {
    if(extremaCol_flatIndices.contains(extremaRow_flatIndices[i]))
    {
      rcIntersection.insert(extremaRow_flatIndices[i]);
      extremaRow_flatIndices.removeAll(extremaCol_flatIndices[i]);
    }
  }

  // Peaks through diagonals
  QList<int> rcIntersectionList = rcIntersection.toList();
  int rcIntersectionListSize = rcIntersectionList.size();
  for(int i = 0; i < rcIntersectionListSize; i++)
  {
    int extremaIndex = rcIntersectionList[i];
    size_t x = 0, y = 0, z = 0;
    m_Filter->ind2sub(tDims, extremaIndex, x, y, z);

    // Top left direction
    int xDiag = x - 1;
    int yDiag = y - 1;
    while(xDiag >= 0 && yDiag >= 0)
    {
      size_t diagIndex = m_Filter->sub2ind(tDims, xDiag, yDiag, z);
      if(thresholdArray->getValue(diagIndex) > thresholdArray->getValue(extremaIndex))
      {
        extremaIndex = diagIndex;
      }
      xDiag--;
      yDiag--;
    }

    // Bottom right direction
    xDiag = x + 1;
    yDiag = y + 1;
    while(xDiag < xDim && yDiag < yDim)
    {
      size_t diagIndex = m_Filter->sub2ind(tDims, xDiag, yDiag, z);
      if(thresholdArray->getValue(diagIndex) > thresholdArray->getValue(extremaIndex))
      {
        extremaIndex = diagIndex;
      }
      xDiag++;
      yDiag++;
    }

    // Bottom left direction
    xDiag = x - 1;
    yDiag = y + 1;
    while(xDiag >= 0 && yDiag < yDim)
    {
      size_t diagIndex = m_Filter->sub2ind(tDims, xDiag, yDiag, z);
      if(thresholdArray->getValue(diagIndex) > thresholdArray->getValue(extremaIndex))
      {
        extremaIndex = diagIndex;
      }
      xDiag--;
      yDiag++;
    }

    // Top right direction
    xDiag = x + 1;
    yDiag = y - 1;
    while(xDiag < xDim && yDiag >= 0)
    {
      size_t diagIndex = m_Filter->sub2ind(tDims, xDiag, yDiag, z);
      if(thresholdArray->getValue(diagIndex) > thresholdArray->getValue(extremaIndex))
      {
        extremaIndex = diagIndex;
      }
      xDiag++;
      yDiag--;
    }

    extrema.insert(extremaIndex);
  }

  QList<int> extremaList = extrema.toList();
  std::sort(extremaList.begin(), extremaList.end());

  return extremaList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPair<size_t, size_t> DetectEllipsoidsImpl::plotlineEdgeInter(int x0, int y0, int x1, int y1, DoubleArrayType::Pointer binImage, QVector<size_t> imageDims) const
{
  QPair<size_t, size_t> edge;

  // Store Initial Point
  size_t xi = x0;
  size_t yi = y0;

  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);

  int sx, sy;

  if(x0 < x1)
  {
    sx = 1;
  }
  else
  {
    sx = -1;
  }

  if(y0 < y1)
  {
    sy = 1;
  }
  else
  {
    sy = -1;
  }

  int err = dx - dy;
  int e2;

  if(err == 0)
  {
    edge.first = 0;
    edge.second = 0;
    return edge;
  }

  size_t z = 0; // 3DIM: This can be changed later to handle 3-dimensions

  // Search forward
  while(true)
  {
    if(binImage->getValue(m_Filter->sub2ind(imageDims, y0, x0, z)) == 0)
    {
      if(x0 == x1 && y0 == y1)
      {
        edge.first = m_Filter->sub2ind(imageDims, y1, x1, z);
        break;
      }
      if(x0 == x1 && y0 + 1 == y1)
      {
        edge.first = m_Filter->sub2ind(imageDims, y1, x1, z);
        break;
      }
      if(x0 + 1 == x1 && y0 + 1 == y1)
      {
        edge.first = m_Filter->sub2ind(imageDims, y1, x1, z);
        break;
      }
      if(x0 + 1 == x1 && y0 == y1)
      {
        edge.first = m_Filter->sub2ind(imageDims, y1, x1, z);
        break;
      }
      else if(x0 + 1 == x1 && y0 - 1 == y1)
      {
        edge.first = m_Filter->sub2ind(imageDims, y1, x1, z);
        break;
      }
      else if(x0 == x1 && y0 - 1 == y1)
      {
        edge.first = m_Filter->sub2ind(imageDims, y1, x1, z);
        break;
      }
      else if(x0 - 1 == x1 && y0 - 1 == y1)
      {
        edge.first = m_Filter->sub2ind(imageDims, y1, x1, z);
        break;
      }
      else if(x0 - 1 == x1 && y0 == y1)
      {
        edge.first = m_Filter->sub2ind(imageDims, y1, x1, z);
        break;
      }
      else if(x0 - 1 == x1 && y0 + 1 == y1)
      {
        edge.first = m_Filter->sub2ind(imageDims, y1, x1, z);
        break;
      }
      else
      {
        edge.first = 0;
        edge.second = 0;
        return edge;
      }
    }

    e2 = 2 * err;

    if(e2 > -dy)
    {
      err -= dy;
      x0 += sx;
    }

    if(e2 < dx)
    {
      err += dx;
      y0 += sy;
    }
  }

  // Reverse direction!
  x0 = xi;
  y0 = yi;

  if(x0 > x1)
  {
    sx = 1;
  }
  else
  {
    sx = -1;
  }

  if(y0 > y1)
  {
    sy = 1;
  }
  else
  {
    sy = -1;
  }

  err = dx - dy;

  while(true)
  {
    if(binImage->getComponent(m_Filter->sub2ind(imageDims, y0, x0, z), 0) == 0)
    {
      edge.second = m_Filter->sub2ind(imageDims, y0, x0, z);
      break;
    }

    e2 = 2 * err;

    if(e2 > -dy)
    {
      err -= dy;
      x0 += sx;
    }

    if(e2 < dx)
    {
      err += dx;
      y0 += sy;
    }
  }

  return edge;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SizeTArrayType::Pointer DetectEllipsoidsImpl::bitwiseMatrixCombination(SizeTArrayType::Pointer matrix1, Int8ArrayType::Pointer matrix2) const
{
  SizeTArrayType::Pointer result;

  if(matrix1->getNumberOfTuples() != matrix2->getNumberOfTuples() || matrix1->getNumberOfComponents() != matrix2->getNumberOfComponents())
  {
    result = nullptr;
  }
  else
  {
    result = SizeTArrayType::CreateArray(matrix1->getNumberOfTuples(), matrix1->getComponentDimensions(), "Bitwise Matrix Combination");

    for(int y = 0; y < matrix1->getNumberOfTuples(); y++)
    {
      for(int x = 0; x < matrix1->getNumberOfComponents(); x++)
      {
        result->setComponent(y, x, matrix1->getComponent(y, x) & matrix2->getComponent(y, x));
      }
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetectEllipsoidsImpl::analyzeEdgePair(SizeTArrayType::Pointer obj_edge_pair_a1, SizeTArrayType::Pointer obj_edge_pair_b1, size_t index, QVector<size_t> obj_tDims,
                                           Int8ArrayType::Pointer obj_mask_edge, size_t& can_num, DoubleArrayType::Pointer cenx_can, DoubleArrayType::Pointer ceny_can,
                                           DoubleArrayType::Pointer maj_can, DoubleArrayType::Pointer min_can, DoubleArrayType::Pointer rot_can, DoubleArrayType::Pointer accum_can) const
{
  const int daxis = m_Axis_Max - m_Axis_Min;
  int axis_min_sq = m_Axis_Min * m_Axis_Min;

  // Matlab is column-based, so x & y are swapped
  double dobj_x = obj_tDims[1];
  double dobj_y = obj_tDims[0];

  double x1 = obj_edge_pair_a1->getComponent(index, 1);
  double y1 = obj_edge_pair_a1->getComponent(index, 0);

  double x2 = obj_edge_pair_b1->getComponent(index, 1);
  double y2 = obj_edge_pair_b1->getComponent(index, 0);

  double x0 = (x1 + x2) / 2;
  double y0 = (y1 + y2) / 2;
  double a = sqrt(std::pow((x2 - x1), 2) + std::pow((y2 - y1), 2)) / 2;
  double alpha = atan2(y2 - y1, x2 - x1);

  SizeTArrayType::Pointer accum = SizeTArrayType::CreateArray(daxis, QVector<size_t>(1, 1), "Accumulator");
  accum->initializeWithZeros();

  if(a >= m_Axis_Min && a <= m_Axis_Max)
  {
    for(int k = 0; k < obj_edge_pair_a1->getNumberOfTuples(); k++)
    {

      double x3 = obj_edge_pair_a1->getComponent(k, 1);
      double y3 = obj_edge_pair_a1->getComponent(k, 0);

      double dsq = std::pow((x3 - x0), 2) + std::pow((y3 - y0), 2);
      double asq = a * a;

      if(dsq > axis_min_sq && dsq < asq)
      {
        double d = sqrt(dsq);
        double fsq = std::pow((x3 - x2), 2) + std::pow((y3 - y2), 2);

        double costau = (asq + dsq - fsq) / (2 * a * d);

        double costau_sq = costau * costau;
        double sintau_sq = 1 - costau_sq;

        double bsq = (asq * dsq * sintau_sq) / (asq - dsq * costau_sq);

        double b = sqrt(bsq);

        // Add one to count from one
        int bidx = static_cast<int>(std::round(b) - m_Axis_Min);

        if(bidx <= daxis && bidx > 0)
        {
          size_t value = accum->getValue(bidx);
          accum->setValue(bidx, value + 1);
        }
      }
    }

    int accum_idx = getIdOfMax<size_t>(accum);
    double accum_max = accum->getValue(accum_idx);

    if(accum_max > 5)
    {
      double b = accum_idx + m_Axis_Min;

      if(b / a > m_Ba_Min)
      {
        // Draw ellipse and compare to object
        size_t count = 0;
        DoubleArrayType::Pointer ellipseCoords = m_Filter->plotEllipsev2(std::round(x0), std::round(y0), std::round(a), std::round(b), alpha, count);

        // create I_check as a 2D array and assign all values to zero
        QVector<size_t> I_check_dims;
        I_check_dims.push_back(dobj_y);
        I_check_dims.push_back(dobj_x);

        SizeTArrayType::Pointer I_check = SizeTArrayType::CreateArray(I_check_dims, QVector<size_t>(1, 1), "I_check");
        I_check->initializeWithZeros();

        for(int k = 0; k < ellipseCoords->getNumberOfTuples(); k++)
        {
          double x = ellipseCoords->getComponent(k, 0);
          double y = ellipseCoords->getComponent(k, 1);
          double z = 0; // 3DIM: This can be changed later to handle 3-dimensions

          if(x >= 0 && x < dobj_x && y >= 0 && y < dobj_y)
          {
            size_t index = m_Filter->sub2ind(I_check_dims, y, x, z);
            I_check->setValue(index, 1);

            if(x + 1 < dobj_x)
            {
              index = m_Filter->sub2ind(I_check_dims, y, x + 1, z);
              I_check->setValue(index, 1);
            }
            if(x - 1 >= 0)
            {
              index = m_Filter->sub2ind(I_check_dims, y, x - 1, z);
              I_check->setValue(index, 1);
            }
            if(y + 1 < dobj_y)
            {
              index = m_Filter->sub2ind(I_check_dims, y + 1, x, z);
              I_check->setValue(index, 1);
            }
            if(y - 1 >= 0)
            {
              index = m_Filter->sub2ind(I_check_dims, y - 1, x, z);
              I_check->setValue(index, 1);
            }
            if(x + 1 < dobj_x && y + 1 < dobj_y)
            {
              index = m_Filter->sub2ind(I_check_dims, y + 1, x + 1, z);
              I_check->setValue(index, 1);
            }
            if(x - 1 >= 0 && y + 1 < dobj_y)
            {
              index = m_Filter->sub2ind(I_check_dims, y + 1, x - 1, z);
              I_check->setValue(index, 1);
            }
            if(x + 1 < dobj_x && y - 1 >= 0)
            {
              index = m_Filter->sub2ind(I_check_dims, y - 1, x + 1, z);
              I_check->setValue(index, 1);
            }
            if(x - 1 >= 0 && y - 1 >= 0)
            {
              index = m_Filter->sub2ind(I_check_dims, y - 1, x - 1, z);
              I_check->setValue(index, 1);
            }
          }
        }

        SizeTArrayType::Pointer combinedMatrix = bitwiseMatrixCombination(I_check, obj_mask_edge);
        SizeTArrayType::Pointer overlap = findNonZeroIndices<size_t>(combinedMatrix, I_check_dims);

        // Estimate perimeter length using Ramanujan'a approximation.
        double perim = SIMPLib::Constants::k_Pi * (3 * (a + b) - sqrt((3 * a + b) * (a + 3 * b)));
        // Calculate pixel tolerance based on
        // the calculated perimeter
        double tol_pix = std::round(perim * m_TolEllipse);

        if(overlap->getNumberOfTuples() > tol_pix)
        {
          // Accept point as a new candidate
          if(can_num >= cenx_can->getNumberOfTuples())
          {
            cenx_can->resize(cenx_can->getNumberOfTuples() + 5);
            ceny_can->resize(cenx_can->getNumberOfTuples() + 5);
            maj_can->resize(cenx_can->getNumberOfTuples() + 5);
            min_can->resize(cenx_can->getNumberOfTuples() + 5);
            rot_can->resize(cenx_can->getNumberOfTuples() + 5);
            accum_can->resize(cenx_can->getNumberOfTuples() + 5);
          }

          cenx_can->setComponent(can_num, 0, std::round(x0)); // x - coordinate of ellipse
          ceny_can->setComponent(can_num, 0, std::round(y0)); // y - coordinate of ellipse
          maj_can->setComponent(can_num, 0, std::round(a));   // major semi - axis
          min_can->setComponent(can_num, 0, std::round(b));   // minor semi - axis
          rot_can->setComponent(can_num, 0, alpha);           // Counter clockwise rotation from x - axis
          accum_can->setComponent(can_num, 0, accum_max);     // Accumulation matrix
          can_num++;
        }
      }
    }
  }
}
