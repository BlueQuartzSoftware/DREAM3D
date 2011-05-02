/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
#ifndef _AIM_ALGO_HPP_
#define _AIM_ALGO_HPP_

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include "tbb/atomic.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/task_group.h"

#include "AIM/Common/ReconstructionFunc.h"
#include "AIM/Common/MisorientationCalculations.h"

class ParallelRenumberGrains
{
    const ReconstructionFunc* d;

  public:
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      //std::cout << "Threaded RenumberGrains." << std::endl;
      for (size_t j = r.begin(); j != r.end(); ++j)
      {
        int grainname = d->voxels[j].grainname;
        if (grainname >= 1)
        {
          int newgrainname = d->m_Grains[grainname]->newgrainname;
          d->voxels[j].grainname = newgrainname;
        }

      }
    }
    ParallelRenumberGrains(ReconstructionFunc* recon) :
      d(recon)
    {
    }
};


class FindEuclideanMap
{
    const ReconstructionFunc* d;
    const int loop;

  public:
    void operator()() const
    {
      std::cout << "  FindEuclideanMap: Loop = " << loop << std::endl;
      int nearestneighbordistance = 0;
      int count = 1;
//      int good = 1;
//      double x, y, z;
      int neighpoint;
      int nearestneighbor;
      int neighbors[6];
      int xpoints = d->xpoints;
      int ypoints = d->ypoints;
      int zpoints = d->zpoints;
      double resx = d->resx;
      double resy = d->resy;
      double resz = d->resz;

      neighbors[0] = -d->xpoints * d->ypoints;
      neighbors[1] = -d->xpoints;
      neighbors[2] = -1;
      neighbors[3] = 1;
      neighbors[4] = d->xpoints;
      neighbors[5] = d->xpoints * d->ypoints;
      int* voxel_NearestNeighbor = new int[d->totalpoints];
      double* voxel_NearestNeighborDistance = new double[d->totalpoints];
      nearestneighbordistance = 0;
      for (int a = 0; a < (d->totalpoints); ++a)
      {
        voxel_NearestNeighbor[a] = d->voxels[a].nearestneighbor[loop];
        voxel_NearestNeighborDistance[a] = d->voxels[a].nearestneighbordistance[loop];
      }
      count = 1;
      int i;
      char mask[6] = {0,0,0,0,0,0};
      while (count != 0)
      {
        count = 0;
        nearestneighbordistance++;

        for (int z = 0; z < zpoints; ++z)
        {
          mask[0] = mask[5] = 1;
          if (z == 0 ) { mask[0] = 0; }
          if (z == zpoints - 1) { mask[5] = 0; }

          for (int y = 0; y < ypoints; ++y)
          {
            mask[1] = mask[4] = 1;
            if (y == 0 ) { mask[1] = 0; }
            if (y == ypoints - 1) { mask[4] = 0; }

            for (int x = 0; x < xpoints; ++x)
            {
              mask[2] = mask[3] = 1;
              if (x == 0 ) { mask[2] = 0; }
              if (x == xpoints - 1) { mask[3] = 0; }

              i = (z * xpoints*ypoints) + (y*xpoints) + x;
              if (voxel_NearestNeighbor[i] == -1)
              {
                for (int j = 0; j < 6; j++)
                {
                  neighpoint = i + neighbors[j];
                  if (mask[j] == 1)
                  {
                    count++;
                    if (voxel_NearestNeighborDistance[neighpoint] != -1.0)
                    {
                      voxel_NearestNeighbor[i] = voxel_NearestNeighbor[neighpoint];
                    }
                  }
                }
              }
            }
          }
        }
        for (int j = 0; j < (d->totalpoints); ++j)
        {
          if (voxel_NearestNeighbor[j] != -1 && voxel_NearestNeighborDistance[j] == -1)
          {
            voxel_NearestNeighborDistance[j] = nearestneighbordistance;
          }
        }
      }
      double x1, x2, y1, y2, z1, z2;
      double dist;
      for (int j = 0; j < (d->totalpoints); j++)
      {
        nearestneighbor = voxel_NearestNeighbor[j];
        x1 = resx * double(j % xpoints); // find_xcoord(j);
        y1 = resy * double((j / xpoints) % ypoints);// find_ycoord(j);
        z1 = resz * double(j / (xpoints * ypoints)); // find_zcoord(j);
        x2 = resx * double(nearestneighbor % xpoints); // find_xcoord(nearestneighbor);
        y2 = resy * double((nearestneighbor / xpoints) % ypoints); // find_ycoord(nearestneighbor);
        z2 = resz * double(nearestneighbor / (xpoints * ypoints)); // find_zcoord(nearestneighbor);
        dist = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2));
        dist = pow(dist, 0.5);
        voxel_NearestNeighborDistance[j] = dist + (0.5 * resx);
      }
      for (int a = 0; a < (d->totalpoints); ++a)
      {
        d->voxels[a].nearestneighbor[loop] = voxel_NearestNeighbor[a];
        d->voxels[a].nearestneighbordistance[loop] = voxel_NearestNeighborDistance[a];
      }
      delete[] voxel_NearestNeighbor;
      delete[] voxel_NearestNeighborDistance;
    }

    /**
     *
     * @param recon
     */
    FindEuclideanMap(ReconstructionFunc* recon, int l) :
      d(recon),
      loop (l)
    {}

};


#if 0
class ParallelFindKernels
{
    const ReconstructionFunc* d;
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
          q1[1] = d->voxels[i].quat[1];
          q1[2] = d->voxels[i].quat[2];
          q1[3] = d->voxels[i].quat[3];
          q1[4] = d->voxels[i].quat[4];
          col = i % d->xpoints;
          row = (i / d->xpoints) % d->ypoints;
          plane = i / (d->xpoints * d->ypoints);
          for (int j = -steps; j < steps + 1; j++)
          {
            jStride = j * d->xpoints * d->ypoints;
            for (int k = -steps; k < steps + 1; k++)
            {
              kStride = k * d->xpoints;
              for (int l = -steps; l < steps + 1; l++)
              {
                good = 1;
                neighbor = i + (jStride) + (kStride) + (l);
                if (plane + j < 0) good = 0;
                if (plane + j > d->zpoints - 1) good = 0;
                if (row + k < 0) good = 0;
                if (row + k > d->ypoints - 1) good = 0;
                if (col + l < 0) good = 0;
                if (col + l > d->xpoints - 1) good = 0;
                if (good == 1 && gnames[i] == gnames[neighbor] && unassigned[neighbor] != 1)
                {
                  numVoxel++;
                  q2[1] = d->voxels[neighbor].quat[1];
                  q2[2] = d->voxels[neighbor].quat[2];
                  q2[3] = d->voxels[neighbor].quat[3];
                  q2[4] = d->voxels[neighbor].quat[4];
                  if (d->crystruct == AIM::Reconstruction::Hexagonal)
                  {
                    w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
                  }
                  if (d->crystruct == AIM::Reconstruction::Cubic)
                  {
                    w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
                  }
                  totalmisorientation = totalmisorientation + w;
                }
              }
            }
          }
          d->voxels[i].kernelmisorientation = totalmisorientation / (float)numVoxel;
        }
        if (gnames[i] == 0 || unassigned[i] == 1)
        {
          d->voxels[i].kernelmisorientation = 0;
        }
      }
    }


    ParallelFindKernels(ReconstructionFunc* recon, int* gnames, int* unassigned) :
      d(recon), gnames(gnames), unassigned(unassigned)
    {
    }

};
#endif


#endif /* _AIM_ALGO_HPP_ */
