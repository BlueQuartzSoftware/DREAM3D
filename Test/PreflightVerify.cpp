#include "PreflightVerify.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

PreflightVerify::PreflightVerify(QObject* parent)
: QObject(parent)
{
}
PreflightVerify::~PreflightVerify() = default;

void PreflightVerify::widgetChanged(const QString& msg)
{
  m_widgetChanged = true;
}

void PreflightVerify::beforePreflight()
{
  m_beforePreflight = true;
}

void PreflightVerify::afterPreflight()
{
  m_afterPreflight = true;
}

void PreflightVerify::filterNeedsInputParameters(AbstractFilter* filter)
{
  m_filterNeedsInputParameters = true;
}
