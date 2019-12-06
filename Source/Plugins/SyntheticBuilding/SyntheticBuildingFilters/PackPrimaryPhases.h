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

#pragma once


#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ShapeOps/ShapeOps.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/SIMPLArray.hpp"

#include "OrientationLib/LaueOps/OrthoRhombicOps.h"

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

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The PackPrimaryPhases class. See [Filter documentation](@ref packprimaryphases) for details.
 */
class SyntheticBuilding_EXPORT PackPrimaryPhases : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(PackPrimaryPhases SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(PackPrimaryPhases)
  PYB11_FILTER_NEW_MACRO(PackPrimaryPhases)
  PYB11_FILTER_PARAMETER(DataArrayPath, OutputCellAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(QString, FeatureIdsArrayName)
  PYB11_FILTER_PARAMETER(QString, CellPhasesArrayName)
  PYB11_FILTER_PARAMETER(QString, FeaturePhasesArrayName)
  PYB11_FILTER_PARAMETER(QString, NumFeaturesArrayName)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputPhaseNamesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputShapeTypesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, MaskArrayPath)
  PYB11_FILTER_PARAMETER(bool, UseMask)
  PYB11_FILTER_PARAMETER(int, FeatureGeneration)
  PYB11_FILTER_PARAMETER(QString, FeatureInputFile)
  PYB11_FILTER_PARAMETER(QString, CsvOutputFile)
  PYB11_FILTER_PARAMETER(bool, PeriodicBoundaries)
  PYB11_FILTER_PARAMETER(bool, WriteGoalAttributes)
  PYB11_FILTER_PARAMETER(int, SaveGeometricDescriptions)
  PYB11_FILTER_PARAMETER(DataArrayPath, NewAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SelectedAttributeMatrixPath)
  PYB11_PROPERTY(DataArrayPath OutputCellAttributeMatrixPath READ getOutputCellAttributeMatrixPath WRITE setOutputCellAttributeMatrixPath)
  PYB11_PROPERTY(QString OutputCellFeatureAttributeMatrixName READ getOutputCellFeatureAttributeMatrixName WRITE setOutputCellFeatureAttributeMatrixName)
  PYB11_PROPERTY(QString OutputCellEnsembleAttributeMatrixName READ getOutputCellEnsembleAttributeMatrixName WRITE setOutputCellEnsembleAttributeMatrixName)
  PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
  PYB11_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)
  PYB11_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)
  PYB11_PROPERTY(QString NumFeaturesArrayName READ getNumFeaturesArrayName WRITE setNumFeaturesArrayName)
  PYB11_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)
  PYB11_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)
  PYB11_PROPERTY(DataArrayPath InputPhaseNamesArrayPath READ getInputPhaseNamesArrayPath WRITE setInputPhaseNamesArrayPath)
  PYB11_PROPERTY(DataArrayPath InputShapeTypesArrayPath READ getInputShapeTypesArrayPath WRITE setInputShapeTypesArrayPath)
  PYB11_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)
  PYB11_PROPERTY(bool UseMask READ getUseMask WRITE setUseMask)
  PYB11_PROPERTY(int FeatureGeneration READ getFeatureGeneration WRITE setFeatureGeneration)
  PYB11_PROPERTY(QString FeatureInputFile READ getFeatureInputFile WRITE setFeatureInputFile)
  PYB11_PROPERTY(QString CsvOutputFile READ getCsvOutputFile WRITE setCsvOutputFile)
  PYB11_PROPERTY(bool PeriodicBoundaries READ getPeriodicBoundaries WRITE setPeriodicBoundaries)
  PYB11_PROPERTY(bool WriteGoalAttributes READ getWriteGoalAttributes WRITE setWriteGoalAttributes)
  PYB11_PROPERTY(int SaveGeometricDescriptions READ getSaveGeometricDescriptions WRITE setSaveGeometricDescriptions)
  PYB11_PROPERTY(DataArrayPath NewAttributeMatrixPath READ getNewAttributeMatrixPath WRITE setNewAttributeMatrixPath)
  PYB11_PROPERTY(DataArrayPath SelectedAttributeMatrixPath READ getSelectedAttributeMatrixPath WRITE setSelectedAttributeMatrixPath)
#endif

public:
  using Self = PackPrimaryPhases;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for PackPrimaryPhases
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for PackPrimaryPhases
   */
  static QString ClassName();

  ~PackPrimaryPhases() override;

  using EnumType = unsigned int;

  enum class SaveMethod : EnumType
  {
    DoNotSave = 0,
    SaveToNew = 1,
    AppendToExisting = 2
  };

  /**
   * @brief Setter property for OutputCellAttributeMatrixPath
   */
  void setOutputCellAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for OutputCellAttributeMatrixPath
   * @return Value of OutputCellAttributeMatrixPath
   */
  DataArrayPath getOutputCellAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath OutputCellAttributeMatrixPath READ getOutputCellAttributeMatrixPath WRITE setOutputCellAttributeMatrixPath)

  /**
   * @brief Setter property for OutputCellFeatureAttributeMatrixName
   */
  void setOutputCellFeatureAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for OutputCellFeatureAttributeMatrixName
   * @return Value of OutputCellFeatureAttributeMatrixName
   */
  QString getOutputCellFeatureAttributeMatrixName() const;

  Q_PROPERTY(QString OutputCellFeatureAttributeMatrixName READ getOutputCellFeatureAttributeMatrixName WRITE setOutputCellFeatureAttributeMatrixName)

  /**
   * @brief Setter property for OutputCellEnsembleAttributeMatrixName
   */
  void setOutputCellEnsembleAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for OutputCellEnsembleAttributeMatrixName
   * @return Value of OutputCellEnsembleAttributeMatrixName
   */
  QString getOutputCellEnsembleAttributeMatrixName() const;

  Q_PROPERTY(QString OutputCellEnsembleAttributeMatrixName READ getOutputCellEnsembleAttributeMatrixName WRITE setOutputCellEnsembleAttributeMatrixName)

  /**
   * @brief Setter property for FeatureIdsArrayName
   */
  void setFeatureIdsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureIdsArrayName
   * @return Value of FeatureIdsArrayName
   */
  QString getFeatureIdsArrayName() const;

  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

  /**
   * @brief Setter property for CellPhasesArrayName
   */
  void setCellPhasesArrayName(const QString& value);
  /**
   * @brief Getter property for CellPhasesArrayName
   * @return Value of CellPhasesArrayName
   */
  QString getCellPhasesArrayName() const;

  Q_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)

  /**
   * @brief Setter property for FeaturePhasesArrayName
   */
  void setFeaturePhasesArrayName(const QString& value);
  /**
   * @brief Getter property for FeaturePhasesArrayName
   * @return Value of FeaturePhasesArrayName
   */
  QString getFeaturePhasesArrayName() const;

  Q_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)

  /**
   * @brief Setter property for NumFeaturesArrayName
   */
  void setNumFeaturesArrayName(const QString& value);
  /**
   * @brief Getter property for NumFeaturesArrayName
   * @return Value of NumFeaturesArrayName
   */
  QString getNumFeaturesArrayName() const;

  Q_PROPERTY(QString NumFeaturesArrayName READ getNumFeaturesArrayName WRITE setNumFeaturesArrayName)

  /**
   * @brief Setter property for InputStatsArrayPath
   */
  void setInputStatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputStatsArrayPath
   * @return Value of InputStatsArrayPath
   */
  DataArrayPath getInputStatsArrayPath() const;

  Q_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)

  /**
   * @brief Setter property for InputPhaseTypesArrayPath
   */
  void setInputPhaseTypesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputPhaseTypesArrayPath
   * @return Value of InputPhaseTypesArrayPath
   */
  DataArrayPath getInputPhaseTypesArrayPath() const;

  Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)

  /**
   * @brief Setter property for InputPhaseNamesArrayPath
   */
  void setInputPhaseNamesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputPhaseNamesArrayPath
   * @return Value of InputPhaseNamesArrayPath
   */
  DataArrayPath getInputPhaseNamesArrayPath() const;

  Q_PROPERTY(DataArrayPath InputPhaseNamesArrayPath READ getInputPhaseNamesArrayPath WRITE setInputPhaseNamesArrayPath)

  /**
   * @brief Setter property for InputShapeTypesArrayPath
   */
  void setInputShapeTypesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputShapeTypesArrayPath
   * @return Value of InputShapeTypesArrayPath
   */
  DataArrayPath getInputShapeTypesArrayPath() const;

  Q_PROPERTY(DataArrayPath InputShapeTypesArrayPath READ getInputShapeTypesArrayPath WRITE setInputShapeTypesArrayPath)

  /**
   * @brief Setter property for MaskArrayPath
   */
  void setMaskArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for MaskArrayPath
   * @return Value of MaskArrayPath
   */
  DataArrayPath getMaskArrayPath() const;

  Q_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)

  /**
   * @brief Setter property for UseMask
   */
  void setUseMask(bool value);
  /**
   * @brief Getter property for UseMask
   * @return Value of UseMask
   */
  bool getUseMask() const;

  Q_PROPERTY(bool UseMask READ getUseMask WRITE setUseMask)

  /**
   * @brief Setter property for FeatureGeneration
   */
  void setFeatureGeneration(int value);
  /**
   * @brief Getter property for FeatureGeneration
   * @return Value of FeatureGeneration
   */
  int getFeatureGeneration() const;

  Q_PROPERTY(int FeatureGeneration READ getFeatureGeneration WRITE setFeatureGeneration)

  /**
   * @brief Setter property for FeatureInputFile
   */
  void setFeatureInputFile(const QString& value);
  /**
   * @brief Getter property for FeatureInputFile
   * @return Value of FeatureInputFile
   */
  QString getFeatureInputFile() const;

  Q_PROPERTY(QString FeatureInputFile READ getFeatureInputFile WRITE setFeatureInputFile)

  /**
   * @brief Setter property for CsvOutputFile
   */
  void setCsvOutputFile(const QString& value);
  /**
   * @brief Getter property for CsvOutputFile
   * @return Value of CsvOutputFile
   */
  QString getCsvOutputFile() const;

  Q_PROPERTY(QString CsvOutputFile READ getCsvOutputFile WRITE setCsvOutputFile)

  /**
   * @brief Setter property for PeriodicBoundaries
   */
  void setPeriodicBoundaries(bool value);
  /**
   * @brief Getter property for PeriodicBoundaries
   * @return Value of PeriodicBoundaries
   */
  bool getPeriodicBoundaries() const;

  Q_PROPERTY(bool PeriodicBoundaries READ getPeriodicBoundaries WRITE setPeriodicBoundaries)

  /**
   * @brief Setter property for WriteGoalAttributes
   */
  void setWriteGoalAttributes(bool value);
  /**
   * @brief Getter property for WriteGoalAttributes
   * @return Value of WriteGoalAttributes
   */
  bool getWriteGoalAttributes() const;

  Q_PROPERTY(bool WriteGoalAttributes READ getWriteGoalAttributes WRITE setWriteGoalAttributes)

  /**
   * @brief Setter property for SaveGeometricDescriptions
   */
  void setSaveGeometricDescriptions(int value);
  /**
   * @brief Getter property for SaveGeometricDescriptions
   * @return Value of SaveGeometricDescriptions
   */
  int getSaveGeometricDescriptions() const;

  Q_PROPERTY(int SaveGeometricDescriptions READ getSaveGeometricDescriptions WRITE setSaveGeometricDescriptions)

  /**
   * @brief Setter property for NewAttributeMatrixPath
   */
  void setNewAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NewAttributeMatrixPath
   * @return Value of NewAttributeMatrixPath
   */
  DataArrayPath getNewAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath NewAttributeMatrixPath READ getNewAttributeMatrixPath WRITE setNewAttributeMatrixPath)

  /**
   * @brief Setter property for SelectedAttributeMatrixPath
   */
  void setSelectedAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedAttributeMatrixPath
   * @return Value of SelectedAttributeMatrixPath
   */
  DataArrayPath getSelectedAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath SelectedAttributeMatrixPath READ getSelectedAttributeMatrixPath WRITE setSelectedAttributeMatrixPath)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
  * @brief readFilterParametersFromJson Reads the filter parameters from a file
  * @param reader Reader that is used to read the parameters from a file
  */
  void readFilterParameters(QJsonObject& obj) override;

  /**
  * @brief writeFilterParametersToJson Writes the filter parameters to a file
  * @param root The root json object
  */
  void writeFilterParameters(QJsonObject& obj) const override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

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
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief initialize_packinggrid Initializes internal variables for packing grid dimensions based on
   * the incoming grid set by the user
   * @return
   */
  Int32ArrayType::Pointer initializePackingGrid();

  /**
   * @brief place_features Organizes the placement of Features into the packing volume while ensuring
   * proper size and morphological statistics are maintained
   * @param featureOwnersPtr Array of Feature Ids for each packing point
   */
  void placeFeatures(Int32ArrayType::Pointer featureOwnersPtr);

  /**
   * @brief generate_feature Creates a Feature by sampling the size and morphological statistical distributions
   * @param phase Index of the Ensemble type for the Feature to be generated
   * @param feature Feature_t struct pointer to be intialized
   * @param shapeclass Type of Feature shape to be generated
   */
  void generateFeature(int32_t phase, Feature_t* feature, uint32_t shapeclass);

  /**
   * @brief load_features Reads a list of Features from a file to be used as the packed volume
   */
  void loadFeatures();

  /**
   * @brief transfer_attributes Moves variables held in the Feature_t struct into other arrays
   * @param gnum Id for the Feature to be copied
   * @param feature Feature_t struct pointer to be copied
   */
  void transferAttributes(int32_t gnum, Feature_t* feature);

  /**
   * @brief insert_feature Performs the insertion of a Feature into the packing volume
   * @param gnum Id for the Feature to be inserted
   */
  void insertFeature(size_t gnum);

  /**
   * @brief move_feature Moves a Feature to the supplied (x,y,z) centroid coordinate
   * @param gnum Id for the Feature to be moved
   * @param xc x centroid coordinate
   * @param yc y centroid coordinate
   * @param zc z centroid coordinate
   */
  void moveFeature(size_t gnum, float xc, float yc, float zc);

  /**
   * @brief check_sizedisterror Computes the error between the current Feature size distribution
   * and the goal Feature size distribution
   * @param feature Feature_t struct pointer used to determine the Ensemble type
   * @return Float error value between two distributions
   */
  float checkSizeDistError(Feature_t* feature);

  /**
   * @brief determine_neighbors Determines the neighbors for a given Feature
   * @param gnum Id for the Feature for which to find neighboring Features
   * @param add Value that determines whether to add or remove a Feature from the
   * list of neighbors
   */
  void determineNeighbors(size_t gnum, bool add);

  /**
   * @brief check_neighborhooderror Computes the error between the current Feature neighbor distribution
   * and the goal Feature neighbor distribution
   * @param gadd Value that determines whether to add a Feature for the neighbor list computation
   * @param gremove Value that determines whether to remove a Feature for the neighbor list computation
   * @return Float error value between two distributions
   */
  float checkNeighborhoodError(int32_t gadd, int32_t gremove);

  /**
   * @brief check_fillingerror Computes the percentage of unassigned or multiple assigned packing points
   * @param gadd Value that determines whether to add point Ids to be filled
   * @param gremove Value that determines whether to add point Ids to be removed
   * @param featureOwnersPtr Array of Feature Ids for each packing point
   * @param exclusionOwnersPtr Array of exlusion Ids for each packing point
   * @return Float percentage value for the ratio of unassinged/"garbage" packing points
   */
  float checkFillingError(int32_t gadd, int32_t gremove, Int32ArrayType::Pointer featureOwnersPtr, Int32ArrayType::Pointer exclusionOwnersPtr);

  /**
   * @brief update_availablepoints Updates the maps used to associate packing points with an "available" state
   * @param availablePoints Map between Feature owners and number of available points
   * @param availablePointsInv Inverse associations for the availablePoints map
   */
  void updateAvailablePoints(std::map<size_t, size_t>& availablePoints, std::map<size_t, size_t>& availablePointsInv);

  /**
   * @brief assign_voxels Assigns Feature Id values to voxels within the packing grid
   */
  void assignVoxels();

  /**
   * @brief assign_gaps_only Assigns Feature Id values to unassigned gaps within the packing grid
   */
  void assignGapsOnly();

  /**
   * @brief cleanup_features Assigns Feature Id values by cleaning up boundaries between Features
   */
  void cleanupFeatures();

  /**
   * @brief write_goal_attributes Outputs important algorithm data
   */
  void writeGoalAttributes();

  /**
   * @brief compare_1Ddistributions Computes the 1D Bhattacharyya distance
   * @param sqrerror Float 1D Bhattacharyya distance
   */
  void compare1dDistributions(std::vector<float>, std::vector<float>, float& sqrerror);

  /**
   * @brief compare_2Ddistributions Computes the 2D Bhattacharyya distance
   * @param sqrerror Float 1D Bhattacharyya distance
   */
  void compare2dDistributions(std::vector<std::vector<float>>, std::vector<std::vector<float>>, float& sqrerror);

  /**
   * @brief compare_3Ddistributions Computes the 3D Bhattacharyya distance
   * @param sqrerror Float 1D Bhattacharyya distance
   */
  void compare3dDistributions(std::vector<std::vector<std::vector<float>>>, std::vector<std::vector<std::vector<float>>>, float& sqrerror);

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
   * @param xres Spacing of x dimension
   * @param yres Spacing of y dimension
   * @param zres Spacing of z dimension
   * @return Integer number of esimtated Features
   */
  int32_t estimateNumFeatures(size_t xpoints, size_t ypoints, size_t zpoints, float xres, float yres, float zres);

  /**
   * @brief Moves the temporary arrays that hold the inputs into the shape algorithms
   * into another attribute matrix
   */
  void moveShapeDescriptions();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<bool>> m_MaskPtr;
  bool* m_Mask = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NeighborhoodsPtr;
  int32_t* m_Neighborhoods = nullptr;
  std::weak_ptr<DataArray<float>> m_CentroidsPtr;
  float* m_Centroids = nullptr;
  std::weak_ptr<DataArray<float>> m_VolumesPtr;
  float* m_Volumes = nullptr;
  std::weak_ptr<DataArray<float>> m_AxisLengthsPtr;
  float* m_AxisLengths = nullptr;
  std::weak_ptr<DataArray<float>> m_AxisEulerAnglesPtr;
  float* m_AxisEulerAngles = nullptr;
  std::weak_ptr<DataArray<float>> m_Omega3sPtr;
  float* m_Omega3s = nullptr;
  std::weak_ptr<DataArray<float>> m_EquivalentDiametersPtr;
  float* m_EquivalentDiameters = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_PhaseTypesPtr;
  uint32_t* m_PhaseTypes = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_ShapeTypesPtr;
  uint32_t* m_ShapeTypes = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NumFeaturesPtr;
  int32_t* m_NumFeatures = nullptr;

  DataArrayPath m_OutputCellAttributeMatrixPath = {};
  QString m_OutputCellFeatureAttributeMatrixName = {};
  QString m_OutputCellEnsembleAttributeMatrixName = {};
  QString m_FeatureIdsArrayName = {};
  QString m_CellPhasesArrayName = {};
  QString m_FeaturePhasesArrayName = {};
  QString m_NumFeaturesArrayName = {};
  DataArrayPath m_InputStatsArrayPath = {};
  DataArrayPath m_InputPhaseTypesArrayPath = {};
  DataArrayPath m_InputPhaseNamesArrayPath = {};
  DataArrayPath m_InputShapeTypesArrayPath = {};
  DataArrayPath m_MaskArrayPath = {};
  bool m_UseMask = {};
  int m_FeatureGeneration = {};
  QString m_FeatureInputFile = {};
  QString m_CsvOutputFile = {};
  bool m_PeriodicBoundaries = {};
  bool m_WriteGoalAttributes = {};
  int m_SaveGeometricDescriptions = {};
  DataArrayPath m_NewAttributeMatrixPath = {};
  DataArrayPath m_SelectedAttributeMatrixPath = {};

  // Names for the arrays used by the packing algorithm
  // These arrays are temporary and are removed from the Feature Attribute Matrix after completion
  QString m_NeighborhoodsArrayName;
  QString m_CentroidsArrayName;
  QString m_VolumesArrayName;
  QString m_AxisLengthsArrayName;
  QString m_AxisEulerAnglesArrayName;
  QString m_Omega3sArrayName;
  QString m_EquivalentDiametersArrayName;
  int64_t* m_Neighbors = nullptr;

  int8_t* m_BoundaryCells = nullptr;

  StringDataArray::WeakPointer m_PhaseNamesPtr;

  StatsDataArray::WeakPointer m_StatsDataArray;

  // All other private variables
  QVector<ShapeOps::Pointer> m_ShapeOps;
  ShapeOps::Pointer m_UnknownShapeOps;
  ShapeOps::Pointer m_CubicOctohedronOps;
  ShapeOps::Pointer m_CylinderOps;
  ShapeOps::Pointer m_EllipsoidOps;
  ShapeOps::Pointer m_SuperEllipsoidOps;

  std::vector<std::vector<int64_t>> m_ColumnList;
  std::vector<std::vector<int64_t>> m_RowList;
  std::vector<std::vector<int64_t>> m_PlaneList;
  std::vector<std::vector<float>> m_EllipFuncList;

  std::vector<size_t> m_PointsToAdd;
  std::vector<size_t> m_PointsToRemove;

  uint64_t m_Seed;

  int32_t m_FirstPrimaryFeature;

  float m_SizeX;
  float m_SizeY;
  float m_SizeZ;
  float m_TotalVol;
  FloatVec3Type m_HalfPackingRes;
  FloatVec3Type m_OneOverPackingRes;
  FloatVec3Type m_OneOverHalfPackingRes;
  FloatVec3Type m_PackingRes;

  int64_t m_PackingPoints[3];
  int64_t m_TotalPackingPoints;

  std::vector<std::vector<float>> m_FeatureSizeDist;
  std::vector<std::vector<float>> m_SimFeatureSizeDist;
  std::vector<std::vector<std::vector<float>>> m_NeighborDist;
  std::vector<std::vector<std::vector<float>>> m_SimNeighborDist;

  std::vector<float> m_FeatureSizeDistStep;
  std::vector<float> m_NeighborDistStep;

  std::vector<int64_t> m_PackQualities;
  std::vector<int64_t> m_GSizes;

  std::vector<int32_t> m_PrimaryPhases;
  std::vector<float> m_PrimaryPhaseFractions;

  size_t m_AvailablePointsCount;
  float m_FillingError, m_OldFillingError;
  float m_CurrentNeighborhoodError, m_OldNeighborhoodError;
  float m_CurrentSizeDistError, m_OldSizeDistError;

  QString m_ErrorOutputFile;
  QString m_VtkOutputFile;

  /**
   * @brief updateFeatureInstancePointers Resets the raw pointers that belong to a
   * Feature Attribute Matrix
   */
  void updateFeatureInstancePointers();

public:
  PackPrimaryPhases(const PackPrimaryPhases&) = delete; // Copy Constructor Not Implemented
  PackPrimaryPhases(PackPrimaryPhases&&) = delete;      // Move Constructor Not Implemented
  PackPrimaryPhases& operator=(const PackPrimaryPhases&) = delete; // Copy Assignment Not Implemented
  PackPrimaryPhases& operator=(PackPrimaryPhases&&) = delete;      // Move Assignment Not Implemented
};

