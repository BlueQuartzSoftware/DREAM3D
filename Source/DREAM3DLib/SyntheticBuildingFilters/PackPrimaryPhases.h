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

#ifndef PACKPRIMARYPHASES_H_
#define PACKPRIMARYPHASES_H_

#include <vector>
#include <QtCore/QString>

#include <boost/shared_array.hpp>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/StatsData.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VolumeDataContainer.h"
#include "DREAM3DLib/ShapeOps/ShapeOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

typedef struct {
    float m_Volumes;
    float m_EquivalentDiameters;
    float m_AxisLengths[3];
    float m_AxisEulerAngles[3];
    float m_Omega3s;
    int m_FieldPhases;
    int m_Neighborhoods;
} Field;

/**
 * @class PackPrimaryPhases PackPrimaryPhases.h DREAM3DLib/SyntheticBuilderFilters/PackPrimaryPhases.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT PackPrimaryPhases : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(PackPrimaryPhases)
    DREAM3D_STATIC_NEW_MACRO(PackPrimaryPhases)
    DREAM3D_TYPE_MACRO_SUPER(PackPrimaryPhases, AbstractFilter)

    virtual ~PackPrimaryPhases();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(ActiveArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisLengthsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CentroidsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(EquivalentDiametersArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NeighborhoodsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Omega3sArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(VolumesArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseTypesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ShapeTypesArrayName)

    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::SyntheticBuildingFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::PackingFilters; }
    virtual const QString getHumanLabel() { return "Pack Primary Phases"; }

    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorOutputFile)
    DREAM3D_INSTANCE_STRING_PROPERTY(VtkOutputFile)
    DREAM3D_INSTANCE_STRING_PROPERTY(CsvOutputFile)
    DREAM3D_INSTANCE_PROPERTY(bool, PeriodicBoundaries)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGoalAttributes)


    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);
    
    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    virtual void preflight();

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();


  protected:
    PackPrimaryPhases();

    void initialize_packinggrid();

    void generate_grain(int phase, int Seed, Field* grain, unsigned int shapeclass);

    void transfer_attributes(int gnum, Field* field);
    void insert_grain(size_t grainNum);

    void move_grain(size_t grainNum, float xc, float yc, float zc);

    float check_sizedisterror(Field* field);
    void determine_neighbors(size_t grainNum, int add);
    float check_neighborhooderror(int gadd, int gremove);

  float check_fillingerror(int gadd, int gremove, Int32ArrayType::Pointer grainOwnersPtr, BoolArrayType::Pointer exclusionZonesPtr);
    void assign_voxels();
    void assign_gaps_only();
    void cleanup_grains();
  void write_goal_attributes();

    void compare_1Ddistributions(QVector<float>, QVector<float>, float &sqrerror);
    void compare_2Ddistributions(QVector<QVector<float> >, QVector<QVector<float> >, float &sqrerror);
    void compare_3Ddistributions(QVector<QVector<QVector<float> > >, QVector<QVector<QVector<float> > >, float &sqrerror);

    int writeVtkFile(int32_t* grainOwners, bool* exclusionZonesPtr);
    int estimate_numgrains(int xpoints, int ypoints, int zpoints, float xres, float yres, float zres);


  private:
    int32_t* m_Neighbors;

    // Cell Data - make sure these are all initialized to NULL in the constructor
    int32_t* m_GrainIds;
    int32_t* m_CellPhases;
    int8_t*  m_SurfaceVoxels;

    // Field Data - make sure these are all initialized to NULL in the constructor
    bool* m_Active;
    int32_t* m_FieldPhases;
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
    QMap<unsigned int, ShapeOps*> m_ShapeOps;
    ShapeOps::Pointer m_UnknownShapeOps;
    ShapeOps::Pointer m_CubicOctohedronOps;
    ShapeOps::Pointer m_CylinderOps;
    ShapeOps::Pointer m_EllipsoidOps;
    ShapeOps::Pointer m_SuperEllipsoidOps;

    OrthoRhombicOps::Pointer m_OrthoOps;

    QVector<QVector<int> > columnlist;
    QVector<QVector<int> > rowlist;
    QVector<QVector<int> > planelist;
    QVector<QVector<float> > ellipfunclist;

    unsigned long long int Seed;

    int firstPrimaryField;

    float sizex;
    float sizey;
    float sizez;
    float totalvol;

    float m_HalfPackingRes[3];
    float m_OneOverPackingRes[3];
    float m_OneOverHalfPackingRes[3];

    float m_PackingRes[3];
    int m_PackingPoints[3];
    int m_TotalPackingPoints;

    QVector<QVector<float> > grainsizedist;
    QVector<QVector<float> > simgrainsizedist;
    QVector<QVector<QVector<float> > > neighbordist;
    QVector<QVector<QVector<float> > > simneighbordist;

    QVector<float> grainsizediststep;
    QVector<float> neighbordiststep;

    QVector<int> newnames;
    QVector<int> packqualities;
    QVector<int> gsizes;

    QVector<int> primaryphases;
    QVector<float> primaryphasefractions;

    float fillingerror, oldfillingerror;
    float currentneighborhooderror, oldneighborhooderror;
    float currentsizedisterror, oldsizedisterror;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);


    PackPrimaryPhases(const PackPrimaryPhases&); // Copy Constructor Not Implemented
    void operator=(const PackPrimaryPhases&); // Operator '=' Not Implemented
};

#endif /* PackPrimaryPhases_H_ */
