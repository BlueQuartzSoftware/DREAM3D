#ifndef _OutputPathWidget_H_
#define _OutputPathWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_OutputPathWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT OutputPathWidget : public QWidget, private Ui::OutputPathWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    OutputPathWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~OutputPathWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    OutputPathWidget(const OutputPathWidget&); // Copy Constructor Not Implemented
    void operator=(const OutputPathWidget&); // Operator '=' Not Implemented

};

#endif /* _OutputPathWidget_H_ */

