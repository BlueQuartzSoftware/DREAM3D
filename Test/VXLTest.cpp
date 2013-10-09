/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <stdlib.h>


// This is core/vnl/examples/vnl_lsqr_test.cxx
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_sparse_matrix.h>
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <vnl/algo/vnl_lsqr.h>



int main(int argc, char** argv)
{
  vnl_sparse_matrix<double> A(10000, 1000);

  vcl_vector<int> cols(50);
  vcl_vector<double> vals(50);

  for (int row_ = 0; row_ < 10000; ++row_)
  {
    double sum = 0;
    for (int i = 0; i < 50; ++i)
    {
      cols[i] = vcl_rand() % 999;
      vals[i] = (double) vcl_rand() / (double) RAND_MAX;
      sum += vals[i];
    }
    A.set_row(row_, cols, vals);
    A.scale_row(row_, 1.0 / sum);
  }

  vnl_vector<double> x(1000);
  for (int i = 0; i < 1000; ++i)
  { x[i] = (double) vcl_rand() / (double) RAND_MAX; }

  vnl_vector<double> b(10000);
  A.mult(x, b);

  for (int i = 0; i < 10000; ++i)
  { b[i] += 0.01 * (((double) vcl_rand() / (double) RAND_MAX) - 0.5); }

  vnl_sparse_matrix_linear_system<double> linear_system(A, b);
  vnl_lsqr lsqr(linear_system);
  vnl_vector<double> result(1000);
  lsqr.minimize(result);
  lsqr.diagnose_outcome(vcl_cerr);

  vcl_cerr << "Ground truth relative residual : " << (x - result).two_norm() / x.two_norm() << vcl_endl;
  return EXIT_SUCCESS;
}


