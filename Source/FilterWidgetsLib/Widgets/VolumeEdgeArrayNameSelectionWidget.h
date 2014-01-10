#ifndef _VolumeEdgeArrayNameSelectionWidget_H_
#define _VolumeEdgeArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_VolumeEdgeArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT VolumeEdgeArrayNameSelectionWidget : public QWidget, private Ui::VolumeEdgeArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    VolumeEdgeArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~VolumeEdgeArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    VolumeEdgeArrayNameSelectionWidget(const VolumeEdgeArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const VolumeEdgeArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _VolumeEdgeArrayNameSelectionWidget_H_ */

