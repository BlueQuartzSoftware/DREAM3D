/*
 * Your License or Copyright can go here
 */

#ifndef _FindGBPD_MetricBased_H_
#define _FindGBPD_MetricBased_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

/**
 * @brief The FindGBPD_MetricBased class. See [Filter documentation](@ref findgbpd_metricbased) for details.
 */
class FindGBPD_MetricBased : public SurfaceMeshFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(FindGBPD_MetricBased)
    SIMPL_STATIC_NEW_MACRO(FindGBPD_MetricBased)
    SIMPL_TYPE_MACRO_SUPER(FindGBPD_MetricBased, SurfaceMeshFilter)

    virtual ~FindGBPD_MetricBased();

    SIMPL_FILTER_PARAMETER(int, PhaseOfInterest)
    Q_PROPERTY(int PhaseOfInterest READ getPhaseOfInterest WRITE setPhaseOfInterest)

    SIMPL_FILTER_PARAMETER(float, LimitDist)
    Q_PROPERTY(float LimitDist READ getLimitDist WRITE setLimitDist)

    SIMPL_FILTER_PARAMETER(int, NumSamplPts)
    Q_PROPERTY(int NumSamplPts READ getNumSamplPts WRITE setNumSamplPts)

   /* SIMPL_FILTER_PARAMETER(bool, AddMorePtsNearEquator)
    Q_PROPERTY(bool AddMorePtsNearEquator READ getAddMorePtsNearEquator WRITE setAddMorePtsNearEquator)*/

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

 // signals:
    /**
     * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
     * be pushed from a user-facing control (such as a widget)
     * @param filter Filter instance pointer 
     */
  //  void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged Emitted when any Filter parameter is changed internally
     */
  //  void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before calling dataCheck()
     */
  //  void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
  //  void preflightExecuted();

  protected:
    FindGBPD_MetricBased();

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

    FindGBPD_MetricBased(const FindGBPD_MetricBased&); // Copy Constructor Not Implemented
    void operator=(const FindGBPD_MetricBased&); // Operator '=' Not Implemented
};

#endif /* _FindGBPD_MetricBased_H_ */
