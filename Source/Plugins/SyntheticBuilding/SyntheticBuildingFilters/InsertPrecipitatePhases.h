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

#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/LaueOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ShapeOps/ShapeOps.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

class LaueOps;

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

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The InsertPrecipitatePhases class. See [Filter documentation](@ref insertprecipitatephases) for details.
 */
class SyntheticBuilding_EXPORT InsertPrecipitatePhases : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(InsertPrecipitatePhases SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(InsertPrecipitatePhases)
  PYB11_FILTER_NEW_MACRO(InsertPrecipitatePhases)
  PYB11_FILTER_PARAMETER(QString, CsvOutputFile)
  PYB11_FILTER_PARAMETER(DataArrayPath, MaskArrayPath)
  PYB11_FILTER_PARAMETER(bool, UseMask)
  PYB11_FILTER_PARAMETER(int, FeatureGeneration)
  PYB11_FILTER_PARAMETER(QString, PrecipInputFile)
  PYB11_FILTER_PARAMETER(bool, PeriodicBoundaries)
  PYB11_FILTER_PARAMETER(bool, MatchRDF)
  PYB11_FILTER_PARAMETER(bool, WriteGoalAttributes)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputShapeTypesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, BoundaryCellsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(QString, NumCellsArrayName)
  PYB11_FILTER_PARAMETER(QString, EquivalentDiametersArrayName)
  PYB11_FILTER_PARAMETER(QString, VolumesArrayName)
  PYB11_FILTER_PARAMETER(QString, Omega3sArrayName)
  PYB11_FILTER_PARAMETER(QString, CentroidsArrayName)
  PYB11_FILTER_PARAMETER(QString, AxisEulerAnglesArrayName)
  PYB11_FILTER_PARAMETER(QString, AxisLengthsArrayName)
  PYB11_FILTER_PARAMETER(DataArrayPath, NumFeaturesArrayPath)
  PYB11_FILTER_PARAMETER(int, SaveGeometricDescriptions)
  PYB11_FILTER_PARAMETER(DataArrayPath, NewAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SelectedAttributeMatrixPath)
  PYB11_PROPERTY(QString CsvOutputFile READ getCsvOutputFile WRITE setCsvOutputFile)
  PYB11_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)
  PYB11_PROPERTY(bool UseMask READ getUseMask WRITE setUseMask)
  PYB11_PROPERTY(int FeatureGeneration READ getFeatureGeneration WRITE setFeatureGeneration)
  PYB11_PROPERTY(QString PrecipInputFile READ getPrecipInputFile WRITE setPrecipInputFile)
  PYB11_PROPERTY(bool PeriodicBoundaries READ getPeriodicBoundaries WRITE setPeriodicBoundaries)
  PYB11_PROPERTY(bool MatchRDF READ getMatchRDF WRITE setMatchRDF)
  PYB11_PROPERTY(bool WriteGoalAttributes READ getWriteGoalAttributes WRITE setWriteGoalAttributes)
  PYB11_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)
  PYB11_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)
  PYB11_PROPERTY(DataArrayPath InputShapeTypesArrayPath READ getInputShapeTypesArrayPath WRITE setInputShapeTypesArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath BoundaryCellsArrayPath READ getBoundaryCellsArrayPath WRITE setBoundaryCellsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(QString NumCellsArrayName READ getNumCellsArrayName WRITE setNumCellsArrayName)
  PYB11_PROPERTY(QString EquivalentDiametersArrayName READ getEquivalentDiametersArrayName WRITE setEquivalentDiametersArrayName)
  PYB11_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)
  PYB11_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)
  PYB11_PROPERTY(QString CentroidsArrayName READ getCentroidsArrayName WRITE setCentroidsArrayName)
  PYB11_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)
  PYB11_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)
  PYB11_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)
  PYB11_PROPERTY(int SaveGeometricDescriptions READ getSaveGeometricDescriptions WRITE setSaveGeometricDescriptions)
  PYB11_PROPERTY(DataArrayPath NewAttributeMatrixPath READ getNewAttributeMatrixPath WRITE setNewAttributeMatrixPath)
  PYB11_PROPERTY(DataArrayPath SelectedAttributeMatrixPath READ getSelectedAttributeMatrixPath WRITE setSelectedAttributeMatrixPath)
#endif

public:
  using Self = InsertPrecipitatePhases;
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
   * @brief Returns the name of the class for InsertPrecipitatePhases
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for InsertPrecipitatePhases
   */
  static QString ClassName();

  ~InsertPrecipitatePhases() override;

  using EnumType = unsigned int;

  enum class SaveMethod : EnumType
  {
    DoNotSave = 0,
    SaveToNew = 1,
    AppendToExisting = 2
  };

  /**
   * @brief Setter property for ClusteringListArrayName
   */
  void setClusteringListArrayName(const QString& value);
  /**
   * @brief Getter property for ClusteringListArrayName
   * @return Value of ClusteringListArrayName
   */
  QString getClusteringListArrayName() const;

  /**
   * @brief Setter property for ErrorOutputFile
   */
  void setErrorOutputFile(const QString& value);
  /**
   * @brief Getter property for ErrorOutputFile
   * @return Value of ErrorOutputFile
   */
  QString getErrorOutputFile() const;

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
   * @brief Setter property for PrecipInputFile
   */
  void setPrecipInputFile(const QString& value);
  /**
   * @brief Getter property for PrecipInputFile
   * @return Value of PrecipInputFile
   */
  QString getPrecipInputFile() const;

  Q_PROPERTY(QString PrecipInputFile READ getPrecipInputFile WRITE setPrecipInputFile)

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
   * @brief Setter property for MatchRDF
   */
  void setMatchRDF(bool value);
  /**
   * @brief Getter property for MatchRDF
   * @return Value of MatchRDF
   */
  bool getMatchRDF() const;

  Q_PROPERTY(bool MatchRDF READ getMatchRDF WRITE setMatchRDF)

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
   * @brief Setter property for FeatureIdsArrayPath
   */
  void setFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsArrayPath
   * @return Value of FeatureIdsArrayPath
   */
  DataArrayPath getFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  /**
   * @brief Setter property for CellPhasesArrayPath
   */
  void setCellPhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellPhasesArrayPath
   * @return Value of CellPhasesArrayPath
   */
  DataArrayPath getCellPhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  /**
   * @brief Setter property for BoundaryCellsArrayPath
   */
  void setBoundaryCellsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for BoundaryCellsArrayPath
   * @return Value of BoundaryCellsArrayPath
   */
  DataArrayPath getBoundaryCellsArrayPath() const;

  Q_PROPERTY(DataArrayPath BoundaryCellsArrayPath READ getBoundaryCellsArrayPath WRITE setBoundaryCellsArrayPath)

  /**
   * @brief Setter property for FeaturePhasesArrayPath
   */
  void setFeaturePhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeaturePhasesArrayPath
   * @return Value of FeaturePhasesArrayPath
   */
  DataArrayPath getFeaturePhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  /**
   * @brief Setter property for NumCellsArrayName
   */
  void setNumCellsArrayName(const QString& value);
  /**
   * @brief Getter property for NumCellsArrayName
   * @return Value of NumCellsArrayName
   */
  QString getNumCellsArrayName() const;

  Q_PROPERTY(QString NumCellsArrayName READ getNumCellsArrayName WRITE setNumCellsArrayName)

  /**
   * @brief Setter property for EquivalentDiametersArrayName
   */
  void setEquivalentDiametersArrayName(const QString& value);
  /**
   * @brief Getter property for EquivalentDiametersArrayName
   * @return Value of EquivalentDiametersArrayName
   */
  QString getEquivalentDiametersArrayName() const;

  Q_PROPERTY(QString EquivalentDiametersArrayName READ getEquivalentDiametersArrayName WRITE setEquivalentDiametersArrayName)

  /**
   * @brief Setter property for VolumesArrayName
   */
  void setVolumesArrayName(const QString& value);
  /**
   * @brief Getter property for VolumesArrayName
   * @return Value of VolumesArrayName
   */
  QString getVolumesArrayName() const;

  Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)

  /**
   * @brief Setter property for Omega3sArrayName
   */
  void setOmega3sArrayName(const QString& value);
  /**
   * @brief Getter property for Omega3sArrayName
   * @return Value of Omega3sArrayName
   */
  QString getOmega3sArrayName() const;

  Q_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)

  /**
   * @brief Setter property for CentroidsArrayName
   */
  void setCentroidsArrayName(const QString& value);
  /**
   * @brief Getter property for CentroidsArrayName
   * @return Value of CentroidsArrayName
   */
  QString getCentroidsArrayName() const;

  Q_PROPERTY(QString CentroidsArrayName READ getCentroidsArrayName WRITE setCentroidsArrayName)

  /**
   * @brief Setter property for AxisEulerAnglesArrayName
   */
  void setAxisEulerAnglesArrayName(const QString& value);
  /**
   * @brief Getter property for AxisEulerAnglesArrayName
   * @return Value of AxisEulerAnglesArrayName
   */
  QString getAxisEulerAnglesArrayName() const;

  Q_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)

  /**
   * @brief Setter property for AxisLengthsArrayName
   */
  void setAxisLengthsArrayName(const QString& value);
  /**
   * @brief Getter property for AxisLengthsArrayName
   * @return Value of AxisLengthsArrayName
   */
  QString getAxisLengthsArrayName() const;

  Q_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)

  /**
   * @brief Setter property for NumFeaturesArrayPath
   */
  void setNumFeaturesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NumFeaturesArrayPath
   * @return Value of NumFeaturesArrayPath
   */
  DataArrayPath getNumFeaturesArrayPath() const;

  Q_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)

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
  void generate_precipitate(int32_t phase, Precip_t* precip, ShapeType::Type shapeclass, const LaueOps* OrthoOps);

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
  void compare_2Ddistributions(std::vector<std::vector<float>>, std::vector<std::vector<float>>, float& sqrerror);

  /**
   * @brief compare_3Ddistributions Computes the 3D Bhattacharyya distance
   * @param sqrerror Float 1D Bhattacharyya distance
   */
  void compare_3Ddistributions(std::vector<std::vector<std::vector<float>>>, std::vector<std::vector<std::vector<float>>>, float& sqrerror);

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
  std::weak_ptr<DataArray<int8_t>> m_BoundaryCellsPtr;
  int8_t* m_BoundaryCells = nullptr;
  std::weak_ptr<DataArray<float>> m_AxisEulerAnglesPtr;
  float* m_AxisEulerAngles = nullptr;
  std::weak_ptr<DataArray<float>> m_CentroidsPtr;
  float* m_Centroids = nullptr;
  std::weak_ptr<DataArray<float>> m_AxisLengthsPtr;
  float* m_AxisLengths = nullptr;
  std::weak_ptr<DataArray<float>> m_VolumesPtr;
  float* m_Volumes = nullptr;
  std::weak_ptr<DataArray<float>> m_Omega3sPtr;
  float* m_Omega3s = nullptr;
  std::weak_ptr<DataArray<float>> m_EquivalentDiametersPtr;
  float* m_EquivalentDiameters = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NumCellsPtr;
  int32_t* m_NumCells = nullptr;
  std::weak_ptr<DataArray<PhaseType::EnumType>> m_PhaseTypesPtr;
  PhaseType::EnumType* m_PhaseTypes = nullptr;
  std::weak_ptr<DataArray<ShapeType::EnumType>> m_ShapeTypesPtr;
  ShapeType::EnumType* m_ShapeTypes = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NumFeaturesPtr;
  int32_t* m_NumFeatures = nullptr;

  QString m_ClusteringListArrayName = {};
  QString m_ErrorOutputFile = {};
  QString m_CsvOutputFile = {};
  DataArrayPath m_MaskArrayPath = {};
  bool m_UseMask = {};
  int m_FeatureGeneration = {};
  QString m_PrecipInputFile = {};
  bool m_PeriodicBoundaries = {};
  bool m_MatchRDF = {};
  bool m_WriteGoalAttributes = {};
  DataArrayPath m_InputStatsArrayPath = {};
  DataArrayPath m_InputPhaseTypesArrayPath = {};
  DataArrayPath m_InputShapeTypesArrayPath = {};
  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_CellPhasesArrayPath = {};
  DataArrayPath m_BoundaryCellsArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  QString m_NumCellsArrayName = {};
  QString m_EquivalentDiametersArrayName = {};
  QString m_VolumesArrayName = {};
  QString m_Omega3sArrayName = {};
  QString m_CentroidsArrayName = {};
  QString m_AxisEulerAnglesArrayName = {};
  QString m_AxisLengthsArrayName = {};
  DataArrayPath m_NumFeaturesArrayPath = {};
  int m_SaveGeometricDescriptions = {};
  DataArrayPath m_NewAttributeMatrixPath = {};
  DataArrayPath m_SelectedAttributeMatrixPath = {};

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

  // Feature Data - make sure these are all initialized to nullptr in the constructor

  NeighborList<float>::WeakPointer m_ClusteringList;

  // Ensemble Data - make sure these are all initialized to nullptr in the constructor

  // All other private variables
  QVector<ShapeOps::Pointer> m_ShapeOps;
  ShapeOps::Pointer m_UnknownShapeOps;
  ShapeOps::Pointer m_CubicOctohedronOps;
  ShapeOps::Pointer m_CylinderOps;
  ShapeOps::Pointer m_EllipsoidOps;
  ShapeOps::Pointer m_SuperEllipsoidOps;

  int64_t* m_Neighbors;
  StatsDataArray::WeakPointer m_StatsDataArray;

  std::vector<std::vector<int64_t>> m_ColumnList;
  std::vector<std::vector<int64_t>> m_RowList;
  std::vector<std::vector<int64_t>> m_PlaneList;

  std::vector<size_t> m_PointsToAdd;
  std::vector<size_t> m_PointsToRemove;

  uint64_t m_Seed;

  std::vector<std::vector<float>> m_FeatureSizeDist;
  std::vector<std::vector<float>> m_SimFeatureSizeDist;
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
  void saveToNewAttributeMatrix(QList<IDataArrayShPtrType> incomingArrays);

  /**
   * @brief appendToExistingAttributeMatrix
   */
  void appendToExistingAttributeMatrix(QList<IDataArrayShPtrType> incomingArrays);

public:
  InsertPrecipitatePhases(const InsertPrecipitatePhases&) = delete; // Copy Constructor Not Implemented
  InsertPrecipitatePhases(InsertPrecipitatePhases&&) = delete;      // Move Constructor Not Implemented
  InsertPrecipitatePhases& operator=(const InsertPrecipitatePhases&) = delete; // Copy Assignment Not Implemented
  InsertPrecipitatePhases& operator=(InsertPrecipitatePhases&&) = delete;      // Move Assignment Not Implemented
};

