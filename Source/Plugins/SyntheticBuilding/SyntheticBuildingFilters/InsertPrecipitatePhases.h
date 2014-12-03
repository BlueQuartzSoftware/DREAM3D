/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _InsertPrecipitatePhases_H_
#define _InsertPrecipitatePhases_H_

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
#include "SyntheticBuilding/SyntheticBuildingConstants.h"
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
 * @class InsertPrecipitatePhases InsertPrecipitatePhases.h Plugins/SyntheticBuilding/SyntheticBuilderFilters/InsertPrecipitatePhases.h
 * @brief
 * @author
 * @date Jan 29, 2014
 * @version 5.0
 */
class InsertPrecipitatePhases : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(InsertPrecipitatePhases)
    DREAM3D_STATIC_NEW_MACRO(InsertPrecipitatePhases)
    DREAM3D_TYPE_MACRO_SUPER(InsertPrecipitatePhases, AbstractFilter)

    virtual ~InsertPrecipitatePhases();

    DREAM3D_INSTANCE_STRING_PROPERTY(ClusteringListArrayName)

    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorOutputFile)
    DREAM3D_FILTER_PARAMETER(QString, CsvOutputFile)
    Q_PROPERTY(QString CsvOutputFile READ getCsvOutputFile WRITE setCsvOutputFile)
    DREAM3D_FILTER_PARAMETER(bool, HavePrecips)
    Q_PROPERTY(bool HavePrecips READ getHavePrecips WRITE setHavePrecips)
    DREAM3D_FILTER_PARAMETER(QString, PrecipInputFile)
    Q_PROPERTY(QString PrecipInputFile READ getPrecipInputFile WRITE setPrecipInputFile)
    DREAM3D_FILTER_PARAMETER(bool, PeriodicBoundaries)
    Q_PROPERTY(bool PeriodicBoundaries READ getPeriodicBoundaries WRITE setPeriodicBoundaries)
    DREAM3D_FILTER_PARAMETER(bool, MatchRDF)
    Q_PROPERTY(bool MatchRDF READ getMatchRDF WRITE setMatchRDF)
    DREAM3D_FILTER_PARAMETER(bool, WriteGoalAttributes)
    Q_PROPERTY(bool WriteGoalAttributes READ getWriteGoalAttributes WRITE setWriteGoalAttributes)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
    Q_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
    Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputShapeTypesArrayPath)
    Q_PROPERTY(DataArrayPath InputShapeTypesArrayPath READ getInputShapeTypesArrayPath WRITE setInputShapeTypesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, BoundaryCellsArrayPath)
    Q_PROPERTY(DataArrayPath BoundaryCellsArrayPath READ getBoundaryCellsArrayPath WRITE setBoundaryCellsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, NumCellsArrayName)
    Q_PROPERTY(QString NumCellsArrayName READ getNumCellsArrayName WRITE setNumCellsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, EquivalentDiametersArrayName)
    Q_PROPERTY(QString EquivalentDiametersArrayName READ getEquivalentDiametersArrayName WRITE setEquivalentDiametersArrayName)

    DREAM3D_FILTER_PARAMETER(QString, VolumesArrayName)
    Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, Omega3sArrayName)
    Q_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)

    DREAM3D_FILTER_PARAMETER(QString, CentroidsArrayName)
    Q_PROPERTY(QString CentroidsArrayName READ getCentroidsArrayName WRITE setCentroidsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, AxisEulerAnglesArrayName)
    Q_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, AxisLengthsArrayName)
    Q_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, NumFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();
    virtual const QString getBrandingString() { return SyntheticBuilding::SyntheticBuildingPluginDisplayName + " Filter"; }

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

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    InsertPrecipitatePhases();

    void place_precipitates(Int32ArrayType::Pointer exlusionZonesPtr);
    void generate_precipitate(int phase, int Seed, Precip* precip, unsigned int shapeclass, OrientationOps::Pointer OrthoOps);
    void load_precipitates();

    void transfer_attributes(int gnum, Precip* precip);
    void insert_precipitate(size_t featureNum);

    void move_precipitate(size_t featureNum, float xc, float yc, float zc);

    float check_sizedisterror(Precip* precip);
    void update_exclusionZones(int gadd, int gremove, Int32ArrayType::Pointer exlusionZonesPtr);
    void update_availablepoints(std::map<size_t,size_t> &availablePoints, std::map<size_t,size_t> &availablePointsInv);
    void determine_currentRDF(size_t featureNum, int add, bool double_count);
    void determine_randomRDF(size_t gnum, int add, bool double_count, int largeNumber);
    std::vector<float> normalizeRDF(std::vector<float> rdf, int num_bins, float stepsize, float rdfmin, size_t numPPTfeatures, float volume);
    float check_RDFerror(int gadd, int gremove, bool double_count);

    void assign_voxels();
    void assign_gaps();
    void write_goal_attributes();

    float find_xcoord(long long int index);
    float find_ycoord(long long int index);
    float find_zcoord(long long int index);

    void compare_1Ddistributions(std::vector<float>, std::vector<float>, float& sqrerror);
    void compare_2Ddistributions(std::vector<std::vector<float> >, std::vector<std::vector<float> >, float& sqrerror);

    void compare_3Ddistributions(std::vector<std::vector<std::vector<float> > >, std::vector<std::vector<std::vector<float> > >, float& sqrerror);

    std::vector<int> precipitatephases;
    std::vector<float> precipitatephasefractions;

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

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int8_t, BoundaryCells)

    DEFINE_CREATED_DATAARRAY_VARIABLE(float, AxisEulerAngles)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Centroids)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, AxisLengths)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Volumes)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Omega3s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, EquivalentDiameters)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, NumCells)
    NeighborList<float>::WeakPointer m_ClusteringList;

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, PhaseTypes)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, ShapeTypes)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, NumFeatures)
    StatsDataArray::WeakPointer m_StatsDataArray;

    OrthoRhombicOps::Pointer m_OrthoOps;

    std::vector<std::vector<int> > columnlist;
    std::vector<std::vector<int> > rowlist;
    std::vector<std::vector<int> > planelist;

    std::vector<size_t> pointsToAdd;
    std::vector<size_t> pointsToRemove;

    std::vector<std::vector<float> > featuresizedist;
    std::vector<std::vector<float> > simfeaturesizedist;
    std::vector<float> m_rdfTargetDist;
    std::vector<float> m_rdfCurrentDist;
    std::vector<float> m_rdfCurrentDistNorm;

    std::vector<float> m_RandomCentroids;
    std::vector<float> m_rdfRandom;


    std::vector<float> featuresizediststep;

    std::vector<int> newnames;
    std::vector<int> gsizes;

    size_t availablePointsCount;
    float m_currentRDFerror, m_oldRDFerror;
    float currentsizedisterror, oldsizedisterror;
    float m_rdfMax;
    float m_rdfMin;
    float m_StepSize;
    int m_numRDFbins;

    void dataCheck();
    void updateFeatureInstancePointers();

    InsertPrecipitatePhases(const InsertPrecipitatePhases&); // Copy Constructor Not Implemented
    void operator=(const InsertPrecipitatePhases&); // Operator '=' Not Implemented
};

#endif /* InsertPrecipitatePhases_H_ */



