#ifndef _RemoveArraysObserver_H_
#define _RemoveArraysObserver_H_


#include <QtCore/QObject>
#include <QtCore/QList>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/PipelineMessage.h"


class RemoveArraysObserver : public QObject
{
    Q_OBJECT

    SIMPL_INSTANCE_PROPERTY(QList<PipelineMessage>, ErrorList)
    Q_PROPERTY(QList<PipelineMessage> ErrorList READ getErrorList WRITE setErrorList)

  public:
    SIMPL_SHARED_POINTERS(RemoveArraysObserver)
    SIMPL_TYPE_MACRO(RemoveArraysObserver)
    SIMPL_STATIC_NEW_MACRO(RemoveArraysObserver)

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
