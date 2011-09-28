

#ifndef FileConversion_H_
#define FileConversion_H_

#include <MXA/Common/MXASetGetMacros.h>
#include "MXA/MXA.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/AbstractPipeline.h"
#include "DREAM3D/Common/Observer.h"
#include "FileConversionFunc.h"



/**
* @class FileConversion FileConversion.h AIM/FileConversion/FileConversion.h
* @brief This class serves as the main entry point to execute the FileConversion
* pipeline
* @author
* @author
* @date
* @version 1.0
*/
class FileConversion : public AbstractPipeline, public Observer
{
  public:
    MXA_SHARED_POINTERS(FileConversion);
    MXA_TYPE_MACRO(FileConversion);

    MXA_STATIC_NEW_MACRO(FileConversion);

    virtual ~FileConversion();

    /**
    * @brief Main method to run the operation
    */
    virtual void execute();

  protected:
    FileConversion();

  private:
    FileConversionFunc::Pointer m;

    FileConversion(const FileConversion&);    // Copy Constructor Not Implemented
    void operator=(const FileConversion&);  // Operator '=' Not Implemented
};



#endif /* FileConversion_H_ */
