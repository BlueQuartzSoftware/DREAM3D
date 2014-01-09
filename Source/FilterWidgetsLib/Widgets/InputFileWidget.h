
#ifndef _InputFileWidget_H_
#define _InputFileWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>

#include "FilterWidgetsLib/ui_InputFileWidget.h"

#include "FilterWidgetsLib/FilterWidgetsLib.h"

class FilterWidgetsLib_EXPORT InputFileWidget : public QWidget, private Ui::InputFileWidget
{
  Q_OBJECT

  public:
    InputFileWidget(QWidget* parent = NULL);
    virtual ~InputFileWidget();

  private:
    InputFileWidget(const InputFileWidget&); // Copy Constructor Not Implemented
    void operator=(const InputFileWidget&); // Operator '=' Not Implemented


};

#endif /* _InputFileWidget_H_ */

