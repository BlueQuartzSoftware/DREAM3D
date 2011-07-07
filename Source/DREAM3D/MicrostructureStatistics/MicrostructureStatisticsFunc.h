/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
#include "DREAM3D/Common/AIMRandomNG.h"
#include "DREAM3D/Common/OrientationMath.h"
#include "DREAM3D/Common/OrientationOps/CubicOps.h"
#include "DREAM3D/Common/OrientationOps/HexagonalOps.h"
#include "DREAM3D/Common/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3D/HDF5/H5ReconStatsWriter.h"

class DREAM3DLib_EXPORT MicrostructureStatisticsFunc
{
  public:
    MXA_SHARED_POINTERS(MicrostructureStatisticsFunc)
    MXA_STATIC_NEW_MACRO(MicrostructureStatisticsFunc)

    virtual ~MicrostructureStatisticsFunc();

    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;

    std::vector<AIM::Reconstruction::CrystalStructure> crystruct;
    std::vector<AIM::Reconstruction::PhaseType> phaseType;
    std::vector<float> pptFractions;

    std::vector<int> grain_indicies;
    std::vector<int> phases;
    std::vector<float> euler1s;
    std::vector<float> euler2s;
    std::vector<float> euler3s;
    std::vector<int> neighbors;
    std::vector<int> surfacevoxels;
    std::vector<float> grainmisorientations;
    std::vector<float> misorientationgradients;
    std::vector<float> kernelmisorientations;
    std::vector<std::vector<int> > neighborlists;
    std::vector<std::vector<float> > quats;
    std::vector<std::vector<int> > nearestneighbors;
    std::vector<std::vector<float> > nearestneighbordistances;

    SharedIntArray graincounts;
    std::vector<Grain::Pointer> m_Grains;

    std::vector<std::vector<float> > graincenters;
    std::vector<std::vector<float> > grainmoments;

    std::vector<float> totalsurfacearea;
    std::vector<float> phasefraction;
    std::vector<float> totalvol;
    std::vector<float> totalaxes;
    std::vector<int> maxdiameter;
    std::vector<int> mindiameter;

    float resx;
    float resy;
    float resz;
    float sizex;
    float sizey;
    float sizez;

    int xpoints;
    int ypoints;
    int zpoints;
    int totalpoints;

	bool computesizes;
	bool computeshapes;
	bool computeneighbors;
	bool computeodf;
	bool computemdf;

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
    void find_vectors(H5ReconStatsWriter::Pointer h5io);
    void find_centroids();
    void find_moments();
    void find_axes();
    void find_colors();
    void find_schmids();
    void find_vectors2D(H5ReconStatsWriter::Pointer h5io);
    void find_centroids2D();
    void find_moments2D();
    void find_axes2D();
    void deformation_stats(const std::string &filename, const std::string &filename2);
    void write_graindata(const std::string &graindataFile, bool writesizes, bool writeshapes, bool writeneighbors, bool writeavgorientations);
    void find_eulerodf(H5ReconStatsWriter::Pointer h5io);
    void measure_misorientations(H5ReconStatsWriter::Pointer h5io);
    int volume_stats(H5ReconStatsWriter::Pointer h5io, float BinStepSize);
    int volume_stats2D(H5ReconStatsWriter::Pointer h5io, float BinStepSize);
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
