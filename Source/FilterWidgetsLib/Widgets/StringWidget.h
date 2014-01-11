#ifndef _StringWidget_H_
#define _StringWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_StringWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT StringWidget : public QWidget, private Ui::StringWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    StringWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~StringWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    StringWidget(const StringWidget&); // Copy Constructor Not Implemented
    void operator=(const StringWidget&); // Operator '=' Not Implemented

};

#endif /* _StringWidget_H_ */

