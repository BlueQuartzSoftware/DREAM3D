#ifndef _VolumeFaceArrayNameSelectionWidget_H_
#define _VolumeFaceArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_VolumeFaceArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT VolumeFaceArrayNameSelectionWidget : public QWidget, private Ui::VolumeFaceArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    VolumeFaceArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~VolumeFaceArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    VolumeFaceArrayNameSelectionWidget(const VolumeFaceArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const VolumeFaceArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _VolumeFaceArrayNameSelectionWidget_H_ */

