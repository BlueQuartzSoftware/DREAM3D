#ifndef _VolumeFeatureArrayNameSelectionWidget_H_
#define _VolumeFeatureArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_VolumeFeatureArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT VolumeFeatureArrayNameSelectionWidget : public QWidget, private Ui::VolumeFeatureArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    VolumeFeatureArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~VolumeFeatureArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    VolumeFeatureArrayNameSelectionWidget(const VolumeFeatureArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const VolumeFeatureArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _VolumeFeatureArrayNameSelectionWidget_H_ */

