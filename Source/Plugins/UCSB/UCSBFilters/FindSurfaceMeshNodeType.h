/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef FindSurfaceMeshNodeType_H_
#define FindSurfaceMeshNodeType_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"

/**
 * @class FindSurfaceMeshNodeType FindSurfaceMeshNodeType.h DREAM3DLib/Plugins/UCSB/UCSBFilters/FindSurfaceMeshNodeType.h
 * @brief
 * @author Will Lenthe
 * @date Mar 26, 2014
 * @version 1.0
 */
class DREAM3DLib_EXPORT FindSurfaceMeshNodeType : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindSurfaceMeshNodeType)
    DREAM3D_STATIC_NEW_MACRO(FindSurfaceMeshNodeType)
    DREAM3D_TYPE_MACRO_SUPER(FindSurfaceMeshNodeType, AbstractFilter)

    virtual ~FindSurfaceMeshNodeType();

    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshFaceLabelsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshNodeTypeArrayName)

    virtual const std::string getGroupName() { return "UCSB"; }
    virtual const std::string getSubGroupName()  { return DREAM3D::FilterSubGroups::MiscFilters; }
    virtual const std::string getHumanLabel() { return "Find Surface Mesh Node Type"; }

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
    FindSurfaceMeshNodeType();


  private:
    void dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles);
    int32_t* m_SurfaceMeshFaceLabels;
    int8_t* m_SurfaceMeshNodeType;

    FindSurfaceMeshNodeType(const FindSurfaceMeshNodeType&); // Copy Constructor Not Implemented
    void operator=(const FindSurfaceMeshNodeType&); // Operator '=' Not Implemented
};

#endif
