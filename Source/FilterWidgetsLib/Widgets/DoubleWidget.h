#ifndef _DoubleWidget_H_
#define _DoubleWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_DoubleWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT DoubleWidget : public QWidget, private Ui::DoubleWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    DoubleWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~DoubleWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    DoubleWidget(const DoubleWidget&); // Copy Constructor Not Implemented
    void operator=(const DoubleWidget&); // Operator '=' Not Implemented

};

#endif /* _DoubleWidget_H_ */

