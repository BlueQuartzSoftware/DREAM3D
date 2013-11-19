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

#include <QtCore/QString>
#include <vector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"
#include "DREAM3DLib/StatsData/StatsData.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/ShapeOps/ShapeOps.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

typedef struct
{
  float m_Volumes;
  float m_EquivalentDiameters;
  float m_AxisLengths[3];
  float m_AxisEulerAngles[3];
  float m_Omega3s;
  int m_FeaturePhases;
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
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(FeatureIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceVoxelsArrayName)
    //------ Created Feature Data
    DREAM3D_INSTANCE_STRING_PROPERTY(ActiveArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisLengthsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CentroidsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(EquivalentDiametersArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ClusteringListArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NumCellsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Omega3sArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FeaturePhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(VolumesArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseTypesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ShapeTypesArrayName)
    //------ Created Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(NumFeaturesArrayName)

    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorOutputFile)
    DREAM3D_INSTANCE_STRING_PROPERTY(CsvOutputFile)
    DREAM3D_INSTANCE_PROPERTY(bool, PeriodicBoundaries)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGoalAttributes)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::SyntheticBuildingFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::PackingFilters; }
    virtual const QString getHumanLabel() { return "Insert Precipitate Phases"; }

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

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */

    virtual void execute();
    virtual void preflight();

  protected:
    InsertPrecipitatePhases();

    Int32ArrayType::Pointer initialize_packinggrid();

    void place_precipitates(Int32ArrayType::Pointer featureOwnersPtr);
    void generate_precipitate(int phase, int Seed, Precip* precip, unsigned int shapeclass, OrientationOps::Pointer OrthoOps);

    void transfer_attributes(int gnum, Precip* precip);
    void insert_precipitate(size_t featureNum);

    void move_precipitate(size_t featureNum, float xc, float yc, float zc);

    float check_sizedisterror(Precip* precip);
    void determine_clustering(size_t featureNum, int add);
    float check_clusteringerror(int gadd, int gremove);

    float check_fillingerror(int gadd, int gremove, Int32ArrayType::Pointer featureOwnersPtr);
    void assign_voxels();
    void assign_gaps();
    void cleanup_features();
    void write_goal_attributes();

    float find_xcoord(long long int index);
    float find_ycoord(long long int index);
    float find_zcoord(long long int index);

    void compare_1Ddistributions(QVector<float>, QVector<float>, float& sqrerror);
    void compare_2Ddistributions(QVector<QVector<float> >, QVector<QVector<float> >, float& sqrerror);

    void compare_3Ddistributions(QVector<QVector<QVector<float> > >, QVector<QVector<QVector<float> > >, float& sqrerror);

    QVector<int> precipitatephases;
    QVector<float> precipitatephasefractions;

  private:

    int firstPrecipitateFeature;
    unsigned long long int Seed;
    float sizex;
    float sizey;
    float sizez;
    float totalvol;

    QMap<unsigned int, ShapeOps*> m_ShapeOps;
    ShapeOps::Pointer m_UnknownShapeOps;
    ShapeOps::Pointer m_CubicOctohedronOps;
    ShapeOps::Pointer m_CylinderOps;
    ShapeOps::Pointer m_EllipsoidOps;
    ShapeOps::Pointer m_SuperEllipsoidOps;

    int32_t* m_FeatureIds;
    int32_t* m_CellPhases;
    int8_t* m_SurfaceVoxels;

    float* m_AxisEulerAngles;
    float* m_Centroids;
    float* m_AxisLengths;
    float* m_Volumes;
    float* m_Omega3s;
    float* m_EquivalentDiameters;
    bool* m_Active;
    int32_t* m_FeaturePhases;
    int32_t* m_NumCells;
    NeighborList<float>* m_ClusteringList;

    unsigned int* m_PhaseTypes;
    unsigned int* m_ShapeTypes;
    int32_t* m_NumFeatures;
    StatsDataArray* m_StatsDataArray;

    OrthoRhombicOps::Pointer m_OrthoOps;

    QVector<QVector<int> > columnlist;
    QVector<QVector<int> > rowlist;
    QVector<QVector<int> > planelist;

    float m_HalfPackingRes[3];
    float m_OneOverHalfPackingRes[3];

    float m_PackingRes[3];
    int m_PackingPoints[3];
    int m_TotalPackingPoints;

    QVector<QVector<float> > featuresizedist;
    QVector<QVector<float> > simfeaturesizedist;
    QVector<QVector<QVector<float> > > clusteringdist;
    QVector<QVector<QVector<float> > > simclusteringdist;

    QVector<float> featuresizediststep;
    QVector<float> clusteringdiststep;

    QVector<int> newnames;
    QVector<int> packqualities;
    QVector<int> gsizes;

    float fillingerror, oldfillingerror;
    float currentclusteringerror, oldclusteringerror;
    float currentsizedisterror, oldsizedisterror;

    void dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles);

    InsertPrecipitatePhases(const InsertPrecipitatePhases&); // Copy Constructor Not Implemented
    void operator=(const InsertPrecipitatePhases&); // Operator '=' Not Implemented
};

#endif /* InsertPrecipitatePhases_H_ */
