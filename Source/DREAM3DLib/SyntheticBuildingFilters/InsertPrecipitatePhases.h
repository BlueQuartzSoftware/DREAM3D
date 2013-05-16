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

#ifndef InsertPrecipitatePhases_H_
#define InsertPrecipitatePhases_H_

#include <string>
#include <vector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/StatsData.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
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
    int m_FieldPhases;
    int m_Neighborhoods;
} Precip;

/**
 * @class InsertPrecipitatePhases InsertPrecipitatePhases.h DREAM3DLib/SyntheticBuilderFilters/InsertPrecipitatePhases.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT InsertPrecipitatePhases : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(InsertPrecipitatePhases)
    DREAM3D_STATIC_NEW_MACRO(InsertPrecipitatePhases)
    DREAM3D_TYPE_MACRO_SUPER(InsertPrecipitatePhases, AbstractFilter)

    virtual ~InsertPrecipitatePhases();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceVoxelsArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(ActiveArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisLengthsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CentroidsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(EquivalentDiametersArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NeighborhoodsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NumCellsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Omega3sArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(VolumesArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseTypesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ShapeTypesArrayName)
    //------ Created Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(NumFieldsArrayName)

    DREAM3D_INSTANCE_STRING_PROPERTY(CsvOutputFile)
    DREAM3D_INSTANCE_PROPERTY(bool, PeriodicBoundaries)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGoalAttributes)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::SyntheticBuildingFilters; }
  virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::PackingFilters; }
    virtual const std::string getHumanLabel() { return "Insert Precipitate Phases"; }

    virtual void setupFilterParameters();
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */

    virtual void execute();
    virtual void preflight();

  protected:
    InsertPrecipitatePhases();

    Int32ArrayType::Pointer initialize_packinggrid();

    void place_precipitates(Int32ArrayType::Pointer grainOwnersPtr);
    void generate_precipitate(int phase, int Seed, Precip* precip, unsigned int shapeclass, OrientationMath::Pointer OrthoOps);

    void transfer_attributes(int gnum, Precip* precip);
    void insert_precipitate(size_t grainNum);

    void move_precipitate(size_t grainNum, float xc, float yc, float zc);

    float check_sizedisterror(Precip* precip);
    void determine_neighbors(size_t grainNum, int add);
    float check_neighborhooderror(int gadd, int gremove);

    float check_fillingerror(int gadd, int gremove, Int32ArrayType::Pointer grainOwnersPtr);
    void assign_voxels();
    void assign_gaps();
    void cleanup_grains();
	void write_goal_attributes();

    float find_xcoord(long long int index);
    float find_ycoord(long long int index);
    float find_zcoord(long long int index);

    void compare_1Ddistributions(std::vector<float>, std::vector<float>, float &sqrerror);
    void compare_2Ddistributions(std::vector<std::vector<float> >, std::vector<std::vector<float> >, float &sqrerror);

    void compare_3Ddistributions(std::vector<std::vector<std::vector<float> > >, std::vector<std::vector<std::vector<float> > >, float &sqrerror);

    std::vector<int> precipitatephases;
    std::vector<float> precipitatephasefractions;

  private:

    int firstPrecipitateField;
    unsigned long long int Seed;
    float sizex;
    float sizey;
    float sizez;
    float totalvol;

    std::map<unsigned int, ShapeOps*> m_ShapeOps;
    ShapeOps::Pointer m_UnknownShapeOps;
    ShapeOps::Pointer m_CubicOctohedronOps;
    ShapeOps::Pointer m_CylinderOps;
    ShapeOps::Pointer m_EllipsoidOps;
    ShapeOps::Pointer m_SuperEllipsoidOps;

    int32_t* m_GrainIds;
    int32_t* m_CellPhases;
    int8_t* m_SurfaceVoxels;

    float* m_AxisEulerAngles;
    float* m_Centroids;
    float* m_AxisLengths;
    float* m_Volumes;
    float* m_Omega3s;
    float* m_EquivalentDiameters;
    bool* m_Active;
    int32_t* m_FieldPhases;
    int32_t* m_NumCells;
    int32_t* m_Neighborhoods;

    unsigned int* m_PhaseTypes;
    unsigned int* m_ShapeTypes;
    int32_t* m_NumFields;
    StatsDataArray* m_StatsDataArray;

    std::vector<OrientationMath::Pointer> m_OrientationOps;
    OrientationMath::Pointer m_CubicOps;
    OrientationMath::Pointer m_HexOps;
    OrientationMath::Pointer m_OrthoOps;

    std::vector<std::vector<int> > columnlist;
    std::vector<std::vector<int> > rowlist;
    std::vector<std::vector<int> > planelist;

    float m_HalfPackingRes[3];
    float m_OneOverHalfPackingRes[3];

    float m_PackingRes[3];
    int m_PackingPoints[3];
    int m_TotalPackingPoints;

    std::vector<std::vector<float> > grainsizedist;
    std::vector<std::vector<float> > simgrainsizedist;
    std::vector<std::vector<std::vector<float> > > neighbordist;
    std::vector<std::vector<std::vector<float> > > simneighbordist;

    std::vector<float> grainsizediststep;
    std::vector<float> neighbordiststep;

    std::vector<int> newnames;
    std::vector<int> packqualities;
    std::vector<int> gsizes;

    float fillingerror, oldfillingerror;
    float currentneighborhooderror, oldneighborhooderror;
    float currentsizedisterror, oldsizedisterror;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    InsertPrecipitatePhases(const InsertPrecipitatePhases&); // Copy Constructor Not Implemented
    void operator=(const InsertPrecipitatePhases&); // Operator '=' Not Implemented
};

#endif /* InsertPrecipitatePhases_H_ */
