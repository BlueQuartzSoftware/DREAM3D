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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/StatsData.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/ShapeOps/ShapeOps.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

typedef struct {
    float m_Volumes;
    float m_EquivalentDiameters;
    float m_AxisLengths[3];
    float m_AxisEulerAngles[3];
    float m_Omega3s;
    int m_PhasesF;
    int m_Neighborhoods;
} Field;

/**
 * @class PackGrainsGen2 PackGrainsGen2.h DREAM3DLib/SyntheticBuilderFilters/PackGrainsGen2.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT PackGrainsGen2 : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(PackGrainsGen2);
    DREAM3D_STATIC_NEW_MACRO(PackGrainsGen2);
    DREAM3D_TYPE_MACRO_SUPER(PackGrainsGen2, AbstractFilter);

    virtual ~PackGrainsGen2();

    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;


    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::SyntheticBuilderFilters; }
    virtual const std::string getHumanLabel() { return "Pack Grains"; }

    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorOutputFile)
    DREAM3D_INSTANCE_STRING_PROPERTY(VtkOutputFile)
    DREAM3D_INSTANCE_PROPERTY(int, MaxIterations)
    DREAM3D_INSTANCE_PROPERTY(bool, PeriodicBoundaries);
    DREAM3D_INSTANCE_PROPERTY(bool, WriteIntendedAttributes);
    DREAM3D_INSTANCE_PROPERTY(float, NeighborhoodErrorWeight);


    virtual void setupFilterOptions();

    virtual void preflight();

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    float check_sizedisterror(Field* field);
	static void generate_grain(int phase, int Seed, Field* grain, StatsDataArray* m_StatsDataArray, unsigned int shapeclass, OrientationMath::Pointer OrthoOps);

    void transfer_attributes(int gnum, Field* field);


  protected:
    PackGrainsGen2();

    void initialize_packinggrid();

    void pack_grains();
    void insert_grain(size_t grainNum);

    void add_grain(size_t grainNum);
    void move_grain(size_t grainNum, float xc, float yc, float zc);

    void remove_grain(size_t grainNum);
    void determine_neighbors(size_t grainNum, int add);
    float check_neighborhooderror(int gadd, int gremove);

    float check_fillingerror(int gadd, int gremove);
    void assign_voxels();
    void assign_gaps();
    void cleanup_grains();


    void compare_1Ddistributions(std::vector<float>, std::vector<float>, float &sqrerror);
    void compare_2Ddistributions(std::vector<std::vector<float> >, std::vector<std::vector<float> >, float &sqrerror);

    void compare_3Ddistributions(std::vector<std::vector<std::vector<float> > >, std::vector<std::vector<std::vector<float> > >, float &sqrerror);

    int writeVtkFile();

  private:
    // Cell Data - make sure these are all initialized to NULL in the constructor
    int32_t* m_GrainIds;
    int32_t* m_PhasesC;
    int8_t*  m_SurfaceVoxels;

    // Field Data - make sure these are all initialized to NULL in the constructor
    bool* m_Active;
    int32_t* m_PhasesF;
    int32_t* m_Neighborhoods;
    float* m_Centroids;
    float* m_Volumes;
    float* m_AxisLengths;
    float* m_AxisEulerAngles;
    float* m_Omega3s;
    float* m_EquivalentDiameters;

    // Ensemble Data - make sure these are all initialized to NULL in the constructor
    unsigned int* m_PhaseTypes;
    unsigned int* m_ShapeTypes;
    StatsDataArray* m_StatsDataArray;

    // All other private variables
    std::map<unsigned int, ShapeOps*> m_ShapeOps;
    ShapeOps::Pointer m_UnknownShapeOps;
    ShapeOps::Pointer m_CubicOctohedronOps;
    ShapeOps::Pointer m_CylinderOps;
    ShapeOps::Pointer m_EllipsoidOps;
    ShapeOps::Pointer m_SuperEllipsoidOps;

    std::vector<OrientationMath*> m_OrientationOps;
    OrientationMath::Pointer m_CubicOps;
    OrientationMath::Pointer m_HexOps;
    OrientationMath::Pointer m_OrthoOps;

    std::vector<std::vector<int> > columnlist;
    std::vector<std::vector<int> > rowlist;
    std::vector<std::vector<int> > planelist;

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

    std::vector<float> grainsizediststep;

    std::vector<int> newnames;
    std::vector<int> packqualities;
    std::vector<int> gsizes;

    std::vector<int> primaryphases;
    std::vector<float> primaryphasefractions;

    float fillingerror, oldfillingerror;
    float currentneighborhooderror, oldneighborhooderror;
    float currentsizedisterror, oldsizedisterror;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);


    PackGrainsGen2(const PackGrainsGen2&); // Copy Constructor Not Implemented
    void operator=(const PackGrainsGen2&); // Operator '=' Not Implemented
};

#endif /* PACKGRAINSGEN2_H_ */
