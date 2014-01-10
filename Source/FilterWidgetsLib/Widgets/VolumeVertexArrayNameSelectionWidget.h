#ifndef _VolumeVertexArrayNameSelectionWidget_H_
#define _VolumeVertexArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_VolumeVertexArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT VolumeVertexArrayNameSelectionWidget : public QWidget, private Ui::VolumeVertexArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    VolumeVertexArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~VolumeVertexArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    VolumeVertexArrayNameSelectionWidget(const VolumeVertexArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const VolumeVertexArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _VolumeVertexArrayNameSelectionWidget_H_ */

