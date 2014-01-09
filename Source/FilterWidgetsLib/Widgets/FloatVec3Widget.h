#ifndef _FloatVec3Widget_H_
#define _FloatVec3Widget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>

#include "FilterWidgetsLib/ui_FloatVec3Widget.h"

#include "FilterWidgetsLib/FilterWidgetsLib.h"

class FilterWidgetsLib_EXPORT FloatVec3Widget : public QWidget, private Ui::FloatVec3Widget
{
  Q_OBJECT

  public:
    FloatVec3Widget(QWidget* parent = NULL);
    virtual ~FloatVec3Widget();

  private:
    FloatVec3Widget(const FloatVec3Widget&); // Copy Constructor Not Implemented
    void operator=(const FloatVec3Widget&); // Operator '=' Not Implemented


};



#endif /* _FloatVec3Widget_H_ */
