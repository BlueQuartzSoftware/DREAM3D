#ifndef _EdgeEnsembleArrayNameSelectionWidget_H_
#define _EdgeEnsembleArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_EdgeEnsembleArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT EdgeEnsembleArrayNameSelectionWidget : public QWidget, private Ui::EdgeEnsembleArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    EdgeEnsembleArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~EdgeEnsembleArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    EdgeEnsembleArrayNameSelectionWidget(const EdgeEnsembleArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const EdgeEnsembleArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _EdgeEnsembleArrayNameSelectionWidget_H_ */

