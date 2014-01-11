#ifndef _IntVec3Widget_H_
#define _IntVec3Widget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_IntVec3Widget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT IntVec3Widget : public QWidget, private Ui::IntVec3Widget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    IntVec3Widget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~IntVec3Widget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    IntVec3Widget(const IntVec3Widget&); // Copy Constructor Not Implemented
    void operator=(const IntVec3Widget&); // Operator '=' Not Implemented

};

#endif /* _IntVec3Widget_H_ */

