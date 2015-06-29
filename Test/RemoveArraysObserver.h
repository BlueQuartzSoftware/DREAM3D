#ifndef _RemoveArraysObserver_H_
#define _RemoveArraysObserver_H_


#include <QtCore/QObject>
#include <QtCore/QList>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DREAM3DLibVersion.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/PipelineMessage.h"


class RemoveArraysObserver : public QObject
{
    Q_OBJECT

    DREAM3D_INSTANCE_PROPERTY(QList<PipelineMessage>, ErrorList)
    Q_PROPERTY(QList<PipelineMessage> ErrorList READ getErrorList WRITE setErrorList)

  public:
    DREAM3D_SHARED_POINTERS(RemoveArraysObserver)
    DREAM3D_TYPE_MACRO(RemoveArraysObserver)
    DREAM3D_STATIC_NEW_MACRO(RemoveArraysObserver)

    RemoveArraysObserver() {}
    virtual ~RemoveArraysObserver() {}

  public slots:
    void processPipelineMessage(const PipelineMessage& msg)
    {
      m_ErrorList.push_back(msg);
    }

  private:
    RemoveArraysObserver(const RemoveArraysObserver&); // Copy Constructor Not Implemented
    void operator=(const RemoveArraysObserver&); // Operator '=' Not Implemented
};


#endif /* _RemoveArraysObserver_H_ */
