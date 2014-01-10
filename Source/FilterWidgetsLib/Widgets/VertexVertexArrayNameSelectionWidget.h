#ifndef _VertexVertexArrayNameSelectionWidget_H_
#define _VertexVertexArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_VertexVertexArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT VertexVertexArrayNameSelectionWidget : public QWidget, private Ui::VertexVertexArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    VertexVertexArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~VertexVertexArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    VertexVertexArrayNameSelectionWidget(const VertexVertexArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const VertexVertexArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _VertexVertexArrayNameSelectionWidget_H_ */

