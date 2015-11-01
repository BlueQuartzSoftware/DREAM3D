/*
 * Your License or Copyright can go here
 */

#ifndef _FindDistsToCharactGBs_H_
#define _FindDistsToCharactGBs_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"


/**
 * @brief The FindDistsToCharactGBs class. See [Filter documentation](@ref finddiststocharactgbs) for details.
 */
class FindDistsToCharactGBs : public SurfaceMeshFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(FindDistsToCharactGBs)
    SIMPL_STATIC_NEW_MACRO(FindDistsToCharactGBs)
    SIMPL_TYPE_MACRO_SUPER(FindDistsToCharactGBs, SurfaceMeshFilter)

    virtual ~FindDistsToCharactGBs();

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


      SIMPL_FILTER_PARAMETER(DataArrayPath, DistToTiltArrayPath)
      Q_PROPERTY(DataArrayPath DistToTiltArrayPath READ getDistToTiltArrayPath WRITE setDistToTiltArrayPath)

      SIMPL_FILTER_PARAMETER(DataArrayPath, DistToTwistArrayPath)
      Q_PROPERTY(DataArrayPath DistToTwistArrayPath READ getDistToTwistArrayPath WRITE setDistToTwistArrayPath)

      SIMPL_FILTER_PARAMETER(DataArrayPath, DistToSymmetricArrayPath)
      Q_PROPERTY(DataArrayPath DistToSymmetricArrayPath READ getDistToSymmetricArrayPath WRITE setDistToSymmetricArrayPath)

      SIMPL_FILTER_PARAMETER(DataArrayPath, DistTo180TiltArrayPath)
      Q_PROPERTY(DataArrayPath DistTo180TiltArrayPath READ getDistTo180TiltArrayPath WRITE setDistTo180TiltArrayPath)

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

  //signals:
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
   // void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
   // void preflightExecuted();

  protected:
    FindDistsToCharactGBs();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)

    DEFINE_DATAARRAY_VARIABLE(double, DistToTilt)
    DEFINE_DATAARRAY_VARIABLE(double, DistToTwist)
    DEFINE_DATAARRAY_VARIABLE(double, DistToSymmetric)
    DEFINE_DATAARRAY_VARIABLE(double, DistTo180Tilt)

    FindDistsToCharactGBs(const FindDistsToCharactGBs&); // Copy Constructor Not Implemented
    void operator=(const FindDistsToCharactGBs&); // Operator '=' Not Implemented
};

#endif /* _FindDistsToCharactGBs_H_ */
