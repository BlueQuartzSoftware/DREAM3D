/*
 * Your License or Copyright can go here
 */

#ifndef _computefeaturerect_h_
#define _computefeaturerect_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The ComputeFeatureRect class. See [Filter documentation](@ref computefeaturerect) for details.
 */
class ComputeFeatureRect : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(ComputeFeatureRect)
  SIMPL_STATIC_NEW_MACRO(ComputeFeatureRect)
  SIMPL_TYPE_MACRO_SUPER(ComputeFeatureRect, AbstractFilter)

  virtual ~ComputeFeatureRect();

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureRectArrayPath)
  Q_PROPERTY(DataArrayPath FeatureRectArrayPath READ getFeatureRectArrayPath WRITE setFeatureRectArrayPath)

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
  ComputeFeatureRect();

  /**
  * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
  */
  void dataCheck();

  /**
  * @brief Initializes all the private instance variables.
  */
  void initialize();

  /**
   * @brief sub2ind Helper Method.  Computes index from matrix coordinates
   * @param tDims
   * @param x
   * @param y
   * @param z
   * @return
   */
  size_t sub2ind(QVector<size_t> tDims, size_t x, size_t y, size_t z) const;

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, FeatureRect)

  ComputeFeatureRect(const ComputeFeatureRect&); // Copy Constructor Not Implemented
  void operator=(const ComputeFeatureRect&);     // Operator '=' Not Implemented
};

#endif /* _ComputeFeatureRect_H_ */
