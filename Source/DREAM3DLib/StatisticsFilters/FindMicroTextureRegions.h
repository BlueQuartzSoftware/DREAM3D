
#ifndef FINDMICROTEXTUREREGIONS_H_
#define FINDMICROTEXTUREREGIONS_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"

/**
 * @class FindMicroTextureRegions FindMicroTextureRegions.h DREAM3DLib/GenericFilters/FindMicroTextureRegions.h
 * @brief
 * @author
 * @date Sep 3, 2013
 * @version 1.0
 */
class DREAM3DLib_EXPORT FindMicroTextureRegions : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindMicroTextureRegions)
    DREAM3D_STATIC_NEW_MACRO(FindMicroTextureRegions)
    DREAM3D_TYPE_MACRO_SUPER(FindMicroTextureRegions, AbstractFilter)

    virtual ~FindMicroTextureRegions();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(MicroTextureRegionNumCellsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MicroTextureRegionFractionOccupiedArrayName)
    //------ Required Field Data
    //------ Required Ensemble Data

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::MorphologicalFilters; }
    virtual const std::string getHumanLabel() { return "Find MicroTexture Regions"; }

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */

    virtual void setupFilterParameters();
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    virtual void execute();
    virtual void preflight();

  protected:
    FindMicroTextureRegions();
    void find_microtextureregions();


  private:
    int32_t* m_MicroTextureRegionNumCells;
    float* m_MicroTextureRegionFractionOccupied;
    int32_t* m_GrainIds;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    FindMicroTextureRegions(const FindMicroTextureRegions&); // Copy Constructor Not Implemented
    void operator=(const FindMicroTextureRegions&); // Operator '=' Not Implemented
};

#endif /* FINDMICROTEXTUREREGIONS_H_ */
