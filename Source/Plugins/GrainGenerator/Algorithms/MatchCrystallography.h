/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef MATCHCRYSTALLOGRAPHY_H_
#define MATCHCRYSTALLOGRAPHY_H_


#include <string>
#include <numeric>
#include <limits>

#include <boost/shared_array.hpp>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observable.h"
#include "GrainGenerator/GrainGeneratorFunc.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/HDF5/H5StatsReader.h"


/**
 * @class MatchCrystallography MatchCrystallography.h GrainGenerator/Algorithms/MatchCrystallography.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class MatchCrystallography : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(MatchCrystallography);
    DREAM3D_STATIC_NEW_MACRO(MatchCrystallography);
    DREAM3D_TYPE_MACRO(MatchCrystallography);

    virtual ~MatchCrystallography();

    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;

    DREAM3D_INSTANCE_STRING_PROPERTY(H5StatsFile)
    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition);
    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorMessage);
    void setGrainGenFunc(GrainGeneratorFunc* gg) { m = gg; }
    GrainGeneratorFunc*getGrainGenFunc() { return m; }

    std::vector<Ebsd::CrystalStructure> crystruct;

	std::vector<SharedFloatArray> actualodf;
    std::vector<SharedFloatArray> simodf;
    std::vector<SharedFloatArray> actualmdf;
    std::vector<SharedFloatArray> simmdf;

    virtual void execute();

	void initializeArrays(std::vector<Ebsd::CrystalStructure> structures);
	int readODFData(H5StatsReader::Pointer h5io);
    int readMisorientationData(H5StatsReader::Pointer h5io);

    std::vector<float> unbiasedvol;
    DECLARE_WRAPPED_ARRAY(totalsurfacearea, m_TotalSurfaceArea, float);

	void assign_eulers();
    void swapOutOrientation(int & badtrycount, int & numbins, float currentodferror, float currentmdferror);
    void switchOrientations(int & badtrycount, int & numbins, float currentodferror, float currentmdferror);
    void MC_LoopBody1(int phase, size_t neighbor, int j, std::vector<float>* misolist, std::vector<float>* neighborsurfarealist, float &mdfchange);
    void MC_LoopBody2(int phase, size_t neighbor, int j, std::vector<float>* misolist, std::vector<float>* neighborsurfarealist);
    void matchCrystallography();
    void measure_misorientations();

    std::vector<OrientationMath*> m_OrientatioOps;

protected:
    MatchCrystallography();
    GrainGeneratorFunc* m;

  private:

    OrientationMath::Pointer m_CubicOps;
    OrientationMath::Pointer m_HexOps;
    OrientationMath::Pointer m_OrthoOps;

    MatchCrystallography(const MatchCrystallography&); // Copy Constructor Not Implemented
    void operator=(const MatchCrystallography&); // Operator '=' Not Implemented
};

#endif /* MATCHCRYSTALLOGRAPHY_H_ */
