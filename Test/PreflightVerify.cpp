#include "PreflightVerify.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"



//#include "moc_PreflightVerify.cpp"

PreflightVerify::PreflightVerify(QObject* parent) :
  QObject(parent),
  m_widgetChanged(false),
  m_beforePreflight(false),
  m_afterPreflight(false),
  m_filterNeedsInputParameters(false)
{

}
PreflightVerify::~PreflightVerify()
{

}

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


