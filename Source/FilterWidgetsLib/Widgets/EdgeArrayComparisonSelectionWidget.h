#ifndef _EdgeArrayComparisonSelectionWidget_H_
#define _EdgeArrayComparisonSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_EdgeArrayComparisonSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT EdgeArrayComparisonSelectionWidget : public QWidget, private Ui::EdgeArrayComparisonSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    EdgeArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~EdgeArrayComparisonSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    EdgeArrayComparisonSelectionWidget(const EdgeArrayComparisonSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const EdgeArrayComparisonSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _EdgeArrayComparisonSelectionWidget_H_ */

