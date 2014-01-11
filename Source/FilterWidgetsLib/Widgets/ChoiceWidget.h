#ifndef _ChoiceWidget_H_
#define _ChoiceWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_ChoiceWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT ChoiceWidget : public QWidget, private Ui::ChoiceWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    ChoiceWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~ChoiceWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    ChoiceWidget(const ChoiceWidget&); // Copy Constructor Not Implemented
    void operator=(const ChoiceWidget&); // Operator '=' Not Implemented

};

#endif /* _ChoiceWidget_H_ */

