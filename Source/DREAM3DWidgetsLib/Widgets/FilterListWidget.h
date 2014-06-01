/*
 * FilterListWidget.h
 *
 *  Created on: Feb 2, 2012
 *      Author: mjackson
 */

#ifndef _QFILTERLISTWIDGET_H_
#define _QFILTERLISTWIDGET_H_

#include <QtCore/QPoint>
#include <QtGui/QListWidget>


#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
/*
 *
 */
class DREAM3DWidgetsLib_EXPORT FilterListWidget: public QListWidget
{
    Q_OBJECT

  public:
    FilterListWidget(QWidget* parent = 0);
    virtual ~FilterListWidget();

  protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);

  private:
    void performDrag();

    QPoint startPos;
};

#endif /* QFILTERLISTWIDGET_H_ */

