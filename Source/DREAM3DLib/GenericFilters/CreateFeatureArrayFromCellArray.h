#ifndef CREATEFIELDARRAYFROMCELLARRAY_H_
#define CREATEFIELDARRAYFROMCELLARRAY_H_

#include <string>
#include <set>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


/**
 * @class CreateFeatureArrayFromCellArray CreateFeatureArrayFromCellArray.h /FilterCategoryFilters/CreateFeatureArrayFromCellArray.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT CreateFeatureArrayFromCellArray : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(CreateFeatureArrayFromCellArray)
    DREAM3D_STATIC_NEW_MACRO(CreateFeatureArrayFromCellArray)
    DREAM3D_TYPE_MACRO_SUPER(CreateFeatureArrayFromCellArray, AbstractFilter)

    virtual ~CreateFeatureArrayFromCellArray();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)

    DREAM3D_INSTANCE_STRING_PROPERTY(SelectedCellArrayName)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::GenericFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::MemoryManagementFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Create Feature Array From Cell Array"; }

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
    CreateFeatureArrayFromCellArray();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)

    CreateFeatureArrayFromCellArray(const CreateFeatureArrayFromCellArray&); // Copy Constructor Not Implemented
    void operator=(const CreateFeatureArrayFromCellArray&); // Operator '=' Not Implemented
};


#endif /* CREATEFIELDARRAYFROMCELLARRAY_H_ */
