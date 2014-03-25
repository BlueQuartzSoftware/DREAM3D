/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef CROPSURFACEMESH_H_
#define CROPSURFACEMESH_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"

/**
 * @class CropSurfaceMesh CropSurfaceMesh.h DREAM3DLib/Plugins/UCSB/UCSBFilters/CropSurfaceMesh.h
 * @brief
 * @author Will Lenthe
 * @date Mar 24, 2014
 * @version 1.0
 */
class DREAM3DLib_EXPORT CropSurfaceMesh : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(CropSurfaceMesh)
    DREAM3D_STATIC_NEW_MACRO(CropSurfaceMesh)
    DREAM3D_TYPE_MACRO_SUPER(CropSurfaceMesh, AbstractFilter)

    virtual ~CropSurfaceMesh();

    DREAM3D_INSTANCE_PROPERTY(int, XMin)
    DREAM3D_INSTANCE_PROPERTY(int, YMin)
    DREAM3D_INSTANCE_PROPERTY(int, ZMin)

    DREAM3D_INSTANCE_PROPERTY(int, XMax)
    DREAM3D_INSTANCE_PROPERTY(int, YMax)
    DREAM3D_INSTANCE_PROPERTY(int, ZMax)
    DREAM3D_INSTANCE_PROPERTY(bool, UpdateOrigin)


    virtual const std::string getGroupName() { return "UCSB"; }
  virtual const std::string getSubGroupName()  { return DREAM3D::FilterSubGroups::CropCutFilters; }
    virtual const std::string getHumanLabel() { return "Crop Surface Mesh"; }

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
    virtual void execute();
    virtual void preflight();

  protected:
    CropSurfaceMesh();


  private:

    void dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles);
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    CropSurfaceMesh(const CropSurfaceMesh&); // Copy Constructor Not Implemented
    void operator=(const CropSurfaceMesh&); // Operator '=' Not Implemented
};

#endif /* CROPSURFACEMESH_H_ */
