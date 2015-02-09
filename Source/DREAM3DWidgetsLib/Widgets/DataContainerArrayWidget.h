#ifndef _DataContainerArrayWidget_H_
#define _DataContainerArrayWidget_H_

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtWidgets/QWidget>
#include <QtGui/QStandardItemModel>
#include <QtCore/QItemSelectionModel>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"

#include "DREAM3DLib/DataContainers/DataContainerArrayProxy.h"

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

    void updateModelFromProxy(DataContainerArrayProxy& proxy);


  protected slots:
    void updateView();

  protected:
    QList<QStandardItem*> findChildItems(QStandardItem* parent, QString text);
    void removeNonExistantChildren(QStandardItem* parent, QStringList possibleNames);
    void clearStandardItemModel();

  private:
    AbstractFilter*     m_Filter;


    template<typename T>
    QStandardItem* getColumnItem(QStandardItem* parent, QString name, T& proxy)
    {
      QStandardItem* item = NULL;
      QList<QStandardItem*> items = findChildItems(parent, name);
      if (items.count() == 0)
      {
        // Create a new item because we did not find this item already
        item = new QStandardItem(proxy.name);
        //item->setCheckState( (proxy.flag == 2 ? Qt::Checked : Qt::Unchecked) );
        //item->setCheckable(false);
        parent->appendRow(item);
      }
      else if (items.count() > 1)
      {
        item = NULL;
      }
      else
      {
        item = items.at(0);
        //item->setCheckState( (proxy.flag == 2 ? Qt::Checked : Qt::Unchecked) );
        //item->setCheckable(false);
      }

      return item;
    }


    template<typename T>
    QStandardItem* updateProxyItem(QStandardItem* parent, QString name, T& proxy)
    {
      QStandardItem* item = NULL;
      if(NULL == parent) { return item; }
      QList<QStandardItem*> items = findChildItems(parent, name);
      if (items.count() == 1)
      {
        item = items.at(0);
        //   qDebug() << parent->text() << " | " << item->text() << " ::"  << proxy.flag << " (Going to Change to) " << item->checkState();
        proxy.flag = item->checkState();
      }

      return item;
    }


    DataContainerArrayWidget(const DataContainerArrayWidget&); // Copy Constructor Not Implemented
    void operator=(const DataContainerArrayWidget&); // Operator '=' Not Implemented
};

#endif /* _DataContainerArrayWidget_H_ */
