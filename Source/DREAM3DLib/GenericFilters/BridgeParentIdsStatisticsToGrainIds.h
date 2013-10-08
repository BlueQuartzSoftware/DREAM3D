#ifndef BridgeParentIdsStatisticsToGrainIds_H_
#define BridgeParentIdsStatisticsToGrainIds_H_

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

    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellParentIdsArrayName)
    //------ Created Cell Data
    //------ Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldParentIdsArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(NumGrainsPerParentArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::GenericFilters; }
    virtual const QString getSubGroupName() {return DREAM3D::FilterSubGroups::MemoryManagementFilters;}
    virtual const QString getHumanLabel() { return "Bridge ParentIds Statistics To GrainIds"; }

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
    int32_t* m_CellParentIds;
    int32_t* m_FieldParentIds;
    int32_t* m_NumGrainsPerParent;

    unsigned int* m_CrystalStructures;
    QVector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles, bool afterLink);


    BridgeParentIdsStatisticsToGrainIds(const BridgeParentIdsStatisticsToGrainIds&); // Copy Constructor Not Implemented
    void operator=(const BridgeParentIdsStatisticsToGrainIds&); // Operator '=' Not Implemented
};

#endif /* BridgeParentIdsStatisticsToGrainIds_H_ */
