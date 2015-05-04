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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"

#include "OrientationLib/OrientationOps/CubicOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"
#include "OrientationLib/OrientationOps/OrthoRhombicOps.h"

#include "Plugins/SyntheticBuilding/ShapeOps/ShapeOps.h"

typedef struct
{
  float m_Volumes;
  float m_EquivalentDiameters;
  float m_AxisLengths[3];
  float m_AxisEulerAngles[3];
  float m_Omega3s;
  int m_FeaturePhases;
  int m_Neighborhoods;
} Precip_t;

/**
 * @brief The InsertPrecipitatePhases class. See [Filter documentation](@ref insertprecipitatephases) for details.
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

    /**
     * @brief place_precipitates Organizes the placement of precipitates into the packing volume while ensuring
     * proper size and morphological statistics are maintained
     * @param exlusionZonesPtr Array of exclusion zone precipitate Ids for each packing point
     */
    void place_precipitates(Int32ArrayType::Pointer exlusionZonesPtr);

    /**
     * @brief generate_precipitate Creates a precipitate by sampling the size and morphological statistical distributions
     * @param phase Index of the Ensemble type for the Feature to be generated
     * @param Seed Value to intialize random number generator
     * @param precip Precip_t struct pointer to be intialized
     * @param shapeclass Type of precipitate shape to be generated
     * @param OrthoOps Pointer to OrientationOps object
     */
    void generate_precipitate(int32_t phase, uint64_t Seed, Precip_t* precip, uint32_t shapeclass, OrientationOps::Pointer OrthoOps);

    /**
     * @brief load_precipitates Reads a list of precipitates from a file to be used as the packed volume
     */
    void load_precipitates();

    /**
     * @brief transfer_attributes Moves variables held in the Precip_t struct into other arrays
     * @param gnum Id for the Feature to be copied
     * @param precip Precip_t struct pointer to be copied
     */
    void transfer_attributes(int32_t gnum, Precip_t* precip);

    /**
     * @brief insert_precipitate Performs the insertion of a precipitate into the packing volume
     * @param featureNum Id for the precipitate to be inserted
     */
    void insert_precipitate(size_t featureNum);

    /**
     * @brief move_precipitate Moves a precipitate to the supplied (x,y,z) centroid coordinate
     * @param featureNum Id for the precipitate to be moved
     * @param xc x centroid coordinate
     * @param yc y centroid coordinate
     * @param zc z centroid coordinate
     */
    void move_precipitate(int32_t featureNum, float xc, float yc, float zc);

    /**
     * @brief check_sizedisterror Computes the error between the current precipitate size distribution
     * and the goal precipitate size distribution
     * @param precip Precip_t struct pointer used to determine the Ensemble type
     * @return Float error value between two distributions
     */
    float check_sizedisterror(Precip_t* precip);

    /**
     * @brief update_exclusionZones Updates the exclusion owners pointer based on the associated incoming Ids
     * @param gadd Index used to determine which precipitate to add
     * @param gremove Index used to determine which precipitate to remove
     * @param exlusionZonesPtr Array of exclusion zone Feature Ids for each packing point
     */
    void update_exclusionZones(size_t gadd, int32_t gremove, Int32ArrayType::Pointer exlusionZonesPtr);

    /**
     * @brief update_availablepoints Updates the maps used to associate packing points with an "available" state
     * @param availablePoints Map between precipitate owners and number of available points
     * @param availablePointsInv Inverse associations for the availablePoints map
     */
    void update_availablepoints(std::map<size_t, size_t>& availablePoints, std::map<size_t, size_t>& availablePointsInv);

    /**
     * @brief determine_currentRDF Determines the radial distribution function about a given precipitate
     * @param featureNum Index for the precipitate to determine RDF
     * @param add Determines amount to iterate RDF bins
     * @param double_count Determines whether to double count items in bins
     */
    void determine_currentRDF(int32_t featureNum, int32_t add, bool double_count);

    /**
     * @brief determine_randomRDF Determines a random radial distribution function
     * @param gnum Index for the precipitate to determine RDF
     * @param add Determines amount to iterate RDF bins
     * @param double_count Determines whether to double count items in bins
     * @param largeNumber Placeholder value for large address space
     */
    void determine_randomRDF(size_t gnum, int32_t add, bool double_count, int32_t largeNumber);

    /**
     * @brief normalizeRDF Normalizes a radial distribution function
     * @param rdf RDF to normalize
     * @param num_bins Number of bins in the discretized RDF
     * @param stepsize Bin sted size for discretized RDF
     * @param rdfmin Minimum value for RDF
     * @param numPPTfeatures Number of precipiate Features
     * @param volume Volume for a given precipitate
     * @return Normalized RDF
     */
    std::vector<float> normalizeRDF(std::vector<float> rdf, int num_bins, float stepsize, float rdfmin, int32_t numPPTfeatures, float volume);

    /**
     * @brief check_RDFerror Computes the error between the current radial distribution function
     * and the goal radial distribution function
     * @param gadd Index used to determine which precipitate to add
     * @param gremove Index used to determine which precipitate to remove
     * @param double_count Determines whether to double count items in bins
     * @return Float error value between two distributions
     */
    float check_RDFerror(int32_t gadd, int32_t gremove, bool double_count);

    /**
     * @brief assign_voxels Assigns precipitate Id values to voxels within the packing grid
     */
    void assign_voxels();

    /**
     * @brief assign_gaps Assigns precipitate Id values to unassigned gaps within the packing grid
     */
    void assign_gaps();

    /**
     * @brief write_goal_attributes Outputs important algorithm data
     */
    void write_goal_attributes();

    /**
     * @brief find_xcoord Returns the x coordinate at a given index
     * @param index Index to determine coordinate
     * @return Float value of x coordinate
     */
    float find_xcoord(int64_t index);

    /**
     * @brief find_ycoord Returns the y coordinate at a given index
     * @param index Index to determine coordinate
     * @return Float value of y coordinate
     */
    float find_ycoord(int64_t index);

    /**
     * @brief find_zcoord Returns the z coordinate at a given index
     * @param index Index to determine coordinate
     * @return Float value of z coordinate
     */
    float find_zcoord(int64_t index);

    /**
     * @brief compare_1Ddistributions Computes the 1D Bhattacharyya distance
     * @param sqrerror Float 1D Bhattacharyya distance
     */
    void compare_1Ddistributions(std::vector<float>, std::vector<float>, float& sqrerror);

    /**
     * @brief compare_2Ddistributions Computes the 2D Bhattacharyya distance
     * @param sqrerror Float 1D Bhattacharyya distance
     */
    void compare_2Ddistributions(std::vector<std::vector<float> >, std::vector<std::vector<float> >, float& sqrerror);

    /**
     * @brief compare_3Ddistributions Computes the 3D Bhattacharyya distance
     * @param sqrerror Float 1D Bhattacharyya distance
     */
    void compare_3Ddistributions(std::vector<std::vector<std::vector<float> > >, std::vector<std::vector<std::vector<float> > >, float& sqrerror);

    std::vector<int> precipitatephases;
    std::vector<float> precipitatephasefractions;

  private:

    int32_t m_FirstPrecipitateFeature;
    uint64_t Seed;
    float m_SizeX;
    float m_SizeY;
    float m_SizeZ;
    float m_XRes;
    float m_YRes;
    float m_ZRes;
    float m_TotalVol;
    int64_t m_XPoints;
    int64_t m_YPoints;
    int64_t m_ZPoints;
    int64_t m_TotalPoints;

    QMap<uint32_t, ShapeOps*> m_ShapeOps;
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

    std::vector<std::vector<int64_t> > columnlist;
    std::vector<std::vector<int64_t> > rowlist;
    std::vector<std::vector<int64_t> > planelist;

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

    std::vector<int64_t> gsizes;

    size_t availablePointsCount;
    float m_currentRDFerror, m_oldRDFerror;
    float m_CurrentSizeDistError, m_OldSizeDistError;
    float m_rdfMax;
    float m_rdfMin;
    float m_StepSize;
    int32_t m_numRDFbins;

    void dataCheck();

    /**
     * @brief updateFeatureInstancePointers Resets the raw pointers that belong to a
     * Feature Attribute Matrix
     */
    void updateFeatureInstancePointers();

    InsertPrecipitatePhases(const InsertPrecipitatePhases&); // Copy Constructor Not Implemented
    void operator=(const InsertPrecipitatePhases&); // Operator '=' Not Implemented
};

#endif /* InsertPrecipitatePhases_H_ */
