

#ifndef PipelineBuilder_H_
#define PipelineBuilder_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/AbstractPipeline.h"
#include "DREAM3DLib/Common/Observer.h"




/**
* @class PipelineBuilder PipelineBuilder.h AIM/PipelineBuilder/PipelineBuilder.h
* @brief This class serves as the main entry point to execute the PipelineBuilder
* pipeline
* @author 
* @author 
* @date 
* @version 1.0
*/
class PipelineBuilder : public AbstractPipeline, public Observer
{
  public:
    DREAM3D_SHARED_POINTERS(PipelineBuilder);
    DREAM3D_TYPE_MACRO(PipelineBuilder);
    DREAM3D_STATIC_NEW_MACRO(PipelineBuilder);

    virtual ~PipelineBuilder();

    /**
    * @brief Main method to run the operation
    */
    virtual void execute();

  protected:
    PipelineBuilder();

  private:

    PipelineBuilder(const PipelineBuilder&);    // Copy Constructor Not Implemented
    void operator=(const PipelineBuilder&);  // Operator '=' Not Implemented
};



#endif /* PipelineBuilder_H_ */
