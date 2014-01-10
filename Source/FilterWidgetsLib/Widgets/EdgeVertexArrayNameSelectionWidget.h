#ifndef _EdgeVertexArrayNameSelectionWidget_H_
#define _EdgeVertexArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_EdgeVertexArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT EdgeVertexArrayNameSelectionWidget : public QWidget, private Ui::EdgeVertexArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    EdgeVertexArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~EdgeVertexArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    EdgeVertexArrayNameSelectionWidget(const EdgeVertexArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const EdgeVertexArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _EdgeVertexArrayNameSelectionWidget_H_ */

