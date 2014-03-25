#ifndef _BridgeParentIdsStatisticsToGrainIds_H_
#define _BridgeParentIdsStatisticsToGrainIds_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

/**
 * @class BridgeParentIdsStatisticsToGrainIds BridgeParentIdsStatisticsToGrainIds.h DREAM3DLib/GenericFilters/BridgeParentIdsStatisticsToGrainIds.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT BridgeParentIdsStatisticsToGrainIds : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(BridgeParentIdsStatisticsToGrainIds)
    DREAM3D_STATIC_NEW_MACRO(BridgeParentIdsStatisticsToGrainIds)
    DREAM3D_TYPE_MACRO_SUPER(BridgeParentIdsStatisticsToGrainIds, AbstractFilter)


    virtual ~BridgeParentIdsStatisticsToGrainIds();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(OldGrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellParentIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ParentDensityArrayName)
    //------ Created Cell Data
    //------ Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldParentIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AvgCAxisMisalignmentsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(NeighborListArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CAxisMisalignmentListArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(NumGrainsPerParentArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(LocalCAxisMisalignmentsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(UnbiasedLocalCAxisMisalignmentsArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::GenericFilters; }
    virtual const std::string getSubGroupName() {return DREAM3D::FilterSubGroups::MemoryManagementFilters;}
    virtual const std::string getHumanLabel() { return "Bridge Parent Statistics To Grains"; }

    DREAM3D_INSTANCE_PROPERTY(bool, CalcLocalCAxis)
    DREAM3D_INSTANCE_PROPERTY(bool, CalcUnbiasedLocalCAxis)

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
    BridgeParentIdsStatisticsToGrainIds();

  private:
    int32_t* m_GrainIds;
  int32_t* m_OldGrainIds;
    int32_t* m_CellParentIds;
    int32_t* m_ParentDensity;
    int32_t* m_FieldParentIds;
    float* m_AvgCAxisMisalignments;
    int32_t* m_NumGrainsPerParent;
    float* m_LocalCAxisMisalignments;
    float* m_UnbiasedLocalCAxisMisalignments;
    NeighborList<int>* m_NeighborList;
    NeighborList<float>* m_CAxisMisalignmentList;

    unsigned int* m_CrystalStructures;
    std::vector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles, bool afterLink);


    BridgeParentIdsStatisticsToGrainIds(const BridgeParentIdsStatisticsToGrainIds&); // Copy Constructor Not Implemented
    void operator=(const BridgeParentIdsStatisticsToGrainIds&); // Operator '=' Not Implemented
};

#endif /* _BridgeParentIdsStatisticsToGrainIds_H_ */
