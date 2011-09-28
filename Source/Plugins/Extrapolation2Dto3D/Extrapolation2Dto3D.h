

#ifndef Extrapolation2Dto3D_H_
#define Extrapolation2Dto3D_H_

#include <MXA/Common/MXASetGetMacros.h>
#include "MXA/MXA.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/AbstractPipeline.h"
#include "DREAM3D/Common/Observer.h"


#include "Extrapolation2Dto3DFunc.h"

/**
* @class Extrapolation2Dto3D Extrapolation2Dto3D.h AIM/Extrapolation2Dto3D/Extrapolation2Dto3D.h
* @brief This class serves as the main entry point to execute the Extrapolation2Dto3D
* pipeline
* @author
* @author
* @date
* @version 1.0
*/
class Extrapolation2Dto3D : public AbstractPipeline, public Observer
{
  public:
    MXA_SHARED_POINTERS(Extrapolation2Dto3D);
    MXA_TYPE_MACRO(Extrapolation2Dto3D);

    MXA_STATIC_NEW_MACRO(Extrapolation2Dto3D);

    virtual ~Extrapolation2Dto3D();

    /**
    * @brief Main method to run the operation
    */
    virtual void execute();

  protected:
    Extrapolation2Dto3D();

  private:
    Extrapolation2Dto3DFunc::Pointer m;

    Extrapolation2Dto3D(const Extrapolation2Dto3D&);    // Copy Constructor Not Implemented
    void operator=(const Extrapolation2Dto3D&);  // Operator '=' Not Implemented
};



#endif /* Extrapolation2Dto3D_H_ */
