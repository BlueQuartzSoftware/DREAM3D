#ifndef _VertexFeatureArrayNameSelectionWidget_H_
#define _VertexFeatureArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_VertexFeatureArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT VertexFeatureArrayNameSelectionWidget : public QWidget, private Ui::VertexFeatureArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    VertexFeatureArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~VertexFeatureArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    VertexFeatureArrayNameSelectionWidget(const VertexFeatureArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const VertexFeatureArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _VertexFeatureArrayNameSelectionWidget_H_ */

