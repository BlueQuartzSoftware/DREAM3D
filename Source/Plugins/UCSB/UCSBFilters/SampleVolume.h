/*
 * Your License or Copyright Information can go here
 */

#ifndef _SampleVolume_H_
#define _SampleVolume_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "SubFilterPipeline.h"

/**
 * @class SampleVolume SampleVolume.h UCSB/Code/UCSBFilters/SampleVolume.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class SampleVolume : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(SampleVolume)
    DREAM3D_STATIC_NEW_MACRO(SampleVolume)
    DREAM3D_TYPE_MACRO_SUPER(SampleVolume, AbstractFilter)

    virtual ~SampleVolume();

    //values for cropping
    typedef struct { int x; int y; int z; } BoxSize_t;
    DREAM3D_INSTANCE_PROPERTY(int, NumberVolumes)
    DREAM3D_INSTANCE_PROPERTY(IntVec3Widget_t, BoxSize)

    //values for output file
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputDirectory)
    DREAM3D_INSTANCE_STRING_PROPERTY(FileName)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteXdmf)
    DREAM3D_INSTANCE_PROPERTY(bool, CopySurfaceMesh)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const std::string getGroupName() { return "UCSB"; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const std::string getHumanLabel() { return "Sample Volume"; }

    /**
    * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
    * a subgroup. It should be readable and understandable by humans.
    */
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::CropCutFilters; }

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
    SampleVolume();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles);
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    SubFilterPipeline::Pointer buildPipeline(int append, int x, int y, int z);

    SampleVolume(const SampleVolume&); // Copy Constructor Not Implemented
    void operator=(const SampleVolume&); // Operator '=' Not Implemented
};

#endif /* _SampleStatistics_H_ */
