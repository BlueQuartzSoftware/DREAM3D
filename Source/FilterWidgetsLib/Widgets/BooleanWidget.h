#ifndef _BooleanWidget_H_
#define _BooleanWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_BooleanWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT BooleanWidget : public QWidget, private Ui::BooleanWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    BooleanWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~BooleanWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    BooleanWidget(const BooleanWidget&); // Copy Constructor Not Implemented
    void operator=(const BooleanWidget&); // Operator '=' Not Implemented

};

#endif /* _BooleanWidget_H_ */

