/*
 * QFilterListWidget.h
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
class QFilterListWidget: public QListWidget
{
    Q_OBJECT

  public:
    QFilterListWidget(QWidget* parent = 0);
    virtual ~QFilterListWidget();

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

