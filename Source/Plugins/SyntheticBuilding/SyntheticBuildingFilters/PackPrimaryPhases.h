/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#ifndef _PackPrimaryPhases_H_
#define _PackPrimaryPhases_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"
#include "DREAM3DLib/Geometry/ShapeOps/ShapeOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"

typedef struct
{
  float m_Volumes;
  float m_EquivalentDiameters;
  float m_AxisLengths[3];
  float m_AxisEulerAngles[3];
  float m_Omega3s;
  int32_t m_FeaturePhases;
  int32_t m_Neighborhoods;
} Feature_t;

/**
 * @brief The PackPrimaryPhases class. See [Filter documentation](@ref packprimaryphases) for details.
 */
class PackPrimaryPhases : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(PackPrimaryPhases)
    DREAM3D_STATIC_NEW_MACRO(PackPrimaryPhases)
    DREAM3D_TYPE_MACRO_SUPER(PackPrimaryPhases, AbstractFilter)

    virtual ~PackPrimaryPhases();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, OutputCellAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath OutputCellAttributeMatrixPath READ getOutputCellAttributeMatrixPath WRITE setOutputCellAttributeMatrixPath)

    DREAM3D_INSTANCE_STRING_PROPERTY(OutputCellFeatureAttributeMatrixName)
    Q_PROPERTY(QString OutputCellFeatureAttributeMatrixName READ getOutputCellFeatureAttributeMatrixName WRITE setOutputCellFeatureAttributeMatrixName)

    DREAM3D_INSTANCE_STRING_PROPERTY(OutputCellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString OutputCellEnsembleAttributeMatrixName READ getOutputCellEnsembleAttributeMatrixName WRITE setOutputCellEnsembleAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, CellPhasesArrayName)
    Q_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, FeaturePhasesArrayName)
    Q_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, NumFeaturesArrayName)
    Q_PROPERTY(QString NumFeaturesArrayName READ getNumFeaturesArrayName WRITE setNumFeaturesArrayName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
    Q_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
    Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputShapeTypesArrayPath)
    Q_PROPERTY(DataArrayPath InputShapeTypesArrayPath READ getInputShapeTypesArrayPath WRITE setInputShapeTypesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, MaskArrayPath)
    Q_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)

    DREAM3D_FILTER_PARAMETER(bool, UseMask)
    Q_PROPERTY(bool UseMask READ getUseMask WRITE setUseMask)

    DREAM3D_FILTER_PARAMETER(bool, HaveFeatures)
    Q_PROPERTY(bool HaveFeatures READ getHaveFeatures WRITE setHaveFeatures)

    DREAM3D_FILTER_PARAMETER(QString, FeatureInputFile)
    Q_PROPERTY(QString FeatureInputFile READ getFeatureInputFile WRITE setFeatureInputFile)

    DREAM3D_FILTER_PARAMETER(QString, CsvOutputFile)
    Q_PROPERTY(QString CsvOutputFile READ getCsvOutputFile WRITE setCsvOutputFile)

    DREAM3D_FILTER_PARAMETER(bool, PeriodicBoundaries)
    Q_PROPERTY(bool PeriodicBoundaries READ getPeriodicBoundaries WRITE setPeriodicBoundaries)

    DREAM3D_FILTER_PARAMETER(bool, WriteGoalAttributes)
    Q_PROPERTY(bool WriteGoalAttributes READ getWriteGoalAttributes WRITE setWriteGoalAttributes)

    // THESE SHOULD GO AWAY THEY ARE FOR DEBUGGING ONLY
    DREAM3D_FILTER_PARAMETER(QString, ErrorOutputFile)
    Q_PROPERTY(QString ErrorOutputFile READ getErrorOutputFile WRITE setErrorOutputFile)

    DREAM3D_FILTER_PARAMETER(QString, VtkOutputFile)
    Q_PROPERTY(QString VtkOutputFile READ getVtkOutputFile WRITE setVtkOutputFile)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

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
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

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
    PackPrimaryPhases();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();
    /**
     * @brief initialize_packinggrid Initializes internal variables for packing grid dimensions based on
     * the incoming grid set by the user
     * @return
     */
    Int32ArrayType::Pointer initialize_packinggrid();

    /**
     * @brief place_features Organizes the placement of Features into the packing volume while ensuring
     * proper size and morphological statistics are maintained
     * @param featureOwnersPtr Array of Feature Ids for each packing point
     */
    void place_features(Int32ArrayType::Pointer featureOwnersPtr);

    /**
     * @brief generate_feature Creates a Feature by sampling the size and morphological statistical distributions
     * @param phase Index of the Ensemble type for the Feature to be generated
     * @param feature Feature_t struct pointer to be intialized
     * @param shapeclass Type of Feature shape to be generated
     */
    void generate_feature(int32_t phase, Feature_t* feature, uint32_t shapeclass);

    /**
     * @brief load_features Reads a list of Features from a file to be used as the packed volume
     */
    void load_features();

    /**
     * @brief transfer_attributes Moves variables held in the Feature_t struct into other arrays
     * @param gnum Id for the Feature to be copied
     * @param feature Feature_t struct pointer to be copied
     */
    void transfer_attributes(int32_t gnum, Feature_t* feature);

    /**
     * @brief insert_feature Performs the insertion of a Feature into the packing volume
     * @param gnum Id for the Feature to be inserted
     */
    void insert_feature(size_t gnum);

    /**
     * @brief move_feature Moves a Feature to the supplied (x,y,z) centroid coordinate
     * @param gnum Id for the Feature to be moved
     * @param xc x centroid coordinate
     * @param yc y centroid coordinate
     * @param zc z centroid coordinate
     */
    void move_feature(size_t gnum, float xc, float yc, float zc);

    /**
     * @brief check_sizedisterror Computes the error between the current Feature size distribution
     * and the goal Feature size distribution
     * @param feature Feature_t struct pointer used to determine the Ensemble type
     * @return Float error value between two distributions
     */
    float check_sizedisterror(Feature_t* feature);

    /**
     * @brief determine_neighbors Determines the neighbors for a given Feature
     * @param gnum Id for the Feature for which to find neighboring Features
     * @param add Value that determines whether to add or remove a Feature from the
     * list of neighbors
     */
    void determine_neighbors(size_t gnum, bool add);

    /**
     * @brief check_neighborhooderror Computes the error between the current Feature neighbor distribution
     * and the goal Feature neighbor distribution
     * @param gadd Value that determines whether to add a Feature for the neighbor list computation
     * @param gremove Value that determines whether to remove a Feature for the neighbor list computation
     * @return Float error value between two distributions
     */
    float check_neighborhooderror(int32_t gadd, int32_t gremove);

    /**
     * @brief check_fillingerror Computes the percentage of unassigned or multiple assigned packing points
     * @param gadd Value that determines whether to add point Ids to be filled
     * @param gremove Value that determines whether to add point Ids to be removed
     * @param featureOwnersPtr Array of Feature Ids for each packing point
     * @param exclusionOwnersPtr Array of exlusion Ids for each packing point
     * @return Float percentage value for the ratio of unassinged/"garbage" packing points
     */
    float check_fillingerror(int32_t gadd, int32_t gremove, Int32ArrayType::Pointer featureOwnersPtr, Int32ArrayType::Pointer exclusionOwnersPtr);

    /**
     * @brief update_availablepoints Updates the maps used to associate packing points with an "available" state
     * @param availablePoints Map between Feature owners and number of available points
     * @param availablePointsInv Inverse associations for the availablePoints map
     */
    void update_availablepoints(std::map<size_t, size_t>& availablePoints, std::map<size_t, size_t>& availablePointsInv);

    /**
     * @brief assign_voxels Assigns Feature Id values to voxels within the packing grid
     */
    void assign_voxels();

    /**
     * @brief assign_gaps_only Assigns Feature Id values to unassigned gaps within the packing grid
     */
    void assign_gaps_only();

    /**
     * @brief cleanup_features Assigns Feature Id values by cleaning up boundaries between Features
     */
    void cleanup_features();

    /**
     * @brief write_goal_attributes Outputs important algorithm data
     */
    void write_goal_attributes();

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
     * @brief writeVtkFile Outputs a debug VTK file for visualization
     * @param featureOwners Array of Feature Ids for each packing point
     * @param exclusionZonesPtr Array of exlusion Ids for each packing point
     * @return Integer error code
     */
    int32_t writeVtkFile(int32_t* featureOwners, int32_t* exclusionZonesPtr);

    /**
     * @brief estimate_numfeatures Estimates the number of Features that will be generated based on the supplied statistics
     * @param xpoints Extent of x dimension
     * @param ypoints Extent of y dimension
     * @param zpoints Extent of z dimension
     * @param xres Resolution of x dimension
     * @param yres Resolution of y dimension
     * @param zres Resolution of z dimension
     * @return Integer number of esimtated Features
     */
    int32_t estimate_numfeatures(size_t xpoints, size_t ypoints, size_t zpoints, float xres, float yres, float zres);

  private:

    // Names for the arrays used by the packing algorithm
    // These arrays are temporary and are removed from the Feature Attribute Matrix after completion
    QString m_NeighborhoodsArrayName;
    QString m_CentroidsArrayName;
    QString m_VolumesArrayName;
    QString m_AxisLengthsArrayName;
    QString m_AxisEulerAnglesArrayName;
    QString m_Omega3sArrayName;
    QString m_EquivalentDiametersArrayName;
    int64_t* m_Neighbors;

    // Cell Data - make sure these are all initialized to NULL in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_DATAARRAY_VARIABLE(bool, Mask)
    int8_t*  m_BoundaryCells;

    // Feature Data - make sure these are all initialized to NULL in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(int32_t, Neighborhoods)
    DEFINE_DATAARRAY_VARIABLE(float, Centroids)
    DEFINE_DATAARRAY_VARIABLE(float, Volumes)
    DEFINE_DATAARRAY_VARIABLE(float, AxisLengths)
    DEFINE_DATAARRAY_VARIABLE(float, AxisEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(float, Omega3s)
    DEFINE_DATAARRAY_VARIABLE(float, EquivalentDiameters)

    // Ensemble Data - make sure these are all initialized to NULL in the constructor
    DEFINE_DATAARRAY_VARIABLE(uint32_t, PhaseTypes)
    DEFINE_DATAARRAY_VARIABLE(uint32_t, ShapeTypes)
    DEFINE_DATAARRAY_VARIABLE(int32_t, NumFeatures)
    StatsDataArray::WeakPointer m_StatsDataArray;

    // All other private variables
    QVector<ShapeOps::Pointer> m_ShapeOps;
    ShapeOps::Pointer m_UnknownShapeOps;
    ShapeOps::Pointer m_CubicOctohedronOps;
    ShapeOps::Pointer m_CylinderOps;
    ShapeOps::Pointer m_EllipsoidOps;
    ShapeOps::Pointer m_SuperEllipsoidOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    std::vector<std::vector<int64_t> > columnlist;
    std::vector<std::vector<int64_t> > rowlist;
    std::vector<std::vector<int64_t> > planelist;
    std::vector<std::vector<float> > ellipfunclist;

    std::vector<size_t> pointsToAdd;
    std::vector<size_t> pointsToRemove;

    uint64_t m_Seed;

    int32_t firstPrimaryFeature;

    float sizex;
    float sizey;
    float sizez;
    float totalvol;
    float m_HalfPackingRes[3];
    float m_OneOverPackingRes[3];
    float m_OneOverHalfPackingRes[3];
    float m_PackingRes[3];

    int64_t m_PackingPoints[3];
    int64_t m_TotalPackingPoints;

    std::vector<std::vector<float> > featuresizedist;
    std::vector<std::vector<float> > simfeaturesizedist;
    std::vector<std::vector<std::vector<float> > > neighbordist;
    std::vector<std::vector<std::vector<float> > > simneighbordist;

    std::vector<float> featuresizediststep;
    std::vector<float> neighbordiststep;

    std::vector<int64_t> packqualities;
    std::vector<int64_t> gsizes;

    std::vector<int32_t> primaryphases;
    std::vector<float> primaryphasefractions;

    size_t availablePointsCount;
    float fillingerror, oldfillingerror;
    float currentneighborhooderror, oldneighborhooderror;
    float currentsizedisterror, oldsizedisterror;

    /**
     * @brief updateFeatureInstancePointers Resets the raw pointers that belong to a
     * Feature Attribute Matrix
     */
    void updateFeatureInstancePointers();

    PackPrimaryPhases(const PackPrimaryPhases&); // Copy Constructor Not Implemented
    void operator=(const PackPrimaryPhases&); // Operator '=' Not Implemented
};

#endif /* PackPrimaryPhases_H_ */
