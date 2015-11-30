/*
 * Your License or Copyright can go here
 */

#ifndef _ImportASCIIData_H_
#define _ImportASCIIData_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "Widgets/ImportASCIIDataWizard/ASCIIWizardData.hpp"

/**
 * @brief The ImportASCIIData class. See [Filter documentation](@ref importasciidata) for details.
 */
class ImportASCIIData : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(ImportASCIIData)
    SIMPL_STATIC_NEW_MACRO(ImportASCIIData)
    SIMPL_TYPE_MACRO_SUPER(ImportASCIIData, AbstractFilter)

    virtual ~ImportASCIIData();

    SIMPL_FILTER_PARAMETER(ASCIIWizardData, WizardData)
      Q_PROPERTY(ASCIIWizardData WizardData READ getWizardData WRITE setWizardData)

      SIMPL_FILTER_PARAMETER(DataArrayPath, AttributeMatrixPath)
      Q_PROPERTY(DataArrayPath AttributeMatrixPath READ getAttributeMatrixPath WRITE setAttributeMatrixPath)

    enum ErrorCodes
    {
      EMPTY_FILE = -100,
      EMPTY_ATTR_MATRIX = -101,
      INCONSISTENT_TUPLES = -102,
      INCONSISTENT_COLS = -103,
      CONVERSION_FAILURE = -104,
      DUPLICATE_NAMES = -105
    };

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
    ImportASCIIData();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    QMap<int, IDataArray::Pointer>                                      m_ASCIIArrayMap;

    ImportASCIIData(const ImportASCIIData&); // Copy Constructor Not Implemented
    void operator=(const ImportASCIIData&); // Operator '=' Not Implemented
};

#endif /* _ImportASCIIData_H_ */
