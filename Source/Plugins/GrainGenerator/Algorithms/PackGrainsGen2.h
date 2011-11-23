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

#ifndef PACKGRAINSGEN2_H_
#define PACKGRAINSGEN2_H_

#include <vector>
#include <string>

#include <boost/shared_array.hpp>

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/ShapeOps/ShapeOps.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/HDF5/H5StatsReader.h"


/**
 * @class PackGrainsGen2 PackGrainsGen2.h GrainGenerator/Algorithms/PackGrainsGen2.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class PackGrainsGen2 : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(PackGrainsGen2);
    DREAM3D_STATIC_NEW_MACRO(PackGrainsGen2);
    DREAM3D_TYPE_MACRO_SUPER(PackGrainsGen2, Observable);

    virtual ~PackGrainsGen2();

    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;

    DREAM3D_INSTANCE_STRING_PROPERTY(H5StatsFile)
    DREAM3D_INSTANCE_PROPERTY(bool, periodic_boundaries);
    DREAM3D_INSTANCE_PROPERTY(float, neighborhooderrorweight);
    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition);
    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorMessage);
    void setGrainGenFunc(DataContainer* gg) { m = gg; }
    DataContainer*getGrainGenFunc() { return m; }

    unsigned long long int Seed;

	float sizex;
    float sizey;
    float sizez;
    float totalvol;

    float packingresx;
    float packingresy;
    float packingresz;
    int packingxpoints;
    int packingypoints;
    int packingzpoints;
    int packingtotalpoints;

    std::vector<std::vector<std::vector<int> > > grainowners;

	std::vector<std::vector<float> > grainsizedist;
    std::vector<std::vector<float> > simgrainsizedist;
    std::vector<std::vector<std::vector<float> > > neighbordist;
    std::vector<std::vector<std::vector<float> > > simneighbordist;

    std::vector<std::vector<std::vector<float> > > bovera;
    std::vector<std::vector<std::vector<float> > > covera;
    std::vector<std::vector<std::vector<float> > > coverb;
    std::vector<std::vector<std::vector<float> > > omega3;
    std::vector<std::vector<std::vector<float> > > neighborparams;

	std::vector<SharedFloatArray> axisodf;

	std::vector<float> binstepsize;
    std::vector<float> grainsizediststep;
    std::vector<float> maxdiameter;
    std::vector<float> mindiameter;
    std::vector<float> avgdiam;
    std::vector<float> sddiam;
    std::vector<int> numdiameterbins;

    std::vector<int> newnames;
    std::vector<int> gsizes;

	std::vector<int> primaryphases;
    std::vector<float> primaryphasefractions;

	float fillingerror, oldfillingerror;
    float currentneighborhooderror, oldneighborhooderror;
    float currentsizedisterror, oldsizedisterror;

	virtual void execute();

	void initializeAttributes();
	void initializeArrays(std::vector<Ebsd::CrystalStructure> structures);
    int readReconStatsData(H5StatsReader::Pointer h5io);
    int readAxisOrientationData(H5StatsReader::Pointer h5io);

    void initialize_packinggrid();
    void generate_grain(int, int);
    void pack_grains();
    void insert_grain(size_t grainNum);

    void add_grain(size_t grainNum);
    void move_grain(size_t grainNum, float xc, float yc, float zc);

    void remove_grain(size_t grainNum);
    void determine_neighbors(size_t grainNum, int add);
    float check_neighborhooderror(int gadd, int gremove);
    float check_sizedisterror(int gadd, int gremove);
    float check_fillingerror(int gadd, int gremove);
    void assign_voxels();
    void assign_gaps();
    void cleanup_grains();


    void compare_1Ddistributions(std::vector<float>, std::vector<float>, float &sqrerror);
    void compare_2Ddistributions(std::vector<std::vector<float> >, std::vector<std::vector<float> >, float &sqrerror);

    void compare_3Ddistributions(std::vector<std::vector<std::vector<float> > >, std::vector<std::vector<std::vector<float> > >, float &sqrerror);

    std::map<DREAM3D::SyntheticBuilder::ShapeType, DREAM3D::ShapeOps*> m_ShapeOps;
    std::vector<OrientationMath*> m_OrientatioOps;

  protected:
    PackGrainsGen2();
    DataContainer* m;

  private:

    DREAM3D::ShapeOps::Pointer m_UnknownShapeOps;
    DREAM3D::ShapeOps::Pointer m_CubicOctohedronOps;
    DREAM3D::ShapeOps::Pointer m_CylinderOps;
    DREAM3D::ShapeOps::Pointer m_EllipsoidOps;
    DREAM3D::ShapeOps::Pointer m_SuprtEllipsoidOps;

    OrientationMath::Pointer m_CubicOps;
    OrientationMath::Pointer m_HexOps;
    OrientationMath::Pointer m_OrthoOps;

	PackGrainsGen2(const PackGrainsGen2&); // Copy Constructor Not Implemented
    void operator=(const PackGrainsGen2&); // Operator '=' Not Implemented
};

#endif /* PACKGRAINSGEN2_H_ */
