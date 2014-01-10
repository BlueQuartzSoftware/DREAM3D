#ifndef _EdgeFeatureArrayNameSelectionWidget_H_
#define _EdgeFeatureArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_EdgeFeatureArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT EdgeFeatureArrayNameSelectionWidget : public QWidget, private Ui::EdgeFeatureArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    EdgeFeatureArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~EdgeFeatureArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    EdgeFeatureArrayNameSelectionWidget(const EdgeFeatureArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const EdgeFeatureArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _EdgeFeatureArrayNameSelectionWidget_H_ */

