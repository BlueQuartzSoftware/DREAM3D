#ifndef _CustomWidget_H_
#define _CustomWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_CustomWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT CustomWidget : public QWidget, private Ui::CustomWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    CustomWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~CustomWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    CustomWidget(const CustomWidget&); // Copy Constructor Not Implemented
    void operator=(const CustomWidget&); // Operator '=' Not Implemented

};

#endif /* _CustomWidget_H_ */

