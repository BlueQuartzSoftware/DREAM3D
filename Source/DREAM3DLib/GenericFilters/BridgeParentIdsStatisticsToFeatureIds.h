#ifndef BridgeParentIdsStatisticsToFeatureIds_H_
#define BridgeParentIdsStatisticsToFeatureIds_H_

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
 * @class BridgeParentIdsStatisticsToFeatureIds BridgeParentIdsStatisticsToFeatureIds.h DREAM3DLib/GenericFilters/BridgeParentIdsStatisticsToFeatureIds.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT BridgeParentIdsStatisticsToFeatureIds : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(BridgeParentIdsStatisticsToFeatureIds)
    DREAM3D_STATIC_NEW_MACRO(BridgeParentIdsStatisticsToFeatureIds)
    DREAM3D_TYPE_MACRO_SUPER(BridgeParentIdsStatisticsToFeatureIds, AbstractFilter)


    virtual ~BridgeParentIdsStatisticsToFeatureIds();

    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)

    //------ Required Cell Data
            //------ Created Cell Data
    //------ Required Feature Data
        //------ Created Feature Data
    DREAM3D_INSTANCE_STRING_PROPERTY(NumFeaturesPerParentArrayName)
    //------ Required Ensemble Data
    
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::GenericFilters; }
    virtual const QString getSubGroupName() {return DREAM3D::FilterSubGroups::MemoryManagementFilters;}
    virtual const QString getHumanLabel() { return "Bridge ParentIds Statistics To FeatureIds"; }

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
    BridgeParentIdsStatisticsToFeatureIds();

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, CellParentIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureParentIds)
    int32_t* m_NumFeaturesPerParent;

    DEFINE_PTR_WEAKPTR_DATAARRAY(unsigned int, CrystalStructures)
    QVector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles, bool afterLink);


    BridgeParentIdsStatisticsToFeatureIds(const BridgeParentIdsStatisticsToFeatureIds&); // Copy Constructor Not Implemented
    void operator=(const BridgeParentIdsStatisticsToFeatureIds&); // Operator '=' Not Implemented
};

#endif /* BridgeParentIdsStatisticsToFeatureIds_H_ */
