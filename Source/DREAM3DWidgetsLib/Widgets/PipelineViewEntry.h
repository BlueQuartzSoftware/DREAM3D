#ifndef _PipelineViewEntry_H_
#define _PipelineViewEntry_H_

#include <QtCore/QObject>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"

#include "FilterWidgetsLib/FilterWidgetsLib.h"
#include "FilterWidgetsLib/Widgets/QFilterWidget.h"


class PipelineViewEntry : public QObject
{
    Q_OBJECT;

  public:
    explicit PipelineViewEntry(const QString& filterClassName );
    explicit PipelineViewEntry(AbstractFilter* filter );

    virtual ~PipelineViewEntry();

    PipelineViewEntry(const PipelineViewEntry&); // Copy Constructor Not Implemented
    void operator=(const PipelineViewEntry&); // Operator '=' Not Implemented

    QFilterWidget* getWidget();
    AbstractFilter* getFilter();
    QVector<QWidget*> getFilterParameterWidgets();


  protected:
    void initialize(const QString& name);
    void instantiateFilter(const QString& filterClassName);

  private:
    AbstractFilter*  m_Filter;
    QFilterWidget*   m_Widget;
    QVector<QWidget*> m_FilterParameterWidgets;



};

#endif /* _PipelineViewEntry_H_ */
