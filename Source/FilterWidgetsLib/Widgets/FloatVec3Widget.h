#ifndef _FloatVec3Widget_H_
#define _FloatVec3Widget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_FloatVec3Widget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT FloatVec3Widget : public QWidget, private Ui::FloatVec3Widget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    FloatVec3Widget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~FloatVec3Widget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    FloatVec3Widget(const FloatVec3Widget&); // Copy Constructor Not Implemented
    void operator=(const FloatVec3Widget&); // Operator '=' Not Implemented

};

#endif /* _FloatVec3Widget_H_ */

