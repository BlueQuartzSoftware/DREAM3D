/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _insertprecipitatephases_h_
#define _insertprecipitatephases_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/Geometry/ShapeOps/ShapeOps.h"
#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/LaueOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"

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
    Q_OBJECT
  public:
    SIMPL_SHARED_POINTERS(InsertPrecipitatePhases)
    SIMPL_STATIC_NEW_MACRO(InsertPrecipitatePhases)
    SIMPL_TYPE_MACRO_SUPER(InsertPrecipitatePhases, AbstractFilter)

    virtual ~InsertPrecipitatePhases();

    using EnumType = unsigned int;

    enum class SaveMethod : EnumType
    {
      DoNotSave = 0,
      SaveToNew = 1,
      AppendToExisting = 2
    };

    SIMPL_INSTANCE_STRING_PROPERTY(ClusteringListArrayName)

    SIMPL_INSTANCE_STRING_PROPERTY(ErrorOutputFile)

    SIMPL_FILTER_PARAMETER(QString, CsvOutputFile)
    Q_PROPERTY(QString CsvOutputFile READ getCsvOutputFile WRITE setCsvOutputFile)

    SIMPL_FILTER_PARAMETER(DataArrayPath, MaskArrayPath)
    Q_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)

    SIMPL_FILTER_PARAMETER(bool, UseMask)
    Q_PROPERTY(bool UseMask READ getUseMask WRITE setUseMask)

    SIMPL_FILTER_PARAMETER(bool, HavePrecips)
    Q_PROPERTY(bool HavePrecips READ getHavePrecips WRITE setHavePrecips)

    SIMPL_FILTER_PARAMETER(QString, PrecipInputFile)
    Q_PROPERTY(QString PrecipInputFile READ getPrecipInputFile WRITE setPrecipInputFile)

    SIMPL_FILTER_PARAMETER(bool, PeriodicBoundaries)
    Q_PROPERTY(bool PeriodicBoundaries READ getPeriodicBoundaries WRITE setPeriodicBoundaries)

    SIMPL_FILTER_PARAMETER(bool, MatchRDF)
    Q_PROPERTY(bool MatchRDF READ getMatchRDF WRITE setMatchRDF)

    SIMPL_FILTER_PARAMETER(bool, WriteGoalAttributes)
    Q_PROPERTY(bool WriteGoalAttributes READ getWriteGoalAttributes WRITE setWriteGoalAttributes)

    SIMPL_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
    Q_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
    Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, InputShapeTypesArrayPath)
    Q_PROPERTY(DataArrayPath InputShapeTypesArrayPath READ getInputShapeTypesArrayPath WRITE setInputShapeTypesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, BoundaryCellsArrayPath)
    Q_PROPERTY(DataArrayPath BoundaryCellsArrayPath READ getBoundaryCellsArrayPath WRITE setBoundaryCellsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    SIMPL_FILTER_PARAMETER(QString, NumCellsArrayName)
    Q_PROPERTY(QString NumCellsArrayName READ getNumCellsArrayName WRITE setNumCellsArrayName)

    SIMPL_FILTER_PARAMETER(QString, EquivalentDiametersArrayName)
    Q_PROPERTY(QString EquivalentDiametersArrayName READ getEquivalentDiametersArrayName WRITE setEquivalentDiametersArrayName)

    SIMPL_FILTER_PARAMETER(QString, VolumesArrayName)
    Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)

    SIMPL_FILTER_PARAMETER(QString, Omega3sArrayName)
    Q_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)

    SIMPL_FILTER_PARAMETER(QString, CentroidsArrayName)
    Q_PROPERTY(QString CentroidsArrayName READ getCentroidsArrayName WRITE setCentroidsArrayName)

    SIMPL_FILTER_PARAMETER(QString, AxisEulerAnglesArrayName)
    Q_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)

    SIMPL_FILTER_PARAMETER(QString, AxisLengthsArrayName)
    Q_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)

    SIMPL_FILTER_PARAMETER(DataArrayPath, NumFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)

    SIMPL_FILTER_PARAMETER(int, SaveGeometricDescriptions)
    Q_PROPERTY(int SaveGeometricDescriptions READ getSaveGeometricDescriptions WRITE setSaveGeometricDescriptions)

    SIMPL_FILTER_PARAMETER(DataArrayPath, NewAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath NewAttributeMatrixPath READ getNewAttributeMatrixPath WRITE setNewAttributeMatrixPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SelectedAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath SelectedAttributeMatrixPath READ getSelectedAttributeMatrixPath WRITE setSelectedAttributeMatrixPath)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString();

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief execute Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  signals:
    /**
     * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
     * be pushed from a user-facing control (such as a widget)
     * @param filter Filter instance pointer
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged Emitted when any Filter parameter is changed internally
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before calling dataCheck()
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
    void preflightExecuted();

  protected:

    InsertPrecipitatePhases();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();

    /**
     * @brief place_precipitates Organizes the placement of precipitates into the packing volume while ensuring
     * proper size and morphological statistics are maintained
     * @param exlusionZonesPtr Array of exclusion zone precipitate Ids for each packing point
     */
    void place_precipitates(Int32ArrayType::Pointer exlusionZonesPtr);

    /**
     * @brief generate_precipitate Creates a precipitate by sampling the size and morphological statistical distributions
     * @param phase Index of the Ensemble type for the Feature to be generated
     * @param precip Precip_t struct pointer to be intialized
     * @param shapeclass Type of precipitate shape to be generated
     * @param OrthoOps Pointer to LaueOps object
     */
    void generate_precipitate(int32_t phase, Precip_t* precip, ShapeType::Type shapeclass, LaueOps::Pointer OrthoOps);

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
    void update_exclusionZones(int32_t gadd, int32_t gremove, Int32ArrayType::Pointer exlusionZonesPtr);

//    /**
//     * @brief check_for_overlap Checks if the current placement will result in overlap for precipitate gNum
//     * @param gNum Index used to determine which precipitate to add
//     * @param exlusionZonesPtr Array of exclusion zone Feature Ids for each packing point
//     */
//    bool check_for_overlap(size_t gNum, Int32ArrayType::Pointer exlusionZonesPtr);

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
    std::vector<float> normalizeRDF(std::vector<float> rdf, int num_bins, float stepsize, float rdfmin, int32_t numPPTfeatures);

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

    /**
     * @brief Moves the temporary arrays that hold the inputs into the shape algorithms
     * into another attribute matrix
     */
    void moveShapeDescriptions();

  private:
    int32_t m_FirstPrecipitateFeature;
    float m_SizeX;
    float m_SizeY;
    float m_SizeZ;
    float m_XRes;
    float m_YRes;
    float m_ZRes;
    float m_TotalVol;
    float m_UseableTotalVol;
    int64_t m_XPoints;
    int64_t m_YPoints;
    int64_t m_ZPoints;
    int64_t m_TotalPoints;

    // Cell Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_DATAARRAY_VARIABLE(bool, Mask)
    DEFINE_DATAARRAY_VARIABLE(int8_t, BoundaryCells)

    // Feature Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(float, AxisEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(float, Centroids)
    DEFINE_DATAARRAY_VARIABLE(float, AxisLengths)
    DEFINE_DATAARRAY_VARIABLE(float, Volumes)
    DEFINE_DATAARRAY_VARIABLE(float, Omega3s)
    DEFINE_DATAARRAY_VARIABLE(float, EquivalentDiameters)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(int32_t, NumCells)
    NeighborList<float>::WeakPointer m_ClusteringList;

    // Ensemble Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(PhaseType::EnumType, PhaseTypes)
    DEFINE_DATAARRAY_VARIABLE(ShapeType::EnumType, ShapeTypes)
    DEFINE_DATAARRAY_VARIABLE(int32_t, NumFeatures)

    // All other private variables
    QVector<ShapeOps::Pointer> m_ShapeOps;
    ShapeOps::Pointer m_UnknownShapeOps;
    ShapeOps::Pointer m_CubicOctohedronOps;
    ShapeOps::Pointer m_CylinderOps;
    ShapeOps::Pointer m_EllipsoidOps;
    ShapeOps::Pointer m_SuperEllipsoidOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    int64_t* m_Neighbors;
    StatsDataArray::WeakPointer m_StatsDataArray;

    std::vector<std::vector<int64_t> > m_ColumnList;
    std::vector<std::vector<int64_t> > m_RowList;
    std::vector<std::vector<int64_t> > m_PlaneList;

    std::vector<size_t> m_PointsToAdd;
    std::vector<size_t> m_PointsToRemove;

    uint64_t m_Seed;

    std::vector<std::vector<float> > m_FeatureSizeDist;
    std::vector<std::vector<float> > m_SimFeatureSizeDist;
    std::vector<float> m_RdfTargetDist;
    std::vector<float> m_RdfCurrentDist;
    std::vector<float> m_RdfCurrentDistNorm;

    std::vector<float> m_RandomCentroids;
    std::vector<float> m_RdfRandom;

    std::vector<float> m_FeatureSizeDistStep;

    std::vector<int64_t> m_GSizes;

    size_t m_AvailablePointsCount;
    float m_currentRDFerror, m_oldRDFerror;
    float m_CurrentSizeDistError, m_OldSizeDistError;
    float m_rdfMax;
    float m_rdfMin;
    float m_StepSize;
    int32_t m_numRDFbins;


    std::vector<int32_t> m_PrecipitatePhases;
    std::vector<float> m_PrecipitatePhaseFractions;

    /**
     * @brief updateFeatureInstancePointers Resets the raw pointers that belong to a
     * Feature Attribute Matrix
     */
    void updateFeatureInstancePointers();

    /**
     * @brief saveToNewAttributeMatrix
     */
    void saveToNewAttributeMatrix(QList<IDataArray::Pointer> incomingArrays);

    /**
     * @brief appendToExistingAttributeMatrix
     */
    void appendToExistingAttributeMatrix(QList<IDataArray::Pointer> incomingArrays);

    InsertPrecipitatePhases(const InsertPrecipitatePhases&); // Copy Constructor Not Implemented
    void operator=(const InsertPrecipitatePhases&); // Operator '=' Not Implemented
};

#endif /* InsertPrecipitatePhases_H_ */
