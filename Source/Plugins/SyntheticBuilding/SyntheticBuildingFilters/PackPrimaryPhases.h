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

#ifndef _PackPrimaryPhases_H_
#define _PackPrimaryPhases_H_

#include <vector>
#include <map>
#include <QtCore/QString>

#include <boost/shared_array.hpp>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"
#include "DREAM3DLib/StatsData/StatsData.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/ShapeOps/ShapeOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
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
} Feature;

/**
 * @class PackPrimaryPhases PackPrimaryPhases.h DREAM3DLib/SyntheticBuilderFilters/PackPrimaryPhases.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class PackPrimaryPhases : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(PackPrimaryPhases)
    DREAM3D_STATIC_NEW_MACRO(PackPrimaryPhases)
    DREAM3D_TYPE_MACRO_SUPER(PackPrimaryPhases, AbstractFilter)

    virtual ~PackPrimaryPhases();
    DREAM3D_FILTER_PARAMETER(DataArrayPath, OutputCellAttributeMatrixName)
    Q_PROPERTY(DataArrayPath OutputCellAttributeMatrixName READ getOutputCellAttributeMatrixName WRITE setOutputCellAttributeMatrixName)

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

    DREAM3D_FILTER_PARAMETER(QString, NeighborhoodsArrayName)
    Q_PROPERTY(QString NeighborhoodsArrayName READ getNeighborhoodsArrayName WRITE setNeighborhoodsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, CentroidsArrayName)
    Q_PROPERTY(QString CentroidsArrayName READ getCentroidsArrayName WRITE setCentroidsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, VolumesArrayName)
    Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, AxisLengthsArrayName)
    Q_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, AxisEulerAnglesArrayName)
    Q_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, Omega3sArrayName)
    Q_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)

    DREAM3D_FILTER_PARAMETER(QString, EquivalentDiametersArrayName)
    Q_PROPERTY(QString EquivalentDiametersArrayName READ getEquivalentDiametersArrayName WRITE setEquivalentDiametersArrayName)


    typedef boost::shared_array<float> SharedFloatArray;
    typedef boost::shared_array<int> SharedIntArray;

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

    virtual void preflight();

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();



  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    PackPrimaryPhases();

    Int32ArrayType::Pointer initialize_packinggrid();

    void place_features(Int32ArrayType::Pointer featureOwnersPtr);
    void generate_feature(int phase, int Seed, Feature* feature, unsigned int shapeclass);
    void load_features();


    void transfer_attributes(int gnum, Feature* feature);
    void insert_feature(size_t featureNum);

    void move_feature(size_t featureNum, float xc, float yc, float zc);

    float check_sizedisterror(Feature* feature);
    void determine_neighbors(size_t featureNum, int add);
    float check_neighborhooderror(int gadd, int gremove);

    float check_fillingerror(int gadd, int gremove, Int32ArrayType::Pointer featureOwnersPtr, Int32ArrayType::Pointer exclusionOwnersPtr);
    void update_availablepoints(std::map<size_t,size_t> &availablePoints, std::map<size_t,size_t> &availablePointsInv);
    void assign_voxels();
    void assign_gaps_only();
    void cleanup_features();
    void write_goal_attributes();

    void compare_1Ddistributions(std::vector<float>, std::vector<float>, float& sqrerror);
    void compare_2Ddistributions(std::vector<std::vector<float> >, std::vector<std::vector<float> >, float& sqrerror);
    void compare_3Ddistributions(std::vector<std::vector<std::vector<float> > >, std::vector<std::vector<std::vector<float> > >, float& sqrerror);

    int writeVtkFile(int32_t* featureOwners, int32_t* exclusionZonesPtr);
    int estimate_numfeatures(int xpoints, int ypoints, int zpoints, float xres, float yres, float zres);


  private:
    int32_t* m_Neighbors;

    // Cell Data - make sure these are all initialized to NULL in the constructor
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, Mask)
    int8_t*  m_BoundaryCells;

    // Feature Data - make sure these are all initialized to NULL in the constructor
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, NumFeatures)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, Neighborhoods)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Centroids)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Volumes)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, AxisLengths)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, AxisEulerAngles)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Omega3s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, EquivalentDiameters)

    // Ensemble Data - make sure these are all initialized to NULL in the constructor
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, PhaseTypes)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, ShapeTypes)
    StatsDataArray::WeakPointer m_StatsDataArray;

    // All other private variables
    QMap<unsigned int, ShapeOps*> m_ShapeOps;
    ShapeOps::Pointer m_UnknownShapeOps;
    ShapeOps::Pointer m_CubicOctohedronOps;
    ShapeOps::Pointer m_CylinderOps;
    ShapeOps::Pointer m_EllipsoidOps;
    ShapeOps::Pointer m_SuperEllipsoidOps;

    OrthoRhombicOps::Pointer m_OrthoOps;

    std::vector<std::vector<int> > columnlist;
    std::vector<std::vector<int> > rowlist;
    std::vector<std::vector<int> > planelist;
    std::vector<std::vector<float> > ellipfunclist;

    std::vector<size_t> pointsToAdd;
    std::vector<size_t> pointsToRemove;

    unsigned long long int m_Seed;

    int firstPrimaryFeature;

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

    std::vector<std::vector<float> > featuresizedist;
    std::vector<std::vector<float> > simfeaturesizedist;
    std::vector<std::vector<std::vector<float> > > neighbordist;
    std::vector<std::vector<std::vector<float> > > simneighbordist;

    std::vector<float> featuresizediststep;
    std::vector<float> neighbordiststep;

    std::vector<int> newnames;
    std::vector<int> packqualities;
    std::vector<int> gsizes;

    std::vector<int> primaryphases;
    std::vector<float> primaryphasefractions;

    size_t availablePointsCount;
    float fillingerror, oldfillingerror;
    float currentneighborhooderror, oldneighborhooderror;
    float currentsizedisterror, oldsizedisterror;

    void dataCheck();
    void updateFeatureInstancePointers();

    PackPrimaryPhases(const PackPrimaryPhases&); // Copy Constructor Not Implemented
    void operator=(const PackPrimaryPhases&); // Operator '=' Not Implemented
};

#endif /* PackPrimaryPhases_H_ */



