#ifndef _SurfaceVertexArrayNameSelectionWidget_H_
#define _SurfaceVertexArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_SurfaceVertexArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT SurfaceVertexArrayNameSelectionWidget : public QWidget, private Ui::SurfaceVertexArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    SurfaceVertexArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~SurfaceVertexArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    SurfaceVertexArrayNameSelectionWidget(const SurfaceVertexArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const SurfaceVertexArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _SurfaceVertexArrayNameSelectionWidget_H_ */

