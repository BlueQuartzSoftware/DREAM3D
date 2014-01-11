#ifndef _VertexEnsembleArrayNameSelectionWidget_H_
#define _VertexEnsembleArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_VertexEnsembleArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT VertexEnsembleArrayNameSelectionWidget : public QWidget, private Ui::VertexEnsembleArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    VertexEnsembleArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~VertexEnsembleArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    VertexEnsembleArrayNameSelectionWidget(const VertexEnsembleArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const VertexEnsembleArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _VertexEnsembleArrayNameSelectionWidget_H_ */

