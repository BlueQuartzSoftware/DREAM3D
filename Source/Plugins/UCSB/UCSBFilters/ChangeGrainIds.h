/* ============================================================================
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _ChangeGrainIds_H_
#define _ChangeGrainIds_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

/**
* @class ChangeGrainIds ChangeGrainIds.h DREAM3DLib/Plugins/UCSB/UCSBFilters/ChangeGrainIds.h
* @brief This filter change grainid's for bad voxels to 0.
* @author William Lenthe UCSB
* @date March 11, 2014
* @version 1.0
*/
class DREAM3DLib_EXPORT ChangeGrainIds : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(ChangeGrainIds)
    DREAM3D_STATIC_NEW_MACRO(ChangeGrainIds)
    DREAM3D_TYPE_MACRO_SUPER(ChangeGrainIds, AbstractFilter)
    virtual ~ChangeGrainIds();

    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(GoodVoxelsArrayName)
    DREAM3D_INSTANCE_PROPERTY(int, NewID)

    virtual const std::string getGroupName() { return "UCSB"; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::MiscFilters; }
    virtual const std::string getHumanLabel() { return "Change Bad Grain IDs"; }

    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();
    virtual void execute();

  protected:
    ChangeGrainIds();

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    int32_t* m_GrainIds;
    bool* m_GoodVoxels;

    ChangeGrainIds(const ChangeGrainIds&); // Copy Constructor Not Implemented
    void operator=(const ChangeGrainIds&); // Operator '=' Not Implemented
};

#endif /* RotateEulerRefFrame_H_ */
