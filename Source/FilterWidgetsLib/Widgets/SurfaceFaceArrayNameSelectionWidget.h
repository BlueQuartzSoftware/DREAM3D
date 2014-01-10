#ifndef _SurfaceFaceArrayNameSelectionWidget_H_
#define _SurfaceFaceArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_SurfaceFaceArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT SurfaceFaceArrayNameSelectionWidget : public QWidget, private Ui::SurfaceFaceArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    SurfaceFaceArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~SurfaceFaceArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    SurfaceFaceArrayNameSelectionWidget(const SurfaceFaceArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const SurfaceFaceArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _SurfaceFaceArrayNameSelectionWidget_H_ */

