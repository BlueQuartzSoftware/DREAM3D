#ifndef _FaceArrayComparisonSelectionWidget_H_
#define _FaceArrayComparisonSelectionWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_FaceArrayComparisonSelectionWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT FaceArrayComparisonSelectionWidget : public QWidget, private Ui::FaceArrayComparisonSelectionWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    FaceArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~FaceArrayComparisonSelectionWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    FaceArrayComparisonSelectionWidget(const FaceArrayComparisonSelectionWidget&); // Copy Constructor Not Implemented
    void operator=(const FaceArrayComparisonSelectionWidget&); // Operator '=' Not Implemented

};

#endif /* _FaceArrayComparisonSelectionWidget_H_ */

