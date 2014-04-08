/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef AlignSectionsPhaseCorrelation_H_
#define AlignSectionsPhaseCorrelation_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"

#include "DREAM3DLib/ReconstructionFilters/AlignSections.h"

#include "Plugins/ImageProcessing/ImageProcessingConstants.h"


/**
 * @class AlignSectionsPhaseCorrelation AlignSectionsPhaseCorrelation.h ImageProcessing/Code/ImageProcessingFilters/AlignSectionsPhaseCorrelation.h
 * @brief
 * @author Will Lenthe
 * @date Apr 8, 2014
 * @version 1.0
 */
class DREAM3DLib_EXPORT AlignSectionsPhaseCorrelation : public AlignSections
{
  public:
    DREAM3D_SHARED_POINTERS(AlignSectionsPhaseCorrelation)
    DREAM3D_STATIC_NEW_MACRO(AlignSectionsPhaseCorrelation)
    DREAM3D_TYPE_MACRO_SUPER(AlignSectionsPhaseCorrelation, AlignSections)

    virtual ~AlignSectionsPhaseCorrelation();

     //Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(RawImageDataArrayName)


    //filter parameters
    DREAM3D_INSTANCE_STRING_PROPERTY(SelectedCellArrayName)

    virtual const std::string getGroupName() { return "ImageProcessing"; }
    virtual const std::string getSubGroupName() {return DREAM3D::FilterSubGroups::AlignmentFilters;}
    virtual const std::string getHumanLabel() { return "Align Sections (Scalar)"; }

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

    virtual void find_shifts(std::vector<int> &xshifts, std::vector<int> &yshifts);

  protected:
    AlignSectionsPhaseCorrelation();

  private:
    ImageProcessing::DefaultPixelType* m_RawImageData;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    AlignSectionsPhaseCorrelation(const AlignSectionsPhaseCorrelation&); // Copy Constructor Not Implemented
    void operator=(const AlignSectionsPhaseCorrelation&); // Operator '=' Not Implemented
};

#endif /* AlignSectionsPhaseCorrelation_H_ */
