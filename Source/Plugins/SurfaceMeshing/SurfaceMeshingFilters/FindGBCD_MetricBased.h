/*
 * Your License or Copyright can go here
 */

#ifndef _FindGBCD_MetricBased_H_
#define _FindGBCD_MetricBased_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

const int NUM_RESOL_CHOICES = 4;
const int DEFAULT_RESOL_CHOICE = 2;
const float RESOL_CHOICES[NUM_RESOL_CHOICES][2] = { { 3.0f, 7.0f }, { 5.0f, 5.0f }, { 5.0f, 7.0f }, { 5.0f, 8.0f } }; // { for misorient., for planes } 
const double BALL_VOLS_M3M[NUM_RESOL_CHOICES] = { 0.0000641361, 0.000139158, 0.000287439, 0.00038019 };

/**
 * @brief The FindGBCD_MetricBased class. See [Filter documentation](@ref findgbcd_metricbased) for details.
 */
class FindGBCD_MetricBased : public SurfaceMeshFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

public:
  SIMPL_SHARED_POINTERS(FindGBCD_MetricBased)
  SIMPL_STATIC_NEW_MACRO(FindGBCD_MetricBased)
  SIMPL_TYPE_MACRO_SUPER(FindGBCD_MetricBased, AbstractFilter)

  virtual ~FindGBCD_MetricBased();

  SIMPL_FILTER_PARAMETER(int, PhaseOfInterest)
  Q_PROPERTY(int PhaseOfInterest READ getPhaseOfInterest WRITE setPhaseOfInterest)
    
  SIMPL_FILTER_PARAMETER(AxisAngleInput_t, MisorientationRotation)
  Q_PROPERTY(AxisAngleInput_t MisorientationRotation READ getMisorientationRotation WRITE setMisorientationRotation)

  SIMPL_FILTER_PARAMETER(int, ChosenLimitDists)
  Q_PROPERTY(int ChosenLimitDists READ getChosenLimitDists WRITE setChosenLimitDists)  

  SIMPL_FILTER_PARAMETER(int, NumSamplPts)
  Q_PROPERTY(int NumSamplPts READ getNumSamplPts WRITE setNumSamplPts)

  SIMPL_FILTER_PARAMETER(bool, AddMorePtsNearEquator)
  Q_PROPERTY(bool AddMorePtsNearEquator READ getAddMorePtsNearEquator WRITE setAddMorePtsNearEquator)

  SIMPL_FILTER_PARAMETER(QString, DistOutputFile)
  Q_PROPERTY(QString DistOutputFile READ getDistOutputFile WRITE setDistOutputFile)

  SIMPL_FILTER_PARAMETER(QString, ErrOutputFile)
  Q_PROPERTY(QString ErrOutputFile READ getErrOutputFile WRITE setErrOutputFile)

  SIMPL_FILTER_PARAMETER(bool, SaveRelativeErr)
  Q_PROPERTY(bool SaveRelativeErr READ getSaveRelativeErr WRITE setSaveRelativeErr)



  SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
  Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceAreasArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFaceAreasArrayPath READ getSurfaceMeshFaceAreasArrayPath WRITE setSurfaceMeshFaceAreasArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFeatureFaceLabelsArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFeatureFaceLabelsArrayPath READ getSurfaceMeshFeatureFaceLabelsArrayPath WRITE setSurfaceMeshFeatureFaceLabelsArrayPath)

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


protected:
  FindGBCD_MetricBased();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

private:
  DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
  DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
  DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceAreas)
  DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
  DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)
  DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFeatureFaceLabels)

  bool doublesEqual(double x, double y);

  FindGBCD_MetricBased(const FindGBCD_MetricBased&); // Copy Constructor Not Implemented
  void operator=(const FindGBCD_MetricBased&); // Operator '=' Not Implemented
};

#endif /* _FindGBCD_MetricBased_H_ */
