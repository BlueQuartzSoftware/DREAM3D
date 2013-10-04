#ifndef PERFORMMTRBRIDGE_H_
#define PERFORMMTRBRIDGE_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

/**
 * @class PerformMTRBridge PerformMTRBridge.h DREAM3DLib/GenericFilters/PerformMTRBridge.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT PerformMTRBridge : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(PerformMTRBridge)
    DREAM3D_STATIC_NEW_MACRO(PerformMTRBridge)
    DREAM3D_TYPE_MACRO_SUPER(PerformMTRBridge, AbstractFilter)


    virtual ~PerformMTRBridge();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellParentIdsArrayName)
    //------ Created Cell Data
    //------ Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldParentIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MTRgKAMArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(avgMTRgKAMArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NumGrainsPerParentArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::GenericFilters; }
    virtual const std::string getSubGroupName() {return DREAM3D::FilterSubGroups::MemoryManagementFilters;}
    virtual const std::string getHumanLabel() { return "Bridge ParentIds Statistics To GrainIds"; }

    virtual void setupFilterParameters();
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  protected:
    PerformMTRBridge();

  private:
    int32_t* m_GrainIds;
    int32_t* m_CellParentIds;
    int32_t* m_FieldParentIds;
    float* m_MTRgKAM;
    float* m_avgMTRgKAM;
    int32_t* m_NumGrainsPerParent;

    unsigned int* m_CrystalStructures;
    std::vector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles, bool afterLink);


    PerformMTRBridge(const PerformMTRBridge&); // Copy Constructor Not Implemented
    void operator=(const PerformMTRBridge&); // Operator '=' Not Implemented
};

#endif /* PERFORMMTRBRIDGE_H_ */
