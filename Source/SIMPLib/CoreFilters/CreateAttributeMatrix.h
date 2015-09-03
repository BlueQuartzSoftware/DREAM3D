/*
 * Your License or Copyright can go here
 */

#ifndef _CreateAttributeMatrix_H_
#define _CreateAttributeMatrix_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"



/**
 * @brief The CreateAttributeMatrix class. See [Filter documentation](@ref createattributematrix) for details.
 */
class CreateAttributeMatrix : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(CreateAttributeMatrix)
    SIMPL_STATIC_NEW_MACRO(CreateAttributeMatrix)
    SIMPL_TYPE_MACRO_SUPER(CreateAttributeMatrix, AbstractFilter)

    virtual ~CreateAttributeMatrix();

    SIMPL_FILTER_PARAMETER(DataArrayPath, CreatedAttributeMatrix)
    Q_PROPERTY(DataArrayPath CreatedAttributeMatrix READ getCreatedAttributeMatrix WRITE setCreatedAttributeMatrix)

    SIMPL_FILTER_PARAMETER(int, AttributeMatrixType)
    Q_PROPERTY(int AttributeMatrixType READ getAttributeMatrixType WRITE setAttributeMatrixType)

    SIMPL_FILTER_PARAMETER(DynamicTableData, TupleDimensions)
    Q_PROPERTY(DynamicTableData TupleDimensions READ getTupleDimensions WRITE setTupleDimensions)

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
    CreateAttributeMatrix();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    CreateAttributeMatrix(const CreateAttributeMatrix&); // Copy Constructor Not Implemented
    void operator=(const CreateAttributeMatrix&); // Operator '=' Not Implemented
};

#endif /* _CreateAttributeMatrix_H_ */
