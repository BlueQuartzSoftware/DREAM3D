/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _ImageRegistration_H_
#define _ImageRegistration_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

/**
 * @class ImageRegistration ImageRegistration.h ImageProcessing/Code/ImageProcessingFilters/ImageRegistration.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class ImageRegistration : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(ImageRegistration);
    DREAM3D_STATIC_NEW_MACRO(ImageRegistration);
    DREAM3D_TYPE_MACRO_SUPER(ImageRegistration, AbstractFilter);

    virtual ~ImageRegistration();

    //Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(RawImageDataArrayName)


    //filter parameters
    DREAM3D_INSTANCE_STRING_PROPERTY(SelectedCellArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AlignmentShiftFileName)
    DREAM3D_INSTANCE_PROPERTY(float, OptimizerParameter1)
    DREAM3D_INSTANCE_PROPERTY(float, OptimizerParameter2)
    DREAM3D_INSTANCE_PROPERTY(int, Iterations)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteShifts)
    DREAM3D_INSTANCE_PROPERTY(bool, ApplyShifts)
    DREAM3D_INSTANCE_PROPERTY(int, Metric)
    DREAM3D_INSTANCE_PROPERTY(int, Optimizer)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const std::string getGroupName() { return "ImageProcessing"; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const std::string getHumanLabel() { return "Image Registration Alignment"; }

    /**
    * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
    * a subgroup. It should be readable and understandable by humans.
    */
    virtual const std::string getSubGroupName() { return "Misc"; }

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
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

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

  protected:
    ImageRegistration();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    uint8_t* m_RawImageData;uint8_t* m_ProcessedImageData;

    ImageRegistration(const ImageRegistration&); // Copy Constructor Not Implemented
    void operator=(const ImageRegistration&); // Operator '=' Not Implemented
};

#endif /* _ImageRegistration_H_ */
