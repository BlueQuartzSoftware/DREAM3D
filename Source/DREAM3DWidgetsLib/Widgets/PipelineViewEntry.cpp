#include "PipelineViewEntry.h"

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"


#include "FilterWidgetsLib/FilterWidgetManager.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineViewEntry::PipelineViewEntry(const QString& filterClassName) :
  m_Filter(NULL),
  m_Widget(NULL)
{
  instantiateFilter(filterClassName);
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineViewEntry::PipelineViewEntry(AbstractFilter* filter ) :
  m_Filter(filter),
  m_Widget(NULL)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineViewEntry::~PipelineViewEntry()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewEntry::instantiateFilter(const QString& filterClassName)
{
  FilterManager::Pointer fm = FilterManager::Instance();
  if(NULL == fm.get() ) { return; }
  IFilterFactory::Pointer ff = fm->getFactoryForFilter(filterClassName);
  if (NULL == ff.get()) { return; }

  // Create an instance of the filter. Since we are dealing with the AbstractFilter interface we can not
  // actually use the actual filter class. We are going to have to rely on QProperties or Signals/Slots
  // to communicate changes back to the filter.
  m_Filter = ff->create();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineViewEntry::initialize(const QString& filterClassName)
{


  // Create a new QFilterWidget
  m_Widget = new QFilterWidget();
  m_Widget->setTitle(m_Filter->getHumanLabel() );




}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* PipelineViewEntry::getWidget()
{
  return m_Widget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* PipelineViewEntry::getFilter()
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QWidget*> PipelineViewEntry::getFilterParameterWidgets()
{
  return m_FilterParameterWidgets;
}
