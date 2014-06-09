/* ============================================================================
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _ConvertOrientation_H_
#define _ConvertOrientation_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"

/**
* @class ConvertOrientation ConvertOrientation.h DREAM3DLib/Plugins/UCSB/UCSBFilters/ConvertOrientation.h
* @brief This filter converts one orientation type to another
* @author William Lenthe UCSB
* @date June 6, 2014
* @version 1.0
*/
class DREAM3DLib_EXPORT ConvertOrientation : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(ConvertOrientation)
    DREAM3D_STATIC_NEW_MACRO(ConvertOrientation)
    DREAM3D_TYPE_MACRO_SUPER(ConvertOrientation, AbstractFilter)
    virtual ~ConvertOrientation();

    DREAM3D_INSTANCE_PROPERTY(unsigned int, OldType)
    DREAM3D_INSTANCE_PROPERTY(unsigned int, NewType)
    DREAM3D_INSTANCE_PROPERTY(bool, FieldArray)

    DREAM3D_INSTANCE_STRING_PROPERTY(EulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(QuatsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(RodriguesVectorsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AvgQuatsArrayName)

    virtual const std::string getGroupName() { return "UCSB"; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::MiscFilters; }
    virtual const std::string getHumanLabel() { return "Convert Orientation"; }

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
    virtual void preflight();
    virtual void execute();

  protected:
    ConvertOrientation();

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    float* m_EulerAngles;
    float* m_Quats;
    float* m_RodriguesVectors;
    float* m_AxisAngles;
    float* m_AvgQuats;

    ConvertOrientation(const ConvertOrientation&); // Copy Constructor Not Implemented
    void operator=(const ConvertOrientation&); // Operator '=' Not Implemented
};

#endif /* RotateEulerRefFrame_H_ */
