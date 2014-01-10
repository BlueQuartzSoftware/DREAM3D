#ifndef _VolumeEnsembleArrayNameSelectionWidget_H_
#define _VolumeEnsembleArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_VolumeEnsembleArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT VolumeEnsembleArrayNameSelectionWidget : public QWidget, private Ui::VolumeEnsembleArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    VolumeEnsembleArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~VolumeEnsembleArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    VolumeEnsembleArrayNameSelectionWidget(const VolumeEnsembleArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const VolumeEnsembleArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _VolumeEnsembleArrayNameSelectionWidget_H_ */

