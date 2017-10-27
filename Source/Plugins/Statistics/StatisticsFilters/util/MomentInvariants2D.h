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
#ifndef _moments2d_H_
#define _moments2d_H_

#include <assert.h>

#include <vector>
#include <iostream>


#include <Eigen/Dense>

class MomentInvariants2D
{
  public:
    MomentInvariants2D();
    virtual ~MomentInvariants2D();


    using DoubleMatrixType = Eigen::Matrix<double,Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    using IntMatrixType = Eigen::Matrix<int,Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

//--------------------
// These variations use Eigen for the matrix computations

    /**
     * @brief binomial
     * @param max_order
     * @return
     */
    DoubleMatrixType binomial(size_t max_order);

    /**
     * @brief getBigX
     * @param max_order
     * @param dim
     * @return
     */
    MomentInvariants2D::DoubleMatrixType getBigX(size_t max_order, size_t dim);

    /**
     * @brief computeMomentInvariants
     * @param input
     * @param inputDims
     * @param max_order
     * @return
     */
    DoubleMatrixType computeMomentInvariants(DoubleMatrixType &input, size_t* inputDims, size_t max_order);

#if 0
    /**
     * @brief binomial
     * @param p
     * @param bn
     */
    void binomial(int p, std::vector<double> &bn);


    /**
     * @brief getBigX
     * @param max_order
     * @param dim
     * @param bigx
     */
    void getBigX(size_t max_order, size_t dim, std::vector<double> &bigx);

    /**
     * @brief computeMoments2D
     * @param input
     * @param inputDims
     * @param bigx
     * @param max_order
     * @param dim
     */
    std::vector<double> computeMoments2D(std::vector<double> &input, size_t* inputDims, size_t max_order);

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static std::vector<double> dindgen(size_t dim)
    {
      std::vector<double> v(dim);
      for(size_t i = 0; i < dim; i++) { v[i] = static_cast<double>(i); }
      return v;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static std::vector<int> indgen(size_t dim)
    {
      std::vector<int> v(dim);
      for(size_t i = 0; i < dim; i++) { v[i] = static_cast<int>(i); }
      return v;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static std::vector<double> dblarr(size_t cols, size_t rows)
    {
      std::vector<double> v(cols * rows);
      return v;
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template<typename T>
    static void print2D(std::vector<T>& vec, size_t cols, size_t rows )
    {
      // Print the 2D matrix
      for(size_t i=0; i < rows; i++)
      {
        for(size_t j=0; j < cols; j++)
        {
          size_t index0 = (i * cols ) + j;
          std::cout << vec[index0] << " ";
        }
        std::cout << std::endl;
      }
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template<typename T>
    static std::vector<T> multiplyComponents(std::vector<T> &a, std::vector<T> &b)
    {
      std::vector<T> result(a.size());
      for(size_t i=0; i < a.size(); i++)
      {
        result[i] = a[i] * b[i];
      }
      return result;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template<typename T>
    static std::vector<T> MatrixMultiply(std::vector<T> &aMatrix, size_t aCols, size_t aRows,
                                         std::vector<T> &bMatrix, size_t bCols, size_t bRows)
    {
      std::vector<T> product(aRows * bCols, static_cast<T>(0.0));

      assert(aCols == bRows);
      for (size_t row = 0; row < aRows; row++)
      {
        for (size_t col = 0; col < bCols; col++)
        {
          // Multiply the row of A by the column of B to get the row, column of product.
          for (size_t inner = 0; inner < aCols; inner++)
          {
            product[row * bCols + col] += aMatrix[row * aCols + inner] * bMatrix[inner * bCols + col];
          }
          //std::cout << product[row * bCols + col] << "  ";
        }
        //std::cout << "\n";
      }

      return product;
    }

    /**
    * @brief Transpose a ROW MAJOR 2D Matrix.
    * @param
    * @param
    * @param
    */
    template<typename T>
    static std::vector<T> Transpose(std::vector<T> &mat, size_t cols, size_t rows)
    {
      size_t matIdx;
      size_t outIdx;


      std::vector<T> output(cols * rows);

      for(size_t r = 0; r < rows; r++ )
      {
        for(size_t c = 0; c < cols; c++)
        {
          matIdx = r * cols + c;
          outIdx = c * rows + r;
          output[outIdx] = mat[matIdx];
        }
      }
      return output;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template<typename T>
    static void copyRowIntoColumn(std::vector<T> &src, size_t r, size_t sCols, size_t sRows,
                           std::vector<T> &target, size_t c, size_t tCols, size_t tRows)
    {

      assert(sCols <= tRows);
      assert(r < sRows);
      assert(c < tCols);

      for(size_t i = 0; i < sCols; i++)
      {
        //Calc proper index to use from source
        size_t sIndex = r * sCols + i;
        // Calc proper index to use for target
        size_t tIndex = i * tCols + c;
        target[tIndex] = src[sIndex];
      }
    }
#endif
  protected:

    /**
     * @brief factorial
     * @param n
     * @return
     */
    int factorial(int n) const;


  private:
    MomentInvariants2D(const MomentInvariants2D&); // Copy Constructor Not Implemented
    void operator=(const MomentInvariants2D&); // Operator '=' Not Implemented
};


#endif /* _moments2d_H_ */
