/*
 * Your License or Copyright can go here
 */

#ifndef _FindDifferenceMap_H_
#define _FindDifferenceMap_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractFilter.h"

/**
 * @brief The FindDifferenceMap class. See [Filter documentation](@ref finddifferencemap) for details.
 */
class FindDifferenceMap : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(FindDifferenceMap)
    SIMPL_STATIC_NEW_MACRO(FindDifferenceMap)
    SIMPL_TYPE_MACRO_SUPER(FindDifferenceMap, AbstractFilter)

    virtual ~FindDifferenceMap();

    SIMPL_FILTER_PARAMETER(DataArrayPath, FirstInputArrayPath)
    Q_PROPERTY(DataArrayPath FirstInputArrayPath READ getFirstInputArrayPath WRITE setFirstInputArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SecondInputArrayPath)
    Q_PROPERTY(DataArrayPath SecondInputArrayPath READ getSecondInputArrayPath WRITE setSecondInputArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, DifferenceMapArrayPath)
    Q_PROPERTY(DataArrayPath DifferenceMapArrayPath READ getDifferenceMapArrayPath WRITE setDifferenceMapArrayPath)

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
    FindDifferenceMap();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    DEFINE_IDATAARRAY_VARIABLE(FirstInputArray)
    DEFINE_IDATAARRAY_VARIABLE(SecondInputArray)
    DEFINE_IDATAARRAY_VARIABLE(DifferenceMap)

    FindDifferenceMap(const FindDifferenceMap&); // Copy Constructor Not Implemented
    void operator=(const FindDifferenceMap&); // Operator '=' Not Implemented
};

#endif /* _FindDifferenceMap_H_ */
