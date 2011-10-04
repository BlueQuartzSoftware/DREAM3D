/*
 * FileWriter.h
 *
 *  Created on: Oct 3, 2011
 *      Author: mjackson
 */

#ifndef FILEWRITER_H_
#define FILEWRITER_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/IO/DREAM3DDataFile.h"

namespace DREAM3D
{

  /*
   *
   */
  class DREAM3DLib_EXPORT FileWriter : public DREAM3DDataFile
  {
    public:
    DREAM3D_SHARED_POINTERS(FileWriter)
    DREAM3D_STATIC_NEW_MACRO(FileWriter)
    DREAM3D_TYPE_MACRO(FileWriter)
      virtual ~FileWriter() {};

      virtual int writeHeader() {return -1;}
      virtual int writeFile() {return -1;}

    protected:
      FileWriter(){}

    private:
      FileWriter(const FileWriter&); // Copy Constructor Not Implemented
      void operator=(const FileWriter&); // Operator '=' Not Implemented
  };

} /* namespace DREAM3D */
#endif /* FILEWRITER_H_ */
