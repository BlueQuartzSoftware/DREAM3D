#ifndef _VertexArrayComparisonSelectionWidget_H_
#define _VertexArrayComparisonSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_VertexArrayComparisonSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT VertexArrayComparisonSelectionWidget : public QWidget, private Ui::VertexArrayComparisonSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    VertexArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~VertexArrayComparisonSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    VertexArrayComparisonSelectionWidget(const VertexArrayComparisonSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const VertexArrayComparisonSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _VertexArrayComparisonSelectionWidget_H_ */

