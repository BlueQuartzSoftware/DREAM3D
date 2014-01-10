#ifndef _FeatureArrayComparisonSelectionWidget_H_
#define _FeatureArrayComparisonSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_FeatureArrayComparisonSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT FeatureArrayComparisonSelectionWidget : public QWidget, private Ui::FeatureArrayComparisonSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    FeatureArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~FeatureArrayComparisonSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    FeatureArrayComparisonSelectionWidget(const FeatureArrayComparisonSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const FeatureArrayComparisonSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _FeatureArrayComparisonSelectionWidget_H_ */

