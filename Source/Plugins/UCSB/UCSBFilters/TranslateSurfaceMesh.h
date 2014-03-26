/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef TRANSLATESURFACEMESH_H_
#define TRANSLATESURFACEMESH_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"

/**
 * @class TranslateSurfaceMesh TranslateSurfaceMesh.h DREAM3DLib/Plugins/UCSB/UCSBFilters/TranslateSurfaceMesh.h
 * @brief
 * @author Will Lenthe
 * @date Mar 26, 2014
 * @version 1.0
 */
class DREAM3DLib_EXPORT TranslateSurfaceMesh : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(TranslateSurfaceMesh)
    DREAM3D_STATIC_NEW_MACRO(TranslateSurfaceMesh)
    DREAM3D_TYPE_MACRO_SUPER(TranslateSurfaceMesh, AbstractFilter)

    virtual ~TranslateSurfaceMesh();

    DREAM3D_INSTANCE_PROPERTY(float, XShift)
    DREAM3D_INSTANCE_PROPERTY(float, YShift)
    DREAM3D_INSTANCE_PROPERTY(float, ZShift)

    virtual const std::string getGroupName() { return "UCSB"; }
    virtual const std::string getSubGroupName()  { return DREAM3D::FilterSubGroups::MiscFilters; }
    virtual const std::string getHumanLabel() { return "Translate Surface Mesh"; }

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
    TranslateSurfaceMesh();


  private:
    void dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    TranslateSurfaceMesh(const TranslateSurfaceMesh&); // Copy Constructor Not Implemented
    void operator=(const TranslateSurfaceMesh&); // Operator '=' Not Implemented
};

#endif /* TRANSLATESURFACEMESH_H_ */
