#ifndef _AbstractFilter_H_
#define _AbstractFilter_H_

#include <set>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/Common/FilterOption.h"
#include "DREAM3DLib/Common/AbstractFilterOptionsWriter.h"

#include "ErrorMessage.h"



/**
 * @class AbstractFilter AbstractFilter.h DREAM3DLib/Common/AbstractFilter.h
 * @brief This class is the basic class to subclass when creating a new Filter for
 * DREAM.3D. The subclass must implement at least the  execute method. If an
 * error occurs during the execution of the filter set the errorCondition to
 * a non zero value and optionally use the setErrorMessage() method to explain what the
 * error was. This class also inherits from Observable so that the filter can
 * provide updates to the user interface during execution.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Nov 28, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT AbstractFilter : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(AbstractFilter)
    DREAM3D_STATIC_NEW_MACRO(AbstractFilter)
    DREAM3D_TYPE_MACRO_SUPER(AbstractFilter, Observable)

    virtual ~AbstractFilter();

    // These should be implemented by the subclass
    virtual const std::string getGroupName() { assert(false); return "";}
    virtual const std::string getHumanLabel() { assert(false); return "";}

    virtual const std::string getFilterVersion() { return DREAM3DLib::Version::Complete(); }

    DREAM3D_INSTANCE_PROPERTY(DataContainer*, DataContainer)

//  private:
//
//    std::string m_ErrorMessage;
//  public:
//
//    void setErrorMessage(const std::string &value)
//    {
//      this->m_ErrorMessage = value;
//    }
//    std::string getErrorMessage()
//    {
//      return m_ErrorMessage;
//    }

    DREAM3D_INSTANCE_PROPERTY(std::vector<ErrorMessage::Pointer>, ErrorMessages)

    void addErrorMessage(ErrorMessage::Pointer msg);
    void addErrorMessage(const std::string &filterName, const std::string &errorDescription, int errorCode);

    void addErrorMessages(std::vector<ErrorMessage::Pointer> msgVector);
    void removeErrorMessage(ErrorMessage::Pointer msg);
    void removeErrorMessage(int index);
    void removeErrorMessages(int start, int end);

    void clearErrorMessages();

    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition)


    DREAM3D_INSTANCE_PROPERTY(int, PipelineIndex)
    DREAM3D_INSTANCE_PROPERTY(AbstractFilter::Pointer, PreviousFilter)
    DREAM3D_INSTANCE_PROPERTY(AbstractFilter::Pointer, NextFilter)

    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, RequiredCellData)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, CreatedCellData)

    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, RequiredFieldData)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, CreatedFieldData)

    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, RequiredEnsembleData)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, CreatedEnsembleData)

    /**
     * @brief Cancel the operation
     */
    DREAM3D_VIRTUAL_INSTANCE_PROPERTY(bool, Cancel)


    DREAM3D_INSTANCE_PROPERTY(std::vector<FilterOption::Pointer>, FilterOptions)

    virtual void printValues(std::ostream &out){}


    virtual void setupFilterOptions();

    virtual void writeFilterOptions(AbstractFilterOptionsWriter* writer);


    /**
     * @brief This method should be fully implemented in subclasses.
     */
    virtual void execute();
    virtual void preflight();

    virtual bool doesPipelineContainFilterBeforeThis(const std::string &name);
    virtual bool doesPipelineContainFilterAfterThis(const std::string &name);

    virtual void addRequiredCellData(const std::string &name);
    virtual void addCreatedCellData(const std::string &name);

    virtual void addRequiredFieldData(const std::string &name);
    virtual void addCreatedFieldData(const std::string &name);

    virtual void addRequiredEnsembleData(const std::string &name);
    virtual void addCreatedEnsembleData(const std::string &name);

  protected:
    AbstractFilter();



  private:
    AbstractFilter(const AbstractFilter&); // Copy Constructor Not Implemented
    void operator=(const AbstractFilter&); // Operator '=' Not Implemented
};




#endif /* _AbstractFilter_H_  */
