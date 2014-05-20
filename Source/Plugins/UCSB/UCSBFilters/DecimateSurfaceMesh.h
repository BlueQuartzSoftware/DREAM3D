/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef DECIMATESURFACEMESH_H_
#define DECIMATESURFACEMESH_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"

/**
 * @class DecimateSurfaceMesh DecimateSurfaceMesh.h DREAM3DLib/Plugins/UCSB/UCSBFilters/DecimateSurfaceMesh.h
 * @brief
 * @author Will Lenthe
 * @date Mar 24, 2014
 * @version 1.0
 */
class DREAM3DLib_EXPORT DecimateSurfaceMesh : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(DecimateSurfaceMesh)
    DREAM3D_STATIC_NEW_MACRO(DecimateSurfaceMesh)
    DREAM3D_TYPE_MACRO_SUPER(DecimateSurfaceMesh, AbstractFilter)

    virtual ~DecimateSurfaceMesh();

    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshFaceNormalsArrayName)
    DREAM3D_INSTANCE_PROPERTY(float, TrianglesKeep)

    virtual const std::string getGroupName() { return "UCSB"; }
  virtual const std::string getSubGroupName()  { return DREAM3D::FilterSubGroups::CropCutFilters; }
    virtual const std::string getHumanLabel() { return "Decimate Surface Mesh"; }

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
    DecimateSurfaceMesh();


  private:
    double* m_SurfaceMeshFaceNormals;

    void dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    DecimateSurfaceMesh(const DecimateSurfaceMesh&); // Copy Constructor Not Implemented
    void operator=(const DecimateSurfaceMesh&); // Operator '=' Not Implemented
};

#endif /* DECIMATESURFACEMESH_H_ */
