#ifndef _SurfaceFeatureArrayNameSelectionWidget_H_
#define _SurfaceFeatureArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_SurfaceFeatureArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT SurfaceFeatureArrayNameSelectionWidget : public QWidget, private Ui::SurfaceFeatureArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    SurfaceFeatureArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~SurfaceFeatureArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    SurfaceFeatureArrayNameSelectionWidget(const SurfaceFeatureArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const SurfaceFeatureArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _SurfaceFeatureArrayNameSelectionWidget_H_ */

