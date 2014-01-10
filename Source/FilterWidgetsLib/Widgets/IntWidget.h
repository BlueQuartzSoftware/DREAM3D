#ifndef _IntWidget_H_
#define _IntWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_IntWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT IntWidget : public QWidget, private Ui::IntWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    IntWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~IntWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    IntWidget(const IntWidget&); // Copy Constructor Not Implemented
    void operator=(const IntWidget&); // Operator '=' Not Implemented

};

#endif /* _IntWidget_H_ */

