#ifndef _AxisAngleWidget_H_
#define _AxisAngleWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_AxisAngleWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT AxisAngleWidget : public QWidget, private Ui::AxisAngleWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    AxisAngleWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~AxisAngleWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    AxisAngleWidget(const AxisAngleWidget&); // Copy Constructor Not Implemented
    void operator=(const AxisAngleWidget&); // Operator '=' Not Implemented

};

#endif /* _AxisAngleWidget_H_ */

