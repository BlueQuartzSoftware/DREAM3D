#ifndef _InputFileWidget_H_
#define _InputFileWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_InputFileWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT InputFileWidget : public QWidget, private Ui::InputFileWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    InputFileWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~InputFileWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    InputFileWidget(const InputFileWidget&); // Copy Constructor Not Implemented
    void operator=(const InputFileWidget&); // Operator '=' Not Implemented

};

#endif /* _InputFileWidget_H_ */

