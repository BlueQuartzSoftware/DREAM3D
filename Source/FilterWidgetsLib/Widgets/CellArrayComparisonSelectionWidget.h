#ifndef _CellArrayComparisonSelectionWidget_H_
#define _CellArrayComparisonSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_CellArrayComparisonSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT CellArrayComparisonSelectionWidget : public QWidget, private Ui::CellArrayComparisonSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    CellArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~CellArrayComparisonSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    CellArrayComparisonSelectionWidget(const CellArrayComparisonSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const CellArrayComparisonSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _CellArrayComparisonSelectionWidget_H_ */

