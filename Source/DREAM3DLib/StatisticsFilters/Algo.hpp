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
#ifndef _AIM_ALGO_HPP_
#define _AIM_ALGO_HPP_

#if AIM_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/atomic.h>
#include <tbb/tick_count.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#endif

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include <vector>

#if AIM_USE_PARALLEL_ALGORITHMS
class ParallelRenumberGrains
{
    const DataContainer* m;

  public:
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      //std::cout << "Threaded RenumberGrains." << std::endl;
      for (size_t j = r.begin(); j != r.end(); ++j)
      {
        int grainname = m->voxels[j].grain_index;
        if (grainname >= 1)
        {
          int newgrainname = m->m_Grains[grainname]->newgrainname;
          m->voxels[j].grain_index = newgrainname;
        }

      }
    }
    ParallelRenumberGrains(DataContainer* recon) :
      m(recon)
    {
    }
};
#endif


#if 0
class ParallelFindKernels
{
    const ReconstructionFunc* m;
    const int* gnames;
    const int* unassigned;

  public:
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      double q1[5];
      double q2[5];
      int numVoxel; // number of voxels in the grain...
      int good = 0;
      int neighbor;
      int col, row, plane;
      double w, totalmisorientation;
      double n1, n2, n3;
      int steps = 1;
      int jStride;
      int kStride;

      for (size_t i = r.begin(); i != r.end(); ++i)
      {
        if (gnames[i] > 0 && unassigned[i] != 1)
        {
          totalmisorientation = 0.0;
          numVoxel = 0;
          q1[1] = m->voxels[i].quat[1];
          q1[2] = m->voxels[i].quat[2];
          q1[3] = m->voxels[i].quat[3];
          q1[4] = m->voxels[i].quat[4];
          col = i % m->getXPoints();
          row = (i / m->getXPoints()) % m->getYPoints();
          plane = i / (m->getXPoints() * m->getYPoints());
          for (int j = -steps; j < steps + 1; j++)
          {
            jStride = j * m->getXPoints() * m->getYPoints();
            for (int k = -steps; k < steps + 1; k++)
            {
              kStride = k * m->getXPoints();
              for (int l = -steps; l < steps + 1; l++)
              {
                good = 1;
                neighbor = i + (jStride) + (kStride) + (l);
                if (plane + j < 0) good = 0;
                if (plane + j > m->getZPoints() - 1) good = 0;
                if (row + k < 0) good = 0;
                if (row + k > m->getYPoints() - 1) good = 0;
                if (col + l < 0) good = 0;
                if (col + l > m->getXPoints() - 1) good = 0;
                if (good == 1 && gnames[i] == gnames[neighbor] && unassigned[neighbor] != 1)
                {
                  numVoxel++;
                  q2[1] = m->voxels[neighbor].quat[1];
                  q2[2] = m->voxels[neighbor].quat[2];
                  q2[3] = m->voxels[neighbor].quat[3];
                  q2[4] = m->voxels[neighbor].quat[4];
                  if (m->crystruct == Ebsd::Hexagonal)
                  {
                    w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
                  }
                  if (m->crystruct == Ebsd::Cubic)
                  {
                    w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
                  }
                  totalmisorientation = totalmisorientation + w;
                }
              }
            }
          }
          m->voxels[i].kernelmisorientation = totalmisorientation / (float)numVoxel;
        }
        if (gnames[i] == 0 || unassigned[i] == 1)
        {
          m->voxels[i].kernelmisorientation = 0;
        }
      }
    }


    ParallelFindKernels(ReconstructionFunc* recon, int* gnames, int* unassigned) :
      m(recon), gnames(gnames), unassigned(unassigned)
    {
    }

};
#endif


#endif /* _AIM_ALGO_HPP_ */
