#ifndef _DataContainerArrayWidget_H_
#define _DataContainerArrayWidget_H_

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtWidgets/QWidget>
#include <QtGui/QStandardItemModel>
#include <QtCore/QItemSelectionModel>


#include "SIMPLib/Common/AbstractFilter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"

#include "ui_DataContainerArrayWidget.h"


class QStandardItem;
class AbstractFilter;
class DataContainerArray;

class DREAM3DWidgetsLib_EXPORT DataContainerArrayWidget : public QWidget, private Ui::DataContainerArrayWidget
{
    Q_OBJECT

  public:
    DataContainerArrayWidget(QWidget* parent = NULL);
    DataContainerArrayWidget(AbstractFilter* filter, QWidget* parent = NULL);

    virtual ~DataContainerArrayWidget();


    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

    void initializeWithFilter(AbstractFilter* filter);

    void updateModelFromProxy(DataContainerArray* dc);


  protected slots:

    void updateView();

    void preflightCompleted();

    void dcaProxyView_indexChanged(const QModelIndex& current, const QModelIndex& previous);

    void on_dcaProxyView_updatePreviewWidget(const QModelIndex& index);



  protected:

    void clearStandardItemModel();

  private:
    AbstractFilter*     m_Filter;


    DataContainerArrayWidget(const DataContainerArrayWidget&); // Copy Constructor Not Implemented
    void operator=(const DataContainerArrayWidget&); // Operator '=' Not Implemented
};

#endif /* _DataContainerArrayWidget_H_ */
