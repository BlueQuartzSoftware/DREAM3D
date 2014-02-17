/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef SubFilterPipeline_H_
#define SubFilterPipeline_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/FilterPipeline.h"

class SubFilterPipeline : public FilterPipeline
{
  public:

    DREAM3D_SHARED_POINTERS(SubFilterPipeline)
    DREAM3D_STATIC_NEW_MACRO(SubFilterPipeline)
    DREAM3D_TYPE_MACRO_SUPER(SubFilterPipeline, FilterPipeline)

    DREAM3D_INSTANCE_PROPERTY(VoxelDataContainer::Pointer, VoxelDataContainer)
    DREAM3D_INSTANCE_PROPERTY(SurfaceMeshDataContainer::Pointer, SurfaceMeshDataContainer)
    DREAM3D_INSTANCE_PROPERTY(SolidMeshDataContainer::Pointer, SolidMeshDataContainer)

    //SubFilterPipeline();
    virtual ~SubFilterPipeline();

    /**
     * @brief runs with modified execute
     */
    virtual void run();

    /**
     * @brief set data container that executed pipeline will use
     */
    //virtual void setVoxelDataContainer(VoxelDataContainer::Pointer data);

    /**
     * @brief modified execute method using provided data container
     */
    virtual void execute();

  protected:
    SubFilterPipeline();

  private:
    SubFilterPipeline(const SubFilterPipeline&); // Copy Constructor Not Implemented
    void operator=(const SubFilterPipeline&); // Operator '=' Not Implemented
};

#endif /* SubFilterPipeline_H_ */
