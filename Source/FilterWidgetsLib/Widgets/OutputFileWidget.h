#ifndef _OutputFileWidget_H_
#define _OutputFileWidget_H_




#include <QtCore/QObject>
#include <QtGui/QWidget>


#include "DREAM3DLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/QFilterParameterWidget.h"
#include "FilterWidgetsLib/FilterWidgetsLib.h"

#include "FilterWidgetsLib/ui_OutputFileWidget.h"


/**
* @brief
* @author
* @version
*/
class FilterWidgetsLib_EXPORT OutputFileWidget : public QWidget, private Ui::OutputFileWidget
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    OutputFileWidget(FilterParameter* parameter, AbstractFilter* filter = NULL, QWidget* parent = NULL);
    
    virtual ~OutputFileWidget();
    
    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

  public slots:


  private:
    AbstractFilter*   m_Filter;
    FilterParameter*  m_FilterParameter;

    OutputFileWidget(const OutputFileWidget&); // Copy Constructor Not Implemented
    void operator=(const OutputFileWidget&); // Operator '=' Not Implemented

};

#endif /* _OutputFileWidget_H_ */

