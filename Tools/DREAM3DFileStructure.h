#ifndef _DREAM3DFileStructure_H_
#define _DREAM3DFileStructure_H_







#include <QtCore/QObject>
#include <QtCore/QString>


#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataContainers/DataContainerArray.h"




class DREAM3DFileStructure : public QObject
{
    Q_OBJECT

  public:
    DREAM3D_SHARED_POINTERS(DREAM3DFileStructure)
    DREAM3D_STATIC_NEW_MACRO(DREAM3DFileStructure)
    DREAM3D_TYPE_MACRO(DREAM3DFileStructure)


    virtual ~DREAM3DFileStructure();


    static DataContainerArray::Pointer ReadFileStructure(const QString& filePath);

  protected:
    DREAM3DFileStructure();



  private:
    DREAM3DFileStructure(const DREAM3DFileStructure&); // Copy Constructor Not Implemented
    void operator=(const DREAM3DFileStructure&); // Operator '=' Not Implemented

};

#endif /* _DREAM3DFileStructure_H_ */

