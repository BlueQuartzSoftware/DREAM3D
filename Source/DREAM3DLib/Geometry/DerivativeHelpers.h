/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _DerivativeHelpers_H_
#define _DerivativeHelpers_H_

#include <math.h>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "Eigen/LU"
#include "Eigen/Eigenvalues"

class QuadGeom;

/**
* @brief This file contains a namespace with classes for computing derivatives on IGeometry objects
*/
namespace DerivativeHelpers
{

  /**
   * @brief QuadJacobian
   */
  typedef Eigen::Matrix<float,2,2,Eigen::RowMajor> QuadJacobian;

  /**
   * @brief The QuadDeriv class
   */
  class QuadDeriv
  {
    public:
      QuadDeriv() {}
      virtual ~QuadDeriv() {}

      void operator()(QuadGeom* quads, int64_t quadId, float* values, float* derivs, int dim);

      int InvertMatrix(float** A, float** AI, int size)
      {
        int *index, iScratch[10];
        float *column, dScratch[10];

        // Check on allocation of working vectors
        //
        if ( size <= 10 )
          {
          index = iScratch;
          column = dScratch;
          }
        else
          {
          index = new int[size];
          column = new float[size];
          }

        int retVal = DerivativeHelpers::QuadDeriv::InvertMatrixF(A, AI, size, index, column);

        if ( size > 10 )
          {
          delete [] index;
          delete [] column;
          }

        return retVal;
      }

      int InvertMatrixF(float** A, float** AI, int size, int* tmp1Size, float* tmp2Size)
      {
        int i, j;

        //
        // Factor matrix; then begin solving for inverse one column at a time.
        // Note: tmp1Size returned value is used later, tmp2Size is just working
        // memory whose values are not used in LUSolveLinearSystem
        //
        if ( DerivativeHelpers::QuadDeriv::LUFactorLinearSystem(A, tmp1Size, size, tmp2Size) == 0 )
          {
          return 0;
          }

        for ( j=0; j < size; j++ )
          {
          for ( i=0; i < size; i++ )
            {
            tmp2Size[i] = 0.0;
            }
          tmp2Size[j] = 1.0;

         DerivativeHelpers::QuadDeriv::LUSolveLinearSystem(A,tmp1Size,tmp2Size,size);

          for ( i=0; i < size; i++ )
            {
            AI[i][j] = tmp2Size[i];
            }
          }

        return 1;
      }

      int LUFactorLinearSystem(float** A, int* index, int size, float* tmpSize)
      {
        int i, j, k;
        int maxI = 0;
        double largest, temp1, temp2, sum;

        //
        // Loop over rows to get implicit scaling information
        //
        for ( i = 0; i < size; i++ )
          {
          for ( largest = 0.0, j = 0; j < size; j++ )
            {
            if ( (temp2 = fabs(A[i][j])) > largest )
              {
              largest = temp2;
              }
            }

          if ( largest == 0.0 )
            {
            //vtkGenericWarningMacro(<<"Unable to factor linear system");
            return 0;
            }
            tmpSize[i] = 1.0 / largest;
          }
        //
        // Loop over all columns using Crout's method
        //
        for ( j = 0; j < size; j++ )
          {
          for (i = 0; i < j; i++)
            {
            sum = A[i][j];
            for ( k = 0; k < i; k++ )
              {
              sum -= A[i][k] * A[k][j];
              }
            A[i][j] = sum;
            }
          //
          // Begin search for largest pivot element
          //
          for ( largest = 0.0, i = j; i < size; i++ )
            {
            sum = A[i][j];
            for ( k = 0; k < j; k++ )
              {
              sum -= A[i][k] * A[k][j];
              }
            A[i][j] = sum;

            if ( (temp1 = tmpSize[i]*fabs(sum)) >= largest )
              {
              largest = temp1;
              maxI = i;
              }
            }
          //
          // Check for row interchange
          //
          if ( j != maxI )
            {
            for ( k = 0; k < size; k++ )
              {
              temp1 = A[maxI][k];
              A[maxI][k] = A[j][k];
              A[j][k] = temp1;
              }
            tmpSize[maxI] = tmpSize[j];
            }
          //
          // Divide by pivot element and perform elimination
          //
          index[j] = maxI;

          if ( fabs(A[j][j]) <= 1.0e-12 )
            {
            //vtkGenericWarningMacro(<<"Unable to factor linear system");
            return 0;
            }

          if ( j != (size-1) )
            {
            temp1 = 1.0 / A[j][j];
            for ( i = j + 1; i < size; i++ )
              {
              A[i][j] *= temp1;
              }
            }
          }

        return 1;
      }

      void LUSolveLinearSystem(float** A, int* index, float* x, int size)
      {
        int i, j, ii, idx;
        double sum;
      //
      // Proceed with forward and backsubstitution for L and U
      // matrices.  First, forward substitution.
      //
        for ( ii = -1, i = 0; i < size; i++ )
          {
          idx = index[i];
          sum = x[idx];
          x[idx] = x[i];

          if ( ii >= 0 )
            {
            for ( j = ii; j <= (i-1); j++ )
              {
              sum -= A[i][j]*x[j];
              }
            }
          else if (sum)
            {
            ii = i;
            }

          x[i] = sum;
        }
      //
      // Now, back substitution
      //
        for ( i = size-1; i >= 0; i-- )
          {
          sum = x[i];
          for ( j = i + 1; j < size; j++ )
            {
            sum -= A[i][j]*x[j];
            }
          x[i] = sum / A[i][i];
          }
      }


  };

}

#endif /* _DerivativeHelpers_H_ */
