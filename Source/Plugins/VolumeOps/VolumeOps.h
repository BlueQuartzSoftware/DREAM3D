

#ifndef VolumeOps_H_
#define VolumeOps_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/AbstractPipeline.h"
#include "DREAM3DLib/Common/Observer.h"
#include "VolumeOpsFunc.h"



/**
* @class VolumeOps VolumeOps.h AIM/VolumeOps/VolumeOps.h
* @brief This class serves as the main entry point to execute the VolumeOps
* pipeline
* @author 
* @author 
* @date 
* @version 1.0
*/
class VolumeOps : public AbstractPipeline, public Observer
{
  public:
    DREAM3D_SHARED_POINTERS(VolumeOps);
    DREAM3D_TYPE_MACRO(VolumeOps);
    DREAM3D_STATIC_NEW_MACRO(VolumeOps);

    virtual ~VolumeOps();

    MXA_INSTANCE_STRING_PROPERTY(H5InputFile)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputFilePrefix)

    MXA_INSTANCE_PROPERTY(int, xMin)
    MXA_INSTANCE_PROPERTY(int, yMin)
    MXA_INSTANCE_PROPERTY(int, zMin)
    MXA_INSTANCE_PROPERTY(int, xMax)
    MXA_INSTANCE_PROPERTY(int, yMax)
    MXA_INSTANCE_PROPERTY(int, zMax)
    MXA_INSTANCE_PROPERTY(float, XRes_New)
    MXA_INSTANCE_PROPERTY(float, YRes_New)
    MXA_INSTANCE_PROPERTY(float, ZRes_New)

	/**
    * @brief Main method to run the operation
    */
    virtual void execute();

  protected:
    VolumeOps();

  private:
    VolumeOpsFunc::Pointer m;

    VolumeOps(const VolumeOps&);    // Copy Constructor Not Implemented
    void operator=(const VolumeOps&);  // Operator '=' Not Implemented
};



#endif /* VolumeOps_H_ */
