/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef SEGMENTBOUNDARIES_H_
#define SEGMENTBOUNDARIES_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"

/**
 * @class SegmentBoundaries SegmentBoundaries.h DREAM3DLib/Plugins/UCSB/UCSBFilters/SegmentBoundaries.h
 * @brief
 * @author Will Lenthe
 * @date Mar 26, 2014
 * @version 1.0
 */
class DREAM3DLib_EXPORT SegmentBoundaries : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(SegmentBoundaries)
    DREAM3D_STATIC_NEW_MACRO(SegmentBoundaries)
    DREAM3D_TYPE_MACRO_SUPER(SegmentBoundaries, AbstractFilter)

    virtual ~SegmentBoundaries();

    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshFaceLabelsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(BoundaryIdsArrayName)

    virtual const std::string getGroupName() { return "UCSB"; }
    virtual const std::string getSubGroupName()  { return DREAM3D::FilterSubGroups::SegmentationFilters; }
    virtual const std::string getHumanLabel() { return "Segment Surface Mesh (Grain IDs)"; }

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
    SegmentBoundaries();
    void dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles);


  private:
    int32_t* m_SurfaceMeshFaceLabels;
    int32_t* m_BoundaryIds;

    SegmentBoundaries(const SegmentBoundaries&); // Copy Constructor Not Implemented
    void operator=(const SegmentBoundaries&); // Operator '=' Not Implemented
};

#endif /* SEGMENTBOUNDARIES_H_ */
