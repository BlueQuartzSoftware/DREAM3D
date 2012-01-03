#ifndef QLoadSlices_H_
#define QLoadSlices_H_
#include <QtCore/QObject>
#include "FilterWidgets/QFilterWidget.h"
#include "DREAM3DLib/ReconstructionFilters/LoadSlices.h"
class QLoadSlicesWidget : public QFilterWidget
{
  Q_OBJECT
    ;
  public:
    QLoadSlicesWidget(QWidget* parent = NULL) :
        QFilterWidget(parent)
    {
      m_Filter = LoadSlices::New();
      setupGui();
      setTitle(QString::fromStdString(m_Filter->getHumanLabel()));
    }
    virtual ~QLoadSlicesWidget()
    {
    }
    virtual AbstractFilter::Pointer getFilter()
    {
      return m_Filter;
    }

    Q_PROPERTY(float MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)
    FILTER_PROPERTY_WRAPPER(float, MisorientationTolerance, m_Filter)
    ;

    Q_PROPERTY(int ZStartIndex READ getZStartIndex WRITE setZStartIndex)
    FILTER_PROPERTY_WRAPPER(int, ZStartIndex, m_Filter)
    ;

    Q_PROPERTY(int ZEndIndex READ getZEndIndex WRITE setZEndIndex)
    FILTER_PROPERTY_WRAPPER(int, ZEndIndex, m_Filter)
    ;
  private:
    LoadSlices::Pointer m_Filter;
};
#endif /* QLoadSlices_H_ */
