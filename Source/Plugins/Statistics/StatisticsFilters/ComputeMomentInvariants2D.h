/*
 * Your License or Copyright can go here
 */

#ifndef _computemomentinvariants2d_h_
#define _computemomentinvariants2d_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The ComputeMomentInvariants2D class. See [Filter documentation](@ref computemomentinvariants2d) for details.
 */
class ComputeMomentInvariants2D : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(ComputeMomentInvariants2D)
  SIMPL_STATIC_NEW_MACRO(ComputeMomentInvariants2D)
  SIMPL_TYPE_MACRO_SUPER(ComputeMomentInvariants2D, AbstractFilter)

  virtual ~ComputeMomentInvariants2D();

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureRectArrayPath)
  Q_PROPERTY(DataArrayPath FeatureRectArrayPath READ getFeatureRectArrayPath WRITE setFeatureRectArrayPath)

  SIMPL_FILTER_PARAMETER(bool, NormalizeMomentInvariants)
  Q_PROPERTY(bool NormalizeMomentInvariants READ getNormalizeMomentInvariants WRITE setNormalizeMomentInvariants)

  SIMPL_FILTER_PARAMETER(DataArrayPath, Omega1ArrayPath)
  Q_PROPERTY(DataArrayPath Omega1ArrayPath READ getOmega1ArrayPath WRITE setOmega1ArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, Omega2ArrayPath)
  Q_PROPERTY(DataArrayPath Omega2ArrayPath READ getOmega2ArrayPath WRITE setOmega2ArrayPath)

  SIMPL_FILTER_PARAMETER(bool, SaveCentralMoments)
  Q_PROPERTY(bool SaveCentralMoments READ getSaveCentralMoments WRITE setSaveCentralMoments)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CentralMomentsArrayPath)
  Q_PROPERTY(DataArrayPath CentralMomentsArrayPath READ getCentralMomentsArrayPath WRITE setCentralMomentsArrayPath)

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
  ComputeMomentInvariants2D();

  /**
  * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
  */
  void dataCheck();

  /**
  * @brief Initializes all the private instance variables.
  */
  void initialize();

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, FeatureRect)
  DEFINE_DATAARRAY_VARIABLE(float, Omega1)
  DEFINE_DATAARRAY_VARIABLE(float, Omega2)
  DEFINE_DATAARRAY_VARIABLE(float, CentralMoments)

  ComputeMomentInvariants2D(const ComputeMomentInvariants2D&); // Copy Constructor Not Implemented
  void operator=(const ComputeMomentInvariants2D&);            // Operator '=' Not Implemented
};

#endif /* _ComputeMomentInvariants2D_H_ */
