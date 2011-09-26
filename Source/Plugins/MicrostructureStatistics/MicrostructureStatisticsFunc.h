/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef MICROSTRUCTURESTATISTICSFUNC_H_
#define MICROSTRUCTURESTATISTICSFUNC_H_

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <cstddef>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>

#include <boost/shared_array.hpp>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/AIMArray.hpp"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/Grain.h"
#include "DREAM3D/Common/DREAM3DRandom.h"
#include "DREAM3D/Common/Observable.h"
#include "DREAM3D/Common/OrientationMath.h"
#include "DREAM3D/Common/OrientationOps/CubicOps.h"
#include "DREAM3D/Common/OrientationOps/HexagonalOps.h"
#include "DREAM3D/Common/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3D/HDF5/H5StatsWriter.h"

class MicrostructureStatisticsFunc : public Observable
{
  public:
    MXA_SHARED_POINTERS(MicrostructureStatisticsFunc)
    MXA_STATIC_NEW_MACRO(MicrostructureStatisticsFunc)

    virtual ~MicrostructureStatisticsFunc();

    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;

    std::vector<Ebsd::CrystalStructure> crystruct;
    std::vector<DREAM3D::Reconstruction::PhaseType> phaseType;
    std::vector<float> pptFractions;

    DECLARE_WRAPPED_ARRAY(grain_indicies, m_GrainIndicies, int)
    DECLARE_WRAPPED_ARRAY(phases, m_Phases, int);
    DECLARE_WRAPPED_ARRAY(euler1s, m_Euler1s, float);
    DECLARE_WRAPPED_ARRAY(euler2s, m_Euler2s, float);
    DECLARE_WRAPPED_ARRAY(euler3s, m_Euler3s, float);
    DECLARE_WRAPPED_ARRAY(neighbors, m_Neighbors, int);
    DECLARE_WRAPPED_ARRAY(surfacevoxels, m_SurfaceVoxels, float);
    DECLARE_WRAPPED_ARRAY(quats, m_Quats, float); // n x 5 array
    DECLARE_WRAPPED_ARRAY(graincounts, m_GrainCounts, int);

    DECLARE_WRAPPED_ARRAY(grainmisorientations, m_GrainMisorientations, float);
    DECLARE_WRAPPED_ARRAY(misorientationgradients, m_MisorientationGradients, float);
    DECLARE_WRAPPED_ARRAY(kernelmisorientations, m_KernelMisorientations, float);

    DECLARE_WRAPPED_ARRAY(neighborlists, m_NeighborLists, float);// N x 6 Array
    DECLARE_WRAPPED_ARRAY(nearestneighbors, m_NearestNeighbors, float); // N x 3 Array
    DECLARE_WRAPPED_ARRAY(nearestneighbordistances, m_NearestNeighborDistances, float); // N x 3 Array

    DECLARE_WRAPPED_ARRAY(graincenters, m_GrainCenters, float); // N x 5 Array
    DECLARE_WRAPPED_ARRAY(grainmoments, m_GrainMoments, float); // N x 6 Array

    DECLARE_WRAPPED_ARRAY(totalsurfacearea, m_TotalSurfaceArea, float);
    DECLARE_WRAPPED_ARRAY(phasefraction, m_PhaseFraction, float);
    DECLARE_WRAPPED_ARRAY(totalvol, m_TotalVol, float);
    DECLARE_WRAPPED_ARRAY(totalaxes, m_TotalAxes, float);
    DECLARE_WRAPPED_ARRAY(maxdiameter, m_MaxDiameter, float);
    DECLARE_WRAPPED_ARRAY(mindiameter, m_MinDiameter, float);


    std::vector<Grain::Pointer> m_Grains;

    float resx;
    float resy;
    float resz;
    float sizex;
    float sizey;
    float sizez;
    float cropminx;
    float cropminy;
    float cropminz;
    float cropmaxx;
    float cropmaxy;
    float cropmaxz;

    int xpoints;
    int ypoints;
    int zpoints;
    int totalpoints;

    bool computesizes;
    bool computeshapes;
    bool computeneighbors;


	void initializeAttributes();
    void initializeGrains();
    void initializeArrays();

    void find_surfacegrains();
    void find_surfacegrains2D();
    void find_boundingboxgrains();
    void find_boundingboxgrains2D();
    void find_neighbors();
    void define_neighborhood();
    void find_euclidean_map();
    void find_vectors(H5StatsWriter::Pointer h5io);
    void find_centroids();
    void find_moments();
    void find_axes();
    void find_colors();
    void find_schmids();
    void find_vectors2D(H5StatsWriter::Pointer h5io);
    void find_centroids2D();
    void find_moments2D();
    void find_axes2D();
    void deformation_stats(const std::string &filename, const std::string &filename2);
    void write_graindata(const std::string &graindataFile, bool writesizes, bool writeshapes, bool writeneighbors, bool writeavgorientations);
    void find_eulerodf(H5StatsWriter::Pointer h5io);
    void measure_misorientations(H5StatsWriter::Pointer h5io);
    int volume_stats(H5StatsWriter::Pointer h5io, float BinStepSize);
    int volume_stats2D(H5StatsWriter::Pointer h5io, float BinStepSize);
    void find_grain_and_kernel_misorientations();
    void find_grainorientations();
    float find_xcoord(size_t index);
    float find_ycoord(size_t index);
    float find_zcoord(size_t index);

  protected:
    MicrostructureStatisticsFunc();

  private:
    std::vector<OrientationMath*> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    MicrostructureStatisticsFunc(const MicrostructureStatisticsFunc&); // Copy Constructor Not Implemented
    void operator=(const MicrostructureStatisticsFunc&); // Operator '=' Not Implemented
};

#endif /* MICROSTRUCTURESTATISTICSFUNC_H_ */
