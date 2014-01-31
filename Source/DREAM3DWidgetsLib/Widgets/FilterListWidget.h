/*
 * FilterListWidget.h
 *
 *  Created on: Feb 2, 2012
 *      Author: mjackson
 */

#ifndef QFILTERLISTWIDGET_H_
#define QFILTERLISTWIDGET_H_

#include <QtCore/QPoint>
#include <QtGui/QListWidget>

/*
 *
 */
class FilterListWidget: public QListWidget
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

