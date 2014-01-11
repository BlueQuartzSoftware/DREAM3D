#ifndef _InputPathWidget_H_
#define _InputPathWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_InputPathWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT InputPathWidget : public QWidget, private Ui::InputPathWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    InputPathWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~InputPathWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    InputPathWidget(const InputPathWidget&); // Copy Constructor Not Implemented
    void operator=(const InputPathWidget&); // Operator '=' Not Implemented

};

#endif /* _InputPathWidget_H_ */

