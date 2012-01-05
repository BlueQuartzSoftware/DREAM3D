/*
 * QFilterWidget.h
 *
 *  Created on: Dec 30, 2011
 *      Author: mjackson
 */

#ifndef QFILTERWIDGET_H_
#define QFILTERWIDGET_H_

#include <QtGui/QFrame>
#include <QtGui/QGroupBox>

#include "DREAM3DLib/Common/AbstractFilter.h"



#define FILTER_PROPERTY_WRAPPER(type, name, filter_var)\
void set##name(type v) { filter_var->set##name(v); }\
type get##name() { return filter_var->get##name(); }\



class QMouseEvent;


class QFilterWidget : public QGroupBox
{
    Q_OBJECT;
  public:
    QFilterWidget(QWidget* parent = NULL);
    virtual ~QFilterWidget();

    virtual void setupGui();

    virtual AbstractFilter::Pointer getFilter();



   public slots:

     virtual void updateFilterValues();
     virtual void updateQLineEditDoubleValue();
     virtual void updateQLineEditIntValue();
     virtual void selectInputFile();
     virtual void updateComboBoxValue(int v);
     virtual void updateQSpinBoxValue(int v);
     virtual void updateQDoubleSpinBoxValue(double v);

     void changeStyle(bool selected);

  protected:
     virtual void  mouseReleaseEvent ( QMouseEvent* event );

  private:
    QFilterWidget(const QFilterWidget&); // Copy Constructor Not Implemented
    void operator=(const QFilterWidget&); // Operator '=' Not Implemented

};


#endif /* QFILTERWIDGET_H_ */
