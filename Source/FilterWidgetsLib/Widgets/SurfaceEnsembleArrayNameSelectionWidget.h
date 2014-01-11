#ifndef _SurfaceEnsembleArrayNameSelectionWidget_H_
#define _SurfaceEnsembleArrayNameSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_SurfaceEnsembleArrayNameSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT SurfaceEnsembleArrayNameSelectionWidget : public QWidget, private Ui::SurfaceEnsembleArrayNameSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    SurfaceEnsembleArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~SurfaceEnsembleArrayNameSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    SurfaceEnsembleArrayNameSelectionWidget(const SurfaceEnsembleArrayNameSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const SurfaceEnsembleArrayNameSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _SurfaceEnsembleArrayNameSelectionWidget_H_ */

