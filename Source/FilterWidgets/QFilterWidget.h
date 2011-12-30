/*
 * QFilterWidget.h
 *
 *  Created on: Dec 30, 2011
 *      Author: mjackson
 */

#ifndef QFILTERWIDGET_H_
#define QFILTERWIDGET_H_

#include "QtGui/QFrame"

#include "DREAM3DLib/Common/AbstractFilter.h"



class QFilterWidget : public QFrame
{
    Q_OBJECT;
  public:
    QFilterWidget(QWidget* parent = NULL);
    virtual ~QFilterWidget();


    virtual void setupGui();

    virtual AbstractFilter::Pointer getFilter();

};


#endif /* QFILTERWIDGET_H_ */
